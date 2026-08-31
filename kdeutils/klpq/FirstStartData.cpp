/**********************************************************************

	--- Qt Architect generated file ---

	File: FirstStartData.cpp
	Last generated: Sat Feb 7 11:11:14 1998

 *********************************************************************/

#include "FirstStartData.h"
#include "FirstStartData.moc"

#define Inherited QDialog

#include <qlabel.h>
#include <qbttngrp.h>

#include <kapp.h>

FirstStartData::FirstStartData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 0 )
{
	QButtonGroup* dlgedit_ButtonGroup_1;
	dlgedit_ButtonGroup_1 = new QButtonGroup( this, "ButtonGroup_1" );
	dlgedit_ButtonGroup_1->setGeometry( 176, 136, 128, 112 );   /* [2026-08-31] 88→176：随对话框加宽居中 */
	dlgedit_ButtonGroup_1->setMinimumSize( 10, 10 );
	dlgedit_ButtonGroup_1->setMaximumSize( 32767, 32767 );
	dlgedit_ButtonGroup_1->setTitle( "" );
	dlgedit_ButtonGroup_1->setAlignment( 1 );
	dlgedit_ButtonGroup_1->setFrameStyle( QFrame::NoFrame );

	QLabel* dlgedit_Label_1;
	dlgedit_Label_1 = new QLabel( this, "Label_1" );
	dlgedit_Label_1->setGeometry( 24, 16, 432, 120 );   /* [2026-08-31] 264→432：中文译文行宽（对话框同步加宽见尾部） */
	dlgedit_Label_1->setMinimumSize( 10, 10 );
	dlgedit_Label_1->setMaximumSize( 32767, 32767 );
	dlgedit_Label_1->setText( klocale->translate("Please select the spooling system you use. Most systems use the BSD spooler. So if you don't know which spooler is installed on your system, BSD is probably a good choice.") );
	dlgedit_Label_1->setAlignment( 1313 );
	dlgedit_Label_1->setMargin( -1 );

	b_ok = new QPushButton( this, "PushButton_1" );
	b_ok->setGeometry( 190, 266, 100, 24 );   /* [2026-08-31] 104→190：随对话框加宽居中 */
	b_ok->setMinimumSize( 10, 10 );
	b_ok->setMaximumSize( 32767, 32767 );
	b_ok->setText( klocale->translate("OK") );
	b_ok->setAutoRepeat( FALSE );
	b_ok->setAutoResize( FALSE );
	b_ok->setDefault( TRUE );
	b_ok->setAutoDefault( TRUE );

	rb_bsd = new QRadioButton( this, "RadioButton_1" );
	rb_bsd->setGeometry( 208, 144, 100, 30 );   /* [2026-08-31] 随组平移 +88 */
	rb_bsd->setMinimumSize( 10, 10 );
	rb_bsd->setMaximumSize( 32767, 32767 );
	rb_bsd->setText( "BSD" );
	rb_bsd->setAutoRepeat( FALSE );
	rb_bsd->setAutoResize( FALSE );
	rb_bsd->setChecked( TRUE );

	rb_ppr = new QRadioButton( this, "RadioButton_2" );
	rb_ppr->setGeometry( 208, 176, 100, 30 );   /* [2026-08-31] 随组平移 +88 */
	rb_ppr->setMinimumSize( 10, 10 );
	rb_ppr->setMaximumSize( 32767, 32767 );
	rb_ppr->setText( "PPR" );
	rb_ppr->setAutoRepeat( FALSE );
	rb_ppr->setAutoResize( FALSE );

	rb_lprng = new QRadioButton( this, "RadioButton_3" );
	rb_lprng->setGeometry( 208, 208, 100, 30 );   /* [2026-08-31] 随组平移 +88 */
	rb_lprng->setMinimumSize( 10, 10 );
	rb_lprng->setMaximumSize( 32767, 32767 );
	rb_lprng->setText( "LPRNG" );
	rb_lprng->setAutoRepeat( FALSE );
	rb_lprng->setAutoResize( FALSE );

	dlgedit_ButtonGroup_1->insert( rb_bsd );
	dlgedit_ButtonGroup_1->insert( rb_ppr );
	dlgedit_ButtonGroup_1->insert( rb_lprng );

	/* [2026-08-31] 1999 年 304x300 按 English 文宽设计；中文译文每行
	   约 30 全角字符需 ~420px——原宽度下标签右侧被截断。加宽对话框
	   与说明标签，单选钮组/确定键居中随移 */
	resize( 480, 310 );
	setMinimumSize( 480, 310 );
	setMaximumSize( 480, 310 );
}


FirstStartData::~FirstStartData()
{
}
