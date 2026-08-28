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

#ifndef PROJECT_H
#define PROJECT_H

#include <ntqfeatures.h>

#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqptrlist.h>
#include <ntqmap.h>
#include <private/qpluginmanager_p.h>
#include "../interfaces/projectsettingsiface.h"
#include "sourcefile.h"
#include "formfile.h"
#include <ntqobjectlist.h>
#include <ntqptrdict.h>
#include "formfile.h"

class TQObjectList;
struct DesignerProject;
struct DesignerDatabase;
class PixmapCollection;
class Project;

#ifndef TQT_NO_SQL
class TQSqlDatabase;

class DatabaseConnection
{
public:
    DatabaseConnection( Project *p ) :
#ifndef TQT_NO_SQL
	conn( 0 ),
#endif
	project( p ), loaded( false ), iface( 0 ) {}
    ~DatabaseConnection();

    bool refreshCatalog();
    bool open( bool suppressDialog = true );
    void close();
    DesignerDatabase *iFace();

    bool isLoaded() const { return loaded; }
    void setName( const TQString& n ) { nm = n; }
    TQString name() const { return nm; }
    void setDriver( const TQString& d ) { drv = d; }
    TQString driver() const { return drv; }
    void setDatabase( const TQString& db ) { dbName = db; }
    TQString database() const { return dbName; }
    void setUsername( const TQString& u ) { uname = u; }
    TQString username() const { return uname; }
    void setPassword( const TQString& p ) { pword = p; }
    TQString password() const { return pword; }
    void setHostname( const TQString& h ) { hname = h; }
    TQString hostname() const { return hname; }
    void setPort( int p ) { prt = p; }
    int port() const { return prt; }
    TQString lastError() const { return dbErr; }
    void addTable( const TQString& t ) { tbls.append(t); }
    void setFields( const TQString& t, const TQStringList& f ) { flds[t] = f; }
    TQStringList tables() const { return tbls; }
    TQStringList fields( const TQString& t ) { return flds[t]; }
    TQMap<TQString, TQStringList> fields() { return flds; }
#ifndef TQT_NO_SQL
    TQSqlDatabase* connection() const { return conn; }
    void remove();
#endif

private:
    TQString nm;
    TQString drv, dbName, uname, pword, hname;
    TQString dbErr;
    int prt;
    TQStringList tbls;
    TQMap<TQString, TQStringList> flds;
#ifndef TQT_NO_SQL
    TQSqlDatabase *conn;
#endif
    Project *project;
    bool loaded;
    DesignerDatabase *iface;
};

#endif

class Project : public TQObject
{
    TQ_OBJECT
    friend class DatabaseConnection;

public:
    Project( const TQString &fn, const TQString &pName = TQString::null,
	     TQPluginManager<ProjectSettingsInterface> *pm = 0, bool isDummy = false,
	     const TQString &l = "C++" );
    ~Project();

    void setFileName( const TQString &fn, bool doClear = true );
    TQString fileName( bool singlePro = false ) const;
    TQString projectName() const;

    void setDatabaseDescription( const TQString &db );
    TQString databaseDescription() const;

    void setDescription( const TQString &s );
    TQString description() const;

    void setLanguage( const TQString &l );
    TQString language() const;


    bool isValid() const;

    // returns true if this project is the <No Project> project
    bool isDummy() const;

    TQString makeAbsolute( const TQString &f );
    TQString makeRelative( const TQString &f );

    void save( bool onlyProjectFile = false );

#ifndef TQT_NO_SQL
    TQPtrList<DatabaseConnection> databaseConnections() const;
    void setDatabaseConnections( const TQPtrList<DatabaseConnection> &lst );
    void addDatabaseConnection( DatabaseConnection *conn );
    void removeDatabaseConnection( const TQString &conn );
    DatabaseConnection *databaseConnection( const TQString &name );
    TQStringList databaseConnectionList();
    TQStringList databaseTableList( const TQString &connection );
    TQStringList databaseFieldList( const TQString &connection, const TQString &table );
#endif
    void saveConnections();
    void loadConnections();

    bool openDatabase( const TQString &connection, bool suppressDialog = true );
    void closeDatabase( const TQString &connection );

    TQObjectList *formList( bool resolveFakeObjects = false ) const;

    DesignerProject *iFace();

    void setCustomSetting( const TQString &key, const TQString &value );
    TQString customSetting( const TQString &key ) const;

    PixmapCollection *pixmapCollection() const { return pixCollection; }

    void setActive( bool b );

    TQPtrListIterator<SourceFile> sourceFiles() const { return TQPtrListIterator<SourceFile>(sourcefiles); }
    void addSourceFile( SourceFile *sf );
    bool removeSourceFile( SourceFile *sf );
    SourceFile* findSourceFile( const TQString& filename, SourceFile *ignore = 0 ) const;

    TQPtrListIterator<FormFile> formFiles() const { return TQPtrListIterator<FormFile>(formfiles); }
    void addFormFile( FormFile *ff );
    bool removeFormFile( FormFile *ff );
    FormFile* findFormFile( const TQString& filename, FormFile *ignore = 0 ) const;

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

    bool isModified() const { return !isDummy() && modified; }
    void setModified( bool b );

    void addObject( TQObject *o );
    void setObjects( const TQObjectList &ol );
    void removeObject( TQObject *o );
    TQObjectList objects() const;
    FormFile *fakeFormFileFor( TQObject *o ) const;
    TQObject *objectForFakeForm( FormWindow *fw ) const;
    TQObject *objectForFakeFormFile( FormFile *ff ) const;

    void addAndEditFunction( const TQString &functionName, const TQString &functionBody,
			     bool openDeveloper );

    void removeTempProject();
    bool hasParentObject( TQObject *o );
    TQString qualifiedName( TQObject *o );

    bool isCpp() const { return is_cpp; }

    void designerCreated();

    void formOpened( FormWindow *fw );

    TQString locationOfObject( TQObject *o );

    bool hasGUI() const;

signals:
    void projectModified();
    void sourceFileAdded( SourceFile* );
    void sourceFileRemoved( SourceFile* );
    void formFileAdded( FormFile* );
    void formFileRemoved( FormFile* );
    void objectAdded( TQObject * );
    void objectRemoved( TQObject * );
    void newFormOpened( FormWindow *fw );

private:
    void parse();
    void clear();
    void updateCustomSettings();
    void readPlatformSettings( const TQString &contents,
			       const TQString &setting,
			       TQMap<TQString, TQString> &res );
    void removePlatformSettings( TQString &contents, const TQString &setting );
    void writePlatformSettings( TQString &contents, const TQString &setting,
				const TQMap<TQString, TQString> &input );
    bool singleProjectMode() const;
    TQWidget *messageBoxParent() const;

private:
    TQString filename;
    TQString proName;
    TQString desc;
    TQString dbFile;
#ifndef TQT_NO_SQL
    TQPtrList<DatabaseConnection> dbConnections;
#endif
    TQString lang;
    DesignerProject *iface;
    TQMap<TQString, TQString> customSettings;
    TQStringList csList;
    TQPluginManager<ProjectSettingsInterface> *projectSettingsPluginManager;
    PixmapCollection *pixCollection;
    TQPtrList<SourceFile> sourcefiles;
    TQPtrList<FormFile> formfiles;
    TQMap<TQString, TQString> inclPath, defs, lbs, cfg, sources, headers;
    TQString templ;
    bool isDummyProject;
    bool modified;
    TQObjectList objs;
    TQPtrDict<FormFile> fakeFormFiles;
    TQString singleProFileName;
    bool is_cpp;

};

#endif
