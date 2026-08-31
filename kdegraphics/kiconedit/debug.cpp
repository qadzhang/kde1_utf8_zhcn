
#include "debug.h"
#include <kmsgbox.h>
#include <kapp.h>

/* ────────────────────────────────────────────────────────────────────
 * [KDE1 Revival 2026-08-31] 警告不再弹模态框
 * What : TQtWarningMsg 只写 stderr，去掉 KMsgBox 模态弹窗
 * Why  : 启动期 KApplication::readSettings→KCharset::setQFont 在
 *        UTF-8/TQt3 环境下必然产生 "KCharset: Wrong charset!" 警告
 *        （KDE1 字符集层是残留物）——1999 年把每条警告弹成模态框的
 *        调试辅助直接导致 kiconedit1 启动即被警告框卡死、主窗口
 *        永不出现。警告属诊断信息，归 stderr；致命错误保留弹窗+abort
 * Who  : main.cpp 的 qInstallMsgHandler(myMessageOutput) 注册本函数
 * When : TQt3 全局消息处理器的每次回调
 * ──────────────────────────────────────────────────────────────────── */
void myMessageOutput( TQtMsgType type, const char *msg )
{
  switch ( type )
  {
     case TQtDebugMsg:
#ifdef DEBUG
       fprintf( stderr, "Debug: %s\n", msg );
#endif
       break;
     case TQtWarningMsg:
       fprintf( stderr, "Warning: %s\n", msg );
       break;
     case TQtFatalMsg:
       KMsgBox::message(0, i18n("Fatal!"), msg );
       fprintf( stderr, "Fatal: %s\n", msg );
       abort();
       // dump core on purpose
  }
}


