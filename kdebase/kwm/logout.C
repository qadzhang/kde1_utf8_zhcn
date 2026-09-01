// klogout
// Copyright (C) 1997 Matthias Ettrich

//   Modified for the KDE1 Revival Project, 2026
//   Maintainer: <维护者姓名> <邮箱>
//   Modifications written with GLM-5.3 (Z.ai)
//   [2026-08-29] do_grabbing() 移除 XGrabServer：server grab 会冻住
//   VNC/XTEST 注入的点击/按键请求，用户经 x11vnc 根本无法点击本对话框，
//   会话被挂死。模态性由 XGrabKeyboard + grabMouse 保证；屏幕冻结只是
//   化妆性防重绘。机理详见 krootwm.C select_rectangle 注释。
//   （下方失败分支与释放处的 XUngrabServer 因此成为无操作，保留作防御。）

#include "logout.moc"
#include <kapp.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qwindefs.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#undef index  /* TQt3 迁移：Xos 的 index 宏炸 TQListBox::index 等方法名 */

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#include <kapp.h>
#include <kcharsets.h>

#include "manager.h"
#include "main.h"

extern Manager* manager;

extern bool do_not_draw;


Klogout::Klogout( QWidget *parent, const char *name, WFlags f)
  : QDialog(parent, name, False, f){
    setMouseTracking(True);
    frame = new QFrame( this );
    frame->installEventFilter( this );
    frame->setMouseTracking(True);
    frame->setFrameStyle(QFrame::WinPanel | QFrame:: Raised);
    button = new QPushButton(klocale->translate("Logout"), this);
    button_cancel = new QPushButton(klocale->translate("Cancel"), this);
    button->setMouseTracking(True);
    button_cancel->setMouseTracking(True);
    button->setDefault( True );
    installEventFilter( this );
    button->installEventFilter( this );
    button_cancel->installEventFilter( this );
    connect(button, SIGNAL(clicked()), SLOT(logout()));
    connect(button_cancel, SIGNAL(clicked()), SLOT(cleanup()));
    label = new QLabel(klocale->translate("Session prepared for logout"), this);
    label->installEventFilter( this );
    label->setMouseTracking(True);
    label->setAlignment(AlignCenter);

    listboxlabel1 = 
      new QLabel(klocale->translate("The following applications contain unsaved data which\nwill be lost in your next session."), this);
    listboxlabel1->installEventFilter( this );
    listboxlabel1->setMouseTracking(True);
    listboxlabel1->setAlignment(AlignLeft);

    listbox1 = new QListBox(this);
    listbox1->installEventFilter( this );
    listbox1->setMouseTracking(True);
    connect(listbox1, SIGNAL(selected(int)), SLOT(listbox1Select(int)));

    listboxlabel2 = 
      new QLabel(klocale->translate("The following applications cannot be restored in your next session,\nonly restarted. Make sure they do not contain important unsaved data."), this);
    listboxlabel2->installEventFilter( this );
    listboxlabel2->setMouseTracking(True);
    listboxlabel2->setAlignment(AlignLeft);

    listbox2 = new QListBox(this);
    listbox2->installEventFilter( this );
    listbox2->setMouseTracking(True);
    connect(listbox2, SIGNAL(selected(int)), SLOT(listbox2Select(int)));
    
    listboxlabel3 = 
      new QLabel(klocale->translate("The following applications are not X11-aware and though cannot be restored\nin your next session. Make sure they do not contain important unsaved data."), this);
    listboxlabel3->installEventFilter( this );
    listboxlabel3->setMouseTracking(True);
    listboxlabel3->setAlignment(AlignLeft);

    listbox3 = new QListBox(this);
    listbox3->installEventFilter( this );
    listbox3->setMouseTracking(True);
    connect(listbox3, SIGNAL(selected(int)), SLOT(listbox3Select(int)));

}

