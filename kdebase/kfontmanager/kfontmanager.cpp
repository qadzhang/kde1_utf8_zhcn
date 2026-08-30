    /*

    $Id: kfontmanager.cpp,v 1.14 1998/12/13 03:54:29 pbrown Exp $

    Requires the Qt widget libraries, available at no cost at 
    http://www.troll.no
       
    Copyright (C) 1997 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */
 

#include <qstrlist.h>
#include <qfontdatabase.h>
#include <qcstring.h> // [KDE1 Revival 2026] fontconfig 字体枚举（BUG5） 
#include <qfile.h>
#include <qtstream.h> 
#include <qtabdlg.h>


#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <kapp.h>
#include <kcharsets.h>
#include "kfontmanager.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


extern char PICS_PATH[256];

#define HEIGHT 290
#define WIDTH 400

#include <klocale.h>

KFontManager::KFontManager (QWidget * parent, const char *name)
    : QDialog (parent, name)
{


  availableLabel = new QLabel(i18n("Available Fonts"), this,"availlabel");

  availableFontsList = new QListBox(this,"avalableFonts");
  
  connect(availableFontsList,SIGNAL(highlighted(int)),
	  this,SLOT(display_available_example(int)));
  
  selectedLabel = new QLabel(i18n("Fonts made available to KDE"), this,"selectlabel");
  selectedFontsList = new QListBox(this,"avalableFonts");

  connect(selectedFontsList,SIGNAL(highlighted(int)),
	  this,SLOT(display_selected_example(int)));

  add = new QPushButton(i18n("Add"), this);
  connect(add,SIGNAL(clicked()),this,SLOT(add_slot()));

  remove = new QPushButton(i18n("Remove"), this);
  connect(remove,SIGNAL(clicked()),this,SLOT(remove_slot()));

  help = new QPushButton(i18n("Help"), this);
  connect(help,SIGNAL(clicked()),SLOT(helpselected()));

  example_label = new QLabel(this,"examples");
  
  example_label->setAlignment(AlignCenter);
//  example_label->setBackgroundColor(white);
  example_label->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
//  example_label->setText(i18n("The KDE Font Manager Example String"));
  // the following example string is better; is uses all letters in alphabet
  example_label->setText(i18n("The Quick Brown Fox Jumps Over The Lazy Dog\n"
			      "AaBbCc123"));

  connect( parent, SIGNAL( applyButtonPressed() ),
	   this, SLOT( writeKDEInstalledFonts() ) );  	  

  resize(parent->width(),parent->height());

  readSettings();
  queryFonts();

  setColors();
  connect(KApplication::getKApplication(),SIGNAL(kdisplayPaletteChanged()),
	  this,SLOT(setColors()));

  setMinimumSize (100, 100);

}



void KFontManager::about(){

  QMessageBox::message (i18n("About kfontmanager"), i18n("kfontmanager Version 0.2\n"\
			"Copyright 1997\nBernd Johannes Wuebben\n"\
			"wuebben@math.cornell.edu\n"),i18n("OK"));

}

void KFontManager::remove_slot(){

  if(selectedFontsList->currentItem() != -1){
    selectedFontsList->removeItem(selectedFontsList->currentItem());
  }
  else{
    QApplication::beep();
  }

}

void KFontManager::add_slot(){
  
  if(availableFontsList->currentItem() != -1){

    QString new_item = availableFontsList->text(availableFontsList->currentItem());
    QString string;

    for (uint i  = 0; i < selectedFontsList->count();i++){
    
      string = "";
      string = selectedFontsList->text(i);
      if ( string == new_item){
	// already in KDE font list
         QApplication::beep();
	 return;
      }
    }
    selectedFontsList->inSort(new_item);


  }
  else{
    QApplication::beep();
  }
}



void KFontManager::resizeEvent(QResizeEvent *e){

  (void) e;

  availableLabel->setGeometry (15,10,180,25);
  selectedLabel->setGeometry (width()/2 + 5,10,180,25);
  availableFontsList->setGeometry(10,35,(width()-20)/2 -10 ,height()-150);
  selectedFontsList->setGeometry((width()-20)/2 +5,35,(width()-20)/2+5 ,height()-150);
  example_label->setGeometry(20,height() - 100 ,width() - 40, 40);

  add->setGeometry(( width()-20) - 190 , height() - 50, 90, 30);
  remove->setGeometry((width()-20) -90, height() - 50, 90, 30);
  help->setGeometry(20, height() - 50, 90, 30);

}

void KFontManager::helpselected(){

  kapp->invokeHTMLHelp( "kfontmanager/index.html", "" );

}

