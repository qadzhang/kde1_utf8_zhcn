// klogout
// Copyright (C) 1997 Matthias Ettrich

//   Modified for the KDE1 Revival Project, 2026
//   Maintainer: <维护者姓名> <邮箱>
//   Modifications written with GLM-5.3 (Z.ai)
//   [2026-08-29] do_grabbing() 移除 XGrabServer：server grab 会冻住
//   VNC/XTEST 注入的点击/按键请求，用户经 x11vnc 根本无法操作本警告框。
//   模态性由 XGrabKeyboard + grabMouse 保证。机理详见 krootwm.C
//   select_rectangle 注释。（下方 XUngrabServer 因此成为无操作，保留作防御。）

#include "warning.moc"
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
#include "main.h"
#include <sys/types.h>
#include <signal.h>

#include <kapp.h>
#include <kcharsets.h>

#include "manager.h"

extern Manager* manager;

extern bool do_not_draw;


KWarning::KWarning( QWidget *parent, const char *name, WFlags f)
  : QDialog(parent, name, False, f){
    setMouseTracking(True);
    frame = new QFrame( this );
    frame->installEventFilter( this );
    frame->setMouseTracking(True);
    frame->setFrameStyle(QFrame::WinPanel | QFrame:: Raised);
    button = new QPushButton(klocale->translate("Ooops!"), this);
    button->setMouseTracking(True);
    button->setDefault( True );
    installEventFilter( this );
    button->installEventFilter( this );
    connect(button, SIGNAL(clicked()), SLOT(ok()));
    label = new QLabel(this);
    label->setAlignment(AlignCenter);
}

void KWarning::SetPointerGrab(QPoint pos){
  QWidget* w = QApplication::widgetAt( pos, true);
  if (!w)
    return;
  if (w->topLevelWidget() == this){
    if (w != mouseGrabber()){
      // [KDE1 Revival 2026] grab 失败时 mouseGrabber() 为 NULL，防御空指针
      if (mouseGrabber())
        mouseGrabber()->releaseMouse();
      w->removeEventFilter(this);
      w->installEventFilter(this);
      w->setMouseTracking(true);
      w->grabMouse();
    }
  }
}

bool KWarning::eventFilter( QObject *ob, QEvent * e){
  if (e->type() == Event_MouseButtonPress){
    if (ob->isWidgetType() &&
	!rect().contains(
			 mapFromGlobal(
				       ((QWidget*)ob)->mapToGlobal(
								   ((QMouseEvent*)e)->pos())))){
      ok();
    }
  }
  if (e->type() == Event_KeyPress){
    int a = ((QKeyEvent*)e)->ascii();
    if (a == 3 || a == 7 || a == 27)
      ok();
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

void KWarning::setText(const char* text, bool with_button){
  label->setText(text);
  label->adjustSize();
  int w = QMAX(label->width() + 20, 100);
  int bh = with_button?80:20;
  setGeometry(QApplication::desktop()->width()/2-w/2,
	      (QApplication::desktop()->height()-label->height()-bh)/2,
	      w, label->height()+bh);
  label->setGeometry(4,10,w-8,label->height());
  if (with_button){
    button->setGeometry(w/2-30, label->height()+20, 60, 30);
    button->show();
  }
  else {
    button->hide();
  }

  frame->setGeometry(0,0, width(), height());
}

bool KWarning::do_grabbing(){
  reactive = manager->current();
  if (reactive)
    reactive->setactive(False);
  manager->darkenScreen();
  // [KDE1 Revival 2026] 原 XGrabServer 已移除——server grab 冻结 VNC/XTEST
  // 注入输入致警告框无法操作（机理详见文件头标注）
  do_not_draw = true;
  show();
  XSetInputFocus (qt_xdisplay(), winId(), RevertToParent, CurrentTime);
  if (XGrabKeyboard(qt_xdisplay(), winId(),True,GrabModeAsync,
  		    GrabModeAsync,CurrentTime) != GrabSuccess){
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

void KWarning::ok(){
  release();
  manager->refreshScreen();
}


void KWarning::release(){
  XUngrabServer(qt_xdisplay());
  if (mouseGrabber())
    mouseGrabber()->releaseMouse();
  // [KDE1 Revival 2026] 裸 XGrabPointer 的对称释放（幂等，无 grab 时无害）
  XUngrabPointer(qt_xdisplay(), CurrentTime);
  hide();
  do_not_draw = false;
  if (reactive && manager->hasClient( reactive )){
    reactive->setactive(True);
    XSetInputFocus (qt_xdisplay(), reactive->window,
		    RevertToPointerRoot, CurrentTime);
  }
}

