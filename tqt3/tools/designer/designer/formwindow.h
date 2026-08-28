/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef FORMWINDOW_H
#define FORMWINDOW_H

#include "command.h"
#include "metadatabase.h"
#include "sizehandle.h"
#include "actiondnd.h"

#include <ntqwidget.h>
#include <ntqptrdict.h>
#include <ntqpixmap.h>
#include <ntqwidgetlist.h>
#include <ntqmap.h>

class TQPaintEvent;
class TQMouseEvent;
class TQKeyEvent;
class TQPainter;
class TQLabel;
class MainWindow;
class TQTimer;
class TQFocusEvent;
class TQCloseEvent;
class Resource;
class TQResizeEvent;
class BreakLayoutCommand;
class TQPixmap;
class TQSizeGrip;
class Project;
struct DesignerFormWindow;
class FormFile;
class OrderIndicator;

class FormWindow : public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( TQString fileName READ fileName WRITE setFileName )

public:
    FormWindow( FormFile *f, MainWindow *mw, TQWidget *parent, const char *name = 0 );
    FormWindow( FormFile *f, TQWidget *parent, const char *name = 0 );
    ~FormWindow();

    void init();
    virtual void setMainWindow( MainWindow *w );

    virtual TQString fileName() const;
    virtual void setFileName( const TQString &fn );

    virtual TQPoint grid() const;
    virtual TQPoint gridPoint( const TQPoint &p );

    virtual CommandHistory *commandHistory();

    virtual void undo();
    virtual void redo();
    virtual TQString copy();
    virtual void paste( const TQString &cb, TQWidget *parent );
    virtual void lowerWidgets();
    virtual void raiseWidgets();
    virtual void checkAccels();

    virtual void layoutHorizontal();
    virtual void layoutVertical();
    virtual void layoutHorizontalSplit();
    virtual void layoutVerticalSplit();
    virtual void layoutGrid();

    virtual void layoutHorizontalContainer( TQWidget *w );
    virtual void layoutVerticalContainer( TQWidget *w );
    virtual void layoutGridContainer( TQWidget *w );

    virtual void breakLayout( TQWidget *w );

    virtual void selectWidget( TQObject *w, bool select = true );
    virtual void selectAll();
    virtual void updateSelection( TQWidget *w );
    virtual void raiseSelection( TQWidget *w );
    virtual void repaintSelection( TQWidget *w );
    virtual void clearSelection( bool changePropertyDisplay = true );
    virtual void selectWidgets();
    bool isWidgetSelected( TQObject *w );
    virtual void updateChildSelections( TQWidget *w );
    virtual void raiseChildSelections( TQWidget *w );

    virtual void emitUpdateProperties( TQObject *w );
    virtual void emitShowProperties( TQObject *w = 0 );
    virtual void emitSelectionChanged();

    virtual void setPropertyShowingBlocked( bool b );
    bool isPropertyShowingBlocked() const;

    virtual TQLabel *sizePreview() const;
    virtual void checkPreviewGeometry( TQRect &r );

    virtual TQPtrDict<TQWidget> *widgets();
    virtual TQWidgetList selectedWidgets() const;

    virtual TQWidget *designerWidget( TQObject *o ) const;

    virtual void handleContextMenu( TQContextMenuEvent *e, TQWidget *w );
    virtual void handleMousePress( TQMouseEvent *e, TQWidget *w );
    virtual void handleMouseRelease( TQMouseEvent *e, TQWidget *w );
    virtual void handleMouseDblClick( TQMouseEvent *e, TQWidget *w );
    virtual void handleMouseMove( TQMouseEvent *e, TQWidget *w );
    virtual void handleKeyPress( TQKeyEvent *e, TQWidget *w );
    virtual void handleKeyRelease( TQKeyEvent *e, TQWidget *w );

    virtual void updateUndoInfo();

    virtual MainWindow *mainWindow() const { return mainwindow; }

    bool checkCustomWidgets();
    virtual void insertWidget( TQWidget *w, bool checkName = false );
    virtual void removeWidget( TQWidget *w );
    virtual void deleteWidgets();
    virtual void editAdjustSize();
    virtual void editConnections();

    virtual int numSelectedWidgets() const;
    virtual int numVisibleWidgets() const;

    virtual bool hasInsertedChildren( TQWidget *w ) const;

    virtual TQWidget *currentWidget() const { return propertyWidget && propertyWidget->isWidgetType() ? (TQWidget*)propertyWidget : 0; } // #####
    virtual bool unify( TQObject *w, TQString &s, bool changeIt );

    virtual bool isCustomWidgetUsed( MetaDataBase::CustomWidget *w );
    virtual bool isDatabaseWidgetUsed() const;
    virtual bool isDatabaseAware() const;

    virtual TQPoint mapToForm( const TQWidget* w, const TQPoint&  ) const;

    bool isMainContainer( TQObject *w ) const;
    bool isCentralWidget( TQObject *w ) const;
    TQWidget *mainContainer() const { return mContainer; }
    void setMainContainer( TQWidget *w );

    void paintGrid( TQWidget *w, TQPaintEvent *e );

    bool savePixmapInline() const;
    TQString pixmapLoaderFunction() const;
    void setSavePixmapInline( bool b );
    void setPixmapLoaderFunction( const TQString &func );

    bool savePixmapInProject() const;
    void setSavePixmapInProject( bool b );

    void setToolFixed() { toolFixed = true; }

    void setActiveObject( TQObject *o );

    TQPtrList<TQAction> &actionList() { return actions; }
    TQAction *findAction( const TQString &name );

    void setProject( Project *pro );
    Project *project() const;

    void killAccels( TQObject *top );

    DesignerFormWindow *iFace();

    int layoutDefaultSpacing() const;
    int layoutDefaultMargin() const;
    void setLayoutDefaultSpacing( int s );
    void setLayoutDefaultMargin( int s );
    TQString spacingFunction() const;
    TQString marginFunction() const;
    void setSpacingFunction( const TQString &func );
    void setMarginFunction( const TQString &func );
    bool hasLayoutFunctions() const;
    void hasLayoutFunctions( bool b );

    void initSlots();
    FormFile *formFile() const;
    void setFormFile( FormFile *f );

    bool isFake() const { return fake; }
    bool canBeBuddy( const TQWidget* ) const;

