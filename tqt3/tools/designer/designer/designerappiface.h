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

#ifndef DESIGNERAPPIFACE_H
#define DESIGNERAPPIFACE_H

#include "../interfaces/designerinterface.h"
#include "project.h"

class FormWindow;
class MainWindow;
class Project;
class OutputWindow;
class SourceFile;

class DesignerInterfaceImpl : public DesignerInterface
{
public:
    DesignerInterfaceImpl( MainWindow *mw );
    virtual ~DesignerInterfaceImpl();

    DesignerProject *currentProject() const;
    DesignerFormWindow *currentForm() const;
    DesignerSourceFile *currentSourceFile() const;
    TQPtrList<DesignerProject> projectList() const;
    void showStatusMessage( const TQString &, int ms = 0 ) const;
    DesignerDock *createDock() const;
    DesignerOutputDock *outputDock() const;
    void setModified( bool b, TQWidget *window );
    void updateFunctionList();

    void onProjectChange( TQObject *receiver, const char *slot );
    void onFormChange( TQObject *receiver, const char *slot );

    bool singleProjectMode() const;
    void showError( TQWidget *widget, int line, const TQString &message );
    void runFinished();
    void showStackFrame( TQWidget *w, int line );
    void showDebugStep( TQWidget *w, int line );
    void runProjectPrecondition();
    void runProjectPostcondition( TQObjectList *l );


    TQRESULT queryInterface( const TQUuid &uuid, TQUnknownInterface** iface );
    TQ_REFCOUNT

private:
    MainWindow *mainWindow;

};

class DesignerProjectImpl: public DesignerProject
{
public:
    DesignerProjectImpl( Project *pr );

    TQPtrList<DesignerFormWindow> formList() const;
    TQStringList formNames() const;
    TQString formFileName( const TQString &form ) const;
    void addForm( DesignerFormWindow * );
    void removeForm( DesignerFormWindow * );
    TQString fileName() const;
    void setFileName( const TQString & );
    TQString projectName() const;
    void setProjectName( const TQString & );
    TQString databaseFile() const;
    void setDatabaseFile( const TQString & );
    void setupDatabases() const;
    TQPtrList<DesignerDatabase> databaseConnections() const;
    void addDatabase( DesignerDatabase * );
    void removeDatabase( DesignerDatabase * );
    void save() const;
    void setLanguage( const TQString & );
    TQString language() const;
    void setCustomSetting( const TQString &key, const TQString &value );
    TQString customSetting( const TQString &key ) const;
    DesignerPixmapCollection *pixmapCollection() const;
    void breakPoints( TQMap<TQString, TQValueList<uint> > &bps ) const;
    TQString breakPointCondition( TQObject *o, int line ) const;
    void setBreakPointCondition( TQObject *o, int line, const TQString &condition );
    void clearAllBreakpoints() const;
    void setIncludePath( const TQString &platform, const TQString &path );
    void setLibs( const TQString &platform, const TQString &path );
    void setDefines( const TQString &platform, const TQString &path );
    void setConfig( const TQString &platform, const TQString &config );
    void setTemplate( const TQString &t );
    TQString config( const TQString &platform ) const;
    TQString libs( const TQString &platform ) const;
    TQString defines( const TQString &platform ) const;
    TQString includePath( const TQString &platform ) const;
    TQString templte() const;
    bool isGenericObject( TQObject *o ) const;

private:
    Project *project;

};

#ifndef TQT_NO_SQL
class TQSqlDatabase;

class DesignerDatabaseImpl: public DesignerDatabase
{
public:
    DesignerDatabaseImpl( DatabaseConnection *d );

    TQString name() const;
    void setName( const TQString & );
    TQString driver() const;
    void setDriver( const TQString & );
    TQString database() const;
    void setDatabase( const TQString & );
    TQString userName() const;
    void setUserName( const TQString & );
    TQString password() const;
    void setPassword( const TQString & );
    TQString hostName() const;
    void setHostName( const TQString & );
    TQStringList tables() const;
    void setTables( const TQStringList & );
    TQMap<TQString, TQStringList> fields() const;
    void setFields( const TQMap<TQString, TQStringList> & );
    void open( bool suppressDialog = true ) const;
    void close() const;
    TQSqlDatabase* connection();
private:
    DatabaseConnection *db;

};
#endif

