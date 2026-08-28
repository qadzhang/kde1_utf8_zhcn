/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "fontdisplayer.h"
#include <ntqapplication.h>
#include <ntqslider.h>
#include <ntqspinbox.h>
#include <ntqpainter.h>
#include <ntqtoolbar.h>
#include <ntqstatusbar.h>
#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqfontdialog.h>
#include <stdlib.h>


FontRowTable::FontRowTable( TQWidget* parent, const char* name ) :
    TQFrame(parent,name)
{
    setBackgroundMode(PaletteBase);
    setFrameStyle(Panel|Sunken);
    setMargin(8);
    setRow(0);
    tablefont = TQApplication::font();
}

TQSize FontRowTable::sizeHint() const
{
    return 24*cellSize()+TQSize(2,2)*(margin()+frameWidth());
}

TQSize FontRowTable::cellSize() const
{
    TQFontMetrics fm = fontMetrics();
    return TQSize( fm.maxWidth(), fm.lineSpacing()+1 );
}

void FontRowTable::paintEvent( TQPaintEvent* e )
{
    TQFrame::paintEvent(e);
    TQPainter p(this);
    p.setClipRegion(e->region());
    TQRect r = e->rect();
    TQFontMetrics fm = fontMetrics();
    int ml = frameWidth()+margin() + 1 + TQMAX(0,-fm.minLeftBearing());
    int mt = frameWidth()+margin();
    TQSize cell((width()-15-ml)/16,(height()-15-mt)/16);

    if ( !cell.width() || !cell.height() )
	return;

    int mini = r.left() / cell.width();
    int maxi = (r.right()+cell.width()-1) / cell.width();
    int minj = r.top() / cell.height();
    int maxj = (r.bottom()+cell.height()-1) / cell.height();

    int h = fm.height();

    TQColor body(255,255,192);
    TQColor negative(255,192,192);
    TQColor positive(192,192,255);
    TQColor rnegative(255,128,128);
    TQColor rpositive(128,128,255);

    for (int j = minj; j<=maxj; j++) {
	for (int i = mini; i<=maxi; i++) {
	    if ( i < 16 && j < 16 ) {
		int x = i*cell.width();
		int y = j*cell.height();

		TQChar ch = TQChar(j*16+i,row);

		if ( fm.inFont(ch) ) {
		    int w = fm.width(ch);
		    int l = fm.leftBearing(ch);
		    int r = fm.rightBearing(ch);

		    x += ml;
		    y += mt+h;

		    p.fillRect(x,y,w,-h,body);
		    if ( w ) {
			if ( l ) {
			    p.fillRect(x+(l>0?0:l), y-h/2, abs(l),-h/2,
				       l < 0 ? negative : positive);
			}
			if ( r ) {
			    p.fillRect(x+w-(r>0?r:0),y+2, abs(r),-h/2,
				       r < 0 ? rnegative : rpositive);
			}
		    }
		    TQString s;
		    s += ch;
		    p.setPen(TQPen(TQt::black));
		    p.drawText(x,y,s);
		}
	    }
	}
    }
}

void FontRowTable::setRow(int r)
{
    row = r;

    TQFontMetrics fm = fontMetrics();
    TQFontInfo fi = fontInfo();
    TQString str = TQString("%1 %2pt%3%4 mLB=%5 mRB=%6 mW=%7")
		    .arg(fi.family())
		    .arg(fi.pointSize())
		    .arg(fi.bold() ? " bold" : "")
		    .arg(fi.italic() ? " italic" : "")
		    .arg(fm.minLeftBearing())
		    .arg(fm.minRightBearing())
		    .arg(fm.maxWidth());

    emit fontInformation(str);
    update();
}

void FontRowTable::chooseFont()
{
    bool ok;
    TQFont oldfont = tablefont;
    tablefont = TQFontDialog::getFont(&ok, oldfont, this);

    if (ok)
	setFont(tablefont);
    else
	tablefont = oldfont;
}

FontDisplayer::FontDisplayer( TQWidget* parent, const char* name ) :
    TQMainWindow(parent,name)
{
    FontRowTable* table = new FontRowTable(this);
    TQToolBar* controls = new TQToolBar(this);
    (void) new TQLabel(tr("Row:"), controls);
    TQSpinBox *row = new TQSpinBox(0,255,1,controls);
    controls->addSeparator();
    TQPushButton *fontbutton = new TQPushButton(tr("Font..."), controls);

    connect(row,TQ_SIGNAL(valueChanged(int)),table,TQ_SLOT(setRow(int)));
    connect(fontbutton, TQ_SIGNAL(clicked()), table, TQ_SLOT(chooseFont()));
    connect(table,TQ_SIGNAL(fontInformation(const TQString&)),
	    statusBar(),TQ_SLOT(message(const TQString&)));
    table->setRow(0);
    setCentralWidget(table);
}
