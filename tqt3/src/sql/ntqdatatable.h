/****************************************************************************
**
** Definition of TQDataTable class
**
** Created : 2000-11-03
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQDATATABLE_H
#define TQDATATABLE_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqvariant.h"
#include "ntqtable.h"
#include "ntqsql.h"
#include "ntqsqlcursor.h"
#include "ntqsqlindex.h"
#include "ntqsqleditorfactory.h"
#include "ntqiconset.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL_VIEW_WIDGETS

class TQPainter;
class TQSqlField;
class TQSqlPropertyMap;
class TQDataTablePrivate;

class TQM_EXPORT_SQL TQDataTable : public TQTable
{
    TQ_OBJECT

    TQ_PROPERTY( TQString nullText READ nullText WRITE setNullText )
    TQ_PROPERTY( TQString trueText READ trueText WRITE setTrueText )
    TQ_PROPERTY( TQString falseText READ falseText WRITE setFalseText )
    TQ_PROPERTY( DateFormat dateFormat READ dateFormat WRITE setDateFormat )
    TQ_PROPERTY( bool confirmEdits READ confirmEdits WRITE setConfirmEdits )
    TQ_PROPERTY( bool confirmInsert READ confirmInsert WRITE setConfirmInsert )
    TQ_PROPERTY( bool confirmUpdate READ confirmUpdate WRITE setConfirmUpdate )
    TQ_PROPERTY( bool confirmDelete READ confirmDelete WRITE setConfirmDelete )
    TQ_PROPERTY( bool confirmCancels READ confirmCancels WRITE setConfirmCancels )
    TQ_PROPERTY( bool autoEdit READ autoEdit WRITE setAutoEdit )
    TQ_PROPERTY( TQString filter READ filter WRITE setFilter )
    TQ_PROPERTY( TQStringList sort READ sort WRITE setSort )
    TQ_PROPERTY( int numCols READ numCols )
    TQ_PROPERTY( int numRows READ numRows )

public:
    TQDataTable ( TQWidget* parent=0, const char* name=0 );
    TQDataTable ( TQSqlCursor* cursor, bool autoPopulate = false, TQWidget* parent=0, const char* name=0 );
    ~TQDataTable();

    virtual void addColumn( const TQString& fieldName,
			    const TQString& label = TQString::null,
			    int width = -1,
			    const TQIconSet& iconset = TQIconSet() );
    virtual void removeColumn( uint col );
    virtual void setColumn( uint col, const TQString& fieldName,
			    const TQString& label = TQString::null,
			    int width = -1,
			    const TQIconSet& iconset = TQIconSet() );

    TQString      nullText() const;
    TQString      trueText() const;
    TQString      falseText() const;
    DateFormat   dateFormat() const;
    bool         confirmEdits() const;
    bool         confirmInsert() const;
    bool         confirmUpdate() const;
    bool         confirmDelete() const;
    bool         confirmCancels() const;
    bool         autoDelete() const;
    bool         autoEdit() const;
    TQString      filter() const;
    TQStringList  sort() const;

    virtual void setSqlCursor( TQSqlCursor* cursor = 0,
			    bool autoPopulate = false, bool autoDelete = false );
    TQSqlCursor* sqlCursor() const;

    virtual void setNullText( const TQString& nullText );
    virtual void setTrueText( const TQString& trueText );
    virtual void setFalseText( const TQString& falseText );
    virtual void setDateFormat( const DateFormat f );
    virtual void setConfirmEdits( bool confirm );
    virtual void setConfirmInsert( bool confirm );
    virtual void setConfirmUpdate( bool confirm );
    virtual void setConfirmDelete( bool confirm );
    virtual void setConfirmCancels( bool confirm );
    virtual void setAutoDelete( bool enable );
    virtual void setAutoEdit( bool autoEdit );
    virtual void setFilter( const TQString& filter );
    virtual void setSort( const TQStringList& sort );
    virtual void setSort( const TQSqlIndex& sort );

    enum Refresh {
	RefreshData = 1,
	RefreshColumns = 2,
	RefreshAll = 3
    };
    void         refresh( Refresh mode );
    void         sortColumn ( int col, bool ascending = true,
			      bool wholeRows = false );
    TQString      text ( int row, int col ) const;
    TQVariant     value ( int row, int col ) const;
    TQSqlRecord*  currentRecord() const;

    void         installEditorFactory( TQSqlEditorFactory * f );
    void         installPropertyMap( TQSqlPropertyMap* m );

    int          numCols() const;
    int          numRows() const;
    void         setNumCols( int c );
    void         setNumRows ( int r );
    bool         findBuffer( const TQSqlIndex& idx, int atHint = 0 );

    void 	 hideColumn( int col );
    void 	 showColumn( int col );
signals:
    void         currentChanged( TQSqlRecord* record );
    void         primeInsert( TQSqlRecord* buf );
    void         primeUpdate( TQSqlRecord* buf );
    void         primeDelete( TQSqlRecord* buf );
    void         beforeInsert( TQSqlRecord* buf );
    void         beforeUpdate( TQSqlRecord* buf );
    void         beforeDelete( TQSqlRecord* buf );
    void         cursorChanged( TQSql::Op mode );

public slots:
    virtual void find( const TQString & str, bool caseSensitive,
			     bool backwards );
    virtual void sortAscending( int col );
    virtual void sortDescending( int col );
    virtual void refresh();
    void setColumnWidth( int col, int w );
    void adjustColumn( int col );
    void setColumnStretchable( int col, bool stretch );
    void swapColumns( int col1, int col2, bool swapHeaders = false );

protected:
    virtual bool insertCurrent();
    virtual bool updateCurrent();
    virtual bool deleteCurrent();

    virtual TQSql::Confirm confirmEdit( TQSql::Op m );
    virtual TQSql::Confirm confirmCancel( TQSql::Op m );

    virtual void handleError( const TQSqlError& e );

    virtual bool beginInsert();
    virtual TQWidget* beginUpdate ( int row, int col, bool replace );

    bool         eventFilter( TQObject *o, TQEvent *e );
    void         keyPressEvent( TQKeyEvent* );
    void         resizeEvent ( TQResizeEvent * );
    void         contentsMousePressEvent( TQMouseEvent* e );
    void         contentsContextMenuEvent( TQContextMenuEvent* e );
    void         endEdit( int row, int col, bool accept, bool replace );
    TQWidget *    createEditor( int row, int col, bool initFromCell ) const;
    void         activateNextCell();
    int          indexOf( uint i ) const; // ### make this public in 4.0
    void         reset();
    void         setSize( TQSqlCursor* sql );
    void         repaintCell( int row, int col );
    void         paintCell ( TQPainter * p, int row, int col, const TQRect & cr,
			     bool selected, const TQColorGroup &cg );
    virtual void paintField( TQPainter * p, const TQSqlField* field, const TQRect & cr,
			     bool selected );
    void	 drawContents( TQPainter * p, int cx, int cy, int cw, int ch );
    virtual int  fieldAlignment( const TQSqlField* field );
    void         columnClicked ( int col );
    void         resizeData ( int len );

    TQTableItem * item ( int row, int col ) const;
    void         setItem ( int row, int col, TQTableItem * item );
    void         clearCell ( int row, int col ) ;
    void         setPixmap ( int row, int col, const TQPixmap & pix );
    void         takeItem ( TQTableItem * i );

private slots:
    void         loadNextPage();
    void         setCurrentSelection( int row, int col );
    void         updateCurrentSelection();
    void 	 sliderPressed();
    void 	 sliderReleased();
    void	 doInsertCurrent();
    void	 doUpdateCurrent();
    
private:
    TQString 	 fieldToString( const TQSqlField * field );
    void         init();
    TQWidget*     beginEdit ( int row, int col, bool replace );
    void         updateRow( int row );
    void         endInsert();
    void         endUpdate();
    TQDataTablePrivate* d;

#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDataTable( const TQDataTable & );
    TQDataTable &operator=( const TQDataTable & );
#endif
};

#endif
#endif
