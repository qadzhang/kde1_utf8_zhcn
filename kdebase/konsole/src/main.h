/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [main.h]                 Testbed for TE framework                          */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole, an X terminal.                               */
/*                                                                            */
/* The material contained in here more or less directly orginates from        */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org>     */
/*                                                                            */
/* -------------------------------------------------------------------------- */

#ifndef MAIN_H
#define MAIN_H

#include <kfm.h>
#include <kapp.h>
#include <ktmainwindow.h>
//#include <ktopwidget.h>
#include <ksimpleconfig.h>
#include <qstrlist.h>
#include <qstringlist.h> // [KDE1 Revival 2026] 动态等宽字族表

#include "TEShell.h"
#include "TEWidget.h"
#include "TEmuVt102.h"
#include "session.h"
#include "schema.h"

class TEDemo : public KTMainWindow
{ Q_OBJECT

public:

  TEDemo(char* name, QStrList & _args, int login_shell);
  ~TEDemo();
  void setColLin(int columns, int lines);

private slots:
  void configureRequest(TEWidget*,int,int,int);

  void scrollbar_menu_activated(int item);
  void activateSession(int);
  void doneSession(TESession*,int);
  void opt_menu_activated(int item);
  void font_menu_activated(int item);
  void schema_menu_activated(int item);
  void size_menu_activated(int item);
  void pixmap_menu_activated(int item);
  void drop_menu_activated(int item);
  void about();
  void help();
  void tecRef();
  void newSession(int kind);

  void changeColumns(int);
  void notifySize(int,int);
  void setHeader();
  void changeTitle(int, const char*s);
  void onDrop( KDNDDropZone* _zone );
    
protected:

 void saveProperties(KConfig* config);
 void readProperties(KConfig* config);

private slots:

  void setSchema(int n);

private:

  void makeMenu();
  void makeStatusbar();
  void runSession(TESession* s);
  void addSession(TESession* s);
  void setColorPixmaps();

  void setMenuVisible(bool);
  void setFrameVisible(bool);
  void setBsHack(bool);
  
  void setSchema(const char* path);
  void setSchema(const ColorSchema* s);
  void setFont(int fontno);
  // [KDE1 Revival 2026] 自定义 VT 字体应用与勾选同步（BUG5 字体菜单动态化），
  // fam 为空串表示来自 KFontDialog 的任意选择（按 family() 记录）
  void applyVTFont(const QFont &f, const QString &fam);

  void addSessionCommand(const char* path);
  void loadSessionCommands();

private:

  TEWidget*      te;
  TESession*     se;

  KMenuBar*   menubar;
  KStatusBar* statusbar;
  KDNDDropZone  *dropZone;

  QPopupMenu* m_file;
  QPopupMenu* m_sessions;
  QPopupMenu* m_options;
  QPopupMenu* m_scrollbar;
  QPopupMenu* m_font;
  QPopupMenu* m_schema;
  QPopupMenu* m_size;
  QPopupMenu* m_drop;
//
  bool        b_menuvis;
  bool        b_framevis;
  bool        b_bshack;
  int         n_font;
  int         n_scroll;
  QString     s_schema;
  int         n_render;
  QString     pmPath; // pixmap path
  QString     dropText;
  QSize       defaultSize;
  int         curr_schema; // current schema no

  // [KDE1 Revival 2026] BUG5：动态等宽字族表（TQFontDatabase 实测存在且
  // fixedPitch 的家族，含中文字体）与当前生效字族名；n_font=-1 表示
  // "自定义字族"状态（非历史 8 槽位）。旧配置的数字槽位仍可读取回退。
  QStringList vtFamilies;
  QString     vtCurrentFamily;

  QStrList args;

public:

  QString     title;
};

#endif
