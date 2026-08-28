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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <ntqstring.h>
#include <ntqtextstream.h>
#include <ntqvariant.h>
#include <ntqvaluelist.h>
#include <ntqimage.h>
#include "actiondnd.h"

#include "metadatabase.h"

class TQWidget;
class TQObject;
class TQLayout;
class TQStyle;
class TQPalette;
class FormWindow;
class MainWindow;
class TQDomElement;
class TQDesignerGridLayout;
class TQListViewItem;
class TQMainWindow;
struct LanguageInterface;
class FormFile;
class Project;
class PopupMenuEditor;

class Resource
{
public:
    struct Image {
	TQImage img;
	TQString name;
	bool operator==(  const Image &i ) const {
	    return ( i.name == name &&
		     i.img == img );
	}
    };

    Resource();
    Resource( MainWindow* mw );
    ~Resource();

    void setWidget( FormWindow *w );
    TQWidget *widget() const;

    bool load( FormFile *ff, Project *defProject = 0 );
    bool load( FormFile *ff, TQIODevice*, Project *defProject = 0 );
    TQString copy();

    bool save( const TQString& filename, bool formCodeOnly = false);
    bool save( TQIODevice* );
    void paste( const TQString &cb, TQWidget *parent );

    static void saveImageData( const TQImage &img, TQTextStream &ts, int indent );
    static void loadCustomWidgets( const TQDomElement &e, Resource *r );
    static void loadExtraSource( FormFile *formfile, const TQString &currFileName,
				 LanguageInterface *langIface, bool hasFunctions );
    static bool saveFormCode( FormFile *formfile, LanguageInterface *langIface );

private:
    void saveObject( TQObject *obj, TQDesignerGridLayout* grid, TQTextStream &ts, int indent );
    void saveChildrenOf( TQObject* obj, TQTextStream &ts, int indent );
    void saveObjectProperties( TQObject *w, TQTextStream &ts, int indent );
    void saveSetProperty( TQObject *w, const TQString &name, TQVariant::Type t, TQTextStream &ts, int indent );
    void saveEnumProperty( TQObject *w, const TQString &name, TQVariant::Type t, TQTextStream &ts, int indent );
    void saveProperty( TQObject *w, const TQString &name, const TQVariant &value, TQVariant::Type t, TQTextStream &ts, int indent );
    void saveProperty( const TQVariant &value, TQTextStream &ts, int indent );
    void saveItems( TQObject *obj, TQTextStream &ts, int indent );
    void saveItem( const TQStringList &text, const TQPtrList<TQPixmap> &pixmaps, TQTextStream &ts, int indent );
    void saveItem( TQListViewItem *i, TQTextStream &ts, int indent );
    void saveConnections( TQTextStream &ts, int indent );
    void saveCustomWidgets( TQTextStream &ts, int indent );
    void saveTabOrder( TQTextStream &ts, int indent );
    void saveColorGroup( TQTextStream &ts, int indent, const TQColorGroup &cg );
    void saveColor( TQTextStream &ts, int indent, const TQColor &c );
    void saveMetaInfoBefore( TQTextStream &ts, int indent );
    void saveMetaInfoAfter( TQTextStream &ts, int indent );
    void savePixmap( const TQPixmap &p, TQTextStream &ts, int indent, const TQString &tagname = "pixmap" );
    void saveActions( const TQPtrList<TQAction> &actions, TQTextStream &ts, int indent );
    void saveChildActions( TQAction *a, TQTextStream &ts, int indent );
    void saveToolBars( TQMainWindow *mw, TQTextStream &ts, int indent );
    void saveMenuBar( TQMainWindow *mw, TQTextStream &ts, int indent );
    void savePopupMenu( PopupMenuEditor *pm, TQMainWindow *mw, TQTextStream &ts, int indent );

    TQObject *createObject( const TQDomElement &e, TQWidget *parent, TQLayout* layout = 0 );
    TQWidget *createSpacer( const TQDomElement &e, TQWidget *parent, TQLayout *layout, TQt::Orientation o );
    void createItem( const TQDomElement &e, TQWidget *widget, TQListViewItem *i = 0 );
    void createColumn( const TQDomElement &e, TQWidget *widget );
    void setObjectProperty( TQObject* widget, const TQString &prop, const TQDomElement &e);
    TQString saveInCollection( const TQImage &img );
    TQString saveInCollection( const TQPixmap &pix ) { return saveInCollection( pix.convertToImage() ); }
    TQImage loadFromCollection( const TQString &name );
    void saveImageCollection( TQTextStream &ts, int indent );
    void loadImageCollection( const TQDomElement &e );
    void loadConnections( const TQDomElement &e );
    void loadTabOrder( const TQDomElement &e );
    void loadItem( const TQDomElement &n, TQPixmap &pix, TQString &txt, bool &hasPixmap );
    void loadActions( const TQDomElement &n );
    void loadChildAction( TQObject *parent, const TQDomElement &e );
    void loadToolBars( const TQDomElement &n );
    void loadMenuBar( const TQDomElement &n );
    void loadPopupMenu( PopupMenuEditor *pm, const TQDomElement &e );
    TQColorGroup loadColorGroup( const TQDomElement &e );
    TQPixmap loadPixmap( const TQDomElement &e, const TQString &tagname = "pixmap" );

private:
    MainWindow *mainwindow;
    FormWindow *formwindow;
    TQWidget* toplevel;
    TQValueList<Image> images;
    bool copying, pasting;
    bool mainContainerSet;
    TQStringList knownNames;
    TQStringList usedCustomWidgets;
    TQListViewItem *lastItem;

    TQValueList<MetaDataBase::Include> metaIncludes;
    TQValueList<MetaDataBase::Variable> metaVariables;
    TQStringList metaForwards;
    TQStringList metaSignals;
    MetaDataBase::MetaInfo metaInfo;
    TQMap<TQString, TQString> dbControls;
    TQMap<TQString, TQStringList> dbTables;
    TQMap<TQString, TQWidget*> widgets;
    TQString exportMacro;
    bool hadGeometry;
    TQMap<TQString, TQValueList<MetaDataBase::Connection> > langConnections;
    TQString currFileName;
    LanguageInterface *langIface;
    bool hasFunctions;

    TQString uiFileVersion;
};

#endif
