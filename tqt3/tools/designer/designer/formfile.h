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

#ifndef FORMFILE_H
#define FORMFILE_H

#include <ntqobject.h>
#include "timestamp.h"
#include "metadatabase.h"

class Project;
class FormWindow;
class SourceEditor;

class FormFile : public TQObject
{
    TQ_OBJECT
    friend class SourceEditor;

public:
    enum Who {
	WFormWindow = 1,
	WFormCode = 2,
	WAnyOrAll = WFormWindow | WFormCode
    };

    enum UihState { None, Ok, Deleted };

    FormFile( const TQString &fn, bool temp, Project *p, const char *name = 0 );
    ~FormFile();

    void setFormWindow( FormWindow *f );
    void setEditor( SourceEditor *e );
    void setFileName( const TQString &fn );
    void setCode( const TQString &c );
    void setModified( bool m, int who = WAnyOrAll );
    void setCodeEdited( bool b );

    FormWindow *formWindow() const;
    SourceEditor *editor() const;
    TQString fileName() const;
    TQString absFileName() const;

    bool supportsCodeFile() const { return !codeExtension().isEmpty(); }
    TQString codeComment() const;
    TQString codeFile() const;
    TQString code();
    bool isCodeEdited() const;

    bool loadCode();
    void load();
    bool save( bool withMsgBox = true, bool ignoreModified = false );
    bool saveAs( bool ignoreModified = false );
    bool close();
    bool closeEvent();
    bool isModified( int who = WAnyOrAll );
    bool hasFormCode() const;
    void createFormCode();
    void syncCode();
    void checkTimeStamp();
    bool isUihFileUpToDate();
    void addFunctionCode( MetaDataBase::Function function );
    void removeFunctionCode( MetaDataBase::Function function );
    void functionNameChanged( const TQString &oldName, const TQString &newName );
    void functionRetTypeChanged( const TQString &fuName, const TQString &oldType, const TQString &newType );

    void showFormWindow();
    SourceEditor *showEditor( bool askForUih = true );

    static TQString createUnnamedFileName();
    TQString formName() const;

    bool isFake() const { return fake; }

    void parseCode( const TQString &txt, bool allowModify );

    void addConnection( const TQString &sender, const TQString &signal,
			const TQString &receiver, const TQString &slot );
    void removeConnection( const TQString &sender, const TQString &signal,
			   const TQString &receiver, const TQString &slot );

    bool hasTempFileName() const { return fileNameTemp; }
    void setCodeFileState( UihState );
    int codeFileState() const;
    bool setupUihFile( bool askForUih = true );

    Project *project() const { return pro; }

signals:
    void somethingChanged( FormFile* );

private slots:
    void formWindowChangedSomehow();
    void notifyFormWindowChange();

private:
    bool isFormWindowModified() const;
    bool isCodeModified() const;
    void setFormWindowModified( bool m );
    void setCodeModified( bool m );
    TQString codeExtension() const;
    bool checkFileName( bool allowBreak );
    bool shouldOverwriteUi() const;

private:
    TQString filename;
    bool fileNameTemp;
    Project *pro;
    FormWindow *fw;
    SourceEditor *ed;
    TQString cod;
    TimeStamp timeStamp;
    bool codeEdited;
    TQString cachedFormName;
    bool fake;
    bool pkg;
    bool cm;
    UihState codeFileStat;

    TimeStamp timeStampUi;
};

#endif
