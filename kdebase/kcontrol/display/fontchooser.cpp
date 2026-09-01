/* This file is part of the KDE libraries
    Copyright (C) 1998	Mark Donohoe <donohoe@kde.org>
						Stephan Kulow				  

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <qgrpbox.h>
#include <qbttngrp.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qfiledlg.h>
#include <qradiobt.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qlayout.h>
#include <kapp.h>
#include <kcharsets.h>
#include <kconfigbase.h>
#include <ksimpleconfig.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#undef index  /* TQt3 迁移：Xos 的 index 宏炸 TQListBox::index 等方法名 */

#include <ntqfontdatabase.h> /* [KDE1 Revival 2026] fontconfig font enum */
#include <ntqfontmetrics.h>
#include <ntqstringlist.h>

#include "fontchooser.h"
#include "fontchooser.moc"

KFontChooser::KFontChooser( QWidget *parent, const char *name )
	: QWidget( parent, name )
{
	int i;
	
	fnt = QFont( "", 12 ); /* [2026-08-31] 默认族：原硬编码西文字族无 CJK 字形，中文渲染 tofu */
	changed = False;
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 10, 5 );
	topLayout->addStretch( 5 );
	
	QBoxLayout *stackLayout = new QVBoxLayout( 4 );
	
	topLayout->addLayout( stackLayout );
		
	cmbFont = new QComboBox( false, this );
	cmbFont->setFixedHeight( cmbFont->sizeHint().height() );
	
	getFontList( fixedList, true );
	getFontList( fontList );
	
	cmbFont->insertStrList( &fontList );
	QStrListIterator it( fontList );
	for ( i = 0; it.current(); ++it, i++ ) {
		if ( !strcmp( fnt.family(), it.current() ) )
			cmbFont->setCurrentItem( i );
	}
	
	/* [KDE1 Revival 2026] TQComboBox ä»æ activated(int) ä¿¡å·ï¼æ§½æ¹æç´¢å¼åææ¬ */
	connect( cmbFont, SIGNAL( activated( int ) ),
		SLOT( slotSelectFont( int ) ) );
		
	QLabel *label = new QLabel( cmbFont, i18n("&Typeface"), this );
	label->adjustSize();
	label->setMinimumSize( label->size() );
	
	stackLayout->addWidget( label );
	stackLayout->addWidget( cmbFont );

	cbBold = new QCheckBox(  i18n("&Bold"), this );
	cbBold->setMinimumSize( cbBold->sizeHint() );
	cbBold->setChecked( fnt.bold() );
	connect( cbBold, SIGNAL( toggled( bool ) ), SLOT( slotFontBold( bool ) ) );
	
	topLayout->addWidget( cbBold );
	
	cbItalic = new QCheckBox(  i18n("&Italic"), this );
	cbItalic->setMinimumSize( cbItalic->sizeHint() );
	cbItalic->setChecked( fnt.italic() );
	connect( cbItalic, SIGNAL( toggled( bool ) ), SLOT( slotFontItalic( bool ) ) );
	
	topLayout->addWidget( cbItalic );
	
	QBoxLayout *pushLayout = new QHBoxLayout(  2 );
	
	topLayout->addLayout( pushLayout );
	
	stackLayout = new QVBoxLayout( 4 );
	
	pushLayout->addLayout( stackLayout, 10 );
	pushLayout->addSpacing( 10 );
	
	sbSize = new KNumericSpinBox( this );
	
	sbSize->setStep( 1 );
	sbSize->setRange( 8, 16 );
	sbSize->setValue( 12 );
	sbSize->adjustSize();

	connect( sbSize, SIGNAL( valueDecreased() ),
		 SLOT( slotFontSize() ) );
		 
	connect( sbSize, SIGNAL( valueIncreased() ),
		 SLOT( slotFontSize() ) );
	
	label = new QLabel( sbSize, i18n("&Size"), this );
	label->setMinimumSize( label->sizeHint() );

	cmbCharset = new QComboBox( false, this );
	
	cmbCharset->adjustSize();
	cmbCharset->setInsertionPolicy( QComboBox::NoInsertion );
	connect( cmbCharset, SIGNAL( activated( int ) ),
		 SLOT( slotCharset( int ) ) );
	
	sbSize->setFixedHeight( cmbCharset->height() );
	sbSize->setMinimumWidth(sbSize->width());
	cmbCharset->setFixedHeight( cmbCharset->height() );
	cmbCharset->setMinimumWidth( cmbCharset->width());

	stackLayout->addWidget( label );
	stackLayout->addWidget( sbSize );
	
	stackLayout = new QVBoxLayout( 4 );
	
	pushLayout->addLayout( stackLayout, 30 );
	
	label = new QLabel( cmbCharset, i18n("&Character set"), this );
	label->adjustSize();
	label->setMinimumSize( label->size() );
	
	stackLayout->addWidget( label );
	stackLayout->addWidget( cmbCharset );

	topLayout->activate();
	fillCharsetCombo();
}

void KFontChooser::setFont( QFont start_fnt, bool fixed )
{
	fnt = start_fnt;
	
	cmbFont->clear();
	if( fixed )
		cmbFont->insertStrList( &fixedList );
	else 
		cmbFont->insertStrList( &fontList );
	
	QStrListIterator it( fixed ? fixedList : fontList );
	for ( int i = 0; it.current(); ++it, i++ ) {
		if ( !strcmp( fnt.family(), it.current() ) )
			cmbFont->setCurrentItem( i );
	}
	
	sbSize->setValue( fnt.pointSize() );
	
	if ( fnt.bold() )
		cbBold->setChecked( true );
	else
		cbBold->setChecked( false );
		
	if ( fnt.italic() )
		cbItalic->setChecked( true );
	else
		cbItalic->setChecked( false );
	fillCharsetCombo();
}

