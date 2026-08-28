/****************************************************************************
**
** Definition of TQMenuData class
**
** Created : 941128
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#ifndef TQMENUDATA_H
#define TQMENUDATA_H

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqiconset.h" // conversion TQPixmap->TQIconset
#include "ntqkeysequence.h"
#include "ntqstring.h"
#include "ntqsignal.h"
#include "ntqfont.h"
#endif // QT_H

#ifndef TQT_NO_MENUDATA

class TQPopupMenu;
class TQMenuDataData;
class TQObject;

class TQCustomMenuItem;
class TQMenuItemData;

class TQ_EXPORT TQMenuItem			// internal menu item class
{
friend class TQMenuData;
public:
    TQMenuItem();
   ~TQMenuItem();

    int		id()		const	{ return ident; }
    TQIconSet   *iconSet()	const	{ return iconset_data; }
    TQString	text()		const	{ return text_data; }
    TQString	whatsThis()	const	{ return whatsthis_data; }
    TQPixmap    *pixmap()	const	{ return pixmap_data; }
    TQPopupMenu *popup()		const	{ return popup_menu; }
    TQWidget *widget()		const	{ return widget_item; }
    TQCustomMenuItem *custom()	const;
#ifndef TQT_NO_ACCEL
    TQKeySequence key()		const	{ return accel_key; }
#endif
    TQSignal    *signal()	const	{ return signal_data; }
    bool	isSeparator()	const	{ return is_separator; }
    bool	isEnabled()	const	{ return is_enabled; }
    bool	isChecked()	const	{ return is_checked; }
    bool	isDirty()	const	{ return is_dirty; }
    bool	isVisible()	const	{ return is_visible; }
    bool	isEnabledAndVisible() const { return is_enabled && is_visible; }

    void	setText( const TQString &text ) { text_data = text; }
    void	setDirty( bool dirty )	       { is_dirty = dirty; }
    void	setVisible( bool visible )	       { is_visible = visible; }
    void	setWhatsThis( const TQString &text ) { whatsthis_data = text; }

    // Do not use these methods unless you know exactly what you are doing!
    void	setSeparator( bool separator )   { is_separator = separator; }
    void	setEnabled( bool enabled )       { is_enabled = enabled; }
    void	setChecked( bool checked )       { is_checked = checked; }
    void	setIdent( int identifier )       { ident = identifier; }
    void	setIconSet( TQIconSet* iconset ) { iconset_data = iconset; }
    void	setPixmap( TQPixmap* pixmap )    { pixmap_data = pixmap; }
    void	setPopup( TQPopupMenu* popup )   { popup_menu = popup; }
    void	setWidget( TQWidget* widget )    { widget_item = widget; }
#ifndef TQT_NO_ACCEL
    void	setKey( TQKeySequence key )      { accel_key = key; }
#endif
    void	setSignal( TQSignal* signal )    { signal_data = signal; }

private:
    int		ident;				// item identifier
    TQIconSet   *iconset_data;			// icons
    TQString	text_data;			// item text
    TQString	whatsthis_data;			// item Whats This help text
    TQPixmap    *pixmap_data;			// item pixmap
    TQPopupMenu *popup_menu;			// item popup menu
    TQWidget    *widget_item;			// widget menu item
#ifndef TQT_NO_ACCEL
    TQKeySequence	accel_key;		// accelerator key (state|ascii)
#endif
    TQSignal    *signal_data;			// connection
    uint	is_separator : 1;		// separator flag
    uint	is_enabled   : 1;		// disabled flag
    uint	is_checked   : 1;		// checked flag
    uint	is_dirty     : 1;		// dirty (update) flag
    uint	is_visible     : 1;		// visibility flag
    TQMenuItemData* d;

    TQMenuItemData* extra();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQMenuItem( const TQMenuItem & );
    TQMenuItem &operator=( const TQMenuItem & );
#endif
};

#include "ntqptrlist.h"
typedef TQPtrList<TQMenuItem>	 TQMenuItemList;
typedef TQPtrListIterator<TQMenuItem> TQMenuItemListIt;


class TQ_EXPORT TQCustomMenuItem : public TQt
{
public:
    TQCustomMenuItem();
    virtual ~TQCustomMenuItem();
    virtual bool fullSpan() const;
    virtual bool isSeparator() const;
    virtual void setFont( const TQFont& font );
    virtual void paint( TQPainter* p, const TQColorGroup& cg, bool act,
			bool enabled, int x, int y, int w, int h ) = 0;
    virtual TQSize sizeHint() = 0;
};


class TQ_EXPORT TQMenuData			// menu data class
{
friend class TQMenuBar;
friend class TQPopupMenu;
public:
    TQMenuData();
    virtual ~TQMenuData();

    uint	count() const;


    int		insertItem( const TQString &text,
			    const TQObject *receiver, const char* member,
			    const TQKeySequence& accel = 0, int id = -1, int index = -1 );
    int		insertItem( const TQIconSet& icon,
			    const TQString &text,
			    const TQObject *receiver, const char* member,
			    const TQKeySequence& accel = 0, int id = -1, int index = -1 );
    int		insertItem( const TQPixmap &pixmap,
			    const TQObject *receiver, const char* member,
			    const TQKeySequence& accel = 0, int id = -1, int index = -1 );
    int		insertItem( const TQIconSet& icon,
			    const TQPixmap &pixmap,
			    const TQObject *receiver, const char* member,
			    const TQKeySequence& accel = 0, int id = -1, int index = -1 );

    int		insertItem( const TQString &text, int id=-1, int index=-1 );
    int		insertItem( const TQIconSet& icon,
			    const TQString &text, int id=-1, int index=-1 );

    int		insertItem( const TQString &text, TQPopupMenu *popup,
			    int id=-1, int index=-1 );
    int		insertItem( const TQIconSet& icon,
			    const TQString &text, TQPopupMenu *popup,
			    int id=-1, int index=-1 );


    int		insertItem( const TQPixmap &pixmap, int id=-1, int index=-1 );
    int		insertItem( const TQIconSet& icon,
			    const TQPixmap &pixmap, int id=-1, int index=-1 );
    int		insertItem( const TQPixmap &pixmap, TQPopupMenu *popup,
			    int id=-1, int index=-1 );
    int		insertItem( const TQIconSet& icon,
			    const TQPixmap &pixmap, TQPopupMenu *popup,
			    int id=-1, int index=-1 );

    int		insertItem( TQWidget* widget, int id=-1, int index=-1 );

    int		insertItem( const TQIconSet& icon, TQCustomMenuItem* custom, int id=-1, int index=-1 );
    int		insertItem( TQCustomMenuItem* custom, int id=-1, int index=-1 );


    int		insertSeparator( int index=-1 );

    void	removeItem( int id );
    void	removeItemAt( int index );
    void	clear();

#ifndef TQT_NO_ACCEL
    TQKeySequence accel( int id )	const;
    void	setAccel( const TQKeySequence& key, int id );
#endif

    TQIconSet    *iconSet( int id )	const;
    TQString text( int id )		const;
    TQPixmap    *pixmap( int id )	const;

    void setWhatsThis( int id, const TQString& );
    TQString whatsThis( int id ) const;


    void	changeItem( int id, const TQString &text );
    void	changeItem( int id, const TQPixmap &pixmap );
    void	changeItem( int id, const TQIconSet &icon, const TQString &text );
    void	changeItem( int id, const TQIconSet &icon, const TQPixmap &pixmap );

    void	changeItem( const TQString &text, int id ) { changeItem( id, text); } // obsolete
    void	changeItem( const TQPixmap &pixmap, int id ) { changeItem( id, pixmap ); } // obsolete
    void	changeItem( const TQIconSet &icon, const TQString &text, int id ) {	// obsolete
	changeItem( id, icon, text );
    }

    bool	isItemActive( int id ) const;

    bool	isItemEnabled( int id ) const;
    void	setItemEnabled( int id, bool enable );

    bool	isItemChecked( int id ) const;
    void	setItemChecked( int id, bool check );

    bool	isItemVisible( int id ) const;
    void	setItemVisible( int id, bool visible );

    virtual void updateItem( int id );

    int		indexOf( int id )	const;
    int		idAt( int index )	const;
    virtual void	setId( int index, int id );

    bool	connectItem( int id,
			     const TQObject *receiver, const char* member );
    bool	disconnectItem( int id,
				const TQObject *receiver, const char* member );

    bool	setItemParameter( int id, int param );
    int	itemParameter( int id ) const;

    TQMenuItem  *findItem( int id )	const;
    TQMenuItem  *findItem( int id, TQMenuData ** parent )	const;
    TQMenuItem * findPopup( TQPopupMenu *, int *index = 0 );

    virtual void activateItemAt( int index );

protected:
    int		   actItem;
    TQMenuItemList *mitems;
    TQMenuData	  *parentMenu;
    uint	   isPopupMenu	: 1;
    uint	   isMenuBar	: 1;
    uint	   badSize	: 1;
    uint	   mouseBtDn	: 1;
    uint	avoid_circularity : 1;
    uint	actItemDown : 1;
    virtual void   menuContentsChanged();
    virtual void   menuStateChanged();
    virtual void   menuInsPopup( TQPopupMenu * );
    virtual void   menuDelPopup( TQPopupMenu * );

private:
    int		insertAny( const TQString *, const TQPixmap *, TQPopupMenu *,
			   const TQIconSet*, int, int, TQWidget* = 0, TQCustomMenuItem* = 0);
    void	removePopup( TQPopupMenu * );
    void	changeItemIconSet( int id, const TQIconSet &icon );

    TQMenuDataData *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQMenuData( const TQMenuData & );
    TQMenuData &operator=( const TQMenuData & );
#endif
};


#endif // TQT_NO_MENUDATA

#endif // TQMENUDATA_H
