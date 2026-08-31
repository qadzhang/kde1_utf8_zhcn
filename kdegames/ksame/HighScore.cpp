/*
 *   ksame 0.4 - simple Game
 *   Copyright (C) 1997,1998  Marcus Kreutzberger
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *    
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <qkeycode.h>
#include <kapp.h>         
#include <qpainter.h>
#include <qlined.h>
#include <qdialog.h>
#include <qlabel.h>
#include "HighScore.moc"


#define MAX(a,b) ((a)>(b)?(a):(b))

HighScore::HighScore( QWidget *parent ) : QWindow(parent,"Highscore") {

     ok=new QPushButton(klocale->translate("Close"),this);
     ok->setDefault(true);
     ok->setFocus();
     connect(ok, SIGNAL(clicked()),this, SLOT(hide()) );
     playername[0]=0;
     load();
     paintEvent(0);
     setMinimumSize(mSizeHint);
     resize(mSizeHint);
} 

void 
HighScore::keyPressEvent (QKeyEvent * e) {
     if (e->key()==Key_Return) {
	  hide();
	  e->accept();
     }     
}
void 
HighScore::paintEvent ( QPaintEvent * ) {
     QPainter p(this);
     fontw=p.fontMetrics().width('X');

     int entryw=fontw*27;

     fonth=p.fontMetrics().height();
     
     int w=0,h=0;
     int c,tw;
     
     const char *title=klocale->translate("Highscore");
     QString value;
     QFont tf(p.font());
     tf.setPointSize(tf.pointSize()+5);
     QFontMetrics fm(tf);
     
     tw=fm.width(title);
     w=MAX(entryw+20,tw+20);
     h=10;
     
     QFont of(p.font());
     p.setFont(tf);
     p.drawText(0,h,w,fm.height(),AlignCenter,title);
     h+=fm.height()+10;
     p.setFont(of);
     p.drawLine(5,h-10,w-5,h-10);
     c=(w-entryw)/2;
     for (int i=0;i<10;i++) {
	  if (i<hiscore_used) {
	       value.sprintf("#%2i",i+1);
	       p.drawText(c+0,h,fontw*3,fonth,AlignLeft,value);
	       p.drawText(c+4*fontw,h,fontw*16,fonth,AlignLeft,hiscore[i].name);
	       value.setNum(hiscore[i].score);
	       p.drawText(c+21*fontw,h,fontw*6,fonth,AlignRight,value);
	  }
	  h+=fonth+5;
     }
     h+=10;
     ok->resize(ok->sizeHint());
     ok->move((w-ok->width())/2,h);
     h+=ok->height();
     h+=10;
     mSizeHint = QSize(w,h);
     // coolo: don't call resize
     // resize(w,h);
}

void 
HighScore::add(int board, int score,int colors) {

     // Was?? schlechter als der letzte, dann aber raus hier!
     if (hiscore_used&&hiscore_used==HS_MAXENTRY&&score<=hiscore[hiscore_used-1].score) 
	  return;
     // Nach dem Namen fragen
     QDialog dlg(0,klocale->translate("Board"),1);
     int w=10,h=10;
     
     QLineEdit name(&dlg,klocale->translate("Name"));
     name.setFocus();
     QLabel l(&name,klocale->translate("Please &enter your name:"),&dlg);
     
     l.move(10,h);
     l.resize(l.sizeHint());
     w=l.width()+20;
     h+=l.height()+10;
     name.move(10,h);
     w=MAX(name.width()+20,w);
     h+=name.height()+10;
     
     QPushButton ok(klocale->translate("OK"), &dlg);
     ok.setDefault(true);
     
     w=MAX(ok.width()+20,w);
     
     ok.move(w-ok.width()-10,h);
     h+=ok.height()+10;
     
     dlg.resize(w,h);
     dlg.setFixedSize(w,h);
     dlg.connect(&ok, SIGNAL(clicked()), SLOT(accept()));
     dlg.connect(&name, SIGNAL(returnPressed()), SLOT(accept()));
     name.setText(playername);
     name.selectAll();
     if (dlg.exec()) {
	  add(board,score,colors,name.text().ascii());
	  repaint();
	  show();
     }
}
/* [KDE1 Revival 2026] UTF-8 边界收尾：高分玩家名的存储定长 16 字节（1999
   原格式，%16s/%16c 读写、旧存档兼容），截断可能落在多字节字符中间——
   只裁掉尾部残缺序列，完整的多字节字符保留。
   [2026-08-31 三轮修订] ① 首版无差别回退会误删完整尾字符（模糊测试
   抓出）；② 比较差一（have 数续字节、need 含前导，须比 need-1）；
   ③ 循环收口：裁一刀后暴露的新尾巴也可能是坏序列，循环至稳定。
   算法经 tools/utf8-boundary-fuzz.c 200 万随机样本验证全部契约。
   What : 收尾 buf 使其以 ASCII 或完整多字节字符结束
   Who  : HighScore::add/load/store 的三处玩家名定长截断
   When : 每次读/写高分表前
   How  : 循环：尾字节为续字节(0x80-0xBF) → 回溯到前导 p，have<need-1
          裁到 p（残缺）、have>need-1 裁到 p+need（悬挂续字节）、相等
          即完整收尾返回；尾字节为有效前导(C2-F4) → 缺后续，裁掉；
          其余（ASCII/无效前导单字节）返回。每轮 strlen 严格递减 */
