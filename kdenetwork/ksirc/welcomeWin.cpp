/**********************************************************************

	--- Qt Architect generated file ---

	File: welcomeWin.cpp
	Last generated: Sat Oct 24 23:35:42 1998

 *********************************************************************/

#include "welcomeWin.h"
#include <qfile.h>
#include <qtextstream.h>

#include "config.h"
#include "irclistitem.h"

extern global_config *kSircConfig; 

#define Inherited welcomeWinData

welcomeWin::welcomeWin
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  DisplayDialog->show();
  setCaption( i18n("Welcome to kSirc") );
  QFile f(kapp->kde_datadir() + "/ksirc/relnotes");
  if(f.open(IO_ReadOnly)){
    QTextStream t(&f);
    QString s;
    while(!t.eof()){
      s = t.readLine();
      DisplayDialog->insertItem(new  ircListItem(s, &black, DisplayDialog));
    }
    f.close();
  }
  else{
    /* [2026-08-31] 硬编码英文改 i18n（资源文件现已随包安装，此为兜底提示） */
    DisplayDialog->insertItem(new  ircListItem(i18n("Unable to open release notes file!!"), &red, DisplayDialog));
  }
  DisplayDialog->updateScrollBars();
}

welcomeWin::~welcomeWin()
{
}

void welcomeWin::dismiss()
{
  if(ShowAgain->isChecked())
    reject();
  else
    accept();
}

void welcomeWin::show()
{
  Inherited::show();
//  DisplayDialog->setTopItem(0);
}

#include "welcomeWin.moc"

