/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef WHATSTHIS_H
#define WHATSTHIS_H

#include <ntqwhatsthis.h> 

class TQHeader;
class TQTable;

class WhatsThis : public TQObject, public TQWhatsThis
{
    TQ_OBJECT
public:
    WhatsThis( TQWidget *w, TQWidget *watch = 0 );

    bool clicked( const TQString &link );
    TQWidget *parentWidget() const;

signals:
    void linkClicked( const TQString &link );

private:
    TQWidget *widget;
};

class HeaderWhatsThis : public WhatsThis
{
public: 
    HeaderWhatsThis( TQHeader *h );

    TQString text( const TQPoint &p );
};

class TableWhatsThis : public WhatsThis
{
public: 
    TableWhatsThis( TQTable *t );

    TQString text( const TQPoint &p );
};

#endif
