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
#include "snitray.h"

#include <qsocketnotifier.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

#include <X11/Xlib.h>

#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
#include <X11/Xutil.h>
}

SNITray::SNITray(QWidget *da, QObject *parent)
  : QObject(parent), dockArea(da), conn(0), notifier(0), lastStamp(0)
{
    clients.setAutoDelete(true);
}

SNITray::~SNITray()
{
    if (conn)
        dbus_connection_unref(conn);
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
    const char *host_sig = "org.kde.StatusNotifierHostRegistered";
    dbus_bus_add_match(conn,
        "type='signal',interface='org.kde.StatusNotifierWatcher',member="
        "StatusNotifierHostRegistered", &err);
    dbus_error_free(&err);

    dbus_connection_add_filter(conn, messageFilter, this, 0);

    /* DBus fd 挂进 TQt3 事件循环 */
    int fd = -1;
    if (dbus_connection_get_unix_fd(conn, &fd) && fd >= 0) {
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, SIGNAL(activated(int)), this, SLOT(slotDispatch()));
    }

    /* 对象路径 /StatusNotifierWatcher 的方法由 messageFilter 的
       method_call 分支处理（Register/Unregister/Get） */

    dbus_connection_flush(conn);
    fprintf(stderr, "kpanel SNI: watcher online\n");
    return true;
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
            /* 参数可能为总线名或完整对象路径——两者都存 */
            char svc[256] = {0}, path[256] = "/";
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
            dbus_message_iter_open_container(&rit, DBUS_TYPE_VARIANT, 0, &vit);
            if (strcmp(prop, "IsStatusNotifierHostAllowed") == 0) {
                dbus_bool_t v = TRUE;
                dbus_message_iter_append_basic(&vit, DBUS_TYPE_BOOLEAN, &v);
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

    /* item 的 Pixmaps 属性应答 → 渲染 */
    if (dbus_message_get_type(m) == DBUS_MESSAGE_TYPE_METHOD_RETURN &&
        self->pendingIconCall == dbus_message_get_reply_serial(m)) {
        /* 解析 v=a(iiay)：宽 高 array-of-bytes(ARGB) */
        DBusMessageIter it, vit, arr, st;
        dbus_message_iter_init(m, &it);
        if (dbus_message_iter_get_arg_type(&it) != DBUS_TYPE_VARIANT)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        dbus_message_iter_recurse(&it, &arr);
        if (dbus_message_iter_get_element_type(&arr) != DBUS_TYPE_STRUCT)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        dbus_message_iter_recurse(&arr, &st);
        dbus_int32_t iw = 0, ih = 0;
        if (dbus_message_iter_get_arg_type(&st) != DBUS_TYPE_INT32)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        dbus_message_iter_get_basic(&st, &iw);
        dbus_message_iter_next(&st);
        if (dbus_message_iter_get_arg_type(&st) != DBUS_TYPE_INT32)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        dbus_message_iter_get_basic(&st, &ih);
        dbus_message_iter_next(&st);
        if (dbus_message_iter_get_arg_type(&st) != DBUS_TYPE_ARRAY ||
            iw <= 0 || ih <= 0 || iw > 256 || ih > 256)
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        DBusMessageIter bytes;
        dbus_message_iter_recurse(&st, &bytes);
        int need = iw * ih * 4;
        unsigned char *buf = (unsigned char *) malloc(need);
        memset(buf, 0, need);
        int got = 0;
        while (dbus_message_iter_get_arg_type(&bytes) == DBUS_TYPE_BYTE &&
               got < need) {
            unsigned char b;
            dbus_message_iter_get_basic(&bytes, &b);
            buf[got++] = b;
            dbus_message_iter_next(&bytes);
        }
        /* 找到对应 item 并绘制 */
        for (SNIClient *cl = self->clients.first(); cl; cl = self->clients.next())
            if (dbus_message_get_reply_serial(m)) { /* 单 item 场景简化 */
                self->renderIcon(cl, buf, iw, ih);
                break;
            }
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
        if (name && old && newo && name[0] == ':' &&
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
    Display *d = qt_xdisplay();
    Window parent = dockArea->winId();
    c->win = XCreateSimpleWindow(d, parent, 0, 0, 24, 24, 0,
                                 BlackPixel(d, DefaultScreen(d)),
                                 WhitePixel(d, DefaultScreen(d)));
    XSelectInput(d, c->win, ExposureMask | ButtonPressMask);
    c->pix = XCreatePixmap(d, c->win, 24, 24, 24);
    XMapWindow(d, c->win);
    clients.append(c);
    emit clientsChanged();

    /* 异步读 IconPixmap 属性 */
    DBusMessage *m = dbus_message_new_method_call(
        service, path, "org.freedesktop.DBus.Properties", "Get");
    if (m) {
        const char *iface = "org.kde.StatusNotifierItem";
        const char *prop = "IconPixmap";
        DBusMessageIter it;
        dbus_message_iter_init_append(m, &it);
        dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &iface);
        dbus_message_iter_append_basic(&it, DBUS_TYPE_STRING, &prop);
        pendingIconCall = dbus_message_get_serial(m);
        dbus_connection_send(conn, m, 0);
        dbus_message_unref(m);
        dbus_connection_flush(conn);
    }
}

void SNITray::handleItemUnregister(const char *service)
{
    SNIClient *c = clientByService(service);
    if (!c)
        return;
    if (c->pix) XFreePixmap(qt_xdisplay(), c->pix);
    if (c->win) XDestroyWindow(qt_xdisplay(), c->win);
    clients.remove(c);
    emit clientsChanged();
}

SNIClient *SNITray::clientByService(const QString &service)
{
    for (SNIClient *c = clients.first(); c; c = clients.next())
        if (c->service == service)
            return c;
    return 0;
}

/* SNI 图标像素序：a(iiay) 的字节流为大端 ARGB32（内存序 A,R,G,B），
   转 TQImage(0xAARRGGBB 小端 uint) 后 smoothScale 到 24x24 经 XPutImage
   画到后备位图，设为窗口背景位图触发显示。 */
void SNITray::renderIcon(SNIClient *c, const unsigned char *argb,
                         int w, int h)
{
    if (!c || w <= 0 || h <= 0)
        return;
    Display *d = qt_xdisplay();
    QImage img(w, h, 32);
    for (int y = 0; y < h; y++) {
        unsigned int *dst = (unsigned int *) img.scanLine(y);
        const unsigned char *src = argb + (long) y * w * 4;
        for (int x = 0; x < w; x++, src += 4) {
            unsigned int a = src[0], r = src[1], g = src[2], b = src[3];
            dst[x] = ((a << 24) | (r << 16) | (g << 8) | b) & 0xffffffffu;
        }
    }
    QImage scaled = img.smoothScale(24, 24);

    Pixmap newpix = XCreatePixmap(d, c->win, 24, 24, 24);
    XImage *xi = XCreateImage(d, DefaultVisual(d, DefaultScreen(d)), 24,
                              ZPixmap, 0, 0, 24, 24, 32, 0);
    if (xi) {
        char *xdata = (char *) malloc(xi->bytes_per_line * 24);
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

#include "snitray.moc"

