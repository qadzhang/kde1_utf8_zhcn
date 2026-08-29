/*
 * kstart.h Part of the KDE project.
 *
 * Copyright (C) 1997 Matthias Ettrich
 *
 */


#ifndef K1GUARD_kdebase_kstart_kstart_h
#define K1GUARD_kdebase_kstart_kstart_h
#include <qapp.h>
#include <qcursor.h>
#include <qlist.h>
#include <qstring.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <X11/X.h>
#ifndef Bool
#define Bool int
#endif
#ifndef Status
#define Status int
#endif
#include <X11/Xlib.h>
/* X11 头的 #define Bool int / #define Status int 会击穿下方才拉入的
 * TQt3 头（ntqvariant.h 的 enum Type 含 Bool）。本头未以宏形式使用二者，
 * 在此摘除；X11 函数声明已于上方解析完毕。 */
#undef Bool
#undef Status
#include <qwidget.h>
#include <qpopmenu.h>
#include <qstrlist.h>
#include <kwmmapp.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>


class KStart: public QObject {
  Q_OBJECT

public:
  KStart(KWMModuleApplication* kwmmapp_arg,
	 const char* command_arg,
	 const char* window_arg,
	 int desktop_arg,
	 bool activate_arg,
	 bool maximize_arg,
	 bool iconify_arg,
	 bool sticky_arg,
	 int decoration_arg);
  ~KStart(){};

public slots:

  void initialized();
  void windowAdd(Window);

private:
    
  void applyStyle(Window);
    
    
  KWMModuleApplication* kwmmapp;
  const char* command;
  const char* window;
  int desktop;
  bool activate;
  bool maximize;
  bool iconify;
  bool sticky;
  int decoration;
};
#endif /* K1GUARD_kdebase_kstart_kstart_h */
