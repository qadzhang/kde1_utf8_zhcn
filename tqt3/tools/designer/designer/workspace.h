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

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <ntqlistview.h>

class FormWindow;
class TQResizeEvent;
class TQCloseEvent;
class TQDropEvent;
class TQDragMoveEvent;
class TQDragEnterEvent;
class MainWindow;
class Project;
class SourceFile;
class FormFile;
class TQCompletionEdit;
class SourceEditor;

class WorkspaceItem : public TQListViewItem
{
public:
    enum Type { ProjectType, FormFileType, FormSourceType, SourceFileType, ObjectType };

    WorkspaceItem( TQListView *parent, Project* p );
    WorkspaceItem( TQListViewItem *parent, SourceFile* sf );
    WorkspaceItem( TQListViewItem *parent, FormFile* ff, Type t = FormFileType );
    WorkspaceItem( TQListViewItem *parent, TQObject *o, Project *p );

    void paintCell( TQPainter *p, const TQColorGroup &cg, int column, int width, int align );

    Type type() const { return t; }

    bool isModified() const;

    TQString text( int ) const;

    void fillCompletionList( TQStringList& completion );
    bool checkCompletion( const TQString& completion );

    TQString key( int, bool ) const; // column sorting key

    Project* project;
    SourceFile* sourceFile;
    FormFile* formFile;
    TQObject *object;

    void setOpen( bool );

    void setAutoOpen( bool );
    bool isAutoOpen() const { return isOpen() && autoOpen; }

    bool useOddColor;

private:
    void init();
    bool autoOpen;
    TQColor backgroundColor();
    Type t;
};

class Workspace : public TQListView
{
    TQ_OBJECT

public:
    Workspace( TQWidget *parent , MainWindow *mw );

    void setCurrentProject( Project *pro );

    void contentsDropEvent( TQDropEvent *e );
    void contentsDragEnterEvent( TQDragEnterEvent *e );
    void contentsDragMoveEvent( TQDragMoveEvent *e );

    void setBufferEdit( TQCompletionEdit *edit );

public slots:

    void update();
    void update( FormFile* );

    void activeFormChanged( FormWindow *fw );
    void activeEditorChanged( SourceEditor *se );

protected:
    void closeEvent( TQCloseEvent *e );
    bool eventFilter( TQObject *, TQEvent * );


private slots:
    void itemClicked( int, TQListViewItem *i, const TQPoint& pos  );
    void itemDoubleClicked( TQListViewItem *i );
    void rmbClicked( TQListViewItem *i, const TQPoint& pos  );
    void bufferChosen( const TQString &buffer );

    void projectDestroyed( TQObject* );

    void sourceFileAdded( SourceFile* );
    void sourceFileRemoved( SourceFile* );

    void formFileAdded( FormFile* );
    void formFileRemoved( FormFile* );

    void objectAdded( TQObject* );
    void objectRemoved( TQObject * );

private:
    WorkspaceItem *findItem( FormFile *ff );
    WorkspaceItem *findItem( SourceFile *sf );
    WorkspaceItem *findItem( TQObject *o );

    void closeAutoOpenItems();

private:
    MainWindow *mainWindow;
    Project *project;
    WorkspaceItem *projectItem;
    TQCompletionEdit *bufferEdit;
    bool blockNewForms;
    void updateBufferEdit();
    bool completionDirty;
    void updateColors();

};

#endif
