 /**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef DESIGNERINTERFACE_H
#define DESIGNERINTERFACE_H

//
//  W A R N I N G  --  PRIVATE INTERFACES
//  --------------------------------------
//
// This file and the interfaces declared in the file are not
// public. It exists for internal purpose. This header file and
// interfaces may change from version to version (even binary
// incompatible) without notice, or even be removed.
//
// We mean it.
//
//

#include <private/qcom_p.h>
#include <ntqptrlist.h>
#include <ntqstring.h>
#include <ntqmap.h>
#include <ntqstringlist.h>
#include <ntqwidgetlist.h>
#include <ntqobjectlist.h>

struct DesignerProject;
struct DesignerDatabase;
struct DesignerFormWindow;
struct DesignerDock;
struct DesignerOutputDock;
struct DesignerOutput;
struct DesignerOutputError;
struct DesignerPixmapCollection;
struct DesignerSourceFile;

class TQDockWindow;
class TQWidget;
class TQObject;
class TQAction;
class TQIconSet;
class TQSqlDatabase;

// {0e661da-f45c-4830-af47-03ec53eb1633}
#ifndef IID_Designer
#define IID_Designer TQUuid( 0xa0e661da, 0xf45c, 0x4830, 0xaf, 0x47, 0x3, 0xec, 0x53, 0xeb, 0x16, 0x33 )
#endif

/*! These are the interfaces implemented by the TQt Designer which should
  be used by plugins to access and use functionality of the TQt Designer.
*/

struct DesignerInterface : public TQUnknownInterface
{
    virtual DesignerProject *currentProject() const = 0;
    virtual DesignerFormWindow *currentForm() const = 0;
    virtual DesignerSourceFile *currentSourceFile() const = 0;
    virtual TQPtrList<DesignerProject> projectList() const = 0;
    virtual void showStatusMessage( const TQString &, int ms = 0 ) const = 0;
    virtual DesignerDock *createDock() const = 0;
    virtual DesignerOutputDock *outputDock() const = 0;
    virtual void setModified( bool b, TQWidget *window ) = 0;
    virtual void updateFunctionList() = 0;

    virtual void onProjectChange( TQObject *receiver, const char *slot ) = 0;
    virtual void onFormChange( TQObject *receiver, const char *slot ) = 0;

    virtual bool singleProjectMode() const = 0;
    virtual void showError( TQWidget *widget, int line, const TQString &message ) = 0;
    virtual void runFinished() = 0;
    virtual void showStackFrame( TQWidget *w, int line ) = 0;
    virtual void showDebugStep( TQWidget *w, int line ) = 0;
    virtual void runProjectPrecondition() = 0;
    virtual void runProjectPostcondition( TQObjectList *l ) = 0;

    };

struct DesignerProject
{
    DesignerProject() {}
    virtual ~DesignerProject() {}

    virtual TQPtrList<DesignerFormWindow> formList() const = 0;
    virtual TQStringList formNames() const = 0;
    virtual TQString formFileName( const TQString &form ) const = 0;
    virtual void addForm( DesignerFormWindow * ) = 0;
    virtual void removeForm( DesignerFormWindow * ) = 0;
    virtual TQString fileName() const = 0;
    virtual void setFileName( const TQString & ) = 0;
    virtual TQString projectName() const = 0;
    virtual void setProjectName( const TQString & ) = 0;
    virtual TQString databaseFile() const = 0;
    virtual void setDatabaseFile( const TQString & ) = 0;
    virtual void setupDatabases() const = 0;
    virtual TQPtrList<DesignerDatabase> databaseConnections() const = 0;
    virtual void addDatabase( DesignerDatabase * ) = 0;
    virtual void removeDatabase( DesignerDatabase * ) = 0;
    virtual void save() const = 0;
    virtual void setLanguage( const TQString & ) = 0;
    virtual TQString language() const = 0;
    virtual void setCustomSetting( const TQString &key, const TQString &value ) = 0;
    virtual TQString customSetting( const TQString &key ) const = 0;
    virtual DesignerPixmapCollection *pixmapCollection() const = 0;
    virtual void breakPoints( TQMap<TQString, TQValueList<uint> > &bps ) const = 0;
    virtual TQString breakPointCondition( TQObject *o, int line ) const = 0;
    virtual void setBreakPointCondition( TQObject *o, int line, const TQString &condition ) = 0;
    virtual void clearAllBreakpoints() const = 0;
    virtual void setIncludePath( const TQString &platform, const TQString &path ) = 0;
    virtual void setLibs( const TQString &platform, const TQString &path ) = 0;
    virtual void setDefines( const TQString &platform, const TQString &path ) = 0;
    virtual void setConfig( const TQString &platform, const TQString &config ) = 0;
    virtual void setTemplate( const TQString &t ) = 0;
    virtual TQString config( const TQString &platform ) const = 0;
    virtual TQString libs( const TQString &platform ) const = 0;
    virtual TQString defines( const TQString &platform ) const = 0;
    virtual TQString includePath( const TQString &platform ) const = 0;
    virtual TQString templte() const = 0;
    virtual bool isGenericObject( TQObject *o ) const = 0;

};

struct DesignerDatabase
{
    DesignerDatabase() {}
    virtual ~DesignerDatabase() {}

