/****************************************************************************
**
** Definition of TQFileDialog class
**
** Created : 950428
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#ifndef TQFILEDIALOG_H
#define TQFILEDIALOG_H

class TQPushButton;
class TQButton;
class TQLabel;
class TQWidget;
class TQFileDialog;
class TQTimer;
class TQNetworkOperation;
class TQLineEdit;
class TQListViewItem;
class TQListBoxItem;
class TQFileDialogPrivate;
class TQFileDialogTQFileListView;

#ifndef QT_H
#include "ntqdir.h"
#include "ntqdialog.h"
#include "ntqurloperator.h"
#include "ntqurlinfo.h"
#endif // QT_H

#if __GNUC__ - 0 > 3
#pragma GCC system_header
#endif

#ifndef TQT_NO_FILEDIALOG

class TQ_EXPORT TQFileIconProvider : public TQObject
{
    TQ_OBJECT
public:
    TQFileIconProvider( TQObject * parent = 0, const char* name = 0 );
    virtual const TQPixmap * pixmap( const TQFileInfo & );

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQFileIconProvider( const TQFileIconProvider & );
    TQFileIconProvider& operator=( const TQFileIconProvider & );
#endif
};

class TQ_EXPORT TQFilePreview
{
public:
    TQFilePreview();
    virtual void previewUrl( const TQUrl &url ) = 0;

};

class TQ_EXPORT TQFileDialog : public TQDialog
{
    TQ_OBJECT
    TQ_ENUMS( Mode ViewMode PreviewMode )
    // ##### Why are this read-only properties ?
    TQ_PROPERTY( TQString selectedFile READ selectedFile )
    TQ_PROPERTY( TQString selectedFilter READ selectedFilter )
    TQ_PROPERTY( TQStringList selectedFiles READ selectedFiles )
    // #### Should not we be able to set the path ?
    TQ_PROPERTY( TQString dirPath READ dirPath )
    TQ_PROPERTY( bool showHiddenFiles READ showHiddenFiles WRITE setShowHiddenFiles )
    TQ_PROPERTY( Mode mode READ mode WRITE setMode )
    TQ_PROPERTY( ViewMode viewMode READ viewMode WRITE setViewMode )
    TQ_PROPERTY( PreviewMode previewMode READ previewMode WRITE setPreviewMode )
    TQ_PROPERTY( bool infoPreview READ isInfoPreviewEnabled WRITE setInfoPreviewEnabled )
    TQ_PROPERTY( bool contentsPreview READ isContentsPreviewEnabled WRITE setContentsPreviewEnabled )

public:
    TQFileDialog( const TQString& dirName, const TQString& filter = TQString::null,
		 TQWidget* parent=0, const char* name=0, bool modal = false );
    TQFileDialog( TQWidget* parent=0, const char* name=0, bool modal = false );
    ~TQFileDialog();

    // recommended static functions

    static TQString getOpenFileName( const TQString &initially = TQString::null,
				    const TQString &filter = TQString::null,
				    TQWidget *parent = 0, const char* name = 0,
				    const TQString &caption = TQString::null,
				    TQString *selectedFilter = 0,
				    bool resolveSymlinks = true);
    static TQString getSaveFileName( const TQString &initially = TQString::null,
				    const TQString &filter = TQString::null,
				    TQWidget *parent = 0, const char* name = 0,
				    const TQString &caption = TQString::null,
				    TQString *selectedFilter = 0,
				    bool resolveSymlinks = true);
    static TQString getExistingDirectory( const TQString &dir = TQString::null,
					 TQWidget *parent = 0,
					 const char* name = 0,
					 const TQString &caption = TQString::null,
					 bool dirOnly = true,
					 bool resolveSymlinks = true);
    static TQStringList getOpenFileNames( const TQString &filter= TQString::null,
					 const TQString &dir = TQString::null,
					 TQWidget *parent = 0,
					 const char* name = 0,
					 const TQString &caption = TQString::null,
					 TQString *selectedFilter = 0,
					 bool resolveSymlinks = true);

    // other static functions

    static void setIconProvider( TQFileIconProvider * );
    static TQFileIconProvider * iconProvider();

    // non-static function for special needs

    TQString selectedFile() const;
    TQString selectedFilter() const;
    virtual void setSelectedFilter( const TQString& );
    virtual void setSelectedFilter( int );

    void setSelection( const TQString &);

    void selectAll( bool b );

    TQStringList selectedFiles() const;

    TQString dirPath() const;

    void setDir( const TQDir & );
    const TQDir *dir() const;

    void setShowHiddenFiles( bool s );
    bool showHiddenFiles() const;

    void rereadDir();
    void resortDir();

    enum Mode { AnyFile, ExistingFile, Directory, ExistingFiles, DirectoryOnly };
    void setMode( Mode );
    Mode mode() const;

    enum ViewMode { Detail, List };
    enum PreviewMode { NoPreview, Contents, Info };
    void setViewMode( ViewMode m );
    ViewMode viewMode() const;
    void setPreviewMode( PreviewMode m );
    PreviewMode previewMode() const;

    bool eventFilter( TQObject *, TQEvent * );

    bool isInfoPreviewEnabled() const;
    bool isContentsPreviewEnabled() const;
    void setInfoPreviewEnabled( bool );
    void setContentsPreviewEnabled( bool );

    void setInfoPreview( TQWidget *w, TQFilePreview *preview );
    void setContentsPreview( TQWidget *w, TQFilePreview *preview );

    TQUrl url() const;

    void addFilter( const TQString &filter );

public slots:
    void done( int );
    void setDir( const TQString& );
    void setUrl( const TQUrlOperator &url );
    void setFilter( const TQString& );
    void setFilters( const TQString& );
    void setFilters( const char ** );
    void setFilters( const TQStringList& );

protected:
    void resizeEvent( TQResizeEvent * );
    void keyPressEvent( TQKeyEvent * );

    void addWidgets( TQLabel *, TQWidget *, TQPushButton * );
    void addToolButton( TQButton *b, bool separator = false );
    void addLeftWidget( TQWidget *w );
    void addRightWidget( TQWidget *w );

signals:
    void fileHighlighted( const TQString& );
    void fileSelected( const TQString& );
    void filesSelected( const TQStringList& );
    void dirEntered( const TQString& );
    void filterSelected( const TQString& );

private slots:
    void detailViewSelectionChanged();
    void listBoxSelectionChanged();
    void changeMode( int );
    void fileNameEditReturnPressed();
    void stopCopy();
    void removeProgressDia();

    void fileSelected( int );
    void fileHighlighted( int );
    void dirSelected( int );
    void pathSelected( int );

    void updateFileNameEdit( TQListViewItem *);
    void selectDirectoryOrFile( TQListViewItem * );
    void popupContextMenu( TQListViewItem *, const TQPoint &, int );
    void popupContextMenu( TQListBoxItem *, const TQPoint & );
    void updateFileNameEdit( TQListBoxItem *);
    void selectDirectoryOrFile( TQListBoxItem * );
    void fileNameEditDone();

    void okClicked();
    void filterClicked(); // not used
    void cancelClicked();

    void cdUpClicked();
    void newFolderClicked();

    void fixupNameEdit();

    void doMimeTypeLookup();

    void updateGeometries();
    void modeButtonsDestroyed();
    void urlStart( TQNetworkOperation *op );
    void urlFinished( TQNetworkOperation *op );
    void dataTransferProgress( int bytesDone, int bytesTotal, TQNetworkOperation * );
    void insertEntry( const TQValueList<TQUrlInfo> &fi, TQNetworkOperation *op );
    void removeEntry( TQNetworkOperation * );
    void createdDirectory( const TQUrlInfo &info, TQNetworkOperation * );
    void itemChanged( TQNetworkOperation * );
    void goBack();

private:
    enum PopupAction {
	PA_Open = 0,
	PA_Delete,
	PA_Rename,
	PA_SortName,
	PA_SortSize,
	PA_SortType,
	PA_SortDate,
	PA_SortUnsorted,
	PA_Cancel,
	PA_Reload,
	PA_Hidden
    };

    void init();
    bool trySetSelection( bool isDir, const TQUrlOperator &, bool );
    void deleteFile( const TQString &filename );
    void popupContextMenu( const TQString &filename, bool withSort,
			   PopupAction &action, const TQPoint &p );
    void updatePreviews( const TQUrl &u );

    TQDir reserved; // was cwd
    TQString fileName;

    friend class TQFileDialogTQFileListView;
    friend class TQFileListBox;

    TQFileDialogPrivate *d;
    TQFileDialogTQFileListView  *files;

    TQLineEdit  *nameEdit; // also filter
    TQPushButton *okB;
    TQPushButton *cancelB;

#if defined(TQ_WS_WIN)
    static TQString winGetOpenFileName( const TQString &initialSelection,
				       const TQString &filter,
				       TQString* workingDirectory,
				       TQWidget *parent = 0,
				       const char* name = 0,
				       const TQString& caption = TQString::null,
				       TQString* selectedFilter = 0 );
    static TQString winGetSaveFileName( const TQString &initialSelection,
				       const TQString &filter,
				       TQString* workingDirectory,
				       TQWidget *parent = 0,
				       const char* name = 0,
				       const TQString& caption = TQString::null,
    				       TQString* selectedFilter = 0 );
    static TQStringList winGetOpenFileNames( const TQString &filter,
					    TQString* workingDirectory,
					    TQWidget *parent = 0,
					    const char* name = 0,
					    const TQString& caption = TQString::null,
					    TQString* selectedFilter = 0 );
    static TQString winGetExistingDirectory( const TQString &initialDirectory,
					    TQWidget* parent = 0,
					    const char* name = 0,
					    const TQString& caption = TQString::null);
    static TQString resolveLinkFile( const TQString& linkfile );
#endif
#if defined(TQ_WS_MACX) || defined(TQ_WS_MAC9)
    static TQString macGetSaveFileName( const TQString &, const TQString &,
				       TQString *, TQWidget *, const char*,
				       const TQString&, TQString *);
    static TQStringList macGetOpenFileNames( const TQString &, TQString*,
					    TQWidget *, const char *,
					    const TQString&, TQString *,
					    bool = true, bool = false );
#endif


private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQFileDialog( const TQFileDialog & );
    TQFileDialog &operator=( const TQFileDialog & );
#endif
};

#endif

#endif // TQFILEDIALOG_H
