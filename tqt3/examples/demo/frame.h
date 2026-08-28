/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqmainwindow.h>
#include <ntqintdict.h>
#include "categoryinterface.h"

class TQToolBox;
class TQStyle;
class TQWidgetStack;

class Frame : public TQMainWindow
{
    TQ_OBJECT

public:
    Frame( TQWidget *parent=0, const char *name=0 );
    void setCategories( const TQPtrList<CategoryInterface> &l );

    static void updateTranslators();

    TQWidgetStack *widgetStack() const { return stack; }

private slots:
    void setStyle( const TQString& );

protected:
    bool event( TQEvent *e );

private:
    TQWidget *createCategoryPage( CategoryInterface *c );

private:
    TQToolBox *toolBox;
    TQWidgetStack *stack;
    TQIntDict<TQWidget> categoryPages;
    TQPtrList<CategoryInterface> categories;

};
