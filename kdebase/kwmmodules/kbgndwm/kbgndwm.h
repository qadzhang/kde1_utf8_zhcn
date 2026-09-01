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

  /* [KDE1 Revival 2026] 全屏壁纸画布（对齐 Trinity kdesktop 架构）
   * ┌─ What : 一块覆盖全屏的 OR(override-redirect) 窗口，壁纸 pixmap
   * │        设为它的窗口背景——暴露重绘由 X 服务器按画布自身的
   * │        背景属性填充，不再单纯依赖根窗填充语义
   * │  Why  : 1999 方案只设根窗背景，暴露路径脆弱（VirtualBox 等
   * │        动态分辨率场景出现大片黑区/残留，用户 2026-09-01 截图
   * │        实测）。KDE2+/Trinity 的 kdesktop 即全屏桌面窗口自绘。
   * │  Who  : KBGndManager 私有；click 全部穿透（不选按钮事件掩码，
   * │        按键事件落回根窗，krootwm 桌面菜单/行为不受影响）
   * │  When : 构造时创建并映射一次；此后随分辨率变化 resize
   * │  Where: kbgndwm.h/.cpp；X 层用 OR + XLowerWindow 压底
   * │  How  : 伪代码——
   * │        1. 顶层 QWidget → winId() 先建窗
   * │        2. XChangeWindowAttributes 设 override_redirect=1
   * │           （kwm 初始扫描与 MapRequest 均不管理 OR 窗）
   * │        3. XSelectInput 仅留 Exposure|StructureNotify（去按钮）
   * │        4. show() → XLowerWindow 压到堆叠最底（kfm 图标窗之下）
   * │        5. 壁纸渲染结果经 setBackgroundPixmap 设给画布与根窗双份
   * └────────────────────────────────────────────────────────────── */
  QWidget *canvas;

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