void KFontManager::apply(bool){

}


void KFontManager::display_available_example(int i){

  QString string;

  string = availableFontsList->text(i);
  QFont fnt=QFont(string,14);
  kapp->getCharsets()->setQFont(fnt);
  example_label->setFont(fnt);
  
}


void KFontManager::display_selected_example(int i){

  QString string;

  string = selectedFontsList->text(i);
  QFont fnt=QFont(string,14);
  kapp->getCharsets()->setQFont(fnt);
  example_label->setFont(fnt);
  
}

bool KFontManager::loadKDEInstalledFonts(){

  QString fontfilename;

  //TODO replace by QDir::homePath();

  fontfilename =  getenv("HOME");
  if(fontfilename.isEmpty()){
    QMessageBox::message(i18n("Sorry"),i18n("The environment variable HOME\n"\
			 "is not set\n"),i18n("OK"));
    QApplication::exit(1);

  }
    
  fontfilename = fontfilename + "/.kde/share/config/kdefonts";

  QString home;
  home = getenv("HOME");
  home = home + "/.kde";
  struct stat buf;

  if( stat(home.data(),&buf) == -1 ){
    mkdir(home.data(),S_IRUSR | S_IWUSR | S_IXUSR |S_IRGRP | S_IWGRP | S_IXGRP | 
	  S_IROTH | S_IWOTH |S_IXOTH);
  }

  home = home + "/share";
  
  if( stat(home.data(),&buf) == -1 ){
    mkdir(home.data(),S_IRUSR | S_IWUSR | S_IXUSR |S_IRGRP | S_IWGRP | S_IXGRP | 
	  S_IROTH | S_IWOTH |S_IXOTH);
  }

  home = home + "/config";
  
  if( stat(home.data(),&buf) == -1 ){
    mkdir(home.data(),S_IRUSR | S_IWUSR | S_IXUSR |S_IRGRP | S_IWGRP | S_IXGRP | 
	  S_IROTH | S_IWOTH |S_IXOTH);
  }


  QFile fontfile(fontfilename);

  if (!fontfile.exists())
    return false;

  if(!fontfile.open(IO_ReadOnly)){
    return false;
  }

  if (!fontfile.isReadable())
    return false;
  
  selectedFontsList->setAutoUpdate(FALSE);
  
  QTextStream t(&fontfile);


  while ( !t.eof() ) {
    QString s = t.readLine();
    s = s.stripWhiteSpace();
    if (!s.isEmpty())
      selectedFontsList->insertItem( s );
  }

  fontfile.close();

  selectedFontsList->setAutoUpdate(TRUE);
   selectedFontsList->update();

  /* [KDE1 Revival 2026] 空文件（或全空行）视为
     “无自定义列表”：0 字节 kdefonts 会使
     exists() 为真而误判“已安装”，把 fontconfig
     全集整个挡掉（实测即此）。未读到
     有效条目时返回 false，让 fontconfig 路径
     正常填充两个列表。 */
  if (selectedFontsList->count() == 0)
    return false;

  return true;

}

bool KFontManager::writeKDEInstalledFonts(){
  

  QString fontfilename;

  fontfilename =  getenv("HOME");
  fontfilename = fontfilename + "/.kde/share/config/kdefonts";

  QFile fontfile(fontfilename);

  if (!fontfile.open(IO_WriteOnly | IO_Truncate)){
    QMessageBox::message(i18n("Sorry"),i18n("Can not create:\n ~/.kde/share/config/kdefonts\n"),i18n("OK"));
    return false;
  }

  if (!fontfile.isWritable()){
    QMessageBox::message(i18n("Sorry"),i18n("~/.kde/share/config/kdefonts exists but\n"\
			 "is not writeable\n"\
			 "Can't save KDE Fontlist."),i18n("OK"));
    return false;
  }

  QTextStream t(&fontfile);
  
  int number = selectedFontsList->count();

  if( number >  0){
    QString fontname;
    for(int i = 0; i < number; i++){
      fontname = selectedFontsList->text(i);
      fontname = fontname.stripWhiteSpace();
      if (!fontname.isEmpty())
	t << fontname.data() << '\n';
    }
  
  }

  fontfile.close();
  
  return true;

}

