/*************************************************************************

 Main KSirc start 

 $$Id$$

 Main start file that defines 3 global vars, etc

 Does nothing to special, start create new  KApplcaiton (incorrectly,
 btw, should use the code Kalle posted so it can be session managment
 restarted, etc) loads colours then fires off the main widget.  On
 exit it calls a sync so the kConfig get written.  (Is this a bug?)

*************************************************************************/

/*
 * Needed items
 * 4. Send a /quit and/or kill dsirc on exit
 * */


#include "servercontroller.h"
#include "welcomeWin.h"

#include <iostream>
using std::cerr;  /* TQt3 迁移:老 C++ 头改标准头 */
#include <time.h>
#include <unistd.h>

#include <qfont.h>
#include <qmessagebox.h> 

#include <kapp.h>
#include <kconfig.h>

#include "config.h"
#include "version.h"
/* TQt3 迁移:cdate.h 为当年构建系统生成的编译日期头(定义 COMPILE_DATE),
 * 改用编译器内建宏等价 */
#define COMPILE_DATE __DATE__

KApplication *kApp;
KConfig *kConfig;
global_config *kSircConfig;
//QDict<KSircTopLevel> TopList;
//QDict<KSircMessageReceiver> TopList;

int main( int argc, char ** argv )
{
  // Start the KDE application
  kApp = new  KApplication( argc, argv, QString("ksirc") );

  kConfig = kApp->getConfig();

  /* TQt3 迁移(2026):原版对无版本号的开发版实施 8 周过期自杀(1999 年的
   * Alpha 调试机制),2026 年重建必然触发;KDE1 Revival 为历史保存项目,
   * 该时限已无意义,整体禁用(ksirc.po 中两条过期文案随之不再上屏) */
#if 0
  QString ver = KSIRC_VERSION;
  if(ver.contains(".") == 0){ /* ... expired-check disabled ... */ }
#endif


  // Get config, and setup internal structure.

  kSircConfig = new  global_config;

  kConfig->setGroup("Colours");
  kSircConfig->colour_text = new  QColor(kConfig->readColorEntry("text", &k1c_black));
  kSircConfig->colour_info = new  QColor(kConfig->readColorEntry("info", &k1c_blue));
  kSircConfig->colour_chan = new  QColor(kConfig->readColorEntry("chan", &k1c_green));
  kSircConfig->colour_error = new  QColor(kConfig->readColorEntry("error", &k1c_red));
  if(kSircConfig->colour_text == 0x0)
      kSircConfig->colour_text = new  QColor("black");
  if(kSircConfig->colour_info == 0x0)
      kSircConfig->colour_info = new  QColor("blue");
  if(kSircConfig->colour_chan == 0x0)
      kSircConfig->colour_chan = new  QColor("green");
  if(kSircConfig->colour_error == 0x0)
      kSircConfig->colour_error = new  QColor("red");
  

  kSircConfig->colour_background = 0;
  kSircConfig->filterKColour = kConfig->readNumEntry("kcolour", false);
  kSircConfig->filterMColour = kConfig->readNumEntry("mcolour", false);
  kSircConfig->nickFHighlight = kConfig->readNumEntry("nickfcolour", -1);
  kSircConfig->nickBHighlight = kConfig->readNumEntry("nickbcolour", -1);
  kSircConfig->usHighlight = kConfig->readNumEntry("uscolour", -1);
  kSircConfig->transparent = kConfig->readNumEntry("Transparent", false);

  QString ld_path = getenv("LD_LIBRARY_PATH");
  ld_path += ":" + kapp->kde_datadir() + "/ksirc/:";
  ld_path.prepend("LD_LIBRARY_PATH=");
  putenv(ld_path.data());

  kConfig->setGroup("GlobalOptions");
  kSircConfig->defaultfont = kConfig->readFontEntry("MainFont", new  QFont("")); /* [2026-08-31] 默认族：原硬编码西文字族无 CJK 字形，中文渲染 tofu */
  kConfig->setGroup("General");
  kSircConfig->DisplayMode = kConfig->readNumEntry("DisplayMode", 0);
  kSircConfig->WindowLength = kConfig->readNumEntry("WindowLength", 200);
  kSircConfig->transparent = kConfig->readNumEntry("transparent", 0);
  kSircConfig->BackgroundPix = kConfig->readNumEntry("BackgroundPix", 0);
  kSircConfig->BackgroundFile = kConfig->readEntry("BackgroundFile", "");

  kConfig->setGroup("ReleaseNotes");
  if(kConfig->readNumEntry("LastRunRelease", 0) < COMPILE_DATE){
    welcomeWin ww;
    if(ww.exec())
      kConfig->writeEntry("LastRunRelease", COMPILE_DATE);
  }

  if(kApp->isRestored()){
    int n = 1;
    servercontroller *sc = new  servercontroller(0, "servercontroller");
    CHECK_PTR(sc);
    kApp->setMainWidget(sc);
    while (servercontroller::canBeRestored(n)) {
      sc->restore(n);
      n++;
    }
  }
  else{
    servercontroller *control = new  servercontroller(0, "servercontroller");
    control->show();
    kApp->setMainWidget(control);
  }
  
  try {
      kApp->exec();
  }
  catch(...){
      cerr << "Caught Unkown Exception, uhoh!!!\n";
      cerr << "Dying!!!\n";
      exit(10);
  }

  kConfig->sync();
}















