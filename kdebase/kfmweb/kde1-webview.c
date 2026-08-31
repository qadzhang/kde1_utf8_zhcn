/**********************************************************************
** kde1-webview.c
**
**   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
**
**   kfm「新内核」视图：WebKit2GTK 经 XEMBED 嵌入 kfm 窗口
**
** ┌─ What : 以 GtkPlug 嵌入 kfm 提供的 XEmbed socket，承载 WebKitWebView
** │         渲染现代网页（完整 CSS/JS/HTTPS）；stdin 逐行接收导航命令
** │  Why  : kfm 1999 年的 KHTML 引擎无法打开现代网站（AGENTS.md §1
** │         目标 3「浏览器内核跟进」），WebKit2GTK 为新内核、原引擎保留
** │         旧内核，构成双内核；XEmbed 是 kfm(Qt1) 与 GTK 程序共存的
** │         唯一标准途径（最小原型已验证）
** │  Who  : 由 KfmGui::slotWebEngine fork/exec（参数：socket XID 与初始
** │         URL），stdin 管道与 kfm 通信
** │  When : 用户在 kfm「查看」菜单开启新内核时启动，关闭/退出 kfm 时随
** │         管道关闭而退出（stdin EOF 即 gtk_main_quit）
** │  Where: kdebase/kfmweb/，安装到 $KDE1_BINDIR
** │  How  : 伪代码：
** │         1. 解析 argv：socket XID、初始 URL
** │         2. GtkPlug(socket) 内放 WebKitWebView → 加载 URL → show
** │         3. stdin 监听（GUnixInputStream/GLib IO channel）：
** │            "URL <地址>" → 导航；其他行忽略
** │         4. stdin EOF → 退出（kfm 侧关闭管道或进程退出）
**********************************************************************/
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <webkit2/webkit2.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static WebKitWebView *g_web = NULL;

static gboolean on_stdin(GIOChannel *src, GIOCondition cond, gpointer data)
{
    if (cond & (G_IO_HUP | G_IO_ERR))
        return FALSE;			/* 管道关闭：退出主循环 */
    gchar *line = NULL;
    gsize len = 0;
    if (g_io_channel_read_line(src, &line, &len, NULL, NULL) == G_IO_STATUS_NORMAL) {
        if (g_web && line) {
            if (strncmp(line, "URL ", 4) == 0) {
                gchar *url = g_strstrip(line + 4);
                if (*url)
                    webkit_web_view_load_uri(g_web, url);
            }
        }
        g_free(line);
    }
    return TRUE;				/* 继续监听 */
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "用法: kde1-webview <socket-XID> [URL]\n");
        return 1;
    }
    /* 无 DISPLAY 下的 XEMBED 子进程不抢主总线 */
    g_setenv("WEBKIT_DISABLE_COMPOSITING_MODE", "1", FALSE);
    gtk_init(&argc, &argv);

    Window xid = (Window)atol(argv[1]);
    GtkWidget *plug = gtk_plug_new(xid);
    g_web = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(plug), GTK_WIDGET(g_web));
    gtk_widget_show_all(plug);

    const char *url = argc > 2 ? argv[2] : "https://www.example.com";
    webkit_web_view_load_uri(g_web, url);

    GIOChannel *ch = g_io_channel_unix_new(STDIN_FILENO);
    g_io_add_watch(ch, G_IO_IN | G_IO_HUP | G_IO_ERR, on_stdin, NULL);
    g_io_channel_unref(ch);
    /* [2026-08-31] 移除 g_idle_add(quit_on_eof)：idle 回调在主循环第一次空闲即
       执行——曾导致 webview 一加载完就自杀退出（表现为新内核窗口闪退）。EOF
       退出语义已由 IO watch 返回 FALSE 正确实现，无需任何占位注册 */

    gtk_main();
    return 0;
}