void Klogout::prepareToShow(const QStrList* strlist1, 
			    const QStrList* strlist2,
			    const QStrList* strlist3){
  int w = 360;
  int h = 0;
  int lh = 100;
  QFont fnt("helvetica", 24);
  KApplication::getKApplication()->getCharsets()->setQFont(fnt);
  label->setFont( fnt );
  label->adjustSize();
  if (label->width() + 40 > w)
    w = label->width() + 40;
  
  bool do_listbox1 = strlist1 && !strlist1->isEmpty();

  if (!do_listbox1) {
    listboxlabel1->hide();
    listbox1->hide();
    listbox1->setFocusPolicy( NoFocus );
  }
  else {
    listboxlabel1->adjustSize();
    if (listboxlabel1->width() + 40 > w)
      w = listboxlabel1->width() + 40;
    listbox1->clear();
    listbox1->insertStrList(strlist1);
    listboxlabel1->show();
    listbox1->show();
    listbox1->setFocusPolicy( StrongFocus );
  }

  bool do_listbox2 = strlist2 && !strlist2->isEmpty();
  
  if (!do_listbox2){
    listboxlabel2->hide();
    listbox2->hide();
    listbox2->setFocusPolicy( NoFocus );
  }
  else {
    listboxlabel2->adjustSize();
    if (listboxlabel2->width() + 40 > w)
      w = listboxlabel2->width() + 40;
    listbox2->clear();
    listbox2->insertStrList(strlist2);
    listboxlabel2->show();
    listbox2->show();
    listbox2->setFocusPolicy( StrongFocus );
  }

  bool do_listbox3 = strlist3 && !strlist3->isEmpty();
  
  if (!do_listbox3){
    listboxlabel3->hide();
    listbox3->hide();
    listbox3->setFocusPolicy( NoFocus );
  }
  else {
    listboxlabel3->adjustSize();
    if (listboxlabel3->width() + 40 > w)
      w = listboxlabel3->width() + 40;
    listbox3->clear();
    listbox3->insertStrList(strlist3);
    listboxlabel3->show();
    listbox3->show();
    listbox3->setFocusPolicy( StrongFocus );
  }

  label->move(w/2-label->width()/2, 15);
  h = label->geometry().bottom() + 15;
  
  if (do_listbox1 && do_listbox2 && do_listbox3)
    lh = 200/3;

  if (do_listbox1){
    listboxlabel1->move(w/2-listboxlabel1->width()/2, h);
    h = listboxlabel1->geometry().bottom() + 10;
    listbox1->setGeometry(10, h, w-20, lh);
    h = listbox1->geometry().bottom() + 10;
  }

  if (do_listbox2){
    listboxlabel2->move(w/2-listboxlabel2->width()/2, h);
    h = listboxlabel2->geometry().bottom() + 10;
    listbox2->setGeometry(10, h, w-20, lh);
    h = listbox2->geometry().bottom() + 10;
  }

  if (do_listbox3){
    listboxlabel3->move(w/2-listboxlabel3->width()/2, h);
    h = listboxlabel3->geometry().bottom() + 10;
    listbox3->setGeometry(10, h, w-20, lh);
    h = listbox3->geometry().bottom() + 10;
  }

  button->setGeometry(w/9, 
		      h,
		      w/3, 30);
  button_cancel->setGeometry(w/2 + w/18, 
			     h,
			     w/3, 30);
  h = button_cancel->geometry().bottom()+15;
  
  setGeometry(kwmScreenWidth()/2 - w/2,
	      kwmScreenHeight()/2 - h/2,
	      w, h);
  frame->setGeometry(0,0, w, h);
}



void Klogout::resizeEvent(QResizeEvent *){
}

void Klogout::SetPointerGrab(QPoint pos){
  QWidget* w = QApplication::widgetAt( pos, TRUE);
  if (!w)
    return;
  if (w->topLevelWidget() == this){
    if (w != mouseGrabber()){
      // [KDE1 Revival 2026] grab 可能因时间戳过期失败（mouseGrabber()==NULL），
      // TQt3 下 NULL 解引用即崩溃——防御后直接迁移 grab 到目标 widget
      if (mouseGrabber())
        mouseGrabber()->releaseMouse();
      w->removeEventFilter(this);
      w->installEventFilter(this);
      w->setMouseTracking(true);
      w->grabMouse();
    }
  }
}