public slots:
    virtual void widgetChanged( TQObject *w );
    virtual void currentToolChanged();
    virtual void visibilityChanged();
    virtual void modificationChanged( bool m );

signals:
    void showProperties( TQObject *w );
    void updateProperties( TQObject *w );
    void undoRedoChanged( bool undoAvailable, bool redoAvailable,
			  const TQString &undoCmd, const TQString &redoCmd );
    void selectionChanged();
    void modificationChanged( bool m, FormWindow *fw );
    void modificationChanged( bool m, const TQString &s );
    void fileNameChanged( const TQString &s, FormWindow *fw );

protected:
    virtual void closeEvent( TQCloseEvent *e );
    virtual void focusInEvent( TQFocusEvent *e );
    virtual void focusOutEvent( TQFocusEvent *e );
    virtual void resizeEvent( TQResizeEvent *e );
    void mouseDoubleClickEvent( TQMouseEvent *e ) { handleMouseDblClick( e, mainContainer() ); }

private:
    enum RectType { Insert, Rubber };

    void beginUnclippedPainter( bool doNot );
    void endUnclippedPainter();
    void drawConnectionLine();
    void drawSizePreview( const TQPoint &pos, const TQString& text );

    void insertWidget();
    void moveSelectedWidgets( int dx, int dy );

    void startRectDraw( const TQPoint &p, const TQPoint &global, TQWidget *w, RectType t );
    void continueRectDraw( const TQPoint &p, const TQPoint &global, TQWidget *w, RectType t );
    void endRectDraw();

    void checkSelectionsForMove( TQWidget *w );
    BreakLayoutCommand *breakLayoutCommand( TQWidget *w );

    bool allowMove( TQWidget *w );

    void saveBackground();
    void restoreConnectionLine();
    void restoreRect( const TQRect &rect ) ;

    void showOrderIndicators();
    void updateOrderIndicators();
    void repositionOrderIndicators();
    void hideOrderIndicators();

    TQWidget *containerAt( const TQPoint &pos, TQWidget *notParentOf );

private slots:
    void invalidCheckedSelections();
    void updatePropertiesTimerDone();
    void showPropertiesTimerDone();
    void selectionChangedTimerDone();
    void windowsRepaintWorkaroundTimerTimeout();

private:
    int currTool;
    bool oldRectValid, widgetPressed, drawRubber, checkedSelectionsForMove;
    bool validForBuddy;
    TQRect currRect;
    TQPoint rectAnchor;
    TQPainter *unclippedPainter;
    TQPoint sizePreviewPos;
    TQPixmap sizePreviewPixmap;
    MainWindow *mainwindow;
    TQPtrList<WidgetSelection> selections;
    TQPtrDict<WidgetSelection> usedSelections;
    TQRect widgetGeom, rubber;
    TQPoint oldPressPos, origPressPos;
    CommandHistory commands;
    TQMap<TQWidget*, TQPoint> moving;
    TQWidget *insertParent;
    TQObject *propertyWidget;
    TQLabel *sizePreviewLabel;
    TQTimer *checkSelectionsTimer;
    TQPtrDict<TQWidget> insertedWidgets;
    bool propShowBlocked;
    TQTimer* updatePropertiesTimer, *showPropertiesTimer, *selectionChangedTimer,
    *windowsRepaintWorkaroundTimer;
    TQPoint startPos, currentPos;
    TQWidget *startWidget, *endWidget;
    TQPixmap *buffer;
    TQPtrList<OrderIndicator> orderIndicators;
    TQWidgetList orderedWidgets;
    TQWidgetList stackedWidgets;
    TQWidget *mContainer;
    bool pixInline, pixProject;
    TQString pixLoader;
    bool toolFixed;
    TQPtrList<TQAction> actions;
    Project *proj;
    DesignerFormWindow *iface;
    TQWidget* targetContainer;
    TQPalette restorePalette;
    bool hadOwnPalette;
    int defSpacing, defMargin;
    TQString spacFunction, margFunction;
    bool hasLayoutFunc;
    FormFile *ff;
    bool fake;

};

#endif
