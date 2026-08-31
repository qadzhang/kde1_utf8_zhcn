/*
 * snitray.cpp -- StatusNotifierItem host 实现（见 snitray.h 头注释）
 *
 *   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
 *
 * 伪代码（整体流程）：
 *   1. init:
 *        a. dbus_bus_get(SESSION) → conn
 *        b. dbus_bus_request_name("org.kde.StatusNotifierWatcher")
 *        c. add_filter(messageFilter) —— 分发 Add/Unregister 与属性应答
 *        d. QSocketNotifier(dbus fd, Read) → dbus_connection_read_write_dispatch
 *        e. 发 StatusNotifierHostRegistered 信号（item 端据此显示）
 *   2. messageFilter（DBus 线程即主线程，同步派发）：
 *        - signal NameAcquired → 视为就绪
 *        - method_return 于此前发出的 Get(Pixmaps) → 取 a(iiay) 转 QImage
 *          → renderIcon 画到该 item 的 X 窗口
 *        - signal ItemRegistered（自身 watcher 发出）→ handleItemRegister
 *   3. handleItemRegister:
 *        - 新建 24x24 X 窗（dock_area 子窗）+ SNIClient 入清单
 *        - 异步 dbus_message_new_method_call(item service/path,
 *          "org.freedesktop.DBus.Properties", "Get",
 *          args: (iface "org.kde.StatusNotifierItem", prop "IconPixmap"))
 *        - relayout 信号 → kPanel 重排 dock_area
 *   4. 客户端退出：DBus 的 NameOwnerChanged（唯一名消失）→ 摘除 X 窗与条目
 */
/* [KDE1 Revival 2026] TQt3 头必须先于 snitray.h 里的 X11/Xlib.h 解析——
   X11 的宏（Bool/Status/None/Unsorted 等 X.h 宏）会击穿 TQt3 枚举
   （项目既有约定，见 CHANGELOG 2026-08-30「X11 宏污染连锁根治」） */
#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qtl.h>
#include <qsocketnotifier.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

#include "snitray.h"

#include <X11/Xlib.h>

#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
#include <X11/Xutil.h>
}

/* 图标主题查找（实现见下方）：messageFilter 的 IconName 应答分支先于
   定义使用，故前向声明 */
static QString sniFindIconFile(const char *name);

SNITray::SNITray(QWidget *da, QObject *parent)
  : QObject(parent), dockArea(da), conn(0), notifier(0)
{
    clients.setAutoDelete(true);
}

SNITray::~SNITray()
{
    // [KDE1 Revival 2026] 析构顺序：先摘事件源（notifier/filter）再放连接，
    // 避免总线末梢消息回调进入半析构对象
    delete notifier;
    notifier = 0;
    if (conn) {
        dbus_connection_remove_filter(conn, messageFilter, this);
        dbus_connection_unref(conn);
        conn = 0;
    }
}