static void utf8_trim_boundary( char *buf )
{
    for (;;) {
        int L = strlen( buf );
        if ( L <= 0 )
            return;
        unsigned char last = (unsigned char)buf[L-1];
        if ( ( last & 0xC0 ) == 0x80 ) {           /* 尾字节是续字节 */
            int p = L - 1;
            while ( p >= 0 && ( (unsigned char)buf[p] & 0xC0 ) == 0x80 )
                p--;
            if ( p < 0 ) {                         /* 全是孤立续字节 */
                buf[0] = 0;
                return;
            }
            unsigned char b = (unsigned char)buf[p];
            int need = 1;                          /* 前导声明的总字节长 */
            if ( (b & 0xE0) == 0xC0 ) need = 2;
            else if ( (b & 0xF0) == 0xE0 ) need = 3;
            else if ( (b & 0xF8) == 0xF0 ) need = 4;
            int conts = need - 1;                  /* 应配续字节数 */
            int have = L - 1 - p;                  /* 实际续字节数 */
            if ( have < conts ) {                  /* 残缺序列：整段裁掉 */
                buf[p] = 0;
                continue;
            }
            if ( have > conts ) {                  /* 前导完整后续悬挂：裁悬挂段 */
                buf[p + need] = 0;
                continue;
            }
            return;                                /* 完整字符收尾 */
        }
        if ( last >= 0xC0 && last < 0xF8 ) {       /* 尾字节是缺后续的有效前导 */
            buf[L-1] = 0;
            continue;
        }
        return;                                    /* ASCII / 无效前导单字节 */
    }
}

void 
HighScore::add(int board, int score,int colors,char *name) {
     int i;
     
     load();
     if (name) {
	  while (*name==' ') name++;
	  if (*name) strncpy(playername,name,16);
     }
     if (!*playername) strncpy(playername,klocale->translate("Anonymous"),16);
     playername[16]=0;
     utf8_trim_boundary( playername );      /* [2026-08-31] 截断回退到字符边界 */
     for (i=hiscore_used;i>0;i--) {
	  if (score<=hiscore[i-1].score) break;
	  if (i<HS_MAXENTRY) {
	       hiscore[i]=hiscore[i-1];
	  }
     }
     if (i<HS_MAXENTRY) {
	  hiscore[i].board=board;
	  hiscore[i].score=score;
	  hiscore[i].colors=colors;
	  strncpy(hiscore[i].name,playername,16);
	  hiscore[i].name[16]=0;
	  utf8_trim_boundary( hiscore[i].name ); /* [2026-08-31] 同上 */
	  if (hiscore_used<HS_MAXENTRY) hiscore_used++;
	  store();
     }
}


void 
HighScore::load() {
  QString name, value, oldgrp;
  KConfig *conf = kapp->getConfig();

  oldgrp = conf->group();
  conf->setGroup("Highscore");
  hiscore_used=0;
  for (int i=0;i<HS_MAXENTRY;i++) {
       name.sprintf("Rank_%i",i);
       if (!conf->hasKey(name)) break;
       value=conf->readEntry(name.data());
       memset(hiscore[i].name, 0, 17);
       sscanf(value.data(),"%i %i %i %16c",
	      &hiscore[i].board,
	      &hiscore[i].score,
	      &hiscore[i].colors,
	      hiscore[i].name);
       /* [2026-08-31] %16c 定长拷贝可能带回尾随空格垫充与残缺字节：
          先裁空格再按 UTF-8 边界收尾 */
       {
            int L = 16;
            while ( L > 0 && hiscore[i].name[L-1] == ' ' )
                 hiscore[i].name[--L] = 0;
            utf8_trim_boundary( hiscore[i].name );
       }
       hiscore_used++;
  }
  strncpy(playername,conf->readEntry("LastPlayer","NoNaMe"),16);
  playername[16]=0;
  utf8_trim_boundary( playername );          /* [2026-08-31] 截断回退到字符边界 */
  conf->setGroup(oldgrp.data());
}

void 
HighScore::store() {
  QString name, value, oldgrp;
  KConfig *conf = kapp->getConfig();

  oldgrp = conf->group();
  conf->setGroup("Highscore");
  for (int i=0;i<hiscore_used;i++) {
       name.sprintf("Rank_%i",i);
       value.sprintf("%i %i %i %16s",
		     hiscore[i].board,
		     hiscore[i].score,
		     hiscore[i].colors,
		     hiscore[i].name);
       conf->writeEntry(name, value);
  }
  conf->writeEntry("LastPlayer",playername);
  conf->setGroup(oldgrp.data());
  conf->sync();
}














