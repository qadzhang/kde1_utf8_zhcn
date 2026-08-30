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

struct SNIClient {
    QString service;      // DBus 总线唯一名（:1.x）
    QString objectPath;   // item 对象路径
    Window   win;         // kpanel 自建的 24x24 内容窗（dock_area 子窗）
    Pixmap   pix;         // 内容后备位图
    GC       gc;
    // [KDE1 Revival 2026] 本 item 的 IconPixmap Get 调用序号——dbus 侧
    // 在 send 时分配，reply 以之路由回对应 item（多 item 并发取图不串线）
    dbus_uint32_t iconSerial;
};

class SNITray : public QObject
{
    Q_OBJECT
public:
    SNITray(QWidget *dockArea, QObject *parent = 0);
    ~SNITray();

    bool init();                    // 连接 DBus、请求名、注册回调
    void relayout();                // 与 kPanel::layoutDockArea 联动（数量回调）

    QPtrList<SNIClient> clients;
    int count() const { return clients.count(); }

signals:
    void clientsChanged();

public slots:
    void slotDispatch();
private:
    bool registerWatcher();
    void handleItemRegister(const char *service, const char *path);
    void handleItemUnregister(const char *service);
    void fetchIcon(SNIClient *c);   // 异步读 IconPixmap（Get 属性回调绘制）
    void renderIcon(SNIClient *c, const unsigned char *argb,
                    int w, int h);
    static DBusHandlerResult messageFilter(DBusConnection *, DBusMessage *, void *);
    SNIClient *clientByService(const QString &service);

    QWidget  *dockArea;
    DBusConnection *conn;
    QSocketNotifier *notifier;
    unsigned long lastStamp;
};
#endif