    virtual TQString name() const = 0;
    virtual void setName( const TQString & ) = 0;
    virtual TQString driver() const = 0;
    virtual void setDriver( const TQString & ) = 0;
    virtual TQString database() const = 0;
    virtual void setDatabase( const TQString & ) = 0;
    virtual TQString userName() const = 0;
    virtual void setUserName( const TQString & ) = 0;
    virtual TQString password() const = 0;
    virtual void setPassword( const TQString & ) = 0;
    virtual TQString hostName() const = 0;
    virtual void setHostName( const TQString & ) = 0;
    virtual TQStringList tables() const = 0;
    virtual void setTables( const TQStringList & ) = 0;
    virtual TQMap<TQString, TQStringList> fields() const = 0;
    virtual void setFields( const TQMap<TQString, TQStringList> & ) = 0;
    virtual void open( bool suppressDialog = false ) const = 0;
    virtual void close() const = 0;
    virtual TQSqlDatabase* connection() = 0;

};

struct DesignerPixmapCollection
{
    DesignerPixmapCollection() {}
    virtual ~DesignerPixmapCollection() {}

    virtual void addPixmap( const TQPixmap &p, const TQString &name, bool force ) = 0;
    virtual TQPixmap pixmap( const TQString &name ) const = 0;
};

struct DesignerFormWindow
{
    DesignerFormWindow() {}
    virtual ~DesignerFormWindow() {}

    virtual TQString name() const = 0;
    virtual void setName( const TQString &n ) = 0;
    virtual TQString fileName() const = 0;
    virtual void setFileName( const TQString & ) = 0;
    virtual void save() const = 0;
    virtual bool isModified() const = 0;
    virtual void insertWidget( TQWidget * ) = 0;
    virtual void removeWidget( TQWidget * ) = 0;
    virtual TQWidget *create( const char *className, TQWidget *parent, const char *name ) = 0;
    virtual TQWidgetList widgets() const = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void cut() = 0;
    virtual void copy() = 0;
    virtual void paste() = 0;
    virtual void adjustSize() = 0;
    virtual void editConnections() = 0;
    virtual void checkAccels() = 0;
    virtual void layoutH() = 0;
    virtual void layoutV() = 0;
    virtual void layoutHSplit() = 0;
    virtual void layoutVSplit() = 0;
    virtual void layoutG() = 0;
    virtual void layoutHContainer( TQWidget* w ) = 0;
    virtual void layoutVContainer( TQWidget* w ) = 0;
    virtual void layoutGContainer( TQWidget* w ) = 0;
    virtual void breakLayout() = 0;
    virtual void selectWidget( TQWidget * w ) = 0;
    virtual void selectAll() = 0;
    virtual void clearSelection() = 0;
    virtual bool isWidgetSelected( TQWidget * ) const = 0;
    virtual TQWidgetList selectedWidgets() const = 0;
    virtual TQWidget *currentWidget() const = 0;
    virtual TQWidget *form() const = 0;
    virtual void setCurrentWidget( TQWidget * ) = 0;
    virtual TQPtrList<TQAction> actionList() const = 0;
    virtual TQAction *createAction( const TQString& text, const TQIconSet& icon, const TQString& menuText, int accel,
				   TQObject* parent, const char* name = 0, bool toggle = false ) = 0;
    virtual void addAction( TQAction * ) = 0;
    virtual void removeAction( TQAction * ) = 0;
    virtual void preview() const = 0;
    virtual void addFunction( const TQCString &slot, const TQString &specifier, const TQString &access,
			     const TQString &type, const TQString &language, const TQString &returnType ) = 0;
    virtual void addConnection( TQObject *sender, const char *signal, TQObject *receiver, const char *slot ) = 0;
    virtual void setProperty( TQObject *o, const char *property, const TQVariant &value ) = 0;
    virtual TQVariant property( TQObject *o, const char *property ) const = 0;
    virtual void setPropertyChanged( TQObject *o, const char *property, bool changed ) = 0;
    virtual bool isPropertyChanged( TQObject *o, const char *property ) const = 0;
    virtual void setColumnFields( TQObject *o, const TQMap<TQString, TQString> & ) = 0;
    virtual TQStringList implementationIncludes() const = 0;
    virtual TQStringList declarationIncludes() const = 0;
    virtual void setImplementationIncludes( const TQStringList &lst ) = 0;
    virtual void setDeclarationIncludes( const TQStringList &lst ) = 0;
    virtual TQStringList forwardDeclarations() const = 0;
    virtual void setForwardDeclarations( const TQStringList &lst ) = 0;
    virtual TQStringList signalList() const = 0;
    virtual void setSignalList( const TQStringList &lst ) = 0;
    virtual void addMenu( const TQString &text, const TQString &name ) = 0;
    virtual void addMenuAction( const TQString &menu, TQAction *a ) = 0;
    virtual void addMenuSeparator( const TQString &menu ) = 0;
    virtual void addToolBar( const TQString &text, const TQString &name ) = 0;
    virtual void addToolBarAction( const TQString &tb, TQAction *a ) = 0;
    virtual void addToolBarSeparator( const TQString &tb ) = 0;

    virtual void onModificationChange( TQObject *receiver, const char *slot ) = 0;
};

struct DesignerSourceFile
{
    DesignerSourceFile() {}
    virtual ~DesignerSourceFile() {}

    virtual TQString fileName() const = 0;
};

struct DesignerDock
{
    DesignerDock() {}
    virtual ~DesignerDock() {}

    virtual TQDockWindow *dockWindow() const = 0;
};

struct DesignerOutputDock
{
    DesignerOutputDock() {}
    virtual ~DesignerOutputDock() {}

    virtual TQWidget *addView( const TQString &pageName ) = 0;
    virtual void appendDebug( const TQString & ) = 0;
    virtual void clearDebug() = 0;
    virtual void appendError( const TQString &, int ) = 0;
    virtual void clearError() = 0;
};

#endif