bool SNITray::init()
{
    DBusError err;
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (!conn) {
        fprintf(stderr, "kpanel SNI: no session bus: %s\n", err.message);
        dbus_error_free(&err);
        return false;
    }
    dbus_connection_set_exit_on_disconnect(conn, FALSE);

    int rc = dbus_bus_request_name(conn, "org.kde.StatusNotifierWatcher",
                                   DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (rc == -1) {
        fprintf(stderr, "kpanel SNI: request name failed: %s\n", err.message);
        dbus_error_free(&err);
        return false;
    }

    /* Host 角色注册信号：告知 item 端 host 已就绪 */
    dbus_bus_add_match(conn,
        "type='signal',interface='org.kde.StatusNotifierWatcher',member="
        "StatusNotifierHostRegistered", &err);
    dbus_error_free(&err);

    /* [2026-08-31] 补 NameOwnerChanged 订阅：libdbus 裸连接不过滤器推送，
       不加 match rule 永远收不到该广播——SNI 客户端（fcitx5 等）退出后
       指示栏残留永久空格子即此根因（本机 dbus-daemon 实测证实） */
    dbus_bus_add_match(conn,
        "type='signal',sender='org.freedesktop.DBus',"
        "interface='org.freedesktop.DBus',member='NameOwnerChanged'", &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "kpanel SNI: NameOwnerChanged match failed: %s\n",
                err.message);
    }
    dbus_error_free(&err);

    dbus_connection_add_filter(conn, messageFilter, this, 0);

    /* DBus fd 挂进 TQt3 事件循环。
     * [KDE1 Revival 2026] QSocketNotifier 单独驱动不可靠：libdbus 一次读
     * 可能吞进多条消息进内部队列，fd 不再可读而队列滞留（dispatch 未排空）
     * → notifier 永不触发 → watcher 对一切 DBus 调用无响应（实测 dbus-send
     * 全部 NoReply 而 strace 显示应答已 sendmsg 的间歇假象即此）。补一个
     * 100ms 周期定时器兜底 dispatch：notifier 负责低延迟，timer 负责不漏。 */
    int fd = -1;
    if (dbus_connection_get_unix_fd(conn, &fd) && fd >= 0) {
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, SIGNAL(activated(int)), this, SLOT(slotDispatch()));
    }
    {
        QTimer *poll = new QTimer(this);
        connect(poll, SIGNAL(timeout()), this, SLOT(slotDispatch()));
        poll->start(100);
    }

    /* 对象路径 /StatusNotifierWatcher 的方法由 messageFilter 的
       method_call 分支处理（Register/Unregister/Get） */

    /* [KDE1 Revival 2026] 按 SNI 规范主动宣告 host 上线：先于 kpanel 启动、
       正在等待 host 出现的 item（如 fcitx5）据此触发注册重试；同时
       dbus_bus_request_name 本身引发的 NameOwnerChanged 也会被监听
       watcher 总线名的 item 捕获——双通道保证晚启动的 host 不漏单 */
    {
        DBusMessage *sig = dbus_message_new_signal(
            "/StatusNotifierWatcher", "org.kde.StatusNotifierWatcher",
            "StatusNotifierHostRegistered");
        if (sig) {
            dbus_connection_send(conn, sig, 0);
            dbus_message_unref(sig);
        }
    }

    dbus_connection_flush(conn);
    fprintf(stderr, "kpanel SNI: watcher online\n");
    return true;
}

/* [KDE1 Revival 2026] 见 snitray.h 注释：把写缓冲冲刷挪到事件循环安全点 */
void SNITray::slotFlush()
{
    if (conn)
        dbus_connection_flush(conn);
}

// QSocketNotifier 的槽（宏展开的连接需要显式 slot 声明；此处用
// QObject::connect 的 SLOT 定位——需要一个真槽函数）
void SNITray::slotDispatch()
{
    while (dbus_connection_get_dispatch_status(conn) == DBUS_DISPATCH_DATA_REMAINS)
        dbus_connection_dispatch(conn);
    dbus_connection_read_write_dispatch(conn, 0);
}