class DesignerPixmapCollectionImpl : public DesignerPixmapCollection
{
public:
    DesignerPixmapCollectionImpl( PixmapCollection *coll );
    void addPixmap( const TQPixmap &p, const TQString &name, bool force );
    TQPixmap pixmap( const TQString &name ) const;

private:
    PixmapCollection *pixCollection;

};

class DesignerFormWindowImpl: public DesignerFormWindow
{
public:
    DesignerFormWindowImpl( FormWindow *fw );

    TQString name() const;
    void setName( const TQString &n );
    TQString fileName() const;
    void setFileName( const TQString & );
    void save() const;
    bool isModified() const;
    void insertWidget( TQWidget * );
    TQWidget *create( const char *className, TQWidget *parent, const char *name );
    void removeWidget( TQWidget * );
    TQWidgetList widgets() const;
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void adjustSize();
    void editConnections();
    void checkAccels();
    void layoutH();
    void layoutV();
    void layoutHSplit();
    void layoutVSplit();
    void layoutG();
    void layoutHContainer( TQWidget* w );
    void layoutVContainer( TQWidget* w );
    void layoutGContainer( TQWidget* w );
    void breakLayout();
    void selectWidget( TQWidget * );
    void selectAll();
    void clearSelection();
    bool isWidgetSelected( TQWidget * ) const;
    TQWidgetList selectedWidgets() const;
    TQWidget *currentWidget() const;
    TQWidget *form() const;
    void setCurrentWidget( TQWidget * );
    TQPtrList<TQAction> actionList() const;
    TQAction *createAction( const TQString& text, const TQIconSet& icon, const TQString& menuText, int accel,
			   TQObject* parent, const char* name = 0, bool toggle = false );
    void addAction( TQAction * );
    void removeAction( TQAction * );
    void preview() const;
    void addFunction( const TQCString &function, const TQString& specifier, const TQString &access,
		      const TQString &type, const TQString &language, const TQString &returnType );
    void addConnection( TQObject *sender, const char *signal, TQObject *receiver, const char *slot );
    void setProperty( TQObject *o, const char *property, const TQVariant &value );
    TQVariant property( TQObject *o, const char *property ) const;
    void setPropertyChanged( TQObject *o, const char *property, bool changed );
    bool isPropertyChanged( TQObject *o, const char *property ) const;
    void setColumnFields( TQObject *o, const TQMap<TQString, TQString> & );
    TQStringList implementationIncludes() const;
    TQStringList declarationIncludes() const;
    void setImplementationIncludes( const TQStringList &lst );
    void setDeclarationIncludes( const TQStringList &lst );
    TQStringList forwardDeclarations() const;
    void setForwardDeclarations( const TQStringList &lst );
    TQStringList signalList() const;
    void setSignalList( const TQStringList &lst );
    void addMenu( const TQString &text, const TQString &name );
    void addMenuAction( const TQString &menu, TQAction *a );
    void addMenuSeparator( const TQString &menu );
    void addToolBar( const TQString &text, const TQString &name );
    void addToolBarAction( const TQString &tb, TQAction *a );
    void addToolBarSeparator( const TQString &tb );

    void onModificationChange( TQObject *receiver, const char *slot );

private:
    FormWindow *formWindow;

};

class DesignerDockImpl: public DesignerDock
{
public:
    DesignerDockImpl();

    TQDockWindow *dockWindow() const;
};

class DesignerOutputDockImpl: public DesignerOutputDock
{
public:
    DesignerOutputDockImpl( OutputWindow *ow );

    TQWidget *addView( const TQString &pageName );
    void appendDebug( const TQString & );
    void clearDebug();
    void appendError( const TQString &, int );
    void clearError();

private:
    OutputWindow *outWin;

};

class DesignerSourceFileImpl : public DesignerSourceFile
{
public:
    DesignerSourceFileImpl( SourceFile *e );
    TQString fileName() const;

private:
    SourceFile *ed;

};

#endif