bool Klogout::eventFilter( QObject *ob, QEvent * e){
  if (e->type() == Event_MouseButtonPress){
    if (ob->isWidgetType() && 
	!rect().contains(
			 mapFromGlobal(
				       ((QWidget*)ob)->mapToGlobal(
								   ((QMouseEvent*)e)->pos())))){
      cleanup();
    }
  }
  if (e->type() == Event_KeyPress){
    int a = ((QKeyEvent*)e)->ascii();
    if (a == 3 || a == 7 || a == 27)
      cleanup();
  }
  if (e->type() == Event_MouseMove){
    QMouseEvent* mev = (QMouseEvent*) e;
    if (ob->isWidgetType()
	&& !(mev->state() & LeftButton)
	&& !(mev->state() & MidButton)
	&& !(mev->state() & RightButton)
	){
      SetPointerGrab(((QWidget*)ob)->mapToGlobal(mev->pos()));
    }
  }
  return False;
}

bool Klogout::do_grabbing(){
  reactive = manager->current();
  if (reactive)
    reactive->setactive(False);
  //  manager->darkenScreen();
  // [KDE1 Revival 2026] 原 XGrabServer 已移除——server grab 冻结 VNC/XTEST
  // 注入输入致对话框无法点击（机理详见文件头标注）
  do_not_draw = true;
  show();
  XSetInputFocus (qt_xdisplay(), winId(), RevertToParent, CurrentTime);
  int gk = XGrabKeyboard(qt_xdisplay(), winId(),True,GrabModeAsync,
  		    GrabModeAsync,CurrentTime);
  if (gk != GrabSuccess){
    // [KDE1 Revival 2026] 诊断：grab 失败码（1=AlreadyGrabbed 被抢先 等）
    fprintf(stderr, "kwm: Klogout XGrabKeyboard 失败码 %d\n", gk);
    XUngrabServer(qt_xdisplay());
    return False;
  }
  raise();
  // [KDE1 Revival 2026] 模态指针抓取改裸 XGrabPointer(CurrentTime)：TQt3 的
  // QWidget::grabMouse 以内部时间戳 tqt_x_time 请求——在注销链路的同步调用栈
  // 里早于 server 最近一次 grab 而被拒（GrabInvalidTime），鼠标事件被悬挂的
  // root grab 吞掉（"注销窗口点不动"根因）。裸调用以 CurrentTime 抓到本对话
  // 框并 confine，owner_events=True 保持 Qt 子部件事件路由不变。
  XGrabPointer(qt_xdisplay(), winId(), True,
	       ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask,
	       GrabModeAsync, GrabModeAsync, winId(), None, CurrentTime);
  button->setFocus();
  
  return True;
}

void Klogout::logout(){
  cleanup();
  emit doLogout();
}

void Klogout::listbox1Select(int index){
  cleanup();
  QString label = listbox1->text(index);
  emit changeToClient(label);
}

void Klogout::listbox2Select(int index){
  cleanup();
  QString label = listbox2->text(index);
  emit changeToClient(label);
}

void Klogout::listbox3Select(int index){
  cleanup();
  QString label = listbox3->text(index);
  emit changeToClient(label);
}

void Klogout::cleanup(){
  XUngrabServer(qt_xdisplay());
  if (mouseGrabber())
    mouseGrabber()->releaseMouse();
  // [KDE1 Revival 2026] 裸 XGrabPointer 的对称释放（幂等，无 grab 时无害）
  XUngrabPointer(qt_xdisplay(), CurrentTime);
  hide();
  do_not_draw = false;
  if (reactive){
    reactive->setactive(True);
    XSetInputFocus (qt_xdisplay(), reactive->window, 
		    RevertToPointerRoot, CurrentTime);
  }
  manager->refreshScreen();
}


