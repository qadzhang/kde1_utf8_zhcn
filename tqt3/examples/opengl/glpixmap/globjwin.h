/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/****************************************************************************
**
** The GLObjectWindow contains a GLBox and three sliders connected to
** the GLBox's rotation slots.
**
****************************************************************************/

#ifndef GLOBJWIN_H
#define GLOBJWIN_H

#include <ntqwidget.h>

class GLBox;
class TQLabel;
class TQPopupMenu;

class GLObjectWindow : public TQWidget
{
    TQ_OBJECT
public:
    GLObjectWindow( TQWidget* parent = 0, const char* name = 0 );

protected slots:

    void		grabFrameBuffer();
    void		makePixmap();
    void		makePixmapHidden();
    void		makePixmapForMenu();
    void		useFixedPixmapSize();

private:
    void		drawOnPixmap( TQPixmap* pm );
    GLBox* c1;
    TQLabel* lb;
    int fixMenuItemId;
    int insertMenuItemId;
    TQSize pmSz;
    TQPopupMenu* file;
};


#endif // GLOBJWIN_H
