/*
 * kbgndwm.h.  Part of the KDE Project.
 *
 * Copyright (C) 1997 Martin Jones
 *               1998 Matej Koss
 *
 */

#ifndef __KBGNDWM_H__
#define __KBGNDWM_H__

//----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include <kwmmapp.h>
#include <drag.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include "bg.h"

//----------------------------------------------------------------------------

#define MAX_DESKTOPS 8

//----------------------------------------------------------------------------


class KBGndManager: public QWidget
{

  Q_OBJECT

public:
  KBGndManager( KWMModuleApplication * );

protected:
  void applyDesktop( int d );
  void cacheDesktop();
  void readSettings();

  void paintEvent(QPaintEvent *e);

  /* [KDE1 Revival 2026] 分辨率热变更监视（"桌面背影残留"根治）
   * ┌─ What : 2 秒周期比对根窗实际几何，变化即清壁纸缓存并重渲染重铺
   * │  Why  : kbgndwm 只在启动/换桌面时渲染壁纸，且渲染尺寸曾取
   * │        QApplication::desktop() 的构造期缓存——热改分辨率后壁纸
   * │        尺寸过期，弹出菜单关闭等暴露区按错位平铺重填，形成
   * │        "背影"（用户 2026-09-01 报障实测）。TQt3 -no-xrandr 无
   * │        事件可收，只能周期直查 XGetGeometry 真值。
   * │  Who  : KBGndManager 自身；渲染尺寸由 bg.cpp kbg_root_size 提供
   * │  When : 构造时 startTimer(2000)；timerEvent 内几何未变则直接返回
   * │  Where: kbgndwm.h/.cpp
   * │  How  : 伪代码——
   * │        1. XGetGeometry 取根窗 w/h；与上次记录相同 → 返回
   * │        2. 不同 → 记录新值 → QPixmapCache::clear()（旧尺寸渲染
   * │           结果全部作废）→ applyDesktop(current) 重渲染重设
   * │           根窗背景 → 画布 resize + 换背景 → XClearArea 全屏重填
   * └────────────────────────────────────────────────────────────── */
  virtual void timerEvent( QTimerEvent * );
  int last_root_w, last_root_h;

  /* [KDE1 Revival 2026] KDE3 kdesktop 式全屏桌面画布（兼容层）。
   * 创建于构造器（隐藏、override_redirect、仅暴露/结构事件——
   * 点击穿透到根窗，kfm 图标/krootwm 菜单不受影响）；首次渲染
   * 完成时由 bg.cpp kbg_apply_wallpaper 映射并压底。画面承载者，
   * 与根窗服务端填充语义解耦（详见 bg.cpp 的 5W1H）。 */
  TQWidget *canvas;

public slots:
  void desktopChange( int );
  void commandReceived( QString );
  void toggleOneDesktop();

  void timeclick();
  void slotDropEvent( KDNDDropZone * );
  void slotModeSelected( int mode );
  void setUndock();

private slots:
  void mousePressEvent(QMouseEvent *e);
  void displaySettings();
  void settings();
  void dock();
  void undock();

private:
  KWMModuleApplication* kwmmapp;
  QPopupMenu *popup_m;
  QPopupMenu *modePopup;
  QPixmap pixmap;

  KBackground *desktops;
  int current;

  bool oneDesktopMode;
  int desktop;

  bool docked;

  int o_id;

  QString wallpaper;
};



//----------------------------------------------------------------------------

#endif