/* [KDE1 Revival 2026] XLFD 版 getFontList(pattern) 与 addFont(XLFD 解析) 随
 * XListFonts 枚举通道一并退役删除（唯一调用方 getFontList(bool) 已改
 * fontconfig，见上方注释块）。 */


void KFontChooser::getFontList( QStrList &list, bool fixed )
{
	// ┌─ [KDE1 Revival 2026] 字体枚举 fontconfig 化（XListFonts 替换）
	// │  What : 以 TQFontDatabase::families() 枚举系统全部字族（fontconfig），
	// │        替换 1999 年的 XListFonts(XLFD 模式匹配)；等宽清单用
	// │        i/M/W 三字符 advance 相等法判定
	// │  Why  : 现代 Debian 只剩极少 X 核心字体可被 XListFonts 列出——
	// │        Noto Sans CJK 等现代中文字体完全不在结果里（用户报障：
	// │        控制中心字体页看不到系统字体；kfontdialog/kfontmanager/
	// │        konsole 已先行切换，此处为同族残留）。QFontInfo::fixedPitch
	// │        在 TQt3/X11 下恒不可靠，故用 advance 判定（konsole 同款）
	// │  Who  : 控制中心「字体」页的字族下拉（kcmdisplay）
	// │  When : KFontChooser 构造（页面创建）时一次性枚举
	// │  How  : 伪代码——
	// │        1. TQFontDatabase 全量字族 → 过滤 open look → 去重排序入表
	// │        2. fixed=true 时仅保留 i/M/W advance 相等的字族
	// │        3. kdefonts 自定义清单存在且非空 → 与其取交（仅显示既有
	// │           又可用的字族），否则返回全量
	// └───────────────────────────────────────────────────────────────────
	QStrList lstSys;
	{
		TQFontDatabase db;
		TQStringList fams = db.families();
		for ( TQStringList::Iterator it = fams.begin(); it != fams.end(); ++it ) {
			if ( fixed ) {
				TQFont probe( *it, 12 );
				TQFontMetrics fm( probe );
				if ( fm.width( 'i' ) != fm.width( 'M' )
				     || fm.width( 'M' ) != fm.width( 'W' ) )
					continue;
			}
			TQString font = *it;
			if ( font.find( "open look", 0, false ) >= 0 )
				continue;
			if ( lstSys.find( font.latin1() ) != -1 )
				continue;
			lstSys.inSort( font.latin1() );
		}
	}

	QStrList lstKDE;
	if ( !kapp->getKDEFonts( &lstKDE ) || lstKDE.count() == 0 ) {
		list = lstSys;
		return;
	}

	for ( int i = 0; i < (int) lstKDE.count(); i++ ) {
		if ( lstSys.find( lstKDE.at( i ) ) != -1 ) {
			list.append( lstKDE.at( i ) );
		}
	}
}

void KFontChooser::fillCharsetCombo(){
int i;
	cmbCharset->clear();
	KCharsets *charsets=kapp->getCharsets();
        QStrList sets=charsets->displayable(fnt.family());
	cmbCharset->insertItem( i18n("default") );
	for(QString set=sets.first();set;set=sets.next())
	  cmbCharset->insertItem( set );
	cmbCharset->insertItem( i18n("any") );

	QString charset=charsets->name(fnt);
	for(i = 0;i<cmbCharset->count();i++){
	  if (charset==cmbCharset->text(i)){
	    cmbCharset->setCurrentItem(i);
	    break;
	  }
	}
}

void KFontChooser::slotCharset(int index)
{

  KCharsets *charsets=kapp->getCharsets();
  QString name = cmbCharset->text(index);
  if (strcmp(name,"default")==0){
     charsets->setQFont(fnt,klocale->charset());
     defaultCharset=TRUE;
  }   
  else{   
     charsets->setQFont(fnt,name);
     defaultCharset=FALSE;
  }   

  emit fontChanged( fnt );
  changed=TRUE;
}

void KFontChooser::slotSelectFont( int index )
{
//	if( lbFonts->currentItem() == 0 )
	fnt.setFamily( cmbFont->text( index ) );
		
	fillCharsetCombo();	
	//slotPreviewFont(0);
	emit fontChanged( fnt );
	changed=TRUE;
}

void KFontChooser::slotFontSize( )
{
	//const int sizes[] = { 10, 12, 14 };

//	if( lbFonts->currentItem() == 0 )
	int s = sbSize->getValue();
		fnt.setPointSize( s );
	
	//slotPreviewFont(0);
	emit fontChanged( fnt );

	changed=TRUE;
}

void KFontChooser::slotFontBold( bool b )
{
//	if( lbFonts->currentItem() == 0 )
		fnt.setBold( b );
	
	//slotPreviewFont(0);
	emit fontChanged( fnt );

	changed=TRUE;
}

void KFontChooser::slotFontItalic( bool i )
{
//	if( lbFonts->currentItem() == 0 )
		fnt.setItalic( i );
	
	//slotPreviewFont(0);
	emit fontChanged( fnt );

	changed=TRUE;
}