DBusHandlerResult SNITray::messageFilter(DBusConnection *c, DBusMessage *m,
                                         void *user_data)
{
    SNITray *self = (SNITray *) user_data;

    /* item 端调用 watcher 的注册/注销方法 */
    if (dbus_message_is_method_call(m, "org.kde.StatusNotifierWatcher",
                                    "RegisterStatusNotifierItem")) {
        const char *arg = 0;
        DBusMessageIter it;
        dbus_message_iter_init(m, &it);
        if (dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &arg);
        if (arg) {
            /* 参数可能为总线名或完整对象路径——两者都存。
             * [KDE1 Revival 2026] 按总线名注册时对象路径必须用 SNI 规范
             * 默认路径 /StatusNotifierItem（fcitx5/electron 等皆如此）——
             * 原实现留 "/"，item 对象不在 "/" 上，IconPixmap 的 Get 全部
             * 收到 error，图标永远渲染不出来（重启面板"图标消失"的根因） */
            char svc[256] = {0}, path[256] = "/StatusNotifierItem";
            if (arg[0] == '/') {
                snprintf(path, sizeof(path), "%s", arg);
                snprintf(svc, sizeof(svc), "%s",
                         dbus_message_get_sender(m));
            } else {
                snprintf(svc, sizeof(svc), "%s", arg);
            }
            self->handleItemRegister(svc, path);
        }
        DBusMessage *r = dbus_message_new_method_return(m);
        if (r) { dbus_connection_send(c, r, 0); dbus_message_unref(r); }
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    if (dbus_message_is_method_call(m, "org.kde.StatusNotifierWatcher",
                                    "UnregisterStatusNotifierItem")) {
        const char *arg = 0;
        DBusMessageIter it;
        dbus_message_iter_init(m, &it);
        if (dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &arg);
        if (arg)
            self->handleItemUnregister(
                arg[0] == '/' ? dbus_message_get_sender(m) : arg);
        DBusMessage *r = dbus_message_new_method_return(m);
        if (r) { dbus_connection_send(c, r, 0); dbus_message_unref(r); }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    /* 标准属性：IsStatusNotifierHostAllowed / ProtocolVersion / Items */
    if (dbus_message_is_method_call(m, "org.freedesktop.DBus.Properties",
                                    "Get")) {
        const char *iface = 0, *prop = 0;
        DBusMessageIter it;
        dbus_message_iter_init(m, &it);
        if (dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &iface);
        if (dbus_message_iter_next(&it) &&
            dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &prop);
        if (iface && prop &&
            strcmp(iface, "org.kde.StatusNotifierWatcher") == 0) {
            DBusMessage *r = dbus_message_new_method_return(m);
            DBusMessageIter rit, vit;
            dbus_message_iter_init_append(r, &rit);
            /* [KDE1 Revival 2026] VARIANT 容器必须携带类型签名（libdbus 断言
               "(VARIANT && signature != NULL)"，传 NULL 直接 abort 整个进程
               ——fcitx5 等探测 IsStatusNotifierHostRegistered 即触发，kpanel
               应声崩溃的根因）。签名按属性类型逐项给出。 */
            const char *vsig = 0;
            if (strcmp(prop, "RegisteredStatusNotifierItems") == 0)
                vsig = DBUS_TYPE_ARRAY_AS_STRING DBUS_TYPE_STRING_AS_STRING;
            else if (strcmp(prop, "ProtocolVersion") == 0)
                vsig = DBUS_TYPE_INT32_AS_STRING;
            else
                vsig = DBUS_TYPE_BOOLEAN_AS_STRING;
            dbus_message_iter_open_container(&rit, DBUS_TYPE_VARIANT,
                                             vsig, &vit);
            if (strcmp(prop, "IsStatusNotifierHostAllowed") == 0) {
                dbus_bool_t v = TRUE;
                dbus_message_iter_append_basic(&vit, DBUS_TYPE_BOOLEAN, &v);
            } else if (strcmp(prop, "IsStatusNotifierHostRegistered") == 0) {
                /* [2026-08-31] SNI 规范标准属性：item 端（含 fcitx5）探测
                   host 存活即查此属性，缺失会误判无 host 而回退别的路径 */
                dbus_bool_t v = TRUE;
                dbus_message_iter_append_basic(&vit, DBUS_TYPE_BOOLEAN, &v);
            } else if (strcmp(prop, "RegisteredStatusNotifierItems") == 0) {
                /* [2026-08-31] 已注册 item 清单（as）——供其他观察者同步状态 */
                DBusMessageIter ait;
                dbus_message_iter_open_container(&vit, DBUS_TYPE_ARRAY,
                                                 DBUS_TYPE_STRING_AS_STRING, &ait);
                for (SNIClient *cl = self->clients.first(); cl;
                     cl = self->clients.next()) {
                    const char *svc = cl->service.latin1();
                    /* latin1 对总线名（纯 ASCII）无损；防御性跳过空值 */
                    if (svc && *svc)
                        dbus_message_iter_append_basic(&ait, DBUS_TYPE_STRING, &svc);
                }
                dbus_message_iter_close_container(&vit, &ait);
            } else if (strcmp(prop, "ProtocolVersion") == 0) {
                dbus_int32_t v = 0;
                dbus_message_iter_append_basic(&vit, DBUS_TYPE_INT32, &v);
            } else {
                dbus_message_iter_abandon_container(&rit, &vit);
                dbus_message_unref(r);
                r = dbus_message_new_error(m, DBUS_ERROR_UNKNOWN_PROPERTY, prop);
                dbus_connection_send(c, r, 0);
                dbus_message_unref(r);
                return DBUS_HANDLER_RESULT_HANDLED;
            }
            dbus_message_iter_close_container(&rit, &vit);
            dbus_connection_send(c, r, 0);
            dbus_message_unref(r);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }

    /* item 的图标属性应答（含失败 error）→ 渲染或回退。
     * [KDE1 Revival 2026] reply 路由修复：以 reply_serial 匹配各 item
     * 的 iconSerial（send 时由 libdbus 分配、记在 SNIClient 上）——
     * 原实现用单个全局 pendingIconCall 且在 send 之前取 serial（恒为
     * 0），reply 永不命中；多 item 时又固定渲染到第一个 client。
     * 状态机：iconState 1=IconPixmap 应答（空/失败回退 IconName）、
     * 2=IconName 应答（字符串→XDG 图标主题目录查找→QImage 加载）。 */
    if (dbus_message_get_type(m) == DBUS_MESSAGE_TYPE_METHOD_RETURN ||
        dbus_message_get_type(m) == DBUS_MESSAGE_TYPE_ERROR) {
        dbus_uint32_t rserial = dbus_message_get_reply_serial(m);
        SNIClient *target = 0;
        for (SNIClient *cl = self->clients.first(); cl && !target;
             cl = self->clients.next())
            if (cl->iconSerial != 0 && cl->iconSerial == rserial)
                target = cl;
        if (!target)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        int state = target->iconState;
        target->iconSerial = 0;
        target->iconState = 0;
        /* error reply：IconPixmap 取不到（fcitx5 常态：无像素图标）→
           回退 IconName；IconName 也失败则放弃（保留空格子） */
        if (dbus_message_get_type(m) == DBUS_MESSAGE_TYPE_ERROR) {
            if (state == 1)
                self->requestIconName(target);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        /* 成功应答：Get 的返回是 variant，内含 a(iiay)（pixmap）或 s（name） */
        DBusMessageIter it, vit;
        dbus_message_iter_init(m, &it);
        if (dbus_message_iter_get_arg_type(&it) != DBUS_TYPE_VARIANT)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        dbus_message_iter_recurse(&it, &vit);
        int vt = dbus_message_iter_get_arg_type(&vit);
        if (state == 2) {
            /* IconName 应答：s → 图标主题目录查找 → 加载渲染 */
            if (vt != DBUS_TYPE_STRING)
                return DBUS_HANDLER_RESULT_HANDLED;
            const char *name = 0;
            dbus_message_iter_get_basic(&vit, &name);
            if (name && *name) {
                QString file = sniFindIconFile(name);
                if (!file.isEmpty()) {
                    QImage img(file);
                    if (!img.isNull())
                        self->renderQImage(target, img);
                }
            }
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        /* IconPixmap 应答：v=a(iiay) */
        if (vt != DBUS_TYPE_ARRAY ||
            dbus_message_iter_get_element_type(&vit) != DBUS_TYPE_STRUCT) {
            /* 空 pixmap（fcitx5 返回空数组的形态）→ 回退 IconName */
            self->requestIconName(target);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        DBusMessageIter arr, st;
        dbus_message_iter_recurse(&vit, &arr);
        dbus_message_iter_recurse(&arr, &st);
        dbus_int32_t iw = 0, ih = 0;
        if (dbus_message_iter_get_arg_type(&st) != DBUS_TYPE_INT32) {
            /* [KDE1 Revival 2026] 空 a(iiay) 数组（fcitx5 等无像素图标的
               item 常态）——recurse 后无元素即 INVALID，必须回退 IconName，
               原实现直接 return 导致托盘永远白格子 */
            self->requestIconName(target);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        dbus_message_iter_get_basic(&st, &iw);
        dbus_message_iter_next(&st);
        if (dbus_message_iter_get_arg_type(&st) != DBUS_TYPE_INT32)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        dbus_message_iter_get_basic(&st, &ih);
        dbus_message_iter_next(&st);
        if (dbus_message_iter_get_arg_type(&st) != DBUS_TYPE_ARRAY ||
            iw <= 0 || ih <= 0 || iw > 256 || ih > 256) {
            self->requestIconName(target);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
        DBusMessageIter bytes;
        dbus_message_iter_recurse(&st, &bytes);
        int need = iw * ih * 4;
        unsigned char *buf = (unsigned char *) malloc(need);
        if (!buf)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        memset(buf, 0, need);
        int got = 0;
        while (dbus_message_iter_get_arg_type(&bytes) == DBUS_TYPE_BYTE &&
               got < need) {
            unsigned char b;
            dbus_message_iter_get_basic(&bytes, &b);
            buf[got++] = b;
            dbus_message_iter_next(&bytes);
        }
        self->renderIcon(target, buf, iw, ih);
        free(buf);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    /* 客户端退出：DBus NameOwnerChanged（唯一名消失）→ 摘除 */
    if (dbus_message_is_signal(m, "org.freedesktop.DBus", "NameOwnerChanged")) {
        DBusMessageIter it;
        dbus_message_iter_init(m, &it);
        const char *name = 0, *old = 0, *newo = 0;
        if (dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &name);
        if (dbus_message_iter_next(&it) &&
            dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &old);
        if (dbus_message_iter_next(&it) &&
            dbus_message_iter_get_arg_type(&it) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&it, &newo);
        /* [2026-08-31] 放开 name[0]==':' 限定：item 可能以总线名注册
           （fcitx5 = org.fcitx.Fcitx5）也可能以唯一名+路径注册——退出时
           两条 NameOwnerChanged 都会到达，按 service 匹配哪条命中摘哪条 */
        if (name && old && newo &&
            strcmp(newo, "") == 0 && strcmp(old, "") != 0)
            self->handleItemUnregister(name);
        /* 信号继续传递，不做 HANDLED */
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void SNITray::handleItemRegister(const char *service, const char *path)
{
    /* 已注册则跳过 */
    if (clientByService(service))
        return;
    SNIClient *c = new SNIClient();
    c->service = service;
    c->objectPath = path;
    c->iconState = 0;
    Display *d = qt_xdisplay();
    int scr = DefaultScreen(d);
    int depth = DefaultDepth(d, scr); /* [2026-08-31] 深度随屏而非硬编码 24 */
    Window parent = dockArea->winId();
    c->win = XCreateSimpleWindow(d, parent, 0, 0, 24, 24, 0,
                                 BlackPixel(d, scr), WhitePixel(d, scr));
    XSelectInput(d, c->win, ExposureMask | ButtonPressMask);
    c->pix = XCreatePixmap(d, c->win, 24, 24, depth);
    XMapWindow(d, c->win);
    clients.append(c);
    emit clientsChanged();
    emitItemChange("StatusNotifierItemRegistered", service);

    /* 异步读 IconPixmap 属性。
     * [KDE1 Revival 2026] serial 必须经 dbus_connection_send 的出参取得——
     * send 之前 dbus_message_get_serial() 恒为 0（序号由总线侧分配），
     * 原实现把 0 记进全局字段导致 reply 永不匹配。 */
    DBusMessage *m = dbus_message_new_method_call(
        service, path, "org.freedesktop.DBus.Properties", "Get");
    if (m) {
        const char *iface = "org.kde.StatusNotifierItem";
        const char *prop = "IconPixmap";
        DBusMessageIter it;
        dbus_message_iter_init_append(m, &it);
        dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &iface);
        dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &prop);
        dbus_uint32_t serial = 0;
        dbus_connection_send(conn, m, &serial);
        c->iconSerial = serial;
        c->iconState = 1; /* 等 IconPixmap 应答（空/失败将回退 IconName） */
        dbus_message_unref(m);
        QTimer::singleShot(0, this, SLOT(slotFlush()));
    }
}

void SNITray::handleItemUnregister(const char *service)
{
    SNIClient *c = clientByService(service);
    if (!c)
        return;
    if (c->pix) XFreePixmap(qt_xdisplay(), c->pix);
    if (c->win) XDestroyWindow(qt_xdisplay(), c->win);
    /* 先取串再 remove——SNIClient 由清单托管删除，remove 后 c 悬垂 */
    QString svc = c->service;
    clients.remove(c);
    emit clientsChanged();
    emitItemChange("StatusNotifierItemUnregistered", svc.latin1());
}

/* [2026-08-31] 按规范向总线广播 item 注册/注销信号——其他 SNI 观察
   者（及部分 item 端）依赖它们同步状态 */
void SNITray::emitItemChange(const char *member, const char *service)
{
    if (!conn || !service || !*service)
        return;
    DBusMessage *sig = dbus_message_new_signal(
        "/StatusNotifierWatcher", "org.kde.StatusNotifierWatcher", member);
    if (!sig)
        return;
    dbus_message_append_args(sig, DBUS_TYPE_STRING, &service,
                             DBUS_TYPE_INVALID);
    dbus_connection_send(conn, sig, 0);
    dbus_message_unref(sig);
    QTimer::singleShot(0, this, SLOT(slotFlush()));
}

SNIClient *SNITray::clientByService(const QString &service)
{
    for (SNIClient *c = clients.first(); c; c = clients.next())
        if (c->service == service)
            return c;
    return 0;
}

/* SNI 图标像素序：a(iiay) 的字节流为大端 ARGB32（内存序 A,R,G,B），
   转 TQImage(0xAARRGGBB 小端 uint) 后交 renderQImage 绘制。 */
void SNITray::renderIcon(SNIClient *c, const unsigned char *argb,
                         int w, int h)
{
    if (!c || w <= 0 || h <= 0)
        return;
    QImage img(w, h, 32);
    for (int y = 0; y < h; y++) {
        unsigned int *dst = (unsigned int *) img.scanLine(y);
        const unsigned char *src = argb + (long) y * w * 4;
        for (int x = 0; x < w; x++, src += 4) {
            unsigned int a = src[0], r = src[1], g = src[2], b = src[3];
            dst[x] = ((a << 24) | (r << 16) | (g << 8) | b) & 0xffffffffu;
        }
    }
    renderQImage(c, img);
}

/* [KDE1 Revival 2026] 通用渲染出口：QImage（0xAARRGGBB 值语义，来源不限）
   → smoothScale 24x24 → XPutImage 画到后备位图 → 设为窗口背景位图 */
void SNITray::renderQImage(SNIClient *c, const QImage &image)
{
    if (!c || image.isNull())
        return;
    QImage scaled = image.smoothScale(24, 24);
    if (scaled.depth() != 32)
        scaled = scaled.convertDepth(32);

    Display *d = qt_xdisplay();
    int scr = DefaultScreen(d);
    int depth = DefaultDepth(d, scr); /* [2026-08-31] 深度随屏而非硬编码 24 */
    Pixmap newpix = XCreatePixmap(d, c->win, 24, 24, depth);
    XImage *xi = XCreateImage(d, DefaultVisual(d, scr), depth,
                              ZPixmap, 0, 0, 24, 24, 32, 0);
    if (xi) {
        char *xdata = (char *) malloc(xi->bytes_per_line * 24);
        if (!xdata) {
            xi->data = 0;
            XDestroyImage(xi);
            XFreePixmap(d, newpix);
            return;
        }
        xi->data = xdata;
        for (int y = 0; y < 24; y++) {
            unsigned int *dst = (unsigned int *) (xdata + y * xi->bytes_per_line);
            unsigned int *src = (unsigned int *) scaled.scanLine(y);
            for (int x = 0; x < 24; x++)
                dst[x] = src[x];
        }
        GC gc = XCreateGC(d, newpix, 0, 0);
        XPutImage(d, newpix, gc, xi, 0, 0, 0, 0, 24, 24);
        XFreeGC(d, gc);
        xi->data = 0;
        XDestroyImage(xi);
        if (c->pix)
            XFreePixmap(d, c->pix);
        c->pix = newpix;
        XSetWindowBackgroundPixmap(d, c->win, c->pix);
        XClearWindow(d, c->win);
    } else {
        XFreePixmap(d, newpix);
    }
}

/* ┌─ What : XDG 图标主题目录中按图标名查找图标文件（png/xpm）
// │  Why  : SNI item 的 IconPixmap 常为空（fcitx5 以主题图标名工作），
// │        必须按 icon theme spec 在标准目录树中解析 IconName
// │  Who  : messageFilter 的 IconName 应答分支
// │  When : IconPixmap 空/失败回退到 IconName 且应答到达时
// │  Where: kpanel 进程内；只读文件系统标准路径
// │  How  : 伪代码——
// │        1. 图标根目录依次尝试：$HOME/.icons、/usr/share/icons、
// │           /usr/local/share/icons、/usr/share/pixmaps
// │        2. 对 icon 根：主题固定查 hicolor（规范要求的 fallback 主题，
// │           现代发行版所有应用图标必装于此），遍历其下全部尺寸子目录
// │           （48x48、scalable 等），分类按 apps→status→places→devices
// │           →actions 顺序找 <name>.png / <name>.xpm（svg 不支持，
// │           TQt3 无 SVG 图像插件）
// │        3. pixmaps 根直接找 <name>.png / <name>.xpm
// │        4. 命中即返回绝对路径；全落空返回空串（上层保留空格子）
// │        注意尺寸不限：找到后一律 smoothScale 到 24x24
// */
// ┌─ What : XDG 图标主题目录中按图标名查找图标文件（png/xpm）
// │  Why  : SNI item 的 IconPixmap 常为空（fcitx5 以主题图标名工作，其
// │        IconName 为 input-keyboard-symbolic 一类的 symbolic 名），必须
// │        按 icon theme spec 在标准目录树解析出实际文件
// │  Who  : messageFilter 的 IconName 应答分支
// │  When : IconPixmap 空/失败回退到 IconName 且应答到达时
// │  Where: kpanel 进程内；只读文件系统标准路径
// │  How  : 伪代码——
// │        1. 图标根：$HOME/.icons、/usr/share/icons、/usr/local/share/icons
// │        2. 主题顺序：hicolor（规范 fallback 主题）优先，其余主题按名字序
// │        3. 每主题遍历尺寸目录（NNxNN 命名，取最大），分类 apps→status→
// │           places→devices→actions，找 <name>.png/.xpm（svg 无法被
// │           TQt3 加载故跳过）
// │        4. "-symbolic" 后缀剥离重试一遍（symbolic 单色变体缺失时用
// │           普通版，fcitx5 即此场景）
// │        5. /usr/share/pixmaps 兜底；全落空返回空串（上层保留空格子）
static QString sniFindIconInTheme(const QString &themedir, const char *name)
{
    static const char *groups[] = { "apps", "status", "places",
                                    "devices", "actions", 0 };
    static const char *exts[] = { ".png", ".xpm", 0 };
    QDir td(themedir);
    if (!td.exists())
        return QString::null;
    /* 尺寸目录取最大：收集 NNxNN 数字命名目录按尺寸排序 */
    QValueList<int> sizes;
    for (unsigned int i = 0; i < td.count(); i++) {
        QString sub = td[i];
        int w = sub.section('x', 0, 0).toInt();
        if (w > 0 && sub.section('x', 1, 1).toInt() == w)
            sizes.append(w);
    }
    qHeapSort(sizes);
    for (int s = sizes.size() - 1; s >= 0; s--) {
        QString sizedir = themedir + "/" + QString::number(sizes[s]) +
                          "x" + QString::number(sizes[s]);
        for (int g = 0; groups[g]; g++) {
            for (int e = 0; exts[e]; e++) {
                QString cand = sizedir + "/" + groups[g] + "/" +
                               name + exts[e];
                if (QFile::exists(cand))
                    return cand;
            }
        }
    }
    return QString::null;
}

static QString sniFindIconFile(const char *name)
{
    if (!name || !*name)
        return QString::null;
    QStringList roots;
    const char *home = getenv("HOME");
    if (home && *home)
        roots.append(QString(home) + "/.icons");
    roots.append("/usr/share/icons");
    roots.append("/usr/local/share/icons");

    /* 原名与剥 -symbolic 后缀两轮尝试 */
    QStringList candidates;
    candidates.append(name);
    int sl = strlen(name);
    if (sl > 9 && strcmp(name + sl - 9, "-symbolic") == 0)
        candidates.append(QString(name).left(sl - 9));

    for (QStringList::Iterator ci = candidates.begin();
         ci != candidates.end(); ++ci) {
        QByteArray nb = (*ci).utf8();
        const char *n = nb.data();
        /* hicolor 优先，随后遍历其他主题（Tango/Numix 等发行版主题常带
           input-keyboard 等通用图标而 hicolor 反而没有） */
        for (QStringList::Iterator ri = roots.begin();
             ri != roots.end(); ++ri) {
            QString r = sniFindIconInTheme(*ri + "/hicolor", n);
            if (!r.isNull())
                return r;
            QDir iconroot(*ri);
            QStringList themes = iconroot.entryList(QDir::Dirs);
            themes.sort();
            for (QStringList::Iterator ti = themes.begin();
                 ti != themes.end(); ++ti) {
                if (*ti == "." || *ti == ".." || *ti == "hicolor")
                    continue;
                r = sniFindIconInTheme(*ri + "/" + *ti, n);
                if (!r.isNull())
                    return r;
            }
        }
    }
    /* /usr/share/pixmaps 兜底 */
    static const char *pmexts[] = { ".png", ".xpm", 0 };
    for (int e = 0; pmexts[e]; e++) {
        QString cand = QString("/usr/share/pixmaps/") + name + pmexts[e];
        if (QFile::exists(cand))
            return cand;
    }
    return QString::null;
}

/* [KDE1 Revival 2026] IconName 属性异步请求（IconPixmap 失败/为空的回退） */
void SNITray::requestIconName(SNIClient *c)
{
    if (!c)
        return;
    DBusMessage *m = dbus_message_new_method_call(
        c->service.utf8(), c->objectPath.utf8(),
        "org.freedesktop.DBus.Properties", "Get");
    if (!m)
        return;
    const char *iface = "org.kde.StatusNotifierItem";
    const char *prop = "IconName";
    DBusMessageIter it;
    dbus_message_iter_init_append(m, &it);
    dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &iface);
    dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &prop);
    dbus_uint32_t serial = 0;
    dbus_connection_send(conn, m, &serial);
    c->iconSerial = serial;
    c->iconState = 2;
    dbus_message_unref(m);
    QTimer::singleShot(0, this, SLOT(slotFlush()));
}

/* [KDE1 Revival 2026] 按 X 窗口反查 item（点击转发用） */
SNIClient *SNITray::clientByWindow(Window w)
{
    for (SNIClient *c = clients.first(); c; c = clients.next())
        if (c->win == w)
            return c;
    return 0;
}

/* ┌─ What : 把托盘图标上的鼠标点击转成 SNI 交互方法发给 item
// │  Why  : SNI 规范要求 host 把按钮事件转为方法调用——原实现只显示
// │        不交互，fcitx5 图标左右键都无反应即缺此链路
// │  Who  : kpanel main.C 的 x11EventFilter（ButtonPress 分支）
// │  When : 用户点击 SNI 托盘窗口时
// │  How  : 左键→Activate(x,y)；右键→ContextMenu(x,y)（fcitx5 据此弹
// │        自己的 DBusMenu 菜单）；其余（中键等）→SecondaryActivate(x,y)；
// │        坐标取 root 系（SNI 规范要求屏幕坐标）；发完即忘，不等待应答
// */
void SNITray::sendClick(SNIClient *c, int button, int x_root, int y_root)
{
    if (!c || !conn)
        return;
    const char *method = (button == 1) ? "Activate"
                        : (button == 3) ? "ContextMenu"
                                        : "SecondaryActivate";
    DBusMessage *m = dbus_message_new_method_call(
        c->service.utf8(), c->objectPath.utf8(),
        "org.kde.StatusNotifierItem", method);
    if (!m)
        return;
    dbus_int32_t xi = x_root, yi = y_root;
    dbus_message_append_args(m, DBUS_TYPE_INT32, &xi,
                                DBUS_TYPE_INT32, &yi,
                                DBUS_TYPE_INVALID);
    dbus_connection_send(conn, m, 0);
    dbus_message_unref(m);
    QTimer::singleShot(0, this, SLOT(slotFlush()));
}

#include "snitray.moc"