void KFontManager::queryFonts(){

  /* [KDE1 Revival 2026] 字体枚举 fontconfig 化（BUG5）：
   * Why : 原实现 XListFonts("*") 枚举 X 核心字体（XLFD），Debian 12 上
   *       只有少量 URW/杂项位图字体、无任何 CJK 字体——列表与系统实际
   *       可用字体严重不符。TQFontDatabase 经 fontconfig 枚举，与 konsole
   *       字体菜单同源，Noto/文泉驿等中文字体自然入列。
   * How : ① 家目录 kdefonts 自定义列表仍优先（loadKDEInstalledFonts 语义
   *       不变）；② 否则 TQFontDatabase::families() 填充两个列表；③ 数据
   *       库为空时回退历史 XListFonts 路径。家族名取 utf8()——隐式
   *       const char*（latin1）会把 CJK 名变成 NULL/问号且 strcmp 崩溃；
   *       utf8() 返回 TQCString 临时对象，必须保生命周期。两路结果的
   *       列表回填统一放在回退块之外。 */
  QStrList fontlist(TRUE);
  QStrList installedfontlist(TRUE);

  bool have_installed = loadKDEInstalledFonts();

  /* [KDE1 Revival 2026] fontconfig 枚举无条件执行：kdefonts
     自定义列表存在（have_installed）时只影响
     "KDE 可用字体"列是否自动填充，不能把
     系统实际字体整个挡掉（实测 kdefonts
     存在时左列表只剩 X 核心字体）。 */
  {
    QFontDatabase fdb;
    QStringList fams = fdb.families(false);
    for (QStringList::Iterator it = fams.begin(); it != fams.end(); ++it) {
      TQCString fnHolder = (*it).utf8();
      const char *fn = fnHolder;
      if (!fn || !*fn) continue;
      if (fontlist.find(fn) == -1)
        fontlist.inSort(fn);
      if (!have_installed) {
        if (installedfontlist.find(fn) == -1)
          installedfontlist.inSort(fn);
      }
    }
  }

  if (fontlist.isEmpty())
  {
    int numFonts;
    Display *kde_display;
    char** fontNames;
    char** fontNames_copy;
    QString qfontname;

    kde_display = XOpenDisplay( NULL );

    fontNames = XListFonts(kde_display, "*", 32767, &numFonts);
    fontNames_copy = fontNames;

    for( int k = 0; k < numFonts; k++){

      if (**fontNames != '-'){ // font name doesn't start with a dash -- an alias
        fontNames ++;
        continue;
      };

      qfontname = "";
      qfontname = *fontNames;
      int dash = qfontname.find ('-', 1, TRUE); // find next dash
      if (dash == -1) { // No such next dash -- this shouldn't happen.
        fontNames ++;
        continue;
      }

      // the font family name is between the second and third dash therefore
      // let's find the third dash:
      int dash_two = qfontname.find ('-', dash + 1 , TRUE);
      if (dash_two == -1) { // No such next dash -- this shouldn't happen.
        fontNames ++;
        continue;
      }

      // fish the font family name out of the font info string
      qfontname = qfontname.mid(dash +1, dash_two - dash -1);

      if(fontlist.find(qfontname) == -1)
        fontlist.inSort(qfontname);

      if(!have_installed){
        // we don't have a kdefontlist file yet -- prepare a default list
        // of installed fonts
        if( !qfontname.contains("open look", TRUE)){
          if(qfontname != "nil"){
            if(installedfontlist.find(qfontname) == -1)
              installedfontlist.inSort(qfontname);
          }
        }
      }

      fontNames ++;
    }

    XFreeFontNames(fontNames_copy);
    XCloseDisplay(kde_display);
  }

  availableFontsList->setAutoUpdate(FALSE);
  selectedFontsList->setAutoUpdate(FALSE);

  for(fontlist.first(); fontlist.current(); fontlist.next())
   availableFontsList->insertItem(fontlist.current());

  if(!have_installed){
    for(installedfontlist.first(); installedfontlist.current(); installedfontlist.next())
      selectedFontsList->insertItem(installedfontlist.current());
  }

  availableFontsList->setAutoUpdate(TRUE);
  availableFontsList->update();
  selectedFontsList->setAutoUpdate(TRUE);
  selectedFontsList->update();

}


void KFontManager::readSettings(){



  /*	QString str;
	
	config = a->getConfig();

	config->setGroup( "Text Font" );
	*/

}

void KFontManager::writeSettings(){
		
  /*
	config = a->getConfig();
	
	config->setGroup( "Text Font" );


	config->sync();

	*/
}


void KFontManager::setColors(){
 
  /* this is to the the backgound of a widget to white and the
     text color to black -- some lables such as the one of the
     font manager really shouldn't follow colorschemes The
     primary task of those label is to display the text clearly
     an visibly and not to look pretty ...*/

  QPalette mypalette = (example_label->palette());

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(black,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),black,white);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  example_label->setPalette(mypalette);
  example_label->setBackgroundColor(white);
 
}
#include "kfontmanager.moc"

