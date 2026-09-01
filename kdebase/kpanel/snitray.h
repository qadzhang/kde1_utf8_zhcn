/*
 * snitray.h -- StatusNotifierItem (SNI) 系统托盘 host 实现
 *
 *   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
 *
 * ┌─ What : 实现 freedesktop StatusNotifier 托盘协议的 host/watcher 角色，
 * │        让 fcitx5 5.x、Electron 应用（钉钉）等默认走 SNI 的现代程序
 * │        的托盘图标显示在 kpanel 指示栏
 * │  Why  : AGENTS.md §6.7 —— KDE1 主动适配现代软件生态；fcitx5 5.x 默认
 * │        托盘路径是 SNI（DBus），KDE1 原生只有 1999 年的 KWM_DOCKWINDOW
 * │  How  : libdbus C API。三个角色：
 * │        ① 请求总线名 org.kde.StatusNotifierWatcher（watcher）
 * │        ② 实现 /StatusNotifierWatcher 的 Register/Unregister 方法与
 * │          StatusNotifierItemRegistered/Unregistered 信号
 * │        ③ 每个 item 经 DBus 读其 org.kde.StatusNotifierItem 接口的
 * │          IconPixmap 属性（a(iiay)：宽高ARGB行序）转 QImage 绘制到
 * │          kpanel 自建的 24x24 X 窗口（每 item 一个）
 * │        DBus fd 经 QSocketNotifier 挂进 TQt3 事件循环
 */
#ifndef SNITRAY_H
#define SNITRAY_H

#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>
#include <X11/Xlib.h>
#include <dbus/dbus.h>

class QSocketNotifier;
class QWidget;
class TQPopupMenu;
class TQPoint;

struct SNIClient {
    QString service;      // DBus 总线唯一名（:1.x）
    QString objectPath;   // item 对象路径
    Window   win;         // kpanel 自建的 24x24 内容窗（dock_area 子窗）
    Pixmap   pix;         // 内容后备位图
    // [KDE1 Revival 2026] 本 item 的 IconPixmap Get 调用序号——dbus 侧
    // 在 send 时分配，reply 以之路由回对应 item（多 item 并发取图不串线）
    dbus_uint32_t iconSerial;
    // [KDE1 Revival 2026] 图标获取状态机：0=空闲；1=已发 IconPixmap 的
    // Get 等应答；2=IconPixmap 空/失败后改发 IconName 等应答。
    // fcitx5 等以主题图标名工作的 item 其 IconPixmap 为空数组，必须走
    // IconName → XDG 图标主题目录查找的回退链，否则图标永远空白
    int iconState;
};

class SNITray : public QObject
{
    Q_OBJECT
public:
    SNITray(QWidget *dockArea, QObject *parent = 0);
    ~SNITray();

    bool init();                    // 连接 DBus、请求名、注册回调
    /* [2026-08-31] 声明收敛：删除从未实现的 relayout()/registerWatcher()/
       fetchIcon() 与从未使用的 lastStamp/SNIClient::gc——头文件承诺与
       实现一致（kPanel 经 clientsChanged 信号联动重排，无需 relayout） */

    QPtrList<SNIClient> clients;
    int count() const { return clients.count(); }

    // [KDE1 Revival 2026] 点击转发：按 X 窗口找 item / 发送 SNI 交互方法
    SNIClient *clientByWindow(Window w);
    void sendClick(SNIClient *c, int button, int x_root, int y_root);

signals:
    void clientsChanged();

public slots:
    void slotDispatch();
    /* [KDE1 Revival 2026] DBus 写缓冲延迟冲刷——filter 栈内同步 flush 与
       dispatch 循环死锁，统一经 zero-timer 转移到事件循环 */
    void slotFlush();
    /* [KDE1 Revival 2026, 2026-09-01] 右键菜单（DBusMenu 代理）：fcitx5 等
       SNI item 的右键菜单走 com.canonical.dbusmenu 协议，须 host 侧拉取
       GetLayout 渲染成 QPopupMenu 并回发点击事件——原实现只调
       ContextMenu 方法，此类 item 毫无反应（托盘"看得见点不着"的另一半） */
    void slotOpenMenu();
    /* 代理菜单选中 → DBusMenu Event(clicked) 回发 */
    void slotMenuActivated(int id);
private:
    void handleItemRegister(const char *service, const char *path);
    void handleItemUnregister(const char *service);
    void emitItemChange(const char *member, const char *service);
    void renderIcon(SNIClient *c, const unsigned char *argb,
                    int w, int h);
    void renderQImage(SNIClient *c, const QImage &image); // 通用渲染出口
    void requestIconName(SNIClient *c);  // IconPixmap 失败/为空时的回退链
    static DBusHandlerResult messageFilter(DBusConnection *, DBusMessage *, void *);
    SNIClient *clientByService(const QString &service);
    /* [2026-09-01] DBusMenu 代理 internals */
    int  menuFill(TQPopupMenu *pop, const char *service, const char *menupath,
                  DBusMessageIter *layoutIter, int depth);
    void menuEvent(const char *service, const char *menupath,
                   int itemid, const char *eventId);
    SNIClient *menuPending;   // 待开菜单的 item（slotOpenMenu 消费后清空）
    int menuPendingX, menuPendingY;
    TQPopupMenu *menuActive;  // 正在显示的代理菜单（activated 回发事件）
    QString menuService;      // 当前代理菜单对应的 item 总线名/菜单路径
    QString menuPath;

    QWidget  *dockArea;
    DBusConnection *conn;
    QSocketNotifier *notifier;
};
#endif
