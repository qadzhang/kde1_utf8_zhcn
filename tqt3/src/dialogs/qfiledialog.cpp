/****************************************************************************
**
** Implementation of TQFileDialog class
**
** Created : 950429
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

#include "qplatformdefs.h"

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
# undef connect
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "ntqfiledialog.h"

#ifndef TQT_NO_FILEDIALOG

#include "private/qapplication_p.h"
#include "ntqapplication.h"
#include "ntqbitmap.h"
#include "ntqbuttongroup.h"
#include "ntqcheckbox.h"
#include "ntqcleanuphandler.h"
#include "ntqcombobox.h"
#include "ntqcstring.h"
#include "ntqcursor.h"
#include "ntqdragobject.h"
#include "ntqfile.h"
#include "ntqguardedptr.h"
#include "ntqhbox.h"
#include "ntqheader.h"
#include "ntqlabel.h"
#include "ntqlayout.h"
#include "ntqlibrary.h"
#include "ntqlineedit.h"
#include "ntqlistbox.h"
#include "ntqlistview.h"
#include "ntqmap.h"
#include "ntqmessagebox.h"
#include "ntqmime.h"
#include "ntqnetworkprotocol.h"
#include "ntqobjectlist.h"
#include "ntqpainter.h"
#include "ntqpopupmenu.h"
#include "ntqprogressbar.h"
#include "ntqptrvector.h"
#include "ntqpushbutton.h"
#include "ntqregexp.h"
#include "ntqsemimodal.h"
#include "ntqsplitter.h"
#include "ntqstrlist.h"
#include "ntqstyle.h"
#include "ntqtimer.h"
#include "ntqtoolbutton.h"
#include "ntqtooltip.h"
#include "ntqvbox.h"
#include "ntqwidgetstack.h"

#ifdef TQ_WS_X11
#include "private/tqttdeintegration_x11_p.h"
#endif

#ifdef TQ_WS_WIN
#ifdef TQT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // TQT_THREAD_SUPPORT
#endif // TQ_WS_WIN

#if !defined(Q_OS_TEMP)
#include <time.h>
#else
#include <shellapi.h>
#endif
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#ifdef TQ_WS_MAC
#include "qt_mac.h"
extern TQString qt_mac_precomposeFileName(const TQString &); // qglobal.cpp
#undef check
#endif

/* XPM */
static const char * const start_xpm[]={
    "16 15 8 1",
    "a c #cec6bd",
    "# c #000000",
    "e c #ffff00",
    "b c #999999",
    "f c #cccccc",
    "d c #dcdcdc",
    "c c #ffffff",
    ". c None",
    ".....######aaaaa",
    "...bb#cccc##aaaa",
    "..bcc#cccc#d#aaa",
    ".bcef#cccc#dd#aa",
    ".bcfe#cccc#####a",
    ".bcef#ccccccccc#",
    "bbbbbbbbbbbbccc#",
    "bccccccccccbbcc#",
    "bcefefefefee#bc#",
    ".bcefefefefef#c#",
    ".bcfefefefefe#c#",
    "..bcfefefefeeb##",
    "..bbbbbbbbbbbbb#",
    "...#############",
    "................"};

/* XPM */
static const char * const end_xpm[]={
    "16 15 9 1",
    "d c #a0a0a0",
    "c c #c3c3c3",
    "# c #cec6bd",
    ". c #000000",
    "f c #ffff00",
    "e c #999999",
    "g c #cccccc",
    "b c #ffffff",
    "a c None",
    "......####aaaaaa",
    ".bbbb..###aaaaaa",
    ".bbbb.c.##aaaaaa",
    ".bbbb....ddeeeea",
    ".bbbbbbb.bbbbbe.",
    ".bbbbbbb.bcfgfe.",
    "eeeeeeeeeeeeefe.",
    "ebbbbbbbbbbeege.",
    "ebfgfgfgfgff.ee.",
    "aebfgfgfgfgfg.e.",
    "aebgfgfgfgfgf.e.",
    "aaebgfgfgfgffe..",
    "aaeeeeeeeeeeeee.",
    "aaa.............",
    "aaaaaaaaaaaaaaaa"};

/* XPM */
static const char* const open_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "c c #cccccc",
    "a c #ffffff",
    "................",
    "................",
    "...*****........",
    "..*aaaaa*.......",
    ".*abcbcba******.",
    ".*acbcbcaaaaaa*d",
    ".*abcbcbcbcbcb*d",
    "*************b*d",
    "*aaaaaaaaaa**c*d",
    "*abcbcbcbcbbd**d",
    ".*abcbcbcbcbcd*d",
    ".*acbcbcbcbcbd*d",
    "..*acbcbcbcbb*dd",
    "..*************d",
    "...ddddddddddddd",
    "................"};

/* XPM */
static const char * const link_dir_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #000000",
    "b c #ffff00",
    "f c #303030",
    "# c #999999",
    "a c #cccccc",
    "e c #585858",
    "c c #ffffff",
    ". c None",
    "................",
    "................",
    "..#####.........",
    ".#ababa#........",
    "#abababa######..",
    "#cccccccccccc#d.",
    "#cbababababab#d.",
    "#cabababababa#d.",
    "#cbababdddddddd.",
    "#cababadccccccd.",
    "#cbababdcececcd.",
    "#cababadcefdfcd.",
    "#cbababdccgdhcd.",
    "#######dccchccd.",
    ".dddddddddddddd.",
    "................"};

/* XPM */
static const char * const link_file_xpm[]={
    "16 16 10 1",
    "h c #808080",
    "g c #a0a0a0",
    "d c #c3c3c3",
    ". c #7f7f7f",
    "c c #000000",
    "b c #bfbfbf",
    "f c #303030",
    "e c #585858",
    "a c #ffffff",
    "# c None",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaadc###",
    ".aaaaaaaaaadc###",
    ".aaaacccccccc###",
    ".aaaacaaaaaac###",
    ".aaaacaeaeaac###",
    ".aaaacaefcfac###",
    ".aaaacaagchac###",
    ".ddddcaaahaac###",
    "ccccccccccccc###"};

/* XPM */
static const char* const file_xpm[]={
    "16 16 5 1",
    ". c #7f7f7f",
    "# c None",
    "c c #000000",
    "b c #bfbfbf",
    "a c #ffffff",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".bbbbbbbbbbbc###",
    "ccccccccccccc###"};

/* XPM */
static const char * const closed_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};


/* XPM */
static const char* const cdtoparent_xpm[]={
    "15 13 3 1",
    ". c None",
    "* c #000000",
    "a c #ffff99",
    "..*****........",
    ".*aaaaa*.......",
    "***************",
    "*aaaaaaaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaa***aaaaaaa*",
    "*aa*****aaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa*aaaaaaaa*",
    "*aaaa******aaa*",
    "*aaaaaaaaaaaaa*",
    "*aaaaaaaaaaaaa*",
    "***************"};


/* XPM */
static const char* const newfolder_xpm[] = {
    "15 14 4 1",
    "	c None",
    ".	c #000000",
    "+	c #FFFF00",
    "@	c #FFFFFF",
    "          .    ",
    "               ",
    "          .    ",
    "       .     . ",
    "  ....  . . .  ",
    " .+@+@.  . .   ",
    "..........  . .",
    ".@+@+@+@+@..   ",
    ".+@+@+@+@+. .  ",
    ".@+@+@+@+@.  . ",
    ".+@+@+@+@+.    ",
    ".@+@+@+@+@.    ",
    ".+@+@+@+@+.    ",
    "...........    "};

/* XPM */
static const char* const detailedview_xpm[]={
    "14 11 3 1",
    ". c None",
    "* c #000000",
    "a c #000099",
    ".****.***.***.",
    "..............",
    "aaaaaaaaaaaaaa",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***.",
    "..............",
    ".****.***.***."};

/* XPM */
static const char* const previewinfoview_xpm[]={
    "13 13 4 1",
    ". c #00007f",
    "a c black",
    "# c #cec6bd",
    "b c #000000",
    "..#####aaaaaa",
    ".#.#bb#a#####",
    "...####a#bbb#",
    "#######a#####",
    "#######a#bb##",
    "..#####a#####",
    ".#.#bb#a#bbb#",
    "...####a#####",
    "#######a#bb##",
    "#######a#####",
    "..#####a#bbb#",
    ".#.#bb#a#####",
    "...####aaaaaa"};

/* XPM */
static const char* const previewcontentsview_xpm[]={
    "14 13 5 1",
    ". c #00007f",
    "a c black",
    "c c #7f007f",
    "# c #cec6bd",
    "b c #000000",
    "..#####aaaaaaa",
    ".#.#bb#a#####a",
    "...####a#ccc#a",
    "#######a#ccc#a",
    "#######a#####a",
    "..#####a#bbb#a",
    ".#.#bb#a#####a",
    "...####a#bbb#a",
    "#######a#####a",
    "#######a#bbb#a",
    "..#####a#####a",
    ".#.#bb#a#####a",
    "...####aaaaaaa"};

/* XPM */
static const char* const mclistview_xpm[]={
    "15 11 4 1",
    "* c None",
    "b c #000000",
    ". c #000099",
    "a c #ffffff",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****",
    "***************",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****",
    "***************",
    "...*****...****",
    ".a.*bbb*.a.*bbb",
    "...*****...****"};

/* XPM */
static const char * const back_xpm [] = {
    "13 11 3 1",
    "a c #00ffff",
    "# c #000000",
    ". c None",
    ".....#.......",
    "....##.......",
    "...#a#.......",
    "..#aa########",
    ".#aaaaaaaaaa#",
    "#aaaaaaaaaaa#",
    ".#aaaaaaaaaa#",
    "..#aa########",
    "...#a#.......",
    "....##.......",
    ".....#......."};

static TQPixmap * openFolderIcon = 0;
static TQPixmap * closedFolderIcon = 0;
static TQPixmap * detailViewIcon = 0;
static TQPixmap * multiColumnListViewIcon = 0;
static TQPixmap * cdToParentIcon = 0;
static TQPixmap * newFolderIcon = 0;
static TQPixmap * fifteenTransparentPixels = 0;
static TQPixmap * symLinkDirIcon = 0;
static TQPixmap * symLinkFileIcon = 0;
static TQPixmap * fileIcon = 0;
static TQPixmap * startCopyIcon = 0;
static TQPixmap * endCopyIcon = 0;
static TQPixmap * previewContentsViewIcon = 0;
static TQPixmap * previewInfoViewIcon = 0;
static TQPixmap *goBackIcon = 0;
static TQFileIconProvider * fileIconProvider = 0;
static int lastWidth = 0;
static int lastHeight = 0;
static TQString * workingDirectory = 0;

static bool bShowHiddenFiles = false;
static int sortFilesBy = (int)TQDir::Name;
static bool sortAscending = true;
static bool detailViewMode = false;

static TQCleanupHandler<TQPixmap> qfd_cleanup_pixmap;
static TQCleanupHandler<TQString> qfd_cleanup_string;

static TQString toRootIfNotExists( const TQString &path )
{
    if ( !path.isEmpty() )
        return path;

    const TQFileInfoList *drives = TQDir::drives();
    Q_ASSERT( drives && !drives->isEmpty() );
    return drives->getFirst()->filePath();
}

static void tqt_do_qfd_cleanup_pixmap() {
    qfd_cleanup_pixmap.clear();
}

static bool isDirectoryMode( int m )
{
    return m == TQFileDialog::Directory || m == TQFileDialog::DirectoryOnly;
}

static void updateLastSize( TQFileDialog *that )
{
    int extWidth = 0;
    int extHeight = 0;
    if ( that->extension() && that->extension()->isVisible() ) {
	if ( that->orientation() == TQt::Vertical )
	    extHeight = that->extension()->height();
	else
	    extWidth = that->extension()->width();
    }
    lastWidth = that->width() - extWidth;
    lastHeight = that->height() - extHeight;
}

// Don't remove the lines below!
//
// resolving the W methods manually is needed, because Windows 95 doesn't include
// these methods in Shell32.lib (not even stubs!), so you'd get an unresolved symbol
// when TQt calls getEsistingDirectory(), etc.
#if defined(TQ_WS_WIN)

typedef UINT (WINAPI *PtrExtractIconEx)(LPCTSTR,int,HICON*,HICON*,UINT);
static PtrExtractIconEx ptrExtractIconEx = 0;

static void resolveLibs()
{
#ifndef Q_OS_TEMP
    static bool triedResolve = false;

    if ( !triedResolve ) {
#ifdef TQT_THREAD_SUPPORT
	// protect initialization
	TQMutexLocker locker( tqt_global_mutexpool ?
			     tqt_global_mutexpool->get( &triedResolve ) : 0 );
	// check triedResolve again, since another thread may have already
	// done the initialization
	if ( triedResolve ) {
	    // another thread did initialize the security function pointers,
	    // so we shouldn't do it again.
	    return;
	}
#endif
	triedResolve = true;
	if ( qt_winunicode ) {
	    TQLibrary lib("shell32");
	    lib.setAutoUnload( false );
	    ptrExtractIconEx = (PtrExtractIconEx) lib.resolve( "ExtractIconExW" );
	}
    }
#endif
}
#ifdef Q_OS_TEMP
#define PtrExtractIconEx ExtractIconEx
#endif

class TQWindowsIconProvider : public TQFileIconProvider
{
public:
    TQWindowsIconProvider( TQObject *parent=0, const char *name=0 );
    ~TQWindowsIconProvider();

    const TQPixmap * pixmap( const TQFileInfo &fi );

private:
    TQPixmap defaultFolder;
    TQPixmap defaultFile;
    TQPixmap defaultExe;
    TQPixmap pix;
    int pixw, pixh;
    TQMap< TQString, TQPixmap > cache;

};
#endif

static void makeVariables() {
    if ( !openFolderIcon ) {
	workingDirectory = new TQString( ::toRootIfNotExists(TQDir::currentDirPath()) );
        qfd_cleanup_string.add( &workingDirectory );

	openFolderIcon = new TQPixmap( (const char **)open_xpm);
	qfd_cleanup_pixmap.add( &openFolderIcon );
	symLinkDirIcon = new TQPixmap( (const char **)link_dir_xpm);
	qfd_cleanup_pixmap.add( &symLinkDirIcon );
	symLinkFileIcon = new TQPixmap( (const char **)link_file_xpm);
	qfd_cleanup_pixmap.add( &symLinkFileIcon );
	fileIcon = new TQPixmap( (const char **)file_xpm);
	qfd_cleanup_pixmap.add( &fileIcon );
	closedFolderIcon = new TQPixmap( (const char **)closed_xpm);
	qfd_cleanup_pixmap.add( &closedFolderIcon );
	detailViewIcon = new TQPixmap( (const char **)detailedview_xpm);
	qfd_cleanup_pixmap.add( &detailViewIcon );
	multiColumnListViewIcon = new TQPixmap( (const char **)mclistview_xpm);
	qfd_cleanup_pixmap.add( &multiColumnListViewIcon );
	cdToParentIcon = new TQPixmap( (const char **)cdtoparent_xpm);
	qfd_cleanup_pixmap.add( &cdToParentIcon );
	newFolderIcon = new TQPixmap( (const char **)newfolder_xpm);
	qfd_cleanup_pixmap.add( &newFolderIcon );
	previewInfoViewIcon
	    = new TQPixmap( (const char **)previewinfoview_xpm );
	qfd_cleanup_pixmap.add( &previewInfoViewIcon );
	previewContentsViewIcon
	    = new TQPixmap( (const char **)previewcontentsview_xpm );
	qfd_cleanup_pixmap.add( &previewContentsViewIcon );
	startCopyIcon = new TQPixmap( (const char **)start_xpm );
	qfd_cleanup_pixmap.add( &startCopyIcon );
	endCopyIcon = new TQPixmap( (const char **)end_xpm );
	qfd_cleanup_pixmap.add( &endCopyIcon );
	goBackIcon = new TQPixmap( (const char **)back_xpm );
	qfd_cleanup_pixmap.add( &goBackIcon );
	fifteenTransparentPixels = new TQPixmap( closedFolderIcon->width(), 1 );
	qfd_cleanup_pixmap.add( &fifteenTransparentPixels );

	// On X11 the pixmaps should be cleanup before we disconnect from the server
	tqAddPostRoutine(&tqt_do_qfd_cleanup_pixmap);

	TQBitmap m( fifteenTransparentPixels->width(), 1 );
	m.fill( TQt::color0 );
	fifteenTransparentPixels->setMask( m );
	bShowHiddenFiles = false;
	sortFilesBy = (int)TQDir::Name;
	detailViewMode = false;
#if defined(TQ_WS_WIN)
	if ( !fileIconProvider )
	    fileIconProvider = new TQWindowsIconProvider( tqApp );
#endif
    }
}

/******************************************************************
 *
 * Definitions of view classes
 *
 ******************************************************************/

class TQRenameEdit : public TQLineEdit
{
    TQ_OBJECT

public:
    TQRenameEdit( TQWidget *parent )
	: TQLineEdit( parent, "qt_rename_edit" ), doRenameAlreadyEmitted(false)
    {
	connect( this, TQ_SIGNAL(returnPressed()), TQ_SLOT(slotReturnPressed()) );
    }

protected:
    void keyPressEvent( TQKeyEvent *e );
    void focusOutEvent( TQFocusEvent *e );

signals:
    void cancelRename();
    void doRename();

private slots:
    void slotReturnPressed();

private:
    bool doRenameAlreadyEmitted;
};

class TQFileListBox : public TQListBox
{
    friend class TQFileDialog;

    TQ_OBJECT

private:
    TQFileListBox( TQWidget *parent, TQFileDialog *d );

    void clear();
    void show();
    void startRename( bool check = true );
    void viewportMousePressEvent( TQMouseEvent *e );
    void viewportMouseReleaseEvent( TQMouseEvent *e );
    void viewportMouseDoubleClickEvent( TQMouseEvent *e );
    void viewportMouseMoveEvent( TQMouseEvent *e );
#ifndef TQT_NO_DRAGANDDROP
    void viewportDragEnterEvent( TQDragEnterEvent *e );
    void viewportDragMoveEvent( TQDragMoveEvent *e );
    void viewportDragLeaveEvent( TQDragLeaveEvent *e );
    void viewportDropEvent( TQDropEvent *e );
    bool acceptDrop( const TQPoint &pnt, TQWidget *source );
    void setCurrentDropItem( const TQPoint &pnt );
#endif
    void keyPressEvent( TQKeyEvent *e );

private slots:
    void rename();
    void cancelRename();
    void doubleClickTimeout();
    void changeDirDuringDrag();
    void dragObjDestroyed();
    void contentsMoved( int, int );

private:
    TQRenameEdit *lined;
    TQFileDialog *filedialog;
    bool renaming;
    TQTimer* renameTimer;
    TQListBoxItem *renameItem, *dragItem;
    TQPoint pressPos, oldDragPos;
    bool mousePressed;
    int urls;
    TQString startDragDir;
    TQListBoxItem *currDropItem;
    TQTimer *changeDirTimer;
    bool firstMousePressEvent;
    TQUrlOperator startDragUrl;

};


class TQFileDialogTQFileListView : public TQListView
{
    TQ_OBJECT

public:
    TQFileDialogTQFileListView( TQWidget *parent, TQFileDialog *d );

    void clear();
    void startRename( bool check = true );
    void setSorting( int column, bool increasing = true );

    TQRenameEdit *lined;
    bool renaming;
    TQListViewItem *renameItem;

private:
    void viewportMousePressEvent( TQMouseEvent *e );
    void viewportMouseDoubleClickEvent( TQMouseEvent *e );
    void keyPressEvent( TQKeyEvent *e );
    void viewportMouseReleaseEvent( TQMouseEvent *e );
    void viewportMouseMoveEvent( TQMouseEvent *e );
#ifndef TQT_NO_DRAGANDDROP
    void viewportDragEnterEvent( TQDragEnterEvent *e );
    void viewportDragMoveEvent( TQDragMoveEvent *e );
    void viewportDragLeaveEvent( TQDragLeaveEvent *e );
    void viewportDropEvent( TQDropEvent *e );
    bool acceptDrop( const TQPoint &pnt, TQWidget *source );
    void setCurrentDropItem( const TQPoint &pnt );
#endif

private slots:
    void rename();
    void cancelRename();
    void changeSortColumn2( int column );
    void doubleClickTimeout();
    void changeDirDuringDrag();
    void dragObjDestroyed();
    void contentsMoved( int, int );

private:
    TQFileDialog *filedialog;
    TQTimer* renameTimer;
    TQPoint pressPos, oldDragPos;
    bool mousePressed;
    int urls;
    TQString startDragDir;
    TQListViewItem *currDropItem, *dragItem;
    TQTimer *changeDirTimer;
    bool firstMousePressEvent;
    bool ascending;
    int sortcolumn;
    TQUrlOperator startDragUrl;

};

/****************************************************************************
 *
 * Classes for copy progress dialog
 *
 ****************************************************************************/

class TQFDProgressAnimation : public TQWidget
{
    TQ_OBJECT

public:
    TQFDProgressAnimation( TQWidget *parent );
    void start();

private slots:
    void next();

protected:
    void paintEvent( TQPaintEvent *e );

private:
    int step;
    TQTimer *timer;

};

TQFDProgressAnimation::TQFDProgressAnimation( TQWidget *parent )
    : TQWidget( parent, "qt_progressanimation" )
{
    setFixedSize( 300, 50 );
    step = -1;
    next();
    timer = new TQTimer( this );
    connect( timer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( next() ) );
}

void TQFDProgressAnimation::start()
{
    timer->start( 150, false );
}

void TQFDProgressAnimation::next()
{
    ++step;
    if ( step > 10 )
	step = 0;
    repaint();
}

void TQFDProgressAnimation::paintEvent( TQPaintEvent * )
{
    erase();

    TQPainter p;
    p.begin( this );
    if ( step == 0 ) {
	p.drawPixmap( 5, ( height() - startCopyIcon->height() ) / 2,
		      *startCopyIcon );
	p.drawPixmap( width() - 5 - openFolderIcon->width(),
		      ( height() - openFolderIcon->height() ) / 2 , *openFolderIcon );
    } else if ( step == 10 ) {
	p.drawPixmap( 5, ( height() - openFolderIcon->height() ) / 2,
		      *openFolderIcon );
	p.drawPixmap( width() - 5 - endCopyIcon->width(),
		      ( height() - endCopyIcon->height() ) / 2 , *endCopyIcon );
    } else {
	p.drawPixmap( 5, ( height() - openFolderIcon->height() ) / 2,
		      *openFolderIcon );
	p.drawPixmap( width() - 5 - openFolderIcon->width(),
		      ( height() - openFolderIcon->height() ) / 2 , *openFolderIcon );
	int x = 10 + openFolderIcon->width();
	int w = width() - 2 * x;
	int s = w / 9;
	p.drawPixmap( x + s * step, ( height() - fileIcon->height() ) / 2 - fileIcon->height(),
		      *fileIcon );
    }
}


class TQFDProgressDialog : public TQDialog
{
    TQ_OBJECT

public:
    TQFDProgressDialog( TQWidget *parent, const TQString &fn, int steps );

    void setReadProgress( int p );
    void setWriteProgress( int p );
    void setWriteLabel( const TQString &s );

signals:
    void cancelled();

private:
    TQProgressBar *readBar;
    TQProgressBar *writeBar;
    TQLabel *writeLabel;
    TQFDProgressAnimation *animation;

};

TQFDProgressDialog::TQFDProgressDialog( TQWidget *parent, const TQString &fn, int steps )
    : TQDialog( parent, "", true )
{
#ifndef TQT_NO_WIDGET_TOPEXTRA
    setCaption( TQFileDialog::tr( "Copy or Move a File" ) );
#endif
    TQVBoxLayout *layout = new TQVBoxLayout( this );
    layout->setSpacing( 5 );
    layout->setMargin( 5 );

    animation = new TQFDProgressAnimation( this );
    layout->addWidget( animation );

    layout->addWidget( new TQLabel( TQFileDialog::tr( "Read: %1" ).arg( fn ),
		       this, "qt_read_lbl" ) );
    readBar = new TQProgressBar( steps, this, "qt_readbar" );
    readBar->reset();
    readBar->setProgress( 0 );
    layout->addWidget( readBar );
    writeLabel = new TQLabel( TQFileDialog::tr( "Write: %1" ).arg( TQString::null ),
			     this, "qt_write_lbl" );
    layout->addWidget( writeLabel );
    writeBar = new TQProgressBar( steps, this, "qt_writebar" );
    writeBar->reset();
    writeBar->setProgress( 0 );
    layout->addWidget( writeBar );

    TQPushButton *b = new TQPushButton( TQFileDialog::tr( "Cancel" ), this,
				      "qt_cancel_btn" );
    b->setFixedSize( b->sizeHint() );
    layout->addWidget( b );
    connect( b, TQ_SIGNAL( clicked() ),
	     this, TQ_SIGNAL( cancelled() ) );

    animation->start();
}

void TQFDProgressDialog::setReadProgress( int p )
{
    readBar->setProgress( p );
}

void TQFDProgressDialog::setWriteProgress( int p )
{
    writeBar->setProgress( p );
}

void TQFDProgressDialog::setWriteLabel( const TQString &s )
{
    writeLabel->setText( TQFileDialog::tr( "Write: %1" ).arg( s ) );
}

/************************************************************************
 *
 * Private TQFileDialog members
 *
 ************************************************************************/

class TQFileDialogPrivate {
public:
    ~TQFileDialogPrivate();

    TQStringList history;

    bool geometryDirty;
    TQComboBox * paths;
    TQComboBox * types;
    TQLabel * pathL;
    TQLabel * fileL;
    TQLabel * typeL;

    TQVBoxLayout * topLevelLayout;
    TQHBoxLayout *buttonLayout, *leftLayout, *rightLayout;
    TQPtrList<TQHBoxLayout> extraWidgetsLayouts;
    TQPtrList<TQLabel> extraLabels;
    TQPtrList<TQWidget> extraWidgets;
    TQPtrList<TQWidget> extraButtons;
    TQPtrList<TQButton> toolButtons;

    TQWidgetStack * stack;

    TQToolButton * cdToParent, *newFolder, * detailView, * mcView,
	*previewInfo, *previewContents, *goBack;
    TQButtonGroup * modeButtons;

    TQString currentFileName;
    TQListViewItem *last;

    TQListBoxItem *lastEFSelected;

    struct File: public TQListViewItem {
	File( TQFileDialogPrivate * dlgp,
	      const TQUrlInfo * fi, TQListViewItem * parent )
	    : TQListViewItem( parent, dlgp->last ), info( *fi ), d(dlgp), i( 0 ), hasMimePixmap( false )
	{ setup(); dlgp->last = this; }
	File( TQFileDialogPrivate * dlgp,
	      const TQUrlInfo * fi, TQListView * parent )
	    : TQListViewItem( parent, dlgp->last ), info( *fi ), d(dlgp), i( 0 ), hasMimePixmap( false )
	{ setup(); dlgp->last = this; }
	File( TQFileDialogPrivate * dlgp,
	      const TQUrlInfo * fi, TQListView * parent, TQListViewItem * after )
	    : TQListViewItem( parent, after ), info( *fi ), d(dlgp), i( 0 ), hasMimePixmap( false )
	{ setup(); if ( !nextSibling() ) dlgp->last = this; }
	~File();

	TQString text( int column ) const;
	const TQPixmap * pixmap( int ) const;

	TQUrlInfo info;
	TQFileDialogPrivate * d;
	TQListBoxItem *i;
	bool hasMimePixmap;
    };

    class MCItem: public TQListBoxItem {
    public:
	MCItem( TQListBox *, TQListViewItem * item );
	MCItem( TQListBox *, TQListViewItem * item, TQListBoxItem *after );
	TQString text() const;
	const TQPixmap *pixmap() const;
	int height( const TQListBox * ) const;
	int width( const TQListBox * ) const;
	void paint( TQPainter * );
	TQListViewItem * i;
    };

    class UrlInfoList : public TQPtrList<TQUrlInfo> {
    public:
	UrlInfoList() { setAutoDelete( true ); }
	int compareItems( TQPtrCollection::Item n1, TQPtrCollection::Item n2 ) {
	    if ( !n1 || !n2 )
		return 0;

	    TQUrlInfo *i1 = ( TQUrlInfo *)n1;
	    TQUrlInfo *i2 = ( TQUrlInfo *)n2;

	    if ( i1->isDir() && !i2->isDir() )
		return -1;
	    if ( !i1->isDir() && i2->isDir() )
		return 1;

	    if ( i1->name() == ".." )
		return -1;
	    if ( i2->name() == ".." )
		return 1;

	    if ( sortFilesBy == TQDir::Name ) {
#if defined(Q_OS_WIN32)
		TQString name1 = i1->name().lower();
		TQString name2 = i2->name().lower();
		return name1.localeAwareCompare( name2 );
#else
		TQString name1 = i1->name();
		TQString name2 = i2->name();
		return name1.localeAwareCompare( name2 );
#endif
	    }
	    if ( TQUrlInfo::equal( *i1, *i2, sortFilesBy ) )
		return 0;
	    else if ( TQUrlInfo::greaterThan( *i1, *i2, sortFilesBy ) )
		return 1;
	    else if ( TQUrlInfo::lessThan( *i1, *i2, sortFilesBy ) )
		return -1;
	    // can't happen...
	    return 0;
	}
	TQUrlInfo *operator[]( int i ) {
	    return at( i );
	}
    };

    UrlInfoList sortedList;
    TQPtrList<File> pendingItems;

    TQFileListBox * moreFiles;

    TQFileDialog::Mode mode;

    TQString rw;
    TQString ro;
    TQString wo;
    TQString inaccessible;

    TQString symLinkToFile;
    TQString file;
    TQString symLinkToDir;
    TQString dir;
    TQString symLinkToSpecial;
    TQString special;
    TQWidgetStack *preview;
    bool infoPreview, contentsPreview;
    TQSplitter *splitter;
    TQUrlOperator url, oldUrl;
    TQWidget *infoPreviewWidget, *contentsPreviewWidget;
    TQFilePreview *infoPreviewer, *contentsPreviewer;
    bool hadDotDot;

    bool ignoreNextKeyPress;
    // ignores the next refresh operation in case the user forced a selection
    bool ignoreNextRefresh;
    TQFDProgressDialog *progressDia;
    bool checkForFilter;
    bool ignoreStop;

    TQTimer *mimeTypeTimer;
    const TQNetworkOperation *currListChildren;

    // this is similar to TQUrl::encode but does encode "*" and
    // doesn't encode whitespaces
    static TQString encodeFileName( const TQString& fName ) {

	TQString newStr;
	TQCString cName = fName.utf8();
	const TQCString sChars(
#ifdef TQ_WS_WIN
	    "#%"
#else
	    "<>#@\"&%$:,;?={}|^~[]\'`\\*"
#endif
	    );

	int len = cName.length();
	if ( !len )
	    return TQString::null;
	for ( int i = 0; i < len ;++i ) {
	    uchar inCh = (uchar)cName[ i ];
	    if ( inCh >= 128 || sChars.contains(inCh) )
	    {
		newStr += TQChar( '%' );
		ushort c = inCh / 16;
		c += c > 9 ? 'A' - 10 : '0';
		newStr += (char)c;
		c = inCh % 16;
		c += c > 9 ? 'A' - 10 : '0';
		newStr += (char)c;
	    } else {
		newStr += (char)inCh;
	    }
	}
	return newStr;
    }

    static bool fileExists( const TQUrlOperator &url, const TQString& name )
    {
	TQUrl u( url, TQFileDialogPrivate::encodeFileName(name) );
	if ( u.isLocalFile() ) {
	    TQFileInfo f( u.path() );
	    return f.exists();
	} else {
	    TQNetworkProtocol *p = TQNetworkProtocol::getNetworkProtocol( url.protocol() );
	    if ( p && (p->supportedOperations()&TQNetworkProtocol::OpListChildren) ) {
		TQUrlInfo ui( url, name );
		return ui.isValid();
	    }
	}
	return true;
    }

#ifndef Q_NO_CURSOR
    bool cursorOverride; // Remember if the cursor was overridden or not.
#endif
#ifdef TQ_WS_WIN
    int oldPermissionLookup;
#endif
};

TQFileDialogPrivate::~TQFileDialogPrivate()
{
    delete modeButtons;
}



/************************************************************************
 *
 * Internal class TQRenameEdit
 *
 ************************************************************************/

void TQRenameEdit::keyPressEvent( TQKeyEvent *e )
{
    if ( e->key() == Key_Escape )
	emit cancelRename();
    else
	TQLineEdit::keyPressEvent( e );
    e->accept();
}

void TQRenameEdit::focusOutEvent( TQFocusEvent * )
{
    if ( !doRenameAlreadyEmitted )
	emit doRename();
    else
	doRenameAlreadyEmitted = false;
}

void TQRenameEdit::slotReturnPressed()
{
    doRenameAlreadyEmitted = true;
    emit doRename();
}

/************************************************************************
 *
 * Internal class TQFileListBox
 *
 ************************************************************************/

TQFileListBox::TQFileListBox( TQWidget *parent, TQFileDialog *dlg )
    : TQListBox( parent, "filelistbox" ), filedialog( dlg ),
      renaming( false ), renameItem( 0 ), mousePressed( false ),
      firstMousePressEvent( true )
{
    changeDirTimer = new TQTimer( this );
    TQVBox *box = new TQVBox( viewport(), "qt_vbox" );
    box->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    lined = new TQRenameEdit( box );
    lined->setFixedHeight( lined->sizeHint().height() );
    box->hide();
    box->setBackgroundMode( PaletteBase );
    renameTimer = new TQTimer( this );
    connect( lined, TQ_SIGNAL( doRename() ),
	     this, TQ_SLOT (rename() ) );
    connect( lined, TQ_SIGNAL( cancelRename() ),
	     this, TQ_SLOT( cancelRename() ) );
    connect( renameTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doubleClickTimeout() ) );
    connect( changeDirTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( changeDirDuringDrag() ) );
    connect( this, TQ_SIGNAL( contentsMoving(int,int) ),
	     this, TQ_SLOT( contentsMoved(int,int) ) );
    viewport()->setAcceptDrops( true );
    dragItem = 0;
}

void TQFileListBox::show()
{
    setBackgroundMode( PaletteBase );
    viewport()->setBackgroundMode( PaletteBase );
    TQListBox::show();
}

void TQFileListBox::keyPressEvent( TQKeyEvent *e )
{
    if ( ( e->key() == Key_Enter ||
	   e->key() == Key_Return ) &&
	 renaming )
	return;

    TQString keyPressed = ((TQKeyEvent *)e)->text().lower();
    TQChar keyChar = keyPressed[0];
    bool handled = false;
    if ( keyChar.isLetterOrNumber() ) {
	TQListBoxItem * i = 0;
	if ( currentItem() )
	    i = item( currentItem() );
	else
	    i = firstItem();
	if ( i->next() )
	    i = i->next();
	else
	    i = firstItem();
	while ( i != item( currentItem() ) ) {
	    TQString it = text( index( i ) );
	    if ( it[0].lower() == keyChar ) {
	        clearSelection();
	        setCurrentItem( i );
	        handled = true;
	        e->accept();
            } else {
	        if ( i->next() )
	            i = i->next();
	        else
	            i = firstItem();
	    }
	}
    }
    cancelRename();
    if (!handled){
        TQListBox::keyPressEvent( e );
    }
}

void TQFileListBox::viewportMousePressEvent( TQMouseEvent *e )
{
    pressPos = e->pos();
    mousePressed = false;

    bool didRename = renaming;

    cancelRename();
    if ( !hasFocus() && !viewport()->hasFocus() )
	setFocus();

    if ( e->button() != LeftButton ) {
	TQListBox::viewportMousePressEvent( e );
	firstMousePressEvent = false;
	return;
    }

    int i = currentItem();
    bool wasSelected = false;
    if ( i != -1 )
	wasSelected = item( i )->isSelected();
    TQListBox::mousePressEvent( e );

    TQFileDialogPrivate::MCItem *i1 = (TQFileDialogPrivate::MCItem*)item( currentItem() );
    if ( i1 )
	mousePressed =  ( !( (TQFileDialogPrivate::File*)i1->i )->info.isDir() )
			|| ( filedialog->mode() == TQFileDialog::Directory ) || ( filedialog->mode() == TQFileDialog::DirectoryOnly );

    if ( itemAt( e->pos() ) != item( i ) ) {
	firstMousePressEvent = false;
	return;
    }

     if ( !firstMousePressEvent && !didRename && i == currentItem() && currentItem() != -1 &&
	  wasSelected && TQUrlInfo( filedialog->d->url, "." ).isWritable() && item( currentItem() )->text() != ".." ) {
	renameTimer->start( TQApplication::doubleClickInterval(), true );
	renameItem = item( i );
    }

    firstMousePressEvent = false;
}

void TQFileListBox::viewportMouseReleaseEvent( TQMouseEvent *e )
{
    dragItem = 0;
    TQListBox::viewportMouseReleaseEvent( e );
    mousePressed = false;
}

void TQFileListBox::viewportMouseDoubleClickEvent( TQMouseEvent *e )
{
    renameTimer->stop();
    TQListBox::viewportMouseDoubleClickEvent( e );
}

void TQFileListBox::viewportMouseMoveEvent( TQMouseEvent *e )
{
    if ( !dragItem )
	dragItem = itemAt( e->pos() );
    renameTimer->stop();
#ifndef TQT_NO_DRAGANDDROP
    if (  ( pressPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() && mousePressed ) {
	TQListBoxItem *item = dragItem;
	dragItem = 0;
	if ( item ) {
	    if ( !itemRect( item ).contains( e->pos() ) )
		return;
	    TQUriDrag* drag = new TQUriDrag( viewport() );
	    TQStringList files;
	    if ( filedialog->mode() == TQFileDialog::ExistingFiles )
		files = filedialog->selectedFiles();
	    else
		files = filedialog->selectedFile();
	    drag->setFileNames( files );

	    if ( lined->parentWidget()->isVisible() )
		cancelRename();

	    connect( drag, TQ_SIGNAL( destroyed() ),
		     this, TQ_SLOT( dragObjDestroyed() ) );
	    drag->drag();

	    mousePressed = false;
	}
    } else
#endif
    {
	TQListBox::viewportMouseMoveEvent( e );
    }

}

void TQFileListBox::dragObjDestroyed()
{
#ifndef TQT_NO_DRAGANDDROP
    //#######
    //filedialog->rereadDir();
#endif
}

#ifndef TQT_NO_DRAGANDDROP
void TQFileListBox::viewportDragEnterEvent( TQDragEnterEvent *e )
{
    startDragUrl = filedialog->d->url;
    startDragDir = filedialog->dirPath();
    currDropItem = 0;

    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    TQStringList l;
    TQUriDrag::decodeLocalFiles( e, l );
    urls = (int)l.count();

    if ( acceptDrop( e->pos(), e->source() ) ) {
	e->accept();
	setCurrentDropItem( e->pos() );
    } else {
	e->ignore();
	setCurrentDropItem( TQPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void TQFileListBox::viewportDragMoveEvent( TQDragMoveEvent *e )
{
    if ( acceptDrop( e->pos(), e->source() ) ) {
	switch ( e->action() ) {
	case TQDropEvent::Copy:
	    e->acceptAction();
	    break;
	case TQDropEvent::Move:
	    e->acceptAction();
	    break;
	case TQDropEvent::Link:
	    break;
	default:
	    break;
	}
	if ( oldDragPos != e->pos() )
	    setCurrentDropItem( e->pos() );
    } else {
	changeDirTimer->stop();
	e->ignore();
	setCurrentDropItem( TQPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void TQFileListBox::viewportDragLeaveEvent( TQDragLeaveEvent * )
{
    changeDirTimer->stop();
    setCurrentDropItem( TQPoint( -1, -1 ) );
//########
//     if ( startDragDir != filedialog->d->url )
//	filedialog->setUrl( startDragUrl );
}

void TQFileListBox::viewportDropEvent( TQDropEvent *e )
{
    changeDirTimer->stop();

    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    uint i;
    TQStrList l;
    TQUriDrag::decode( e, l );

    bool move = e->action() == TQDropEvent::Move;
//     bool supportAction = move || e->action() == TQDropEvent::Copy;

    TQUrlOperator dest;
    if ( currDropItem )
	dest = TQUrlOperator( filedialog->d->url, TQFileDialogPrivate::encodeFileName( currDropItem->text() ) );
    else
	dest = filedialog->d->url;
    TQStringList lst;
    for ( i = 0; i < l.count(); ++i ) {
	lst << l.at( i );
    }

    // make sure that we can write to the destination before performing the action
    bool exists = false;
    TQString name = (currDropItem ? TQFileDialogPrivate::encodeFileName(currDropItem->text()) : TQString("."));
    TQUrlInfo info(filedialog->d->url, name);
    for ( i = 0; i < lst.count(); ++i ) {
        int slash = lst[i].findRev('/');
        TQString filename = lst[i].right(lst[i].length() - slash - 1);
        exists = exists || TQFileDialogPrivate::fileExists( dest, filename);
    }
    if (info.isWritable() && !exists)
        filedialog->d->url.copy( lst, dest, move );

    // ##### what is supportAction for?
    e->acceptAction();
    currDropItem = 0;
}

bool TQFileListBox::acceptDrop( const TQPoint &pnt, TQWidget *source )
{
    TQListBoxItem *item = itemAt( pnt );
    if ( !item || ( item && !itemRect( item ).contains( pnt ) ) ) {
	if ( source == viewport() && startDragDir == filedialog->dirPath() )
	    return false;
	return true;
    }

    TQUrlInfo fi( filedialog->d->url, item->text() );

    if ( fi.isDir() && itemRect( item ).contains( pnt ) )
	return true;
    return false;
}

void TQFileListBox::setCurrentDropItem( const TQPoint &pnt )
{
    changeDirTimer->stop();

    TQListBoxItem *item = 0;
    if ( pnt != TQPoint( -1, -1 ) )
	item = itemAt( pnt );
    if ( item && !TQUrlInfo( filedialog->d->url, item->text() ).isDir() )
	item = 0;
    if ( item && !itemRect( item ).contains( pnt ) )
	item = 0;

    currDropItem = item;
    if ( currDropItem )
	setCurrentItem( currDropItem );
    changeDirTimer->start( 750 );
}
#endif // TQT_NO_DRAGANDDROP

void TQFileListBox::changeDirDuringDrag()
{
#ifndef TQT_NO_DRAGANDDROP
    if ( !currDropItem )
	return;
    changeDirTimer->stop();
    TQUrl u( filedialog->d->url, TQFileDialogPrivate::encodeFileName(currDropItem->text()) );
    filedialog->setDir( u );
    currDropItem = 0;
#endif
}

void TQFileListBox::doubleClickTimeout()
{
    startRename();
    renameTimer->stop();
}

void TQFileListBox::startRename( bool check )
{
    if ( check && ( !renameItem || renameItem != item( currentItem() ) ) )
	return;

    int i = currentItem();
    setSelected( i, true );
    TQRect r = itemRect( item( i ) );
    int bdr = item( i )->pixmap() ?
	      item( i )->pixmap()->width() : 16;
    int x = r.x() + bdr;
    int y = r.y();
    int w = item( i )->width( this ) - bdr;
    int h = TQMAX( lined->height() + 2, r.height() );
    y = y + r.height() / 2 - h / 2;

    lined->parentWidget()->setGeometry( x, y, w + 6, h );
    lined->setFocus();
    lined->setText( item( i )->text() );
    lined->selectAll();
    lined->setFrame( false );
    lined->parentWidget()->show();
    viewport()->setFocusProxy( lined );
    renaming = true;
}

void TQFileListBox::clear()
{
    cancelRename();
    TQListBox::clear();
}

void TQFileListBox::rename()
{
    if ( !lined->text().isEmpty() ) {
	TQString file = currentText();

	if ( lined->text() != file )
	    filedialog->d->url.rename( file, lined->text() );
    }
    cancelRename();
}

void TQFileListBox::cancelRename()
{
    renameItem = 0;
    lined->parentWidget()->hide();
    viewport()->setFocusProxy( this );
    renaming = false;
    updateItem( currentItem() );
    if ( lined->hasFocus() )
	viewport()->setFocus();
}

void TQFileListBox::contentsMoved( int, int )
{
    changeDirTimer->stop();
#ifndef TQT_NO_DRAGANDDROP
    setCurrentDropItem( TQPoint( -1, -1 ) );
#endif
}

/************************************************************************
 *
 * Internal class TQFileListView
 *
 ************************************************************************/

TQFileDialogTQFileListView::TQFileDialogTQFileListView( TQWidget *parent, TQFileDialog *dlg )
    : TQListView( parent, "qt_filedlg_listview" ), renaming( false ), renameItem( 0 ),
    filedialog( dlg ), mousePressed( false ),
    firstMousePressEvent( true )
{
    changeDirTimer = new TQTimer( this );
    TQVBox *box = new TQVBox( viewport(), "qt_vbox" );
    box->setFrameStyle( TQFrame::Box | TQFrame::Plain );
    lined = new TQRenameEdit( box );
    lined->setFixedHeight( lined->sizeHint().height() );
    box->hide();
    box->setBackgroundMode( PaletteBase );
    renameTimer = new TQTimer( this );
    connect( lined, TQ_SIGNAL( doRename() ),
	     this, TQ_SLOT (rename() ) );
    connect( lined, TQ_SIGNAL( cancelRename() ),
	     this, TQ_SLOT( cancelRename() ) );
    header()->setMovingEnabled( false );
    connect( renameTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doubleClickTimeout() ) );
    connect( changeDirTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( changeDirDuringDrag() ) );
    disconnect( header(), TQ_SIGNAL( sectionClicked(int) ),
		this, TQ_SLOT( changeSortColumn(int) ) );
    connect( header(), TQ_SIGNAL( sectionClicked(int) ),
	     this, TQ_SLOT( changeSortColumn2(int) ) );
    connect( this, TQ_SIGNAL( contentsMoving(int,int) ),
	     this, TQ_SLOT( contentsMoved(int,int) ) );

    viewport()->setAcceptDrops( true );
    sortcolumn = 0;
    ascending = true;
    dragItem = 0;
}

void TQFileDialogTQFileListView::setSorting( int column, bool increasing )
{
    if ( column == -1 ) {
	TQListView::setSorting( column, increasing );
	return;
    }

    sortAscending = ascending = increasing;
    sortcolumn = column;
    switch ( column ) {
    case 0:
	sortFilesBy = TQDir::Name;
	break;
    case 1:
	sortFilesBy = TQDir::Size;
	break;
    case 3:
	sortFilesBy = TQDir::Time;
	break;
    default:
	sortFilesBy = TQDir::Name; // #### ???
	break;
    }

    filedialog->resortDir();
}

void TQFileDialogTQFileListView::changeSortColumn2( int column )
{
    int lcol = header()->mapToLogical( column );
    setSorting( lcol, sortcolumn == lcol ? !ascending : true );
}

void TQFileDialogTQFileListView::keyPressEvent( TQKeyEvent *e )
{
    if ( ( e->key() == Key_Enter ||
	   e->key() == Key_Return ) &&
	 renaming )
	return;

    TQString keyPressed = e->text().lower();
    TQChar keyChar = keyPressed[0];
    if ( keyChar.isLetterOrNumber() ) {
	TQListViewItem * i = 0;
	if ( currentItem() )
	i = currentItem();
	else
	i = firstChild();
	if ( i->nextSibling() )
	i = i->nextSibling();
	else
	i = firstChild();
	while ( i != currentItem() ) {
	    TQString it = i->text(0);
	    if ( it[0].lower() == keyChar ) {
	    clearSelection();
	    ensureItemVisible( i );
	    setCurrentItem( i );
	    } else {
	    if ( i->nextSibling() )
	    i = i->nextSibling();
	    else
	    i = firstChild();
	    }
	}
	return;
    }

    cancelRename();
    TQListView::keyPressEvent( e );
}

void TQFileDialogTQFileListView::viewportMousePressEvent( TQMouseEvent *e )
{
    pressPos = e->pos();
    mousePressed = false;

    bool didRename = renaming;
    cancelRename();
    if ( !hasFocus() && !viewport()->hasFocus() )
	setFocus();

    if ( e->button() != LeftButton ) {
	TQListView::viewportMousePressEvent( e );
	firstMousePressEvent = false;
	return;
    }

    TQListViewItem *i = currentItem();
    TQListView::viewportMousePressEvent( e );

    TQFileDialogPrivate::File *i1 = (TQFileDialogPrivate::File*)currentItem();
    if ( i1 )
	mousePressed = !i1->info.isDir() || ( filedialog->mode() == TQFileDialog::Directory ) || ( filedialog->mode() == TQFileDialog::DirectoryOnly );


    if ( itemAt( e->pos() ) != i ||
	 e->x() + contentsX() > columnWidth( 0 ) ) {
	firstMousePressEvent = false;
	return;
    }

    if ( !firstMousePressEvent && !didRename && i == currentItem() && currentItem() &&
	 TQUrlInfo( filedialog->d->url, "." ).isWritable() && currentItem()->text( 0 ) != ".." ) {
	renameTimer->start( TQApplication::doubleClickInterval(), true );
	renameItem = currentItem();
    }

    firstMousePressEvent = false;
}

void TQFileDialogTQFileListView::viewportMouseDoubleClickEvent( TQMouseEvent *e )
{
    renameTimer->stop();
    TQListView::viewportMouseDoubleClickEvent( e );
}

void TQFileDialogTQFileListView::viewportMouseReleaseEvent( TQMouseEvent *e )
{
    TQListView::viewportMouseReleaseEvent( e );
    mousePressed = false;
    dragItem = 0;
}

void TQFileDialogTQFileListView::viewportMouseMoveEvent( TQMouseEvent *e )
{
    renameTimer->stop();
    if ( !dragItem )
	dragItem = itemAt( e->pos() );
#ifndef TQT_NO_DRAGANDDROP
    if (  ( pressPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() && mousePressed ) {
	TQListViewItem *item = dragItem;
	dragItem = 0;
	if ( item ) {
	    TQUriDrag* drag = new TQUriDrag( viewport() );
	    TQStringList files;
	    if ( filedialog->mode() == TQFileDialog::ExistingFiles )
		files = filedialog->selectedFiles();
	    else
		files = filedialog->selectedFile();
	    drag->setFileNames( files );

	    if ( lined->isVisible() )
		cancelRename();

	    connect( drag, TQ_SIGNAL( destroyed() ),
		     this, TQ_SLOT( dragObjDestroyed() ) );
	    drag->drag();

	    mousePressed = false;
	}
    }
#endif
}

void TQFileDialogTQFileListView::dragObjDestroyed()
{
#ifndef TQT_NO_DRAGANDDROP
    //######
    //filedialog->rereadDir();
#endif
}

#ifndef TQT_NO_DRAGANDDROP
void TQFileDialogTQFileListView::viewportDragEnterEvent( TQDragEnterEvent *e )
{
    startDragUrl = filedialog->d->url;
    startDragDir = filedialog->dirPath();
    currDropItem = 0;

    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    TQStringList l;
    TQUriDrag::decodeLocalFiles( e, l );
    urls = (int)l.count();

    if ( acceptDrop( e->pos(), e->source() ) ) {
	e->accept();
	setCurrentDropItem( e->pos() );
    } else {
	e->ignore();
	setCurrentDropItem( TQPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void TQFileDialogTQFileListView::viewportDragMoveEvent( TQDragMoveEvent *e )
{
    if ( acceptDrop( e->pos(), e->source() ) ) {
	if ( oldDragPos != e->pos() )
	    setCurrentDropItem( e->pos() );
	switch ( e->action() ) {
	case TQDropEvent::Copy:
	    e->acceptAction();
	    break;
	case TQDropEvent::Move:
	    e->acceptAction();
	    break;
	case TQDropEvent::Link:
	    break;
	default:
	    break;
	}
    } else {
	changeDirTimer->stop();
	e->ignore();
	setCurrentDropItem( TQPoint( -1, -1 ) );
    }

    oldDragPos = e->pos();
}

void TQFileDialogTQFileListView::viewportDragLeaveEvent( TQDragLeaveEvent * )
{
    changeDirTimer->stop();
    setCurrentDropItem( TQPoint( -1, -1 ) );
//########
//     if ( startDragDir != filedialog->d->url )
//	filedialog->setUrl( startDragUrl );
}

void TQFileDialogTQFileListView::viewportDropEvent( TQDropEvent *e )
{
    changeDirTimer->stop();

    if ( !TQUriDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }

    TQStringList l;
    TQUriDrag::decodeToUnicodeUris( e, l );

    bool move = e->action() == TQDropEvent::Move;
//     bool supportAction = move || e->action() == TQDropEvent::Copy;

    TQUrlOperator dest;
    if ( currDropItem )
	dest = TQUrlOperator( filedialog->d->url, TQFileDialogPrivate::encodeFileName( currDropItem->text( 0 ) ) );
    else
	dest = filedialog->d->url;
    
    // make sure that we can write to the destination before performing the action
    bool exists = false;
    TQString name = (currDropItem ? TQFileDialogPrivate::encodeFileName(currDropItem->text(0)) : TQString("."));
    TQUrlInfo info(filedialog->d->url, name);
    for (uint i = 0; i < l.count(); ++i) {
        int slash = l[i].findRev('/');
        TQString filename = l[i].right(l[i].length() - slash - 1);
        exists = exists || TQFileDialogPrivate::fileExists(dest, filename);
    }
    if (info.isWritable() && !exists)
        filedialog->d->url.copy( l, dest, move );

    // ##### what is supportAction for?
    e->acceptAction();
    currDropItem = 0;
}

bool TQFileDialogTQFileListView::acceptDrop( const TQPoint &pnt, TQWidget *source )
{
    TQListViewItem *item = itemAt( pnt );
    if ( !item || ( item && !itemRect( item ).contains( pnt ) ) ) {
	if ( source == viewport() && startDragDir == filedialog->dirPath() )
	    return false;
	return true;
    }

    TQUrlInfo fi( filedialog->d->url, item->text( 0 ) );

    if ( fi.isDir() && itemRect( item ).contains( pnt ) )
	return true;
    return false;
}

void TQFileDialogTQFileListView::setCurrentDropItem( const TQPoint &pnt )
{
    changeDirTimer->stop();

    TQListViewItem *item = itemAt( pnt );
    if ( pnt == TQPoint( -1, -1 ) )
	item = 0;
    if ( item && !TQUrlInfo( filedialog->d->url, item->text( 0 ) ).isDir() )
	item = 0;

    if ( item && !itemRect( item ).contains( pnt ) )
	item = 0;

    currDropItem = item;

    if ( currDropItem )
	setCurrentItem( currDropItem );

    changeDirTimer->start( 750 );
}
#endif // TQT_NO_DRAGANDDROP

void TQFileDialogTQFileListView::changeDirDuringDrag()
{
#ifndef TQT_NO_DRAGANDDROP
    if ( !currDropItem )
	return;
    changeDirTimer->stop();
    TQUrl u( filedialog->d->url, TQFileDialogPrivate::encodeFileName(currDropItem->text( 0 ) ) );
    filedialog->setDir( u );
    currDropItem = 0;
#endif // TQT_NO_DRAGANDDROP
}


void TQFileDialogTQFileListView::doubleClickTimeout()
{
    startRename();
    renameTimer->stop();
}

void TQFileDialogTQFileListView::startRename( bool check )
{
    if ( check && ( !renameItem || renameItem != currentItem() ) )
	return;

    TQListViewItem *i = currentItem();
    setSelected( i, true );

    TQRect r = itemRect( i );
    int bdr = i->pixmap( 0 ) ?
	      i->pixmap( 0 )->width() : 16;
    int x = r.x() + bdr;
    int y = r.y();
    int w = columnWidth( 0 ) - bdr;
    int h = TQMAX( lined->height() + 2, r.height() );
    y = y + r.height() / 2 - h / 2;

    lined->parentWidget()->setGeometry( x, y, w + 6, h );
    lined->setFocus();
    lined->setText( i->text( 0 ) );
    lined->selectAll();
    lined->setFrame( false );
    lined->parentWidget()->show();
    viewport()->setFocusProxy( lined );
    renaming = true;
}

void TQFileDialogTQFileListView::clear()
{
    cancelRename();
    TQListView::clear();
}

void TQFileDialogTQFileListView::rename()
{
    if ( !lined->text().isEmpty() ) {
	TQString file = currentItem()->text( 0 );

	if ( lined->text() != file )
	    filedialog->d->url.rename( file, lined->text() );
    }
    cancelRename();
}

void TQFileDialogTQFileListView::cancelRename()
{
    renameItem = 0;
    lined->parentWidget()->hide();
    viewport()->setFocusProxy( this );
    renaming = false;
    if ( currentItem() )
	currentItem()->repaint();
    if ( lined->hasFocus() )
	viewport()->setFocus();
}

void TQFileDialogTQFileListView::contentsMoved( int, int )
{
    changeDirTimer->stop();
#ifndef TQT_NO_DRAGANDDROP
    setCurrentDropItem( TQPoint( -1, -1 ) );
#endif
}


TQFileDialogPrivate::File::~File()
{
    if ( d->pendingItems.findRef( this ) )
	d->pendingItems.removeRef( this );
}

TQString TQFileDialogPrivate::File::text( int column ) const
{
    makeVariables();

    switch( column ) {
    case 0:
        return info.name();
    case 1:
	if ( info.isFile() ) {
#if (TQT_VERSION-0 >= 0x040000)
#error "clean up Large File Support"
#elif defined(QT_ABI_QT4)
	    TQIODevice::Offset size = info.size();
#else
	    uint size = info.size();
#endif
#if defined(QT_LARGEFILE_SUPPORT) && defined(Q_OS_UNIX)
	    // ### the following code should not be needed as soon
	    // ### as TQUrlInfo::size() can return 64-bit
	    if ( size > INT_MAX ) {
		struct stat buffer;
		if ( ::stat( TQFile::encodeName(info.name()), &buffer ) == 0 ) {
		    TQ_ULLONG size64 = (TQ_ULLONG)buffer.st_size;
		    return TQString::number(size64);
		}
	    }
#endif
	    return TQString::number(size);
	} else {
	    return TQString::fromLatin1("");
	}
    case 2:
	if ( info.isFile() && info.isSymLink() ) {
	    return d->symLinkToFile;
	} else if ( info.isFile() ) {
	    return d->file;
	} else if ( info.isDir() && info.isSymLink() ) {
	    return d->symLinkToDir;
	} else if ( info.isDir() ) {
	    return d->dir;
	} else if ( info.isSymLink() ) {
	    return d->symLinkToSpecial;
	} else {
	    return d->special;
	}
    case 3: {
	return info.lastModified().toString( TQt::LocalDate );
    }
    case 4:
	if ( info.isReadable() )
	    return info.isWritable() ? d->rw : d->ro;
	else
	    return info.isWritable() ? d->wo : d->inaccessible;
    }

    return TQString::fromLatin1("<--->");
}

const TQPixmap * TQFileDialogPrivate::File::pixmap( int column ) const
{
    if ( column ) {
	return 0;
    } else if ( TQListViewItem::pixmap( column ) ) {
	return TQListViewItem::pixmap( column );
    } else if ( info.isSymLink() ) {
	if ( info.isFile() )
	    return symLinkFileIcon;
	else
	    return symLinkDirIcon;
    } else if ( info.isDir() ) {
	return closedFolderIcon;
    } else if ( info.isFile() ) {
	return fileIcon;
    } else {
	return fifteenTransparentPixels;
    }
}

TQFileDialogPrivate::MCItem::MCItem( TQListBox * lb, TQListViewItem * item )
    : TQListBoxItem()
{
    i = item;
    if ( lb )
	lb->insertItem( this );
}

TQFileDialogPrivate::MCItem::MCItem( TQListBox * lb, TQListViewItem * item, TQListBoxItem *after )
    : TQListBoxItem()
{
    i = item;
    if ( lb )
	lb->insertItem( this, after );
}

TQString TQFileDialogPrivate::MCItem::text() const
{
    return i->text( 0 );
}


const TQPixmap *TQFileDialogPrivate::MCItem::pixmap() const
{
    return i->pixmap( 0 );
}


int TQFileDialogPrivate::MCItem::height( const TQListBox * lb ) const
{
    int hf = lb->fontMetrics().height();
    int hp = pixmap() ? pixmap()->height() : 0;
    return TQMAX(hf, hp) + 2;
}


int TQFileDialogPrivate::MCItem::width( const TQListBox * lb ) const
{
    TQFontMetrics fm = lb->fontMetrics();
    int w = 2;
    if ( pixmap() )
	w += pixmap()->width() + 4;
    else
	w += 18;
    w += fm.width( text() );
    w += -fm.minLeftBearing();
    w += -fm.minRightBearing();
    w += 6;
    return w;
}


void TQFileDialogPrivate::MCItem::paint( TQPainter * ptr )
{
    TQFontMetrics fm = ptr->fontMetrics();

    int h;

    if ( pixmap() )
	h = TQMAX( fm.height(), pixmap()->height()) + 2;
    else
	h = fm.height() + 2;

    const TQPixmap * pm = pixmap();
    if ( pm )
	ptr->drawPixmap( 2, 1, *pm );

    ptr->drawText( pm ? pm->width() + 4 : 22, h - fm.descent() - 2,
		   text() );
}

static TQStringList makeFiltersList( const TQString &filter )
{
    if ( filter.isEmpty() )
	return TQStringList();

    int i = filter.find( ";;", 0 );
    TQString sep( ";;" );
    if ( i == -1 ) {
	if ( filter.find( "\n", 0 ) != -1 ) {
	    sep = "\n";
	    i = filter.find( sep, 0 );
	}
    }

    return TQStringList::split( sep, filter );
}

/*!
  \class TQFileDialog ntqfiledialog.h
  \brief The TQFileDialog class provides dialogs that allow users to select files or directories.
  \ingroup dialogs
  \mainclass

  The TQFileDialog class enables a user to traverse their file system in
  order to select one or many files or a directory.

  The easiest way to create a TQFileDialog is to use the static
  functions. On Windows, these static functions will call the native
  Windows file dialog and on Mac OS X, these static function will call
  the native Mac OS X file dialog.

  \code
    TQString s = TQFileDialog::getOpenFileName(
		    "/home",
		    "Images (*.png *.xpm *.jpg)",
		    this,
		    "open file dialog",
		    "Choose a file" );
  \endcode

  In the above example, a modal TQFileDialog is created using a static
  function. The startup directory is set to "/home". The file filter
  is set to "Images (*.png *.xpm *.jpg)". The parent of the file dialog
  is set to \e this and it is given the identification name - "open file
  dialog". The caption at the top of file dialog is set to "Choose a
  file". If you want to use multiple filters, separate each one with
  \e two semi-colons, e.g.
  \code
  "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
  \endcode

  You can create your own TQFileDialog without using the static
  functions. By calling setMode(), you can set what can be returned by
  the TQFileDialog.

  \code
    TQFileDialog* fd = new TQFileDialog( this, "file dialog", true );
    fd->setMode( TQFileDialog::AnyFile );
  \endcode

  In the above example, the mode of the file dialog is set to \c
  AnyFile, meaning that the user can select any file, or even specify a
  file that doesn't exist. This mode is useful for creating a "File Save
  As" file dialog. Use \c ExistingFile if the user must select an
  existing file or \c Directory if only a directory may be selected.
  (See the \l TQFileDialog::Mode enum for the complete list of modes.)

  You can retrieve the dialog's mode with mode(). Use setFilter() to set
  the dialog's file filter, e.g.

  \code
    fd->setFilter( "Images (*.png *.xpm *.jpg)" );
  \endcode

  In the above example, the filter is set to "Images (*.png *.xpm
  *.jpg)", this means that only files with the extension \c png, \c xpm
  or \c jpg will be shown in the TQFileDialog. You can apply
  several filters by using setFilters() and add additional filters with
  addFilter(). Use setSelectedFilter() to select one of the filters
  you've given as the file dialog's default filter. Whenever the user
  changes the filter the filterSelected() signal is emitted.

  The file dialog has two view modes, TQFileDialog::List which simply
  lists file and directory names and TQFileDialog::Detail which
  displays additional information alongside each name, e.g. file size,
  modification date, etc. Set the mode with setViewMode().

  \code
    fd->setViewMode( TQFileDialog::Detail );
  \endcode

  The last important function you will need to use when creating your
  own file dialog is selectedFile().

  \code
    TQString fileName;
    if ( fd->exec() == TQDialog::Accepted )
	fileName = fd->selectedFile();
  \endcode

  In the above example, a modal file dialog is created and shown. If
  the user clicked OK, then the file they selected is put in \c
  fileName.

  If you are using the \c ExistingFiles mode then you will need to use
  selectedFiles() which will return the selected files in a TQStringList.

  The dialog's working directory can be set with setDir(). The display
  of hidden files is controlled with setShowHiddenFiles(). The dialog
  can be forced to re-read the directory with rereadDir() and re-sort
  the directory with resortDir(). All the files in the current directory
  can be selected with selectAll().

  \section1 Creating and using preview widgets

  There are two kinds of preview widgets that can be used with
  TQFileDialogs: \e content preview widgets and \e information preview
  widgets. They are created and used in the same way except that the
  function names differ, e.g. setContentsPreview() and setInfoPreview().

  A preview widget is a widget that is placed inside a TQFileDialog so
  that the user can see either the contents of the file, or information
  about the file.

  \code
    class Preview : public TQLabel, public TQFilePreview
    {
    public:
	Preview( TQWidget *parent=0 ) : TQLabel( parent ) {}

	void previewUrl( const TQUrl &u )
	{
	    TQString path = u.path();
	    TQPixmap pix( path );
	    if ( pix.isNull() )
		setText( "This is not a pixmap" );
	    else
		setPixmap( pix );
	}
    };
  \endcode

  In the above snippet, we create a preview widget which inherits from
  TQLabel and TQFilePreview. File preview widgets \e must inherit from
  TQFilePreview.

  Inside the class we reimplement TQFilePreview::previewUrl(), this is
  where we determine what happens when a file is selected. In the
  above example we only show a preview of the file if it is a valid
  pixmap. Here's how to make a file dialog use a preview widget:

  \code
    Preview* p = new Preview;

    TQFileDialog* fd = new TQFileDialog( this );
    fd->setContentsPreviewEnabled( true );
    fd->setContentsPreview( p, p );
    fd->setPreviewMode( TQFileDialog::Contents );
    fd->show();
  \endcode

  The first line creates an instance of our preview widget. We then
  create our file dialog and call setContentsPreviewEnabled( true ),
  this tell the file dialog to preview the contents of the currently
  selected file. We then call setContentsPreview() -- note that we pass
  the same preview widget twice. Finally, before showing the file
  dialog, we call setPreviewMode() setting the mode to \e Contents which
  will show the contents preview of the file that the user has selected.

  If you create another preview widget that is used for displaying
  information about a file, create it in the same way as the contents
  preview widget and call setInfoPreviewEnabled(), and
  setInfoPreview(). Then the user will be able to switch between the
  two preview modes.

  For more information about creating a TQFilePreview widget see
  \l{TQFilePreview}.

  <img src=qfiledlg-m.png> <img src=qfiledlg-w.png>

*/


/*! \enum TQFileDialog::Mode

  This enum is used to indicate what the user may select in the file
  dialog, i.e. what the dialog will return if the user clicks OK.

  \value AnyFile  The name of a file, whether it exists or not.
  \value ExistingFile  The name of a single existing file.
  \value Directory  The name of a directory. Both files and directories
  are displayed.
  \value DirectoryOnly  The name of a directory. The file dialog will only display directories.
  \value ExistingFiles  The names of zero or more existing files.

  See setMode().
*/

/*!
  \enum TQFileDialog::ViewMode

  This enum describes the view mode of the file dialog, i.e. what
  information about each file will be displayed.

  \value List  Display file and directory names with icons.
  \value Detail  Display file and directory names with icons plus
  additional information, such as file size and modification date.

  See setViewMode().
*/

/*!
  \enum TQFileDialog::PreviewMode

  This enum describes the preview mode of the file dialog.

  \value NoPreview  No preview is shown at all.
  \value Contents  Show a preview of the contents of the current file
  using the contents preview widget.
  \value Info  Show information about the current file using the
  info preview widget.

  See setPreviewMode(), setContentsPreview() and setInfoPreview().
*/

/*!
  \fn void TQFileDialog::detailViewSelectionChanged()
  \internal
*/

/*!
  \fn void TQFileDialog::listBoxSelectionChanged()
  \internal
*/

extern const char qt_file_dialog_filter_reg_exp[] =
	"([a-zA-Z0-9 ]*)\\(([a-zA-Z0-9_.*? +;#\\[\\]]*)\\)$";

/*!
  Constructs a file dialog called \a name, with the parent, \a parent.
  If \a modal is true then the file dialog is modal; otherwise it is
  modeless.
*/

TQFileDialog::TQFileDialog( TQWidget *parent, const char *name, bool modal )
    : TQDialog( parent, name, modal,
	       (modal ?
		(WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu) : 0) )
{
    init();
    d->mode = ExistingFile;
    d->types->insertItem( tr( "All Files (*)" ) );
    d->cursorOverride = false;
    emit dirEntered( d->url.dirPath() );
    rereadDir();
}


/*!
  Constructs a file dialog called \a name with the parent, \a parent.
  If \a modal is true then the file dialog is modal; otherwise it is
  modeless.

  If \a dirName is specified then it will be used as the dialog's
  working directory, i.e. it will be the directory that is shown when
  the dialog appears. If \a filter is specified it will be used as the
  dialog's file filter.

*/

TQFileDialog::TQFileDialog( const TQString& dirName, const TQString & filter,
			  TQWidget *parent, const char *name, bool modal )
    : TQDialog( parent, name, modal,
	       (modal ?
		(WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu) : 0) )
{
    init();
    d->mode = ExistingFile;
    rereadDir();
    TQUrlOperator u( dirName );
    if ( !dirName.isEmpty() && ( !u.isLocalFile() || TQDir( dirName ).exists() ) )
	setSelection( dirName );
    else if ( workingDirectory && !workingDirectory->isEmpty() )
	setDir( *workingDirectory );

    if ( !filter.isEmpty() ) {
	setFilters( filter );
	if ( !dirName.isEmpty() ) {
	    int dotpos = dirName.find( TQChar('.'), 0, false );
	    if ( dotpos != -1 ) {
		for ( int b=0 ; b<d->types->count() ; b++ ) {
		    if ( d->types->text(b).contains( dirName.right( dirName.length() - dotpos ) ) ) {
			d->types->setCurrentItem( b );
			setFilter( d->types->text( b ) );
			return;
		    }
		}
	    }
	}
    } else {
	d->types->insertItem( tr( "All Files (*)" ) );
    }
}


#if defined(TQ_WS_WIN)
extern int tqt_ntfs_permission_lookup;
#endif

/*!
  \internal
  Initializes the file dialog.
*/

void TQFileDialog::init()
{
    setSizeGripEnabled( true );
    d = new TQFileDialogPrivate();
    d->mode = AnyFile;
    d->last = 0;
    d->lastEFSelected = 0;
    d->moreFiles = 0;
    d->infoPreview = false;
    d->contentsPreview = false;
    d->hadDotDot = false;
    d->ignoreNextKeyPress = false;
    d->progressDia = 0;
    d->checkForFilter = false;
    d->ignoreNextRefresh = false;
    d->ignoreStop = false;
    d->pendingItems.setAutoDelete( false );
    d->mimeTypeTimer = new TQTimer( this );
    d->cursorOverride = false;
#if defined(TQ_WS_WIN)
    d->oldPermissionLookup = tqt_ntfs_permission_lookup;
#endif
    connect( d->mimeTypeTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doMimeTypeLookup() ) );

    d->url = TQUrlOperator( ::toRootIfNotExists( TQDir::currentDirPath() ) );
    d->oldUrl = d->url;
    d->currListChildren = 0;

    connect( &d->url, TQ_SIGNAL( start(TQNetworkOperation*) ),
	     this, TQ_SLOT( urlStart(TQNetworkOperation*) ) );
    connect( &d->url, TQ_SIGNAL( finished(TQNetworkOperation*) ),
	     this, TQ_SLOT( urlFinished(TQNetworkOperation*) ) );
    connect( &d->url, TQ_SIGNAL( newChildren(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ),
	     this, TQ_SLOT( insertEntry(const TQValueList<TQUrlInfo>&,TQNetworkOperation*) ) );
    connect( &d->url, TQ_SIGNAL( removed(TQNetworkOperation*) ),
	     this, TQ_SLOT( removeEntry(TQNetworkOperation*) ) );
    connect( &d->url, TQ_SIGNAL( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ),
	     this, TQ_SLOT( createdDirectory(const TQUrlInfo&,TQNetworkOperation*) ) );
    connect( &d->url, TQ_SIGNAL( itemChanged(TQNetworkOperation*) ),
	     this, TQ_SLOT( itemChanged(TQNetworkOperation*) ) );
    connect( &d->url, TQ_SIGNAL( dataTransferProgress(int,int,TQNetworkOperation*) ),
	     this, TQ_SLOT( dataTransferProgress(int,int,TQNetworkOperation*) ) );

    nameEdit = new TQLineEdit( this, "name/filter editor" );
    nameEdit->setMaxLength( 255 ); //_POSIX_MAX_PATH
    connect( nameEdit, TQ_SIGNAL(textChanged(const TQString&)),
	     this,  TQ_SLOT(fileNameEditDone()) );
    nameEdit->installEventFilter( this );

    d->splitter = new TQSplitter( this, "qt_splitter" );

    d->stack = new TQWidgetStack( d->splitter, "files and more files" );

    d->splitter->setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding ) );

    files = new TQFileDialogTQFileListView( d->stack, this );
    TQFontMetrics fm = fontMetrics();
    files->addColumn( tr("Name") );
    files->addColumn( tr("Size") );
    files->setColumnAlignment( 1, AlignRight );
    files->addColumn( tr("Type") );
    files->addColumn( tr("Date") );
    files->addColumn( tr("Attributes") );
    files->header()->setStretchEnabled( true, 0 );

    files->setMinimumSize( 50, 25 + 2*fm.lineSpacing() );

    connect( files, TQ_SIGNAL( selectionChanged() ),
	     this, TQ_SLOT( detailViewSelectionChanged() ) );
    connect( files, TQ_SIGNAL(currentChanged(TQListViewItem*)),
	     this, TQ_SLOT(updateFileNameEdit(TQListViewItem*)) );
    connect( files, TQ_SIGNAL(doubleClicked(TQListViewItem*)),
	     this, TQ_SLOT(selectDirectoryOrFile(TQListViewItem*)) );
    connect( files, TQ_SIGNAL(returnPressed(TQListViewItem*)),
	     this, TQ_SLOT(selectDirectoryOrFile(TQListViewItem*)) );
    connect( files, TQ_SIGNAL(rightButtonPressed(TQListViewItem*,const TQPoint&,int)),
	     this, TQ_SLOT(popupContextMenu(TQListViewItem*,const TQPoint&,int)) );

    files->installEventFilter( this );
    files->viewport()->installEventFilter( this );

    d->moreFiles = new TQFileListBox( d->stack, this );
    d->moreFiles->setRowMode( TQListBox::FitToHeight );
    d->moreFiles->setVariableWidth( true );

    connect( d->moreFiles, TQ_SIGNAL(selected(TQListBoxItem*)),
	     this, TQ_SLOT(selectDirectoryOrFile(TQListBoxItem*)) );
    connect( d->moreFiles, TQ_SIGNAL( selectionChanged() ),
	     this, TQ_SLOT( listBoxSelectionChanged() ) );
    connect( d->moreFiles, TQ_SIGNAL(highlighted(TQListBoxItem*)),
      this, TQ_SLOT(updateFileNameEdit(TQListBoxItem*)) );
    connect( d->moreFiles, TQ_SIGNAL( rightButtonPressed(TQListBoxItem*,const TQPoint&) ),
	     this, TQ_SLOT( popupContextMenu(TQListBoxItem*,const TQPoint&) ) );

    d->moreFiles->installEventFilter( this );
    d->moreFiles->viewport()->installEventFilter( this );

    okB = new TQPushButton( tr("&OK"), this, "OK" ); //### Or "Save (see other "OK")
    okB->setDefault( true );
    okB->setEnabled( false );
    connect( okB, TQ_SIGNAL(clicked()), this, TQ_SLOT(okClicked()) );
    cancelB = new TQPushButton( tr("Cancel") , this, "Cancel" );
    connect( cancelB, TQ_SIGNAL(clicked()), this, TQ_SLOT(cancelClicked()) );

    d->paths = new TQComboBox( true, this, "directory history/editor" );
    d->paths->setDuplicatesEnabled( false );
    d->paths->setInsertionPolicy( TQComboBox::NoInsertion );
    const TQFileInfoList * rootDrives = TQDir::drives();
    TQFileInfoListIterator it( *rootDrives );
    TQFileInfo *fi;
    makeVariables();

    while ( (fi = it.current()) != 0 ) {
	++it;
	d->paths->insertItem( *openFolderIcon, fi->absFilePath() );
    }

    if ( !!TQDir::homeDirPath() ) {
	if ( !d->paths->listBox()->findItem( TQDir::homeDirPath() ) )
	    d->paths->insertItem( *openFolderIcon, TQDir::homeDirPath() );
    }

    connect( d->paths, TQ_SIGNAL(activated(const TQString&)),
	     this, TQ_SLOT(setDir(const TQString&)) );

    d->paths->installEventFilter( this );
    TQObjectList *ol = d->paths->queryList( "TQLineEdit" );
    if ( ol && ol->first() )
	( (TQLineEdit*)ol->first() )->installEventFilter( this );
    delete ol;

    d->geometryDirty = true;
    d->types = new TQComboBox( true, this, "file types" );
    d->types->setDuplicatesEnabled( false );
    d->types->setEditable( false );
    connect( d->types, TQ_SIGNAL(activated(const TQString&)),
	     this, TQ_SLOT(setFilter(const TQString&)) );
    connect( d->types, TQ_SIGNAL(activated(const TQString&)),
	     this, TQ_SIGNAL(filterSelected(const TQString&)) );

    d->pathL = new TQLabel( d->paths, tr("Look &in:"), this, "qt_looin_lbl" );
    d->fileL = new TQLabel( nameEdit, tr("File &name:"), this, "qt_filename_lbl" );
    d->typeL = new TQLabel( d->types, tr("File &type:"), this, "qt_filetype_lbl" );

    d->goBack = new TQToolButton( this, "go back" );
    d->goBack->setEnabled( false );
    d->goBack->setFocusPolicy( TabFocus );
    connect( d->goBack, TQ_SIGNAL( clicked() ), this, TQ_SLOT( goBack() ) );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->goBack, tr( "Back" ) );
#endif
    d->goBack->setIconSet( *goBackIcon );

    d->cdToParent = new TQToolButton( this, "cd to parent" );
    d->cdToParent->setFocusPolicy( TabFocus );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->cdToParent, tr( "One directory up" ) );
#endif
    d->cdToParent->setIconSet( *cdToParentIcon );
    connect( d->cdToParent, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(cdUpClicked()) );

    d->newFolder = new TQToolButton( this, "new folder" );
    d->newFolder->setFocusPolicy( TabFocus );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->newFolder, tr( "Create New Folder" ) );
#endif
    d->newFolder->setIconSet( *newFolderIcon );
    connect( d->newFolder, TQ_SIGNAL(clicked()),
	     this, TQ_SLOT(newFolderClicked()) );

    d->modeButtons = new TQButtonGroup( 0, "invisible group" );
    connect( d->modeButtons, TQ_SIGNAL(destroyed()),
	     this, TQ_SLOT(modeButtonsDestroyed()) );
    d->modeButtons->setExclusive( true );
    connect( d->modeButtons, TQ_SIGNAL(clicked(int)),
	     d->stack, TQ_SLOT(raiseWidget(int)) );
    connect( d->modeButtons, TQ_SIGNAL(clicked(int)),
	     this, TQ_SLOT(changeMode(int)) );

    d->mcView = new TQToolButton( this, "mclistbox view" );
    d->mcView->setFocusPolicy( TabFocus );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->mcView, tr( "List View" ) );
#endif
    d->mcView->setIconSet( *multiColumnListViewIcon );
    d->mcView->setToggleButton( true );
    d->stack->addWidget( d->moreFiles, d->modeButtons->insert( d->mcView ) );
    d->detailView = new TQToolButton( this, "list view" );
    d->detailView->setFocusPolicy( TabFocus );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->detailView, tr( "Detail View" ) );
#endif
    d->detailView->setIconSet( *detailViewIcon );
    d->detailView->setToggleButton( true );
    d->stack->addWidget( files, d->modeButtons->insert( d->detailView ) );

    d->previewInfo = new TQToolButton( this, "preview info view" );
    d->previewInfo->setFocusPolicy( TabFocus );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->previewInfo, tr( "Preview File Info" ) );
#endif
    d->previewInfo->setIconSet( *previewInfoViewIcon );
    d->previewInfo->setToggleButton( true );
    d->modeButtons->insert( d->previewInfo );

    d->previewContents = new TQToolButton( this, "preview info view" );
#if defined(TQ_WS_WIN) && !defined(Q_OS_TEMP)
    if ( (tqt_winver & WV_NT_based) > TQt::WV_NT )
#else
    if ( !qstrcmp(style().className(), "TQWindowsStyle") )
#endif
    {
	d->goBack->setAutoRaise( true );
	d->cdToParent->setAutoRaise( true );
	d->newFolder->setAutoRaise( true );
	d->mcView->setAutoRaise( true );
	d->detailView->setAutoRaise( true );
	d->previewInfo->setAutoRaise( true );
	d->previewContents->setAutoRaise( true );
    }
    d->previewContents->setFocusPolicy( TabFocus );
#ifndef TQT_NO_TOOLTIP
    TQToolTip::add( d->previewContents, tr( "Preview File Contents" ) );
#endif
    d->previewContents->setIconSet( *previewContentsViewIcon );
    d->previewContents->setToggleButton( true );
    d->modeButtons->insert( d->previewContents );

    connect( d->detailView, TQ_SIGNAL( clicked() ),
	     d->moreFiles, TQ_SLOT( cancelRename() ) );
    connect( d->detailView, TQ_SIGNAL( clicked() ),
	     files, TQ_SLOT( cancelRename() ) );
    connect( d->mcView, TQ_SIGNAL( clicked() ),
	     d->moreFiles, TQ_SLOT( cancelRename() ) );
    connect( d->mcView, TQ_SIGNAL( clicked() ),
	     files, TQ_SLOT( cancelRename() ) );

    d->stack->raiseWidget( d->moreFiles );
    d->mcView->setOn( true );

    TQHBoxLayout *lay = new TQHBoxLayout( this );
    lay->setMargin( 6 );
    d->leftLayout = new TQHBoxLayout( lay, 5 );
    d->topLevelLayout = new TQVBoxLayout( (TQWidget*)0, 5 );
    lay->addLayout( d->topLevelLayout, 1 );
    d->extraWidgetsLayouts.setAutoDelete( false );
    d->extraLabels.setAutoDelete( false );
    d->extraWidgets.setAutoDelete( false );
    d->extraButtons.setAutoDelete( false );
    d->toolButtons.setAutoDelete( false );

    TQHBoxLayout * h;

    d->preview = new TQWidgetStack( d->splitter, "qt_preview" );

    d->infoPreviewWidget = new TQWidget( d->preview, "qt_preview_info" );
    d->contentsPreviewWidget = new TQWidget( d->preview, "qt_preview_contents" );
    d->infoPreviewer = d->contentsPreviewer = 0;

    h = new TQHBoxLayout( 0 );
    d->buttonLayout = h;
    d->topLevelLayout->addLayout( h );
    h->addWidget( d->pathL );
    h->addSpacing( 8 );
    h->addWidget( d->paths );
    h->addSpacing( 8 );
    if ( d->goBack )
	h->addWidget( d->goBack );
    h->addWidget( d->cdToParent );
    h->addSpacing( 2 );
    h->addWidget( d->newFolder );
    h->addSpacing( 4 );
    h->addWidget( d->mcView );
    h->addWidget( d->detailView );
    h->addWidget( d->previewInfo );
    h->addWidget( d->previewContents );

    d->topLevelLayout->addWidget( d->splitter );

    h = new TQHBoxLayout();
    d->topLevelLayout->addLayout( h );
    h->addWidget( d->fileL );
    h->addWidget( nameEdit );
    h->addSpacing( 15 );
    h->addWidget( okB );

    h = new TQHBoxLayout();
    d->topLevelLayout->addLayout( h );
    h->addWidget( d->typeL );
    h->addWidget( d->types );
    h->addSpacing( 15 );
    h->addWidget( cancelB );

    d->rightLayout = new TQHBoxLayout( lay, 5 );
    d->topLevelLayout->setStretchFactor( d->mcView, 1 );
    d->topLevelLayout->setStretchFactor( files, 1 );

    updateGeometries();

    if ( d->goBack ) {
	setTabOrder( d->paths, d->goBack );
	setTabOrder( d->goBack, d->cdToParent );
    } else {
	setTabOrder( d->paths, d->cdToParent );
    }
    setTabOrder( d->cdToParent, d->newFolder );
    setTabOrder( d->newFolder, d->mcView );
    setTabOrder( d->mcView, d->detailView );
    setTabOrder( d->detailView, d->moreFiles );
    setTabOrder( d->moreFiles, files );
    setTabOrder( files, nameEdit );
    setTabOrder( nameEdit, d->types );
    setTabOrder( d->types, okB );
    setTabOrder( okB, cancelB );

    d->rw = tr( "Read-write" );
    d->ro = tr( "Read-only" );
    d->wo = tr( "Write-only" );
    d->inaccessible = tr( "Inaccessible" );

    d->symLinkToFile = tr( "Symlink to File" );
    d->symLinkToDir = tr( "Symlink to Directory" );
    d->symLinkToSpecial = tr( "Symlink to Special" );
    d->file = tr( "File" );
    d->dir = tr( "Dir" );
    d->special = tr( "Special" );

    if ( lastWidth == 0 ) {
	TQRect screen = TQApplication::desktop()->screenGeometry( pos() );
	if ( screen.width() < 1024 || screen.height() < 768 ) {
	    resize( TQMIN(screen.width(), 420), TQMIN(screen.height(), 236) );
	} else {
	    TQSize s = files->sizeHint();
	    s = TQSize( s.width() + 300, s.height() + 82 );

	    if ( s.width() * 3 > screen.width() * 2 )
		s.setWidth( screen.width() * 2 / 3 );

	    if ( s.height() * 3 > screen.height() * 2 )
		s.setHeight( screen.height() * 2 / 3 );
	    else if ( s.height() * 3 < screen.height() )
		s.setHeight( screen.height() / 3 );

	    resize( s );
	}
	updateLastSize(this);
    } else {
	resize( lastWidth, lastHeight );
    }

    if ( detailViewMode ) {
	d->stack->raiseWidget( files );
	d->mcView->setOn( false );
	d->detailView->setOn( true );
    }

    d->preview->hide();
    nameEdit->setFocus();

    connect( nameEdit, TQ_SIGNAL( returnPressed() ),
	     this, TQ_SLOT( fileNameEditReturnPressed() ) );
}

/*!
  \internal
*/

void TQFileDialog::fileNameEditReturnPressed()
{
    d->oldUrl = d->url;
    if ( !isDirectoryMode( d->mode ) ) {
	okClicked();
    } else {
	d->currentFileName = TQString::null;
	if ( nameEdit->text().isEmpty() ) {
	    emit fileSelected( selectedFile() );
	    accept();
	} else {
	    TQUrlInfo f;
	    TQFileDialogPrivate::File * c
		= (TQFileDialogPrivate::File *)files->currentItem();
	    if ( c && files->isSelected(c) )
		f = c->info;
	    else
		f = TQUrlInfo( d->url, nameEdit->text() );
	    if ( f.isDir() ) {
		setUrl( TQUrlOperator( d->url,
				      TQFileDialogPrivate::encodeFileName(nameEdit->text() + "/" ) ) );
		d->checkForFilter = true;
		trySetSelection( true, d->url, true );
		d->checkForFilter = false;
	    }
	}
	nameEdit->setText( TQString::null );
    }
}

/*!
  \internal
  Update the info and content preview widgets to display \a u.
*/

void TQFileDialog::updatePreviews( const TQUrl &u )
{
    if ( d->infoPreviewer )
	d->infoPreviewer->previewUrl( u );
    if ( d->contentsPreviewer )
	d->contentsPreviewer->previewUrl( u );
}

/*!
  \internal
  Changes the preview mode to the mode specified at \a id.
*/

void TQFileDialog::changeMode( int id )
{
    if ( !d->infoPreview && !d->contentsPreview )
	return;

    TQButton *btn = (TQButton*)d->modeButtons->find( id );
    if ( !btn )
	return;

    if ( btn == d->previewContents && !d->contentsPreview )
	return;
    if ( btn == d->previewInfo && !d->infoPreview )
	return;

    if ( btn != d->previewContents && btn != d->previewInfo ) {
	d->preview->hide();
    } else {
	if ( files->currentItem() )
	    updatePreviews( TQUrl( d->url, files->currentItem()->text( 0 ) ) );
	if ( btn == d->previewInfo )
	    d->preview->raiseWidget( d->infoPreviewWidget );
	else
	    d->preview->raiseWidget( d->contentsPreviewWidget );
	d->preview->show();
    }
}

/*!
  Destroys the file dialog.
*/

TQFileDialog::~TQFileDialog()
{
    // since clear might call setContentsPos which would emit
    // a signal and thus cause a recompute of sizes...
    files->blockSignals( true );
    d->moreFiles->blockSignals( true );
    files->clear();
    d->moreFiles->clear();
    d->moreFiles->blockSignals( false );
    files->blockSignals( false );

#ifndef TQT_NO_CURSOR
    if ( d->cursorOverride )
	TQApplication::restoreOverrideCursor();
#endif

    delete d;
    d = 0;
}


/*!
  \property TQFileDialog::selectedFile

  \brief the name of the selected file

  If a file was selected selectedFile contains the file's name including
  its absolute path; otherwise selectedFile is empty.

  \sa TQString::isEmpty(), selectedFiles, selectedFilter
*/

TQString TQFileDialog::selectedFile() const
{
    TQString s = d->currentFileName;
    // remove the protocol because we do not want to encode it...
    TQString prot = TQUrl( s ).protocol();
    if ( !prot.isEmpty() ) {
        prot += ":";
	s.remove( 0, prot.length() );
    }
    TQUrl u( prot + TQFileDialogPrivate::encodeFileName( s ) );
    if ( u.isLocalFile() ) {
	TQString s = u.toString();
	if ( s.left( 5 ) == "file:" )
	    s.remove( (uint)0, 5 );
	return s;
    }
    return d->currentFileName;
}

/*!
  \property TQFileDialog::selectedFilter

  \brief the filter which the user has selected in the file dialog

  \sa filterSelected(), selectedFiles, selectedFile
*/

TQString TQFileDialog::selectedFilter() const
{
    return d->types->currentText();
}

/*! \overload

  Sets the current filter selected in the file dialog to the
  \a{n}-th filter in the filter list.

  \sa filterSelected(), selectedFilter(), selectedFiles(), selectedFile()
*/

void TQFileDialog::setSelectedFilter( int n )
{
    d->types->setCurrentItem( n );
    TQString f = d->types->currentText();
    TQRegExp r( TQString::fromLatin1(qt_file_dialog_filter_reg_exp) );
    int index = r.search( f );
    if ( index >= 0 )
	f = r.cap( 2 );
    d->url.setNameFilter( f );
    rereadDir();
}

/*!
  Sets the current filter selected in the file dialog to the first
  one that contains the text \a mask.
*/

void TQFileDialog::setSelectedFilter( const TQString& mask )
{
    int n;

    for ( n = 0; n < d->types->count(); n++ ) {
	if ( d->types->text( n ).contains( mask, false ) ) {
	    d->types->setCurrentItem( n );
	    TQString f = mask;
	    TQRegExp r( TQString::fromLatin1(qt_file_dialog_filter_reg_exp) );
	    int index = r.search( f );
	    if ( index >= 0 )
		f = r.cap( 2 );
	    d->url.setNameFilter( f );
	    rereadDir();
	    return;
	}
    }
}

/*!
  \property TQFileDialog::selectedFiles

  \brief the list of selected files

  If one or more files are selected, selectedFiles contains their
  names including their absolute paths. If no files are selected or
  the mode isn't ExistingFiles selectedFiles is an empty list.

  It is more convenient to use selectedFile() if the mode is
  \c ExistingFile, \c Directory or \c DirectoryOnly.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \code
    TQStringList list = myFileDialog.selectedFiles();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  \sa selectedFile, selectedFilter, TQValueList::empty()
*/

TQStringList TQFileDialog::selectedFiles() const
{
    TQStringList lst;

    if ( mode() == ExistingFiles ) {
	TQStringList selectedLst;
	TQString selectedFiles = nameEdit->text();
        if (selectedFiles.findRev('\"') == -1) {
            //probably because Enter was pressed on the nameEdit, so we have one file
            //not in "" but raw
            selectedLst.append(selectedFiles);
        } else {
            selectedFiles.truncate( selectedFiles.findRev( '\"' ) );
            selectedLst = selectedLst.split( TQString("\" "), selectedFiles );
        }
	for ( TQStringList::Iterator it = selectedLst.begin(); it != selectedLst.end(); ++it ) {
	    TQUrl u;
	    if ( (*it)[0] == '\"' ) {
		u = TQUrl( d->url, TQFileDialogPrivate::encodeFileName( (*it).mid(1) ) );
	    } else {
		u = TQUrl( d->url, TQFileDialogPrivate::encodeFileName( (*it) ) );
	    }
	    if ( u.isLocalFile() ) {
		TQString s = u.toString();
		if ( s.left( 5 ) == "file:" )
		    s.remove( (uint)0, 5 );
		lst << s;
	    } else {
		lst << u.toString();
	    }
	}
    }

    return lst;
}

/*!
  Sets the default selection to \a filename. If \a filename is
  absolute, setDir() is also called to set the file dialog's working
  directory to the filename's directory.

  \omit
  Only for external use. Not useful inside TQFileDialog.
  \endomit
*/

void TQFileDialog::setSelection( const TQString & filename )
{
    d->oldUrl = d->url;
    TQString nf = d->url.nameFilter();
    if ( TQUrl::isRelativeUrl( filename ) )
	d->url = TQUrlOperator( d->url, TQFileDialogPrivate::encodeFileName( filename ) );
    else
	d->url = TQUrlOperator( filename );
    d->url.setNameFilter( nf );
    d->checkForFilter = true;
    bool isDirOk;
    bool isDir = d->url.isDir( &isDirOk );
    if ( !isDirOk )
	isDir = d->url.path().right( 1 ) == "/";
    if ( !isDir ) {
	TQUrlOperator u( d->url );
	d->url.setPath( d->url.dirPath() );
	trySetSelection( false, u, true );
	d->ignoreNextRefresh = true;
	nameEdit->selectAll();
	rereadDir();
	emit dirEntered( d->url.dirPath() );
    } else {
	if ( !d->url.path().isEmpty() &&
	     d->url.path().right( 1 ) != "/" ) {
	    TQString p = d->url.path();
	    p += "/";
	    d->url.setPath( p );
	}
	trySetSelection( true, d->url, false );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
	nameEdit->setText( TQString::fromLatin1("") );
    }
    d->checkForFilter = false;
}

/*!
  \property TQFileDialog::dirPath

  \brief the file dialog's working directory

  \sa dir(), setDir()
*/

TQString TQFileDialog::dirPath() const
{
    return d->url.dirPath();
}


/*!

  Sets the filter used in the file dialog to \a newFilter.

  If \a newFilter contains a pair of parentheses containing one or more
  of <em><b>anything*something</b></em> separated by spaces or by
  semi-colons then only the text contained in the parentheses is used as
  the filter. This means that these calls are all equivalent:

  \code
     fd->setFilter( "All C++ files (*.cpp *.cc *.C *.cxx *.c++)" );
     fd->setFilter( "*.cpp *.cc *.C *.cxx *.c++" );
     fd->setFilter( "All C++ files (*.cpp;*.cc;*.C;*.cxx;*.c++)" );
     fd->setFilter( "*.cpp;*.cc;*.C;*.cxx;*.c++" );
  \endcode

  \sa setFilters()
*/

void TQFileDialog::setFilter( const TQString & newFilter )
{
    if ( newFilter.isEmpty() )
	return;
    TQString f = newFilter;
    TQRegExp r( TQString::fromLatin1(qt_file_dialog_filter_reg_exp) );
    int index = r.search( f );
    if ( index >= 0 )
	f = r.cap( 2 );
    d->url.setNameFilter( f );
    if ( d->types->count() == 1 )  {
	d->types->clear();
	d->types->insertItem( newFilter );
    } else {
	for ( int i = 0; i < d->types->count(); ++i ) {
	    if ( d->types->text( i ).left( newFilter.length() ) == newFilter ||
		 d->types->text( i ).left( f.length() ) == f ) {
		d->types->setCurrentItem( i );
		break;
	    }
	}
    }
    rereadDir();
}


/*! \overload
  Sets the file dialog's working directory to \a pathstr.

  \sa dir()
*/

void TQFileDialog::setDir( const TQString & pathstr )
{
    TQString dr = pathstr;
    if ( dr.isEmpty() )
	return;

#if defined(Q_OS_UNIX)
    if ( dr.length() && dr[0] == '~' ) {
	int i = 0;
	while( i < (int)dr.length() && dr[i] != '/' )
	    i++;
	TQCString user;
	if ( i == 1 ) {
#if defined(TQT_THREAD_SUPPORT) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)

#  ifndef _POSIX_LOGIN_NAME_MAX
#    define _POSIX_LOGIN_NAME_MAX 9
#  endif

	    char name[_POSIX_LOGIN_NAME_MAX];
	    if ( ::getlogin_r( name, _POSIX_LOGIN_NAME_MAX ) == 0 )
		user = name;
	    else
#else
	    user = ::getlogin();
	    if ( !user )
#endif
		user = getenv( "LOGNAME" );
	} else
	    user = dr.mid( 1, i-1 ).local8Bit();
	dr = dr.mid( i, dr.length() );
	struct passwd *pw;
#if defined(TQT_THREAD_SUPPORT) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_FREEBSD) && !defined(Q_OS_OPENBSD)
	struct passwd mt_pw;
	char buffer[2048];
	if ( ::getpwnam_r( user, &mt_pw, buffer, 2048, &pw ) == 0 && pw == &mt_pw )
#else
	pw = ::getpwnam( user );
	if ( pw )
#endif
	    dr.prepend( TQString::fromLocal8Bit(pw->pw_dir) );
    }
#endif

    setUrl( dr );
}

/*!
  Returns the current directory shown in the file dialog.

  The ownership of the TQDir pointer is transferred to the caller, so
  it must be deleted by the caller when no longer required.

  \sa setDir()
*/

const TQDir *TQFileDialog::dir() const
{
    if ( d->url.isLocalFile() )
	return  new TQDir( d->url.path() );
    else
	return 0;
}

/*!
  Sets the file dialog's working directory to \a dir.
  \sa dir()
*/

void TQFileDialog::setDir( const TQDir &dir )
{
    d->oldUrl = d->url;
    TQString nf( d->url.nameFilter() );
    d->url = dir.canonicalPath();
    d->url.setNameFilter( nf );
    TQUrlInfo i( d->url, nameEdit->text() );
    d->checkForFilter = true;
    trySetSelection( i.isDir(), TQUrlOperator( d->url, TQFileDialogPrivate::encodeFileName(nameEdit->text() ) ), false );
    d->checkForFilter = false;
    rereadDir();
    emit dirEntered( d->url.path() );
}

/*!
  Sets the file dialog's working directory to the directory specified at \a url.

  \sa url()
*/

void TQFileDialog::setUrl( const TQUrlOperator &url )
{
    d->oldUrl = d->url;
    TQString nf = d->url.nameFilter();

    TQString operatorPath = url.toString( false, false );
    if ( TQUrl::isRelativeUrl( operatorPath ) ) {
	d->url = TQUrl( d->url, operatorPath );
    } else {
	d->url = url;
    }
    d->url.setNameFilter( nf );

    d->checkForFilter = true;
    if ( !d->url.isDir() ) {
	TQUrlOperator u = d->url;
	d->url.setPath( d->url.dirPath() );
	trySetSelection( false, u, false );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
	TQString fn = u.fileName();
	nameEdit->setText( fn );
    } else {
	trySetSelection( true, d->url, false );
	rereadDir();
	emit dirEntered( d->url.dirPath() );
    }
    d->checkForFilter = false;
}

/*!
  \property TQFileDialog::showHiddenFiles

  \brief whether hidden files are shown in the file dialog

  The default is false, i.e. don't show hidden files.
*/

void TQFileDialog::setShowHiddenFiles( bool s )
{
    if ( s == bShowHiddenFiles )
	return;

    bShowHiddenFiles = s;
    rereadDir();
}

bool TQFileDialog::showHiddenFiles() const
{
    return bShowHiddenFiles;
}

/*!
  Rereads the current directory shown in the file dialog.

  The only time you will need to call this function is if the contents of
  the directory change and you wish to refresh the file dialog to reflect
  the change.

  \sa resortDir()
*/

void TQFileDialog::rereadDir()
{
#ifndef TQT_NO_CURSOR
    if ( !d->cursorOverride ) {
	TQApplication::setOverrideCursor( TQCursor( TQt::WaitCursor ) );
	d->cursorOverride = true;
    }
#endif
    d->pendingItems.clear();
    if ( d->mimeTypeTimer->isActive() )
	d->mimeTypeTimer->stop();
    d->currListChildren = d->url.listChildren();
#ifndef TQT_NO_CURSOR
    if ( d->cursorOverride ) {
	TQApplication::restoreOverrideCursor();
	d->cursorOverride = false;
    }
#endif
}


/*!
  \fn void TQFileDialog::fileHighlighted( const TQString& )

  This signal is emitted when the user highlights a file, i.e. makes
  it the current file.

  \sa fileSelected(), filesSelected()
*/

/*!
  \fn void TQFileDialog::fileSelected( const TQString& )

  This signal is emitted when the user selects a file.

  \sa filesSelected(), fileHighlighted(), selectedFile()
*/

/*!
  \fn void TQFileDialog::filesSelected( const TQStringList& )

  This signal is emitted when the user selects one or more files in \e
  ExistingFiles mode.

  \sa fileSelected(), fileHighlighted(), selectedFiles()
*/

/*!
  \fn void TQFileDialog::dirEntered( const TQString& )

  This signal is emitted when the user enters a directory.

  \sa dir()
*/

/*!
  \fn void TQFileDialog::filterSelected( const TQString& )

  This signal is emitted when the user selects a filter.

  \sa selectedFilter()
*/

extern bool tqt_resolve_symlinks; // defined in qapplication.cpp
bool TQ_EXPORT tqt_use_native_dialogs = true;

/*!
  This is a convenience static function that returns an existing file
  selected by the user. If the user pressed Cancel, it returns a null
  string.

  \code
    TQString s = TQFileDialog::getOpenFileName(
                    "/home",
		    "Images (*.png *.xpm *.jpg)",
		    this,
		    "open file dialog",
		    "Choose a file to open" );
  \endcode

  The function creates a modal file dialog called \a name, with
  parent, \a parent. If a parent is not 0, the dialog will be shown
  centered over the parent.

  The file dialog's working directory will be set to \a startWith. If \a
  startWith includes a file name, the file will be selected. The filter
  is set to \a filter so that only those files which match the filter
  are shown. The filter selected is set to \a selectedFilter. The parameters
  \a startWith, \a selectedFilter and \a filter may be TQString::null.

  The dialog's caption is set to \a caption. If \a caption is not
  specified then a default caption will be used.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a TQFileDialog, unless the style of the application
  is set to something other than the native style (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  TQTimers and if parent is not 0 then it will position the dialog just under
  the parent's titlebar).

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  \sa getOpenFileNames(), getSaveFileName(), getExistingDirectory()
*/

TQString TQFileDialog::getOpenFileName( const TQString & startWith,
				      const TQString& filter,
				      TQWidget *parent, const char* name,
				      const TQString& caption,
				      TQString *selectedFilter,
				      bool resolveSymlinks )
{
    bool save_qt_resolve_symlinks = tqt_resolve_symlinks;
    tqt_resolve_symlinks = resolveSymlinks;

    TQStringList filters;
    if ( !filter.isEmpty() )
	filters = makeFiltersList( filter );

    makeVariables();
    TQString initialSelection;
    //### Problem with the logic here: If a startWith is given and a file
    // with that name exists in D->URL, the box will be opened at D->URL instead of
    // the last directory used ('workingDirectory').
    //
    // hm... isn't that problem exactly the documented behaviour? the
    // documented behaviour sounds meaningful.
    if ( !startWith.isEmpty() ) {
	TQUrlOperator u( TQFileDialogPrivate::encodeFileName( startWith ) );
	if ( u.isLocalFile() && TQFileInfo( u.path() ).isDir() ) {
	    *workingDirectory = startWith;
	} else {
	    if ( u.isLocalFile() ) {
		TQFileInfo fi( u.dirPath() );
		if ( fi.exists() ) {
		    *workingDirectory = u.dirPath();
		    initialSelection = u.fileName();
		}
	    } else {
		*workingDirectory = u.toString();
		initialSelection = TQString::null;//u.fileName();
	    }
	}
    }

    if ( workingDirectory->isNull() )
	*workingDirectory = ::toRootIfNotExists( TQDir::currentDirPath() );

#if defined(TQ_WS_X11)
    if ( tqt_use_native_dialogs && TQTDEIntegration::enabled())
	return TQTDEIntegration::getOpenFileNames( filter, workingDirectory, parent, name,
				    caption, selectedFilter, false ).first();
#elif defined(TQ_WS_WIN)
    if ( tqt_use_native_dialogs && tqApp->style().styleHint( TQStyle::SH_GUIStyle ) == WindowsStyle )
	return winGetOpenFileName( initialSelection, filter, workingDirectory,
				   parent, name, caption, selectedFilter );
#elif defined(TQ_WS_MAC)
    if (tqt_use_native_dialogs && (tqApp->style().inherits(TQMAC_DEFAULT_STYLE)
                                  || tqApp->style().inherits("TQMacStyle")))
        return qt_mac_precomposeFileName(macGetOpenFileNames(filter,
                                         startWith.isEmpty() ? 0 : workingDirectory,
                                         parent, name, caption, selectedFilter, false).first());
#endif

    TQFileDialog *dlg = new TQFileDialog( *workingDirectory, TQString::null, parent, name ? name : "qt_filedlg_gofn", true );

    TQ_CHECK_PTR( dlg );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( TQFileDialog::tr( "Open" ) );
#endif

    dlg->setFilters( filters );
    if ( selectedFilter )
	dlg->setFilter( *selectedFilter );
    dlg->setMode( TQFileDialog::ExistingFile );
    TQString result;
    if ( !initialSelection.isEmpty() )
	dlg->setSelection( initialSelection );
    if ( dlg->exec() == TQDialog::Accepted ) {
	result = dlg->selectedFile();
	*workingDirectory = dlg->d->url;
	if ( selectedFilter )
	    *selectedFilter = dlg->selectedFilter();
    }
    delete dlg;

    tqt_resolve_symlinks = save_qt_resolve_symlinks;

    return result;
}

/*!
  This is a convenience static function that will return a file name
  selected by the user. The file does not have to exist.

  It creates a modal file dialog called \a name, with parent, \a parent.
  If a parent is not 0, the dialog will be shown centered over the
  parent.

  \code
    TQString s = TQFileDialog::getSaveFileName(
                    "/home",
		    "Images (*.png *.xpm *.jpg)",
		    this,
		    "save file dialog",
		    "Choose a filename to save under" );
  \endcode

  The file dialog's working directory will be set to \a startWith. If \a
  startWith includes a file name, the file will be selected. The filter
  is set to \a filter so that only those files which match the filter
  are shown. The filter selected is set to \a selectedFilter. The parameters
  \a startWith, \a selectedFilter and \a filter may be TQString::null.

  The dialog's caption is set to \a caption. If \a caption is not
  specified then a default caption will be used.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a TQFileDialog, unless the style of the application
  is set to something other than the native style. (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  TQTimers and if parent is not 0 then it will position the dialog just under
  the parent's titlebar.

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  \sa getOpenFileName(), getOpenFileNames(), getExistingDirectory()
*/

TQString TQFileDialog::getSaveFileName( const TQString & startWith,
				      const TQString& filter,
				      TQWidget *parent, const char* name,
				      const TQString& caption,
				      TQString *selectedFilter,
				      bool resolveSymlinks)
{
    bool save_qt_resolve_symlinks = tqt_resolve_symlinks;
    tqt_resolve_symlinks = resolveSymlinks;

    TQStringList filters;
    if ( !filter.isEmpty() )
	filters = makeFiltersList( filter );

    makeVariables();
    TQString initialSelection;
    if ( !startWith.isEmpty() ) {
	TQUrlOperator u( TQFileDialogPrivate::encodeFileName( startWith ) );
	if ( u.isLocalFile() && TQFileInfo( u.path() ).isDir() ) {
	    *workingDirectory = startWith;
	} else {
	    if ( u.isLocalFile() ) {
		TQFileInfo fi( u.dirPath() );
		if ( fi.exists() ) {
		    *workingDirectory = u.dirPath();
		    initialSelection = u.fileName();
		}
	    } else {
		*workingDirectory = u.toString();
		initialSelection = TQString::null;//u.fileName();
	    }
	}
    }

    if ( workingDirectory->isNull() )
	*workingDirectory = ::toRootIfNotExists( TQDir::currentDirPath() );

#if defined(TQ_WS_X11)
    if ( tqt_use_native_dialogs && TQTDEIntegration::enabled())
	return TQTDEIntegration::getSaveFileName( initialSelection, filter, workingDirectory,
				   parent, name, caption, selectedFilter );
#elif defined(TQ_WS_WIN)
    if ( tqt_use_native_dialogs && tqApp->style().styleHint( TQStyle::SH_GUIStyle ) == WindowsStyle )
	return winGetSaveFileName( initialSelection, filter, workingDirectory,
				   parent, name, caption, selectedFilter );
#elif defined(TQ_WS_MAC)
    if (tqt_use_native_dialogs && (tqApp->style().inherits(TQMAC_DEFAULT_STYLE)
                                  || tqApp->style().inherits("TQMacStyle")))
        return qt_mac_precomposeFileName(macGetSaveFileName(initialSelection, filter,
                                         startWith.isEmpty() ? 0 : workingDirectory, parent, name,
                                         caption, selectedFilter));
#endif

    TQFileDialog *dlg = new TQFileDialog( *workingDirectory, TQString::null, parent, name ? name : "qt_filedlg_gsfn", true );

    TQ_CHECK_PTR( dlg );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( TQFileDialog::tr( "Save As" ) );
#endif

    TQString result;
    dlg->setFilters( filters );
    if ( selectedFilter )
	dlg->setFilter( *selectedFilter );
    dlg->setMode( TQFileDialog::AnyFile );
    if ( !initialSelection.isEmpty() )
	dlg->setSelection( initialSelection );
    if ( dlg->exec() == TQDialog::Accepted ) {
	result = dlg->selectedFile();
	*workingDirectory = dlg->d->url;
	if ( selectedFilter )
	    *selectedFilter = dlg->selectedFilter();
    }
    delete dlg;

    tqt_resolve_symlinks = save_qt_resolve_symlinks;

    return result;
}

/*!
  \internal
  Activated when the "OK" button is clicked.
*/

void TQFileDialog::okClicked()
{
    TQString fn( nameEdit->text() );

#if defined(TQ_WS_WIN)
    TQFileInfo fi( d->url.path() + fn );
    if ( fi.isSymLink() ) {
	nameEdit->setText( fi.readLink() );
    }
#endif

    if ( fn.contains("*") ) {
	addFilter( fn );
	nameEdit->blockSignals( true );
	nameEdit->setText( TQString::fromLatin1("") );
	nameEdit->blockSignals( false );
	return;
    }

    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    updateLastSize(this);

    if ( isDirectoryMode( d->mode ) ) {
	TQUrlInfo f( d->url, nameEdit->text() );
	if ( f.isDir() ) {
	    d->currentFileName = d->url;
	    if ( d->currentFileName.right(1) != "/" )
		d->currentFileName += '/';
	    if ( f.name() != "." )
		d->currentFileName += f.name();
	    accept();
	    return;
	}
	// Since it's not a directory and we clicked ok, we
	// don't really want to do anything else
	return;
    }

    // if we're in multi-selection mode and something is selected,
    // accept it and be done.
    if ( mode() == ExistingFiles ) {
	if ( ! nameEdit->text().isEmpty() ) {
	    TQStringList sf = selectedFiles();
	    bool isdir = false;
	    if ( sf.count() == 1 ) {
		TQUrlOperator u( d->url, sf[0] );
		bool ok;
		isdir = u.isDir(&ok) && ok;
	    }
	    if ( !isdir ) {
		emit filesSelected( sf );
		accept();
		return;
	    }
	}
    }

    if ( mode() == AnyFile ) {
	TQUrlOperator u( d->url, TQFileDialogPrivate::encodeFileName(nameEdit->text()) );
	if ( !u.isDir() ) {
	    d->currentFileName = u;
	    emit fileSelected( selectedFile() );
	    accept();
	    return;
	}
    }

    if ( mode() == ExistingFile ) {
	if ( !TQFileDialogPrivate::fileExists( d->url, nameEdit->text() ) )
	    return;
    }

    // If selection is valid, return it, else try
    // using selection as a directory to change to.
    if ( !d->currentFileName.isNull() && !d->currentFileName.contains( "*" ) ) {
	emit fileSelected( selectedFile() );
	accept();
    } else {
	TQUrlInfo f;
	TQFileDialogPrivate::File * c
	    = (TQFileDialogPrivate::File *)files->currentItem();
	TQFileDialogPrivate::MCItem * m
	    = (TQFileDialogPrivate::MCItem *)d->moreFiles->item( d->moreFiles->currentItem() );
	if ( ( c && files->isVisible() && files->hasFocus() ) ||
	     ( m && d->moreFiles->isVisible() && d->moreFiles->hasFocus() ) ) {
	    if ( c && files->isVisible() )
		f = c->info;
	    else
		f = ( (TQFileDialogPrivate::File*)m->i )->info;
	} else {
	    f = TQUrlInfo( d->url, nameEdit->text() );
	}
	if ( f.isDir() ) {
	    setUrl( TQUrlOperator( d->url, TQFileDialogPrivate::encodeFileName(f.name() + "/" ) ) );
	    d->checkForFilter = true;
	    trySetSelection( true, d->url, true );
	    d->checkForFilter = false;
	} else {
	    if ( !nameEdit->text().contains( "/" ) &&
		 !nameEdit->text().contains( "\\" )
#if defined(Q_OS_WIN32)
		 && nameEdit->text()[ 1 ] != ':'
#endif
		 )
		addFilter( nameEdit->text() );
	    else if ( nameEdit->text()[ 0 ] == '/' ||
		      nameEdit->text()[ 0 ] == '\\'
#if defined(Q_OS_WIN32)
		      || nameEdit->text()[ 1 ] == ':'
#endif
		      )
		setDir( nameEdit->text() );
	    else if ( nameEdit->text().left( 3 ) == "../" || nameEdit->text().left( 3 ) == "..\\" )
		setDir( TQUrl( d->url.toString(), TQFileDialogPrivate::encodeFileName(nameEdit->text() ) ).toString() );
	}
	nameEdit->setText( "" );
    }
}

/*!
  \internal
  Activated when the "Filter" button is clicked.
*/

void TQFileDialog::filterClicked()
{
    // unused
}

/*!
  \internal
  Activated when the "Cancel" button is clicked.
*/

void TQFileDialog::cancelClicked()
{
    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    updateLastSize(this);
    reject();
}


/*!\reimp
*/

void TQFileDialog::resizeEvent( TQResizeEvent * e )
{
    TQDialog::resizeEvent( e );
    updateGeometries();
}

/*
  \internal
  The only correct way to try to set currentFileName
*/
bool TQFileDialog::trySetSelection( bool isDir, const TQUrlOperator &u, bool updatelined )
{
    if ( !isDir && !u.path().isEmpty() && u.path().right( 1 ) == "/" )
	isDir = true;
    if ( u.fileName().contains( "*") && d->checkForFilter ) {
	TQString fn( u.fileName() );
	if ( fn.contains( "*" ) ) {
	    addFilter( fn );
	    d->currentFileName = TQString::null;
	    d->url.setFileName( TQString::null );
	    nameEdit->setText( TQString::fromLatin1("") );
	    return false;
	}
    }

    if ( d->preview && d->preview->isVisible() )
	updatePreviews( u );

    TQString old = d->currentFileName;

    if ( isDirectoryMode( mode() ) ) {
	if ( isDir )
	    d->currentFileName = u;
	else
	    d->currentFileName = TQString::null;
    } else if ( !isDir && mode() == ExistingFiles ) {
	d->currentFileName = u;
    } else if ( !isDir || ( mode() == AnyFile && !isDir ) ) {
	d->currentFileName = u;
    } else {
	d->currentFileName = TQString::null;
    }
    if ( updatelined && !d->currentFileName.isEmpty() ) {
	// If the selection is valid, or if its a directory, allow OK.
	if ( !d->currentFileName.isNull() || isDir ) {
	    if ( u.fileName() != ".." ) {
		TQString fn = u.fileName();
		nameEdit->setText( fn );
	    } else {
		nameEdit->setText("");
	    }
	} else
	    nameEdit->setText( TQString::fromLatin1("") );
    }

    if ( !d->currentFileName.isNull() || isDir ) {
	okB->setEnabled( true );
    } else if ( !isDirectoryMode( d->mode ) ) {
	okB->setEnabled( false );
    }

    if ( d->currentFileName.length() && old != d->currentFileName )
	emit fileHighlighted( selectedFile() );

    return !d->currentFileName.isNull();
}


/*!  Make sure the minimum and maximum sizes of everything are sane.
*/

void TQFileDialog::updateGeometries()
{
    if ( !d || !d->geometryDirty )
	return;

    d->geometryDirty = false;

    TQSize r, t;

    // we really should have a TQSize::unite()
#define RM r.setWidth( TQMAX(r.width(),t.width()) ); \
r.setHeight( TQMAX(r.height(),t.height()) )

    // labels first
    r = d->pathL->sizeHint();
    t = d->fileL->sizeHint();
    RM;
    t = d->typeL->sizeHint();
    RM;
    d->pathL->setFixedSize( d->pathL->sizeHint() );
    d->fileL->setFixedSize( r );
    d->typeL->setFixedSize( r );

    // single-line input areas
    r = d->paths->sizeHint();
    t = nameEdit->sizeHint();
    RM;
    t = d->types->sizeHint();
    RM;
    r.setWidth( t.width() * 2 / 3 );
    t.setWidth( TQWIDGETSIZE_MAX );
    t.setHeight( r.height() );
    d->paths->setMinimumSize( r );
    d->paths->setMaximumSize( t );
    nameEdit->setMinimumSize( r );
    nameEdit->setMaximumSize( t );
    d->types->setMinimumSize( r );
    d->types->setMaximumSize( t );

    // buttons on top row
    r = TQSize( 0, d->paths->minimumSize().height() );
    t = TQSize( 21, 20 );
    RM;
    if ( r.height()+1 > r.width() )
	r.setWidth( r.height()+1 );
    if ( d->goBack )
	d->goBack->setFixedSize( r );
    d->cdToParent->setFixedSize( r );
    d->newFolder->setFixedSize( r );
    d->mcView->setFixedSize( r );
    d->detailView->setFixedSize( r );

    TQButton *b = 0;
    if ( !d->toolButtons.isEmpty() ) {
	for ( b = d->toolButtons.first(); b; b = d->toolButtons.next() )
	    b->setFixedSize( b->sizeHint().width(), r.height() );
    }

    if ( d->infoPreview ) {
	d->previewInfo->show();
	d->previewInfo->setFixedSize( r );
    } else {
	d->previewInfo->hide();
	d->previewInfo->setFixedSize( TQSize( 0, 0 ) );
    }

    if ( d->contentsPreview ) {
	d->previewContents->show();
	d->previewContents->setFixedSize( r );
    } else {
	d->previewContents->hide();
	d->previewContents->setFixedSize( TQSize( 0, 0 ) );
    }

    // open/save, cancel
    r = TQSize( 75, 20 );
    t = okB->sizeHint();
    RM;
    t = cancelB->sizeHint();
    RM;

    okB->setFixedSize( r );
    cancelB->setFixedSize( r );

    d->topLevelLayout->activate();

#undef RM
}


/*! Updates the file name edit box to \a newItem in the file dialog
 when the cursor moves in the listview.
*/

void TQFileDialog::updateFileNameEdit( TQListViewItem * newItem )
{
    if ( !newItem )
	return;

    if ( mode() == ExistingFiles ) {
	detailViewSelectionChanged();
        TQUrl u( d->url, TQFileDialogPrivate::encodeFileName( ((TQFileDialogPrivate::File*)files->currentItem())->info.name() ) );
	TQFileInfo fi( u.toString( false, false ) );
	if ( !fi.isDir() )
	    emit fileHighlighted( u.toString( false, false ) );
    } else if ( files->isSelected( newItem ) ) {
	TQFileDialogPrivate::File * i = (TQFileDialogPrivate::File *)newItem;
	if ( i && i->i && !i->i->isSelected() ) {
	    d->moreFiles->blockSignals( true );
	    d->moreFiles->setSelected( i->i, true );
	    d->moreFiles->blockSignals( false );
	}
	// Encode the filename in case it had any special characters in it
	TQString encFile = TQFileDialogPrivate::encodeFileName( newItem->text( 0 ) );
	trySetSelection( i->info.isDir(), TQUrlOperator( d->url, encFile ), true );
    }
}

void TQFileDialog::detailViewSelectionChanged()
{
    if ( d->mode != ExistingFiles )
	return;

    nameEdit->clear();
    TQString str;
    TQListViewItem * i = files->firstChild();
    d->moreFiles->blockSignals( true );
    while( i ) {
	if ( d->moreFiles && isVisible() ) {
	    TQFileDialogPrivate::File *f = (TQFileDialogPrivate::File *)i;
	    if ( f->i && f->i->isSelected() != i->isSelected() )
		d->moreFiles->setSelected( f->i, i->isSelected() );
	}
	if ( i->isSelected() && !( (TQFileDialogPrivate::File *)i )->info.isDir() )
	    str += TQString( "\"%1\" " ).arg( i->text( 0 ) );
	i = i->nextSibling();
    }
    d->moreFiles->blockSignals( false );
    nameEdit->setText( str );
    nameEdit->setCursorPosition( str.length() );
    okB->setEnabled( true );
    if ( d->preview && d->preview->isVisible() && files->currentItem() ) {
	TQUrl u = TQUrl( d->url, TQFileDialogPrivate::encodeFileName( ((TQFileDialogPrivate::File*)files->currentItem())->info.name() ) );
	updatePreviews( u );
    }
}

void TQFileDialog::listBoxSelectionChanged()
{
    if ( d->mode != ExistingFiles )
	return;

    if ( d->ignoreNextRefresh ) {
	d->ignoreNextRefresh = false;
	return;
    }

    nameEdit->clear();
    TQString str;
    TQListBoxItem * i = d->moreFiles->item( 0 );
    TQListBoxItem * j = 0;
    int index = 0;
    files->blockSignals( true );
    while( i ) {
	TQFileDialogPrivate::MCItem *mcitem = (TQFileDialogPrivate::MCItem *)i;
	if ( files && isVisible() ) {
	    if ( mcitem->i->isSelected() != mcitem->isSelected() ) {
		files->setSelected( mcitem->i, mcitem->isSelected() );

		// What happens here is that we want to emit signal highlighted for
		// newly added items.  But TQListBox apparently emits selectionChanged even
		// when a user clicks on the same item twice.  So, basically emulate the behaivor
		// we have in the "Details" view which only emits highlighted the first time we
		// click on the item.  Perhaps at some point we should have a call to
		// updateFileNameEdit(TQListViewItem) which also emits fileHighlighted() for
		// ExistingFiles.  For better or for worse, this clones the behaivor of the
		// "Details" view quite well.
		if ( mcitem->isSelected() && i != d->lastEFSelected ) {
		    TQUrl u( d->url, TQFileDialogPrivate::encodeFileName( ((TQFileDialogPrivate::File*)(mcitem)->i)->info.name()) );
		    d->lastEFSelected = i;
		    emit fileHighlighted( u.toString(false, false) );
		}
	    }
	}
	if ( d->moreFiles->isSelected( i )
	     && !( (TQFileDialogPrivate::File*)(mcitem)->i )->info.isDir() ) {
	    str += TQString( "\"%1\" " ).arg( i->text() );
	    if ( j == 0 )
		j = i;
	}
	i = d->moreFiles->item( ++index );
    }

    files->blockSignals( false );
    nameEdit->setText( str );
    nameEdit->setCursorPosition( str.length() );
    okB->setEnabled( true );
    if ( d->preview && d->preview->isVisible() && j ) {
	TQUrl u = TQUrl( d->url,
		       TQFileDialogPrivate::encodeFileName( ( (TQFileDialogPrivate::File*)( (TQFileDialogPrivate::MCItem*)j )->i )->info.name() ) );
	updatePreviews( u );
    }
}

/*! \overload */

void TQFileDialog::updateFileNameEdit( TQListBoxItem * newItem )
{
    if ( !newItem )
	return;
    TQFileDialogPrivate::MCItem * i = (TQFileDialogPrivate::MCItem *)newItem;
    if ( i->i ) {
	i->i->listView()->setSelected( i->i, i->isSelected() );
	updateFileNameEdit( i->i );
    }
}


/*!  Updates the dialog when the file name edit changes. */

void TQFileDialog::fileNameEditDone()
{
    TQUrlInfo f( d->url, nameEdit->text() );
    if ( mode() != TQFileDialog::ExistingFiles ) {
	TQUrlOperator u( d->url, TQFileDialogPrivate::encodeFileName( nameEdit->text() ) );
	trySetSelection( f.isDir(), u, false );
	if ( d->preview && d->preview->isVisible() )
	    updatePreviews( u );
    }
}



/*! This private slot reacts to double-clicks in the list view. The item that
was double-clicked is specified in \a newItem */

void TQFileDialog::selectDirectoryOrFile( TQListViewItem * newItem )
{

    *workingDirectory = d->url;
    detailViewMode = files->isVisible();
    updateLastSize(this);

    if ( !newItem )
	return;

    if ( d->url.isLocalFile() ) {
	TQFileInfo fi( d->url.path() + newItem->text(0) );
#if defined(TQ_WS_WIN)
	if ( fi.isSymLink() ) {
	    nameEdit->setText( fi.readLink() );
	    okClicked();
	    return;
	}
#endif
    }

    TQFileDialogPrivate::File * i = (TQFileDialogPrivate::File *)newItem;

    TQString oldName = nameEdit->text();
    if ( i->info.isDir() ) {
	setUrl( TQUrlOperator( d->url, TQFileDialogPrivate::encodeFileName( i->info.name() ) + "/" ) );
	if ( isDirectoryMode( mode() ) ) {
	    TQUrlInfo f ( d->url, TQString::fromLatin1( "." ) );
	    trySetSelection( f.isDir(), d->url, true );
	}
    } else if ( newItem->isSelectable() &&
		trySetSelection( i->info.isDir(), TQUrlOperator( d->url, TQFileDialogPrivate::encodeFileName( i->info.name() ) ), true ) ) {
        if ( !isDirectoryMode( mode() ) ) {
	    if ( mode() == ExistingFile ) {
		if ( TQFileDialogPrivate::fileExists( d->url, nameEdit->text() ) ) {
		    emit fileSelected( selectedFile() );
		    accept();
		}
	    } else {
		emit fileSelected( selectedFile() );
		accept();
	    }
	}
    } else if ( isDirectoryMode( d->mode ) ) {
	d->currentFileName = d->url;
	accept();
    }
    if ( !oldName.isEmpty() && !isDirectoryMode( mode() ) )
	nameEdit->setText( oldName );
}


void TQFileDialog::selectDirectoryOrFile( TQListBoxItem * newItem )
{
    if ( !newItem )
	return;

    TQFileDialogPrivate::MCItem * i = (TQFileDialogPrivate::MCItem *)newItem;
    if ( i->i ) {
	i->i->listView()->setSelected( i->i, i->isSelected() );
	selectDirectoryOrFile( i->i );
    }
}


void TQFileDialog::popupContextMenu( TQListViewItem *item, const TQPoint &p,
				    int )
{
    if ( item ) {
	files->setCurrentItem( item );
	files->setSelected( item, true );
    }

    PopupAction action;
    popupContextMenu( item ? item->text( 0 ) : TQString::null, true, action, p );

    if ( action == PA_Open )
	selectDirectoryOrFile( item );
    else if ( action == PA_Rename )
	files->startRename( false );
    else if ( action == PA_Delete )
	deleteFile( item ? item->text( 0 ) : TQString::null );
    else if ( action == PA_Reload )
	rereadDir();
    else if ( action == PA_Hidden ) {
	bShowHiddenFiles = !bShowHiddenFiles;
	rereadDir();
    } else if ( action == PA_SortName ) {
	sortFilesBy = (int)TQDir::Name;
	sortAscending = true;
	resortDir();
    } else if ( action == PA_SortSize ) {
	sortFilesBy = (int)TQDir::Size;
	sortAscending = true;
	resortDir();
    } else if ( action == PA_SortDate ) {
	sortFilesBy = (int)TQDir::Time;
	sortAscending = true;
	resortDir();
    } else if ( action == PA_SortUnsorted ) {
	sortFilesBy = (int)TQDir::Unsorted;
	sortAscending = true;
	resortDir();
    }

}

void TQFileDialog::popupContextMenu( TQListBoxItem *item, const TQPoint & p )
{
    PopupAction action;
    popupContextMenu( item ? item->text() : TQString::null, false, action, p );

    if ( action == PA_Open )
	selectDirectoryOrFile( item );
    else if ( action == PA_Rename )
	d->moreFiles->startRename( false );
    else if ( action == PA_Delete )
	deleteFile( item->text() );
    else if ( action == PA_Reload )
	rereadDir();
    else if ( action == PA_Hidden ) {
	bShowHiddenFiles = !bShowHiddenFiles;
	rereadDir();
    } else if ( action == PA_SortName ) {
	sortFilesBy = (int)TQDir::Name;
	sortAscending = true;
	resortDir();
    } else if ( action == PA_SortSize ) {
	sortFilesBy = (int)TQDir::Size;
	sortAscending = true;
	resortDir();
    } else if ( action == PA_SortDate ) {
	sortFilesBy = (int)TQDir::Time;
	sortAscending = true;
	resortDir();
    } else if ( action == PA_SortUnsorted ) {
	sortFilesBy = (int)TQDir::Unsorted;
	sortAscending = true;
	resortDir();
    }
}

void TQFileDialog::popupContextMenu( const TQString &filename, bool,
				    PopupAction &action, const TQPoint &p )
{
    action = PA_Cancel;

    bool glob = filename.isEmpty();

    TQPopupMenu m( 0, "file dialog context menu" );
    m.setCheckable( true );

    if ( !glob ) {
	TQString okt;
	if ( TQUrlInfo( d->url, filename ).isDir() ) {
	    okt = tr( "&Open" );
	} else {
	    if ( mode() == AnyFile )
		okt = tr( "&Save" );
	    else
		okt = tr( "&Open" );
	}
	int ok = m.insertItem( okt );

	m.insertSeparator();
	int rename = m.insertItem( tr( "&Rename" ) );
	int del = m.insertItem( tr( "&Delete" ) );

	if ( filename.isEmpty() || !TQUrlInfo( d->url, filename ).isWritable() ||
	     filename == ".." ) {
	    if ( filename.isEmpty() || !TQUrlInfo( d->url, filename ).isReadable() )
		m.setItemEnabled( ok, false );
	    m.setItemEnabled( rename, false );
	    m.setItemEnabled( del, false );
	}

	m.move( p );
	int res = m.exec();

	if ( res == ok )
	    action = PA_Open;
	else if ( res == rename )
	    action = PA_Rename;
	else if ( res == del )
	    action = PA_Delete;
    } else {
	int reload = m.insertItem( tr( "R&eload" ) );

	TQPopupMenu m2( 0, "sort menu" );

	int sname = m2.insertItem( tr( "Sort by &Name" ) );
	//int stype = m2.insertItem( tr( "Sort by &Type" ) );
	int ssize = m2.insertItem( tr( "Sort by &Size" ) );
	int sdate = m2.insertItem( tr( "Sort by &Date" ) );
	m2.insertSeparator();
	int sunsorted = m2.insertItem( tr( "&Unsorted" ) );

	//m2.setItemEnabled( stype, false );

	if ( sortFilesBy == (int)TQDir::Name )
	    m2.setItemChecked( sname, true );
	else if ( sortFilesBy == (int)TQDir::Size )
	    m2.setItemChecked( ssize, true );
//	else if ( sortFilesBy == 0x16 )
//	    m2.setItemChecked( stype, true );
	else if ( sortFilesBy == (int)TQDir::Time )
	    m2.setItemChecked( sdate, true );
	else if ( sortFilesBy == (int)TQDir::Unsorted )
	    m2.setItemChecked( sunsorted, true );

	m.insertItem( tr( "Sort" ), &m2 );

	m.insertSeparator();

	int hidden = m.insertItem( tr( "Show &hidden files" ) );
	m.setItemChecked( hidden, bShowHiddenFiles );

	m.move( p );
	int res = m.exec();

	if ( res == reload )
	    action = PA_Reload;
	else if ( res == hidden )
	    action = PA_Hidden;
	else if ( res == sname )
	    action = PA_SortName;
//	else if ( res == stype )
//	    action = PA_SortType;
	else if ( res == sdate )
	    action = PA_SortDate;
	else if ( res == ssize )
	    action = PA_SortSize;
	else if ( res == sunsorted )
	    action = PA_SortUnsorted;
    }

}

void TQFileDialog::deleteFile( const TQString &filename )
{
    if ( filename.isEmpty() )
	return;

    TQUrlInfo fi( d->url, TQFileDialogPrivate::encodeFileName( filename ) );
    TQString t = tr( "the file" );
    if ( fi.isDir() )
	t = tr( "the directory" );
    if ( fi.isSymLink() )
	t = tr( "the symlink" );

    if ( TQMessageBox::warning( this,
			       tr( "Delete %1" ).arg( t ),
			       tr( "<qt>Are you sure you wish to delete %1 \"%2\"?</qt>" )
			       .arg( t ).arg(filename),
			       tr( "&Yes" ), tr( "&No" ), TQString::null, 1 ) == 0 )
	d->url.remove( TQFileDialogPrivate::encodeFileName( filename ) );

}

void TQFileDialog::fileSelected( int  )
{
    // unused
}

void TQFileDialog::fileHighlighted( int )
{
    // unused
}

void TQFileDialog::dirSelected( int )
{
    // unused
}

void TQFileDialog::pathSelected( int )
{
    // unused
}


void TQFileDialog::cdUpClicked()
{
    TQString oldName = nameEdit->text();
    setUrl( TQUrlOperator( d->url, ".." ) );
    if ( !oldName.isEmpty() )
	nameEdit->setText( oldName );
}

void TQFileDialog::newFolderClicked()
{
    TQString foldername( tr( "New Folder 1" ) );
    int i = 0;
    TQStringList lst;
    TQListViewItemIterator it( files );
    for ( ; it.current(); ++it )
	if ( it.current()->text( 0 ).contains( tr( "New Folder" ) ) )
	    lst.append( it.current()->text( 0 ) );

    if ( !lst.count() == 0 )
	while ( lst.contains( foldername ) )
	    foldername = tr( "New Folder %1" ).arg( ++i );

    d->url.mkdir( foldername );
}

void TQFileDialog::createdDirectory( const TQUrlInfo &info, TQNetworkOperation * )
{
    resortDir();
    if ( d->moreFiles->isVisible() ) {
	for ( uint i = 0; i < d->moreFiles->count(); ++i ) {
	    if ( d->moreFiles->text( i ) == info.name() ) {
		d->moreFiles->setCurrentItem( i );
		d->moreFiles->startRename( false );
		break;
	    }
	}
    } else {
	TQListViewItem *item = files->firstChild();
	while ( item ) {
	    if ( item->text( 0 ) == info.name() ) {
		files->setSelected( item, true );
		files->setCurrentItem( item );
		files->startRename( false );
		break;
	    }
	    item = item->nextSibling();
	}
    }
}


/*!
  This is a convenience static function that will return an existing directory
  selected by the user.

  \code
    TQString s = TQFileDialog::getExistingDirectory(
                    "/home",
		    this,
		    "get existing directory",
		    "Choose a directory",
		    true );
  \endcode

  This function creates a modal file dialog called \a name, with
  parent, \a parent. If parent is not 0, the dialog will be shown
  centered over the parent.

  The dialog's working directory is set to \a dir, and the caption is
  set to \a caption. Either of these may be TQString::null in which case
  the current directory and a default caption will be used respectively.

  Note on Windows that if \a dir is TQString::null then the dialog's working
  directory will be set to the user's My Documents directory.

  If \a dirOnly is true, then only directories will be shown in
  the file dialog; otherwise both directories and files will be shown.

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a TQFileDialog, unless the style of the application
  is set to something other than the native style. (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  TQTimers and if parent is not 0 then it will position the dialog just under
  the parent's titlebar).

  \sa getOpenFileName(), getOpenFileNames(), getSaveFileName()
*/

TQString TQFileDialog::getExistingDirectory( const TQString & dir,
					   TQWidget *parent,
					   const char* name,
					   const TQString& caption,
					   bool dirOnly,
					   bool resolveSymlinks)
{
    bool save_qt_resolve_symlinks = tqt_resolve_symlinks;
    tqt_resolve_symlinks = resolveSymlinks;

    makeVariables();
    TQString wd;
    if ( workingDirectory )
	wd = *workingDirectory;

#if defined(TQ_WS_X11)
    TQString initialDir;
    if ( !dir.isEmpty() ) {
	TQUrlOperator u( dir );
	if ( TQFileInfo( u.path() ).isDir() )
	    initialDir = dir;
    } else
	initialDir = TQString::null;
    if ( tqt_use_native_dialogs && TQTDEIntegration::enabled())
        return TQTDEIntegration::getExistingDirectory( initialDir, parent, name, caption );
#elif defined(TQ_WS_WIN)
    TQString initialDir;
    if ( !dir.isEmpty() ) {
	TQUrlOperator u( dir );
	if ( TQFileInfo( u.path() ).isDir() )
	    initialDir = dir;
    } else
	initialDir = TQString::null;
    if ( tqt_use_native_dialogs && tqApp->style().styleHint( TQStyle::SH_GUIStyle ) == WindowsStyle && dirOnly )
        return winGetExistingDirectory( initialDir, parent, name, caption );
#endif
#if defined(TQ_WS_MAC)
    TQString *initialDir = 0;
    if (!dir.isEmpty()) {
        TQUrlOperator u(dir);
        if (TQFileInfo(u.path()).isDir())
            initialDir = (TQString *)&dir;
    }
    if( tqt_use_native_dialogs && (tqApp->style().inherits(TQMAC_DEFAULT_STYLE)
                                  || tqApp->style().inherits("TQMacStyle")))
	return qt_mac_precomposeFileName(macGetOpenFileNames("", initialDir, parent, name,
                                                      caption, 0, false, true).first());
#endif

    TQFileDialog *dlg = new TQFileDialog( parent, name ? name : "qt_filedlg_ged", true );

    TQ_CHECK_PTR( dlg );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( TQFileDialog::tr("Find Directory") );
#endif

    dlg->setMode( dirOnly ? DirectoryOnly : Directory );

    dlg->d->types->clear();
    dlg->d->types->insertItem( TQFileDialog::tr("Directories") );
    dlg->d->types->setEnabled( false );

    TQString dir_( dir );
    dir_ = dir_.simplifyWhiteSpace();
    if ( dir_.isEmpty() && !wd.isEmpty() )
	dir_ = wd;
    TQUrlOperator u( dir_ );
    if ( u.isLocalFile() ) {
	if ( !dir_.isEmpty() ) {
	    TQFileInfo f( u.path() );
	if ( f.exists() )
	if ( f.isDir() ) {
		dlg->setDir( dir_ );
		wd = dir_;
	    }
	} else if ( !wd.isEmpty() ) {
	    TQUrl tempUrl( wd );
	    TQFileInfo f( tempUrl.path() );
	    if ( f.isDir() ) {
		dlg->setDir( wd );
	    }
	} else {
	    TQString theDir = dir_;
	    if ( theDir.isEmpty() ) {
		theDir = ::toRootIfNotExists( TQDir::currentDirPath() );
	    } if ( !theDir.isEmpty() ) {
		TQUrl tempUrl( theDir );
		TQFileInfo f( tempUrl.path() );
		if ( f.isDir() ) {
		    wd = theDir;
		    dlg->setDir( theDir );
		}
	    }
	}
    } else {
	dlg->setUrl( dir_ );
    }

    TQString result;
    dlg->setSelection( dlg->d->url.toString() );

    if ( dlg->exec() == TQDialog::Accepted ) {
	result = dlg->selectedFile();
	wd = result;
    }
    delete dlg;

    if ( !result.isEmpty() && result.right( 1 ) != "/" )
	result += "/";

    tqt_resolve_symlinks = save_qt_resolve_symlinks;

    return result;
}


/*!
  \property TQFileDialog::mode
  \brief the file dialog's mode

  The default mode is \c ExistingFile.
*/

void TQFileDialog::setMode( Mode newMode )
{
    if ( d->mode != newMode ) {
	d->mode = newMode;
	TQString sel = d->currentFileName;
	int maxnamelen = 255; // _POSIX_MAX_PATH
	if ( isDirectoryMode( newMode ) ) {
	    files->setSelectionMode( TQListView::Single );
	    d->moreFiles->setSelectionMode( TQListBox::Single );
	    if ( sel.isNull() )
		sel = TQString::fromLatin1(".");
	    d->types->setEnabled( false );
	} else if ( newMode == ExistingFiles ) {
	    maxnamelen = INT_MAX;
	    files->setSelectionMode( TQListView::Extended );
	    d->moreFiles->setSelectionMode( TQListBox::Extended );
	    d->types->setEnabled( true );
	} else {
	    files->setSelectionMode( TQListView::Single );
	    d->moreFiles->setSelectionMode( TQListBox::Single );
	    d->types->setEnabled( true );
	}
	nameEdit->setMaxLength(maxnamelen);
	rereadDir();
	TQUrlInfo f( d->url, "." );
	trySetSelection( f.isDir(), d->url, false );
    }

    TQString okt;
    bool changeFilters = false;
    if ( mode() == AnyFile ) {
	okt = tr("&Save");
	d->fileL->setText( tr("File &name:") );
	if ( d->types->count() == 1 ) {
	    d->types->setCurrentItem( 0 );
	    if ( d->types->currentText() == "Directories" ) {
		changeFilters = true;
	    }
	}
    }
    else if ( mode() == Directory || mode() == DirectoryOnly ) {
	okt = tr("&OK");
	d->fileL->setText( tr("Directory:") );
	d->types->clear();
	d->types->insertItem( tr("Directories") );
    }
    else {
	okt = tr("&Open");
	d->fileL->setText( tr("File &name:") );
	if ( d->types->count() == 1 ) {
	    d->types->setCurrentItem( 0 );
	    if ( d->types->currentText() == "Directories" ) {
		changeFilters = true;
	    }
	}
    }

    if ( changeFilters ) {
	d->types->clear();
	d->types->insertItem( tr("All Files (*)") );
    }

    okB->setText( okt );
}

TQFileDialog::Mode TQFileDialog::mode() const
{
    return d->mode;
}

/*! \reimp
*/

void TQFileDialog::done( int i )
{
    if ( i == TQDialog::Accepted && (d->mode == ExistingFile || d->mode == ExistingFiles) ) {
	TQStringList selection = selectedFiles();
	for ( uint f = 0; f < selection.count(); f++ ) {
	    TQString file = selection[f];
	    if ( file.isNull() )
		continue;
	    if ( d->url.isLocalFile() && !TQFile::exists( file ) ) {
		TQMessageBox::information( this, tr("Error"),
					  tr("%1\nFile not found.\nCheck path and filename.").arg( file ) );
		return;
	    }
	}
    }
    TQDialog::done( i );
}

/*!
  \property TQFileDialog::viewMode

  \brief the file dialog's view mode

  If you set the view mode to be \e Detail (the default), then you
  will see the file's details, such as the size of the file and the
  date the file was last modified in addition to the file's name.

  If you set the view mode to be \e List, then you will just
  see a list of the files and folders.

  See \l TQFileDialog::ViewMode
*/


TQFileDialog::ViewMode TQFileDialog::viewMode() const
{
    if ( detailViewMode )
	return Detail;
    else
	return List;
}

void TQFileDialog::setViewMode( ViewMode m )
{
    if ( m == Detail ) {
	detailViewMode = true;
	d->stack->raiseWidget( files );
	d->detailView->setOn( true );
	d->mcView->setOn( false );
    } else if ( m == List ) {
	detailViewMode = false;
	d->stack->raiseWidget( d->moreFiles );
	d->detailView->setOn( false );
	d->mcView->setOn( true );
    }
}


/*!
  \property TQFileDialog::previewMode

  \brief the preview mode for the file dialog

  If you set the mode to be a mode other than \e NoPreview, you must
  use setInfoPreview() or setContentsPreview() to set the dialog's
  preview widget to your preview widget and enable the preview
  widget(s) with setInfoPreviewEnabled() or
  setContentsPreviewEnabled().

  \sa infoPreview, contentsPreview, viewMode
*/

void TQFileDialog::setPreviewMode( PreviewMode m )
{
    if ( m == NoPreview ) {
	d->previewInfo->setOn( false );
	d->previewContents->setOn( false );
    } else if ( m == Info && d->infoPreview ) {
	d->previewInfo->setOn( true );
	d->previewContents->setOn( false );
	changeMode( d->modeButtons->id( d->previewInfo ) );
    } else if ( m == Contents && d->contentsPreview ) {
	d->previewInfo->setOn( false );
	d->previewContents->setOn( true );
	changeMode( d->modeButtons->id( d->previewContents ) );
    }
}
TQFileDialog::PreviewMode TQFileDialog::previewMode() const
{
    if ( d->infoPreview && d->infoPreviewWidget->isVisible() )
	return Info;
    else if ( d->contentsPreview && d->contentsPreviewWidget->isVisible() )
	return Contents;

    return NoPreview;
}


/*!
  Adds the specified widgets to the bottom of the file dialog. The
  label \a l is placed underneath the "file name" and the "file types"
  labels. The widget \a w is placed underneath the file types combobox.
  The button \a b is placed underneath the Cancel pushbutton.

  \code
    MyFileDialog::MyFileDialog( TQWidget* parent, const char* name ) :
	TQFileDialog( parent, name )
    {
	TQLabel* label = new TQLabel( "Added widgets", this );
	TQLineEdit* lineedit = new TQLineEdit( this );
	TQPushButton* pushbutton = new TQPushButton( this );

	addWidgets( label, lineedit, pushbutton );
    }
  \endcode

  If you don't want to have one of the widgets added, pass 0 in that
  widget's position.

  Every time you call this function, a new row of widgets will be added
  to the bottom of the file dialog.

  \sa addToolButton(), addLeftWidget(), addRightWidget()
*/

void TQFileDialog::addWidgets( TQLabel * l, TQWidget * w, TQPushButton * b )
{
    if ( !l && !w && !b )
	return;

    d->geometryDirty = true;

    TQHBoxLayout *lay = new TQHBoxLayout();
    d->extraWidgetsLayouts.append( lay );
    d->topLevelLayout->addLayout( lay );

    if ( !l )
	l = new TQLabel( this, "qt_intern_lbl" );
    d->extraLabels.append( l );
    lay->addWidget( l );

    if ( !w )
	w = new TQWidget( this, "qt_intern_widget" );
    d->extraWidgets.append( w );
    lay->addWidget( w );
    lay->addSpacing( 15 );

    if ( b ) {
	d->extraButtons.append( b );
	lay->addWidget( b );
    } else {
	TQWidget *wid = new TQWidget( this, "qt_extrabuttons_widget" );
	d->extraButtons.append( wid );
	lay->addWidget( wid );
    }

    updateGeometries();
}

/*!
  Adds the tool button \a b to the row of tool buttons at the top of the
  file dialog. The button is appended to the right of
  this row. If \a separator is true, a small space is inserted between the
  last button of the row and the new button \a b.

  \sa addWidgets(), addLeftWidget(), addRightWidget()
*/

void TQFileDialog::addToolButton( TQButton *b, bool separator )
{
    if ( !b || !d->buttonLayout )
	return;

    d->geometryDirty = true;

    d->toolButtons.append( b );
    if ( separator )
	d->buttonLayout->addSpacing( 8 );
    d->buttonLayout->addWidget( b );

    updateGeometries();
}

/*!
  Adds the widget \a w to the left-hand side of the file dialog.

  \sa addRightWidget(), addWidgets(), addToolButton()
*/

void TQFileDialog::addLeftWidget( TQWidget *w )
{
    if ( !w )
	return;
    d->geometryDirty = true;

    d->leftLayout->addWidget( w );
    d->leftLayout->addSpacing( 5 );

    updateGeometries();
}

/*!
  Adds the widget \a w to the right-hand side of the file dialog.

  \sa addLeftWidget(), addWidgets(), addToolButton()
*/

void TQFileDialog::addRightWidget( TQWidget *w )
{
    if ( !w )
	return;
    d->geometryDirty = true;

    d->rightLayout->addSpacing( 5 );
    d->rightLayout->addWidget( w );

    updateGeometries();
}

/*! \reimp */

void TQFileDialog::keyPressEvent( TQKeyEvent * ke )
{
    if ( !d->ignoreNextKeyPress &&
	 ke && ( ke->key() == Key_Enter ||
		 ke->key() == Key_Return ) ) {
	ke->ignore();
	if ( d->paths->hasFocus() ) {
	    ke->accept();
	    if ( d->url == TQUrl(d->paths->currentText()) )
		nameEdit->setFocus();
	} else if ( d->types->hasFocus() ) {
	    ke->accept();
	    // ### is there a suitable condition for this?  only valid
	    // wildcards?
	    nameEdit->setFocus();
	} else if ( nameEdit->hasFocus() ) {
	    if ( d->currentFileName.isNull() ) {
		// maybe change directory
		TQUrlInfo i( d->url, nameEdit->text() );
		if ( i.isDir() ) {
		    nameEdit->setText( TQString::fromLatin1("") );
		    setDir( TQUrlOperator( d->url, TQFileDialogPrivate::encodeFileName(i.name()) ) );
		}
		ke->accept();
	    } else if ( mode() == ExistingFiles ) {
		TQUrlInfo i( d->url, nameEdit->text() );
		if ( i.isFile() ) {
		    TQListViewItem * i = files->firstChild();
		    while ( i && nameEdit->text() != i->text( 0 ) )
			i = i->nextSibling();
		    if ( i )
			files->setSelected( i, true );
		    else
			ke->accept(); // strangely, means to ignore that event
		}
	    }
	} else if ( files->hasFocus() || d->moreFiles->hasFocus() ) {
	    ke->accept();
	}
    } else if ( ke->key() == Key_Escape ) {
	ke->ignore();
    }

    d->ignoreNextKeyPress = false;

    if ( !ke->isAccepted() ) {
	TQDialog::keyPressEvent( ke );
    }
}


/*! \class TQFileIconProvider ntqfiledialog.h

  \brief The TQFileIconProvider class provides icons for TQFileDialog to
  use.

  \ingroup misc

  By default TQFileIconProvider is not used, but any application or
  library can subclass it, reimplement pixmap() to return a suitable
  icon, and make all TQFileDialog objects use it by calling the static
  function TQFileDialog::setIconProvider().

  It is advisable to make all the icons that TQFileIconProvider returns be
  the same size or at least the same width. This makes the list view
  look much better.

  \sa TQFileDialog
*/


/*! Constructs an empty file icon provider called \a name, with the
  parent \a parent.
*/

TQFileIconProvider::TQFileIconProvider( TQObject * parent, const char* name )
    : TQObject( parent, name )
{
    // nothing necessary
}


/*!
  Returns a pointer to a pixmap that should be used to
  signify the file with the information \a info.

  If pixmap() returns 0, TQFileDialog draws the default pixmap.

  The default implementation returns particular icons for files, directories,
  link-files and link-directories. It returns a blank "icon" for other types.

  If you return a pixmap here, it should measure 16x16 pixels.
*/

const TQPixmap * TQFileIconProvider::pixmap( const TQFileInfo & info )
{
    if ( info.isSymLink() ) {
	if ( info.isFile() )
	    return symLinkFileIcon;
	else
	    return symLinkDirIcon;
    } else if ( info.isDir() ) {
	return closedFolderIcon;
    } else if ( info.isFile() ) {
	return fileIcon;
    } else {
	return fifteenTransparentPixels;
    }
}

/*!
  Sets the TQFileIconProvider used by the file dialog to \a provider.

  The default is that there is no TQFileIconProvider and TQFileDialog
  just draws a folder icon next to each directory and nothing next
  to files.

  \sa TQFileIconProvider, iconProvider()
*/

void TQFileDialog::setIconProvider( TQFileIconProvider * provider )
{
    fileIconProvider = provider;
}


/*!
  Returns a pointer to the icon provider currently set on the file dialog.
  By default there is no icon provider, and this function returns 0.

  \sa setIconProvider(), TQFileIconProvider
*/

TQFileIconProvider * TQFileDialog::iconProvider()
{
    return fileIconProvider;
}


#if defined(TQ_WS_WIN)

// ### FIXME: this code is duplicated in qdns.cpp
static TQString getWindowsRegString( HKEY key, const TQString &subKey )
{
    TQString s;
    QT_WA( {
	char buf[1024];
	DWORD bsz = sizeof(buf);
	int r = RegQueryValueEx( key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)buf, &bsz );
	if ( r == ERROR_SUCCESS ) {
	    s = TQString::fromUcs2( (unsigned short *)buf );
	} else if ( r == ERROR_MORE_DATA ) {
	    char *ptr = new char[bsz+1];
	    r = RegQueryValueEx( key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)ptr, &bsz );
	    if ( r == ERROR_SUCCESS )
		s = ptr;
	    delete [] ptr;
	}
    } , {
	char buf[512];
	DWORD bsz = sizeof(buf);
	int r = RegQueryValueExA( key, subKey.local8Bit(), 0, 0, (LPBYTE)buf, &bsz );
	if ( r == ERROR_SUCCESS ) {
	    s = buf;
	} else if ( r == ERROR_MORE_DATA ) {
	    char *ptr = new char[bsz+1];
	    r = RegQueryValueExA( key, subKey.local8Bit(), 0, 0, (LPBYTE)ptr, &bsz );
	    if ( r == ERROR_SUCCESS )
		s = ptr;
	    delete [] ptr;
	}
    } );
    return s;
}

static void initPixmap( TQPixmap &pm )
{
    pm.fill( TQt::white );
}


TQWindowsIconProvider::TQWindowsIconProvider( TQObject *parent, const char *name )
    : TQFileIconProvider( parent, name )
{
    pixw = GetSystemMetrics( SM_CXSMICON );
    pixh = GetSystemMetrics( SM_CYSMICON );

    HKEY k;
    HICON si;
    int r;
    TQString s;
    UINT res = 0;

    // ---------- get default folder pixmap
    const wchar_t iconFolder[] = L"folder\\DefaultIcon"; // workaround for Borland
    QT_WA( {
	r = RegOpenKeyEx( HKEY_CLASSES_ROOT,
			   iconFolder,
			   0, KEY_READ, &k );
    } , {
	r = RegOpenKeyExA( HKEY_CLASSES_ROOT,
			   "folder\\DefaultIcon",
			   0, KEY_READ, &k );
    } );
    resolveLibs();
    if ( r == ERROR_SUCCESS ) {
	s = getWindowsRegString( k, TQString::null );
	RegCloseKey( k );

	TQStringList lst = TQStringList::split( ",", s );
        
    if (lst.count() >= 2) { // don't just assume that lst has two entries
#ifndef Q_OS_TEMP
            QT_WA( {
                    res = ptrExtractIconEx( (TCHAR*)lst[ 0 ].simplifyWhiteSpace().ucs2(),
                        lst[ 1 ].simplifyWhiteSpace().toInt(),
                        0, &si, 1 );
                    } , {
                    res = ExtractIconExA( lst[ 0 ].simplifyWhiteSpace().local8Bit(),
                        lst[ 1 ].simplifyWhiteSpace().toInt(),
                        0, &si, 1 );
                    } );
#else
            res = (UINT)ExtractIconEx( (TCHAR*)lst[ 0 ].simplifyWhiteSpace().ucs2(),
                    lst[ 1 ].simplifyWhiteSpace().toInt(),
                    0, &si, 1 );
#endif
    }
	if ( res ) {
	    defaultFolder.resize( pixw, pixh );
	    initPixmap( defaultFolder );
	    TQPainter p( &defaultFolder );
	    DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, 0,  DI_NORMAL );
	    p.end();
	    defaultFolder.setMask( defaultFolder.createHeuristicMask() );
	    *closedFolderIcon = defaultFolder;
	    DestroyIcon( si );
	} else {
	    defaultFolder = *closedFolderIcon;
	}
    } else {
	RegCloseKey( k );
    }

    //------------------------------- get default file pixmap
#ifndef Q_OS_TEMP
    QT_WA( {
	res = ptrExtractIconEx( L"shell32.dll",
				 0, 0, &si, 1 );
    } , {
	res = ExtractIconExA( "shell32.dll",
				 0, 0, &si, 1 );
    } );
#else
	res = (UINT)ExtractIconEx( L"shell32.dll",
				    0, 0, &si, 1 );
#endif

    if ( res ) {
	defaultFile.resize( pixw, pixh );
	initPixmap( defaultFile );
	TQPainter p( &defaultFile );
	DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, 0,  DI_NORMAL );
	p.end();
	defaultFile.setMask( defaultFile.createHeuristicMask() );
	*fileIcon = defaultFile;
	DestroyIcon( si );
    } else {
	defaultFile = *fileIcon;
    }

    //------------------------------- get default exe pixmap
#ifndef Q_OS_TEMP
    QT_WA( {
	res = ptrExtractIconEx( L"shell32.dll",
			      2, 0, &si, 1 );
    } , {
	res = ExtractIconExA( "shell32.dll",
			  2, 0, &si, 1 );
    } );
#else
	res = (UINT)ExtractIconEx( L"ceshell.dll",
				    10, 0, &si, 1 );
#endif

    if ( res ) {
	defaultExe.resize( pixw, pixh );
	initPixmap( defaultExe );
	TQPainter p( &defaultExe );
	DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, 0,  DI_NORMAL );
	p.end();
	defaultExe.setMask( defaultExe.createHeuristicMask() );
	DestroyIcon( si );
    } else {
	defaultExe = *fileIcon;
    }
}

TQWindowsIconProvider::~TQWindowsIconProvider()
{
    if ( this == fileIconProvider )
	fileIconProvider = 0;
}

const TQPixmap * TQWindowsIconProvider::pixmap( const TQFileInfo &fi )
{
    if (fi.isSymLink()) {
        TQString real = fi.readLink();
        if (!real.isEmpty())
            return pixmap(TQFileInfo(real));
    }

    TQString ext = fi.extension( false ).upper();
    TQString key = ext;
    ext.prepend( "." );
    TQMap< TQString, TQPixmap >::Iterator it;

    if ( fi.isDir() ) {
	return &defaultFolder;
    } else if ( ext != ".EXE" ) {
	it = cache.find( key );
	if ( it != cache.end() )
	    return &( *it );

	HKEY k, k2;
	int r;
	QT_WA( {
	    r = RegOpenKeyEx( HKEY_CLASSES_ROOT, (TCHAR*)ext.ucs2(),
			      0, KEY_READ, &k );
	} , {
	    r = RegOpenKeyExA( HKEY_CLASSES_ROOT, ext.local8Bit(),
			       0, KEY_READ, &k );
	} );
	TQString s;
	if ( r == ERROR_SUCCESS ) {
	    s = getWindowsRegString( k, TQString::null );
	} else {
	    cache[ key ] = defaultFile;
	    RegCloseKey( k );
	    return &defaultFile;
	}
	RegCloseKey( k );

	QT_WA( {
	    r = RegOpenKeyEx( HKEY_CLASSES_ROOT, (TCHAR*)TQString( s + "\\DefaultIcon" ).ucs2(),
			       0, KEY_READ, &k2 );
	} , {
	    r = RegOpenKeyExA( HKEY_CLASSES_ROOT, TQString( s + "\\DefaultIcon" ).local8Bit() ,
	    		       0, KEY_READ, &k2 );
	} );
	if ( r == ERROR_SUCCESS ) {
	    s = getWindowsRegString( k2, TQString::null );
	} else {
	    cache[ key ] = defaultFile;
	    RegCloseKey( k2 );
	    return &defaultFile;
	}
	RegCloseKey( k2 );

	TQStringList lst = TQStringList::split( ",", s );

        HICON si;
        UINT res = 0;
        if (lst.count() >= 2) { // don't just assume that lst has two entries
            TQString filepath = lst[ 0 ].stripWhiteSpace();
            if ( !filepath.isEmpty() ) {
                if ( filepath.find("%1") != -1 ) {
                    filepath = filepath.arg( fi.filePath() );
                    if ( ext == ".DLL" ) {
                        pix = defaultFile;
                        return &pix;
                    }
                }
                if ( filepath[0] == '"' && filepath[(int)filepath.length()-1] == '"' )
                    filepath = filepath.mid( 1, filepath.length()-2 );

                resolveLibs();
#ifndef Q_OS_TEMP
                QT_WA( {
                        res = ptrExtractIconEx( (TCHAR*)filepath.ucs2(), lst[ 1 ].stripWhiteSpace().toInt(),
                            0, &si, 1 );
                        } , {
                        res = ExtractIconExA( filepath.local8Bit(), lst[ 1 ].stripWhiteSpace().toInt(),
                            0, &si, 1 );
                        } );
#else
                res = (UINT)ExtractIconEx( (TCHAR*)filepath.ucs2(), lst[ 1 ].stripWhiteSpace().toInt(),
                        0, &si, 1 );
#endif
            }
        }

	if ( res ) {
	    pix.resize( pixw, pixh );
	    initPixmap( pix );
	    TQPainter p( &pix );
	    DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, 0,  DI_NORMAL );
	    p.end();
	    pix.setMask( pix.createHeuristicMask() );
	    DestroyIcon( si );
	} else {
	    pix = defaultFile;
	}

	cache[ key ] = pix;
	return &pix;
    } else {
	HICON si;
	UINT res = 0;
	if ( !fi.absFilePath().isEmpty() ) {
#ifndef Q_OS_TEMP
	    QT_WA( {
		res = ptrExtractIconEx( (TCHAR*)fi.absFilePath().ucs2(), -1,
				      0, 0, 1 );
	    } , {
		res = ExtractIconExA( fi.absFilePath().local8Bit(), -1,
				      0, 0, 1 );
	    } );

	    if ( res ) {
		QT_WA( {
		    res = ptrExtractIconEx( (TCHAR*)fi.absFilePath().ucs2(), res - 1,
					  0, &si, 1 );
		} , {
		    res = ExtractIconExA( fi.absFilePath().local8Bit(), res - 1,
					  0, &si, 1 );
		} );
	    }
#else
		res = (UINT)ExtractIconEx( (TCHAR*)fi.absFilePath().ucs2(), -1,
					    0, 0, 1 );
		if ( res )
		    res = (UINT)ExtractIconEx( (TCHAR*)fi.absFilePath().ucs2(), res - 1,
						0, &si, 1 );
#endif

	}

	if ( res ) {
	    pix.resize( pixw, pixh );
	    initPixmap( pix );
	    TQPainter p( &pix );
	    DrawIconEx( p.handle(), 0, 0, si, pixw, pixh, 0, 0,  DI_NORMAL );
	    p.end();
	    pix.setMask( pix.createHeuristicMask() );
	    DestroyIcon( si );
	} else {
	    pix = defaultExe;
	}

	return &pix;
    }

    // can't happen!
    return 0;
}
#endif



/*!
  \reimp
*/
bool TQFileDialog::eventFilter( TQObject * o, TQEvent * e )
{
    if ( e->type() == TQEvent::KeyPress && ( (TQKeyEvent*)e )->key() == Key_F5 ) {
	rereadDir();
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress && ( (TQKeyEvent*)e )->key() == Key_F2 &&
		( o == files || o == files->viewport() ) ) {
	if ( files->isVisible() && files->currentItem() ) {
	    if ( TQUrlInfo( d->url, "." ).isWritable() && files->currentItem()->text( 0 ) != ".." ) {
		files->renameItem = files->currentItem();
		files->startRename( true );
	    }
	}
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress && ( (TQKeyEvent*)e )->key() == Key_F2 &&
		( o == d->moreFiles || o == d->moreFiles->viewport() ) ) {
	if ( d->moreFiles->isVisible() && d->moreFiles->currentItem() != -1 ) {
	    if ( TQUrlInfo( d->url, "." ).isWritable() &&
		 d->moreFiles->item( d->moreFiles->currentItem() )->text() != ".." ) {
		d->moreFiles->renameItem = d->moreFiles->item( d->moreFiles->currentItem() );
		d->moreFiles->startRename( true );
	    }
	}
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress && d->moreFiles->renaming ) {
	d->moreFiles->lined->setFocus();
	TQApplication::sendEvent( d->moreFiles->lined, e );
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress && files->renaming ) {
	files->lined->setFocus();
	TQApplication::sendEvent( files->lined, e );
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress &&
		((TQKeyEvent *)e)->key() == Key_Backspace &&
		( o == files ||
		  o == d->moreFiles ||
		  o == files->viewport() ||
		  o == d->moreFiles->viewport() ) ) {
	cdUpClicked();
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress &&
		((TQKeyEvent *)e)->key() == Key_Delete &&
		( o == files ||
		  o == files->viewport() ) ) {
	if ( files->currentItem() )
	    deleteFile( files->currentItem()->text( 0 ) );
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( e->type() == TQEvent::KeyPress &&
		((TQKeyEvent *)e)->key() == Key_Delete &&
		( o == d->moreFiles ||
		  o == d->moreFiles->viewport() ) ) {
	int c = d->moreFiles->currentItem();
	if ( c >= 0 )
	    deleteFile( d->moreFiles->item( c )->text() );
	((TQKeyEvent *)e)->accept();
	return true;
    } else if ( o == files && e->type() == TQEvent::FocusOut && files->currentItem() ) {
    } else if ( o == files && e->type() == TQEvent::KeyPress ) {
	TQTimer::singleShot( 0, this, TQ_SLOT(fixupNameEdit()) );
    } else if ( o == nameEdit && e->type() == TQEvent::KeyPress && d->mode != AnyFile ) {
	if ( ( nameEdit->cursorPosition() == (int)nameEdit->text().length() || nameEdit->hasSelectedText() ) &&
	     isprint(((TQKeyEvent *)e)->ascii()) ) {
#if defined(TQ_WS_WIN)
	    TQString nt( nameEdit->text().lower() );
#else
	    TQString nt( nameEdit->text() );
#endif
	    nt.truncate( nameEdit->cursorPosition() );
	    nt += (char)(((TQKeyEvent *)e)->ascii());
	    TQListViewItem * i = files->firstChild();
#if defined(TQ_WS_WIN)
	    while( i && i->text( 0 ).left(nt.length()).lower() != nt )
#else
	    while( i && i->text( 0 ).left(nt.length()) != nt )
#endif
		i = i->nextSibling();
	    if ( i ) {
		nt = i->text( 0 );
		int cp = nameEdit->cursorPosition()+1;
		nameEdit->validateAndSet( nt, cp, cp, nt.length() );
		return true;
	    }
	}
    } else if ( o == nameEdit && e->type() == TQEvent::FocusIn ) {
	fileNameEditDone();
    } else if ( d->moreFiles->renaming && o != d->moreFiles->lined && e->type() == TQEvent::FocusIn ) {
	d->moreFiles->lined->setFocus();
	return true;
    } else if ( files->renaming && o != files->lined && e->type() == TQEvent::FocusIn ) {
	files->lined->setFocus();
	return true;
    } else if ( ( o == d->moreFiles || o == d->moreFiles->viewport() ) &&
		e->type() == TQEvent::FocusIn ) {
	if ( ( o == d->moreFiles->viewport() && !d->moreFiles->viewport()->hasFocus() ) ||
	     ( o == d->moreFiles && !d->moreFiles->hasFocus() ) )
	    ((TQWidget*)o)->setFocus();
	return false;
    }

    return TQDialog::eventFilter( o, e );
}

/*!
  Sets the filters used in the file dialog to \a filters. Each group
  of filters must be separated by \c{;;} (\e two semi-colons).

  \code
    TQString types("Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
		  "Any files (*)");
    TQFileDialog fd = new TQFileDialog( this );
    fd->setFilters( types );
    fd->show();
  \endcode

*/

void TQFileDialog::setFilters( const TQString &filters )
{
    TQStringList lst = makeFiltersList( filters );
    setFilters( lst );
}

/*!
  \overload

  \a types must be a null-terminated list of strings.

*/

void TQFileDialog::setFilters( const char ** types )
{
    if ( !types || !*types )
	return;

    d->types->clear();
    while( types && *types ) {
	d->types->insertItem( TQString::fromLatin1(*types) );
	types++;
    }
    d->types->setCurrentItem( 0 );
    setFilter( d->types->text( 0 ) );
}


/*! \overload void TQFileDialog::setFilters( const TQStringList & )
*/

void TQFileDialog::setFilters( const TQStringList & types )
{
    if ( types.count() < 1 )
	return;

    d->types->clear();
    for ( TQStringList::ConstIterator it = types.begin(); it != types.end(); ++it )
	d->types->insertItem( *it );
    d->types->setCurrentItem( 0 );
    setFilter( d->types->text( 0 ) );
}

/*!
  Adds the filter \a filter to the list of filters and makes it the
  current filter.

  \code
    TQFileDialog* fd = new TQFileDialog( this );
    fd->addFilter( "Images (*.png *.jpg *.xpm)" );
    fd->show();
  \endcode

  In the above example, a file dialog is created, and the file filter "Images
  (*.png *.jpg *.xpm)" is added and is set as the current filter. The original
  filter, "All Files (*)", is still available.

  \sa setFilter(), setFilters()
*/

void TQFileDialog::addFilter( const TQString &filter )
{
    if ( filter.isEmpty() )
	return;
    TQString f = filter;
    TQRegExp r( TQString::fromLatin1(qt_file_dialog_filter_reg_exp) );
    int index = r.search( f );
    if ( index >= 0 )
	f = r.cap( 2 );
    for ( int i = 0; i < d->types->count(); ++i ) {
	TQString f2( d->types->text( i ) );
	int index = r.search( f2 );
	if ( index >= 0 )
	    f2 = r.cap( 1 );
	if ( f2 == f ) {
	    d->types->setCurrentItem( i );
	    setFilter( f2 );
	    return;
	}
    }

    d->types->insertItem( filter );
    d->types->setCurrentItem( d->types->count() - 1 );
    setFilter( d->types->text( d->types->count() - 1 ) );
}

/*!
  Since modeButtons is a top-level widget, it may be destroyed by the
  kernel at application exit. Notice if this happens to
  avoid double deletion.
*/

void TQFileDialog::modeButtonsDestroyed()
{
    if ( d )
	d->modeButtons = 0;
}


/*!
  This is a convenience static function that will return one or more
  existing files selected by the user.

  \code
    TQStringList files = TQFileDialog::getOpenFileNames(
			    "Images (*.png *.xpm *.jpg)",
			    "/home",
			    this,
			    "open files dialog",
			    "Select one or more files to open" );
  \endcode

  This function creates a modal file dialog called \a name, with
  parent \a parent. If \a parent is not 0, the dialog will be shown
  centered over the parent.

  The file dialog's working directory will be set to \a dir. If \a
  dir includes a file name, the file will be selected. The filter
  is set to \a filter so that only those files which match the filter
  are shown. The filter selected is set to \a selectedFilter. The parameters
  \a dir, \a selectedFilter and \a filter may be TQString::null.

  The dialog's caption is set to \a caption. If \a caption is not
  specified then a default caption will be used.

  Under Windows and Mac OS X, this static function will use the native
  file dialog and not a TQFileDialog, unless the style of the application
  is set to something other than the native style. (Note that on Windows the
  dialog will spin a blocking modal event loop that will not dispatch any
  TQTimers and if parent is not 0 then it will position the dialog just under
  the parent's titlebar).

  Under Unix/X11, the normal behavior of the file dialog is to resolve
  and follow symlinks. For example, if /usr/tmp is a symlink to /var/tmp,
  the file dialog will change to /var/tmp after entering /usr/tmp.
  If \a resolveSymlinks is false, the file dialog will treat
  symlinks as regular directories.

  Note that if you want to iterate over the list of files, you should
  iterate over a copy, e.g.
    \code
    TQStringList list = files;
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  \sa getOpenFileName(), getSaveFileName(), getExistingDirectory()
*/

TQStringList TQFileDialog::getOpenFileNames( const TQString & filter,
					   const TQString& dir,
					   TQWidget *parent,
					   const char* name,
					   const TQString& caption,
					   TQString *selectedFilter,
					   bool resolveSymlinks )
{
    bool save_qt_resolve_symlinks = tqt_resolve_symlinks;
    tqt_resolve_symlinks = resolveSymlinks;

    TQStringList filters;
    if ( !filter.isEmpty() )
	filters = makeFiltersList( filter );

    makeVariables();

    if ( workingDirectory->isNull() )
	*workingDirectory = ::toRootIfNotExists( TQDir::currentDirPath() );

    if ( !dir.isEmpty() ) {
	// #### works only correct for local files
	TQUrlOperator u( TQFileDialogPrivate::encodeFileName(dir) );
	if ( u.isLocalFile() && TQFileInfo( u.path() ).isDir() ) {
	    *workingDirectory = dir;
	} else {
	    *workingDirectory = u.toString();
	}
    }

#if defined(TQ_WS_X11)
    if ( tqt_use_native_dialogs && TQTDEIntegration::enabled())
	return TQTDEIntegration::getOpenFileNames( filter, workingDirectory, parent, name, caption, selectedFilter, true );
#elif defined(TQ_WS_WIN)
    if ( tqt_use_native_dialogs && tqApp->style().styleHint( TQStyle::SH_GUIStyle ) == WindowsStyle )
	return winGetOpenFileNames( filter, workingDirectory, parent, name, caption, selectedFilter );
#elif defined(TQ_WS_MAC)
    if (tqt_use_native_dialogs && (tqApp->style().inherits(TQMAC_DEFAULT_STYLE)
                                  || tqApp->style().inherits("TQMacStyle"))) {
        TQStringList sl = macGetOpenFileNames(filter, dir.isEmpty() ? 0 : workingDirectory, parent,
                                             name, caption, selectedFilter);
        TQStringList::iterator it = sl.begin();
        while (it != sl.end()) {
            *it = qt_mac_precomposeFileName(*it);
            ++it;
        }
	return sl;
    }
#endif

    TQFileDialog *dlg = new TQFileDialog( *workingDirectory, TQString::null, parent, name ? name : "qt_filedlg_gofns", true );

    TQ_CHECK_PTR( dlg );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( !caption.isNull() )
	dlg->setCaption( caption );
    else
	dlg->setCaption( TQFileDialog::tr("Open") );
#endif

    dlg->setFilters( filters );
    if ( selectedFilter )
	dlg->setFilter( *selectedFilter );
    dlg->setMode( TQFileDialog::ExistingFiles );
    TQString result;
    TQStringList lst;
    if ( dlg->exec() == TQDialog::Accepted ) {
	lst = dlg->selectedFiles();
	*workingDirectory = dlg->d->url;
	if ( selectedFilter )
	    *selectedFilter = dlg->selectedFilter();
    }
    delete dlg;

    tqt_resolve_symlinks = save_qt_resolve_symlinks;
    return lst;
}

/*!  Updates the line edit to match the speed-key usage in TQListView. */

void TQFileDialog::fixupNameEdit()
{
    if ( files->currentItem() ) {
	if ( ( (TQFileDialogPrivate::File*)files->currentItem() )->info.isFile() )
	    nameEdit->setText( files->currentItem()->text( 0 ) );
    }
}

/*!
  Returns the URL of the current working directory in the file dialog.

  \sa setUrl()
*/

TQUrl TQFileDialog::url() const
{
    return d->url;
}

static bool isRoot( const TQUrl &u )
{
#if defined(Q_OS_MAC9)
     TQString p = TQDir::convertSeparators(u.path());
     if(p.contains(':') == 1)
	return true;
#elif defined(Q_OS_UNIX)
    if ( u.path() == "/" )
	return true;
#elif defined(Q_OS_WIN32)
    TQString p = u.path();
    if ( p.length() == 3 &&
	 p.right( 2 ) == ":/" )
	return true;
    if ( p[ 0 ] == '/' && p[ 1 ] == '/' ) {
	int slashes = p.contains( '/' );
	if ( slashes <= 3 )
	    return true;
	if ( slashes == 4 && p[ (int)p.length() - 1 ] == '/' )
	    return true;
    }
#else
#if defined(Q_CC_GNU)
#warning "case not covered.."
#endif
#endif

    if ( !u.isLocalFile() && u.path() == "/" )
	return true;

    return false;
}

void TQFileDialog::urlStart( TQNetworkOperation *op )
{
    if ( !op )
	return;

#if defined(TQ_WS_WIN)
    tqt_ntfs_permission_lookup--;
#endif
    if ( op->operation() == TQNetworkProtocol::OpListChildren ) {
#ifndef TQT_NO_CURSOR
	if ( !d->cursorOverride ) {
	    TQApplication::setOverrideCursor( TQCursor( TQt::WaitCursor ) );
	    d->cursorOverride = true;
	}
#endif
	if ( isRoot( d->url ) )
	    d->cdToParent->setEnabled( false );
	else
	    d->cdToParent->setEnabled( true );
	d->mimeTypeTimer->stop();
	d->sortedList.clear();
	d->pendingItems.clear();
	d->moreFiles->clearSelection();
	files->clearSelection();
	d->moreFiles->clear();
	files->clear();
	files->setSorting( -1 );

	TQString s = d->url.toString( false, false );
	bool found = false;
	for ( int i = 0; i < d->paths->count(); ++i ) {
#if defined(TQ_WS_WIN)
	    if ( d->paths->text( i ).lower() == s.lower() ) {
#else
	    if ( d->paths->text( i ) == s ) {
#endif
		found = true;
		d->paths->setCurrentItem( i );
		break;
	    }
	}
	if ( !found ) {
	    d->paths->insertItem( *openFolderIcon, s, -1 );
	    d->paths->setCurrentItem( d->paths->count() - 1 );
	}
	d->last = 0;
	d->hadDotDot = false;

	if ( d->goBack && d->history.last() != d->url.toString() ) {
	    d->history.append( d->url.toString() );
	    if ( d->history.count() > 1 )
		d->goBack->setEnabled( true );
	}
    }
}

void TQFileDialog::urlFinished( TQNetworkOperation *op )
{
    if ( !op )
	return;

#ifndef TQT_NO_CURSOR
    if ( op->operation() == TQNetworkProtocol::OpListChildren &&
	 d->cursorOverride ) {
	TQApplication::restoreOverrideCursor();
	d->cursorOverride = false;
    }
#endif

    if ( op->state() == TQNetworkProtocol::StFailed ) {
	if ( d->paths->hasFocus() )
	    d->ignoreNextKeyPress = true;

	if ( d->progressDia ) {
	    d->ignoreStop = true;
	    d->progressDia->close();
	    delete d->progressDia;
	    d->progressDia = 0;
	}

	int ecode = op->errorCode();
	TQMessageBox::critical( this, tr( "Error" ), op->protocolDetail() );

	if ( ecode == TQNetworkProtocol::ErrListChildren || ecode == TQNetworkProtocol::ErrParse ||
	     ecode == TQNetworkProtocol::ErrUnknownProtocol || ecode == TQNetworkProtocol::ErrLoginIncorrect ||
	     ecode == TQNetworkProtocol::ErrValid || ecode == TQNetworkProtocol::ErrHostNotFound ||
	     ecode == TQNetworkProtocol::ErrFileNotExisting ) {
	    if (d->url != d->oldUrl) {
		d->url = d->oldUrl;
		rereadDir();
	    }
	} else {
	    // another error happened, no need to go back to last dir
	}
    } else if ( op->operation() == TQNetworkProtocol::OpListChildren &&
		op == d->currListChildren ) {
	if ( !d->hadDotDot && !isRoot( d->url ) ) {
	    bool ok = true;
#if defined(TQ_WS_WIN)
	    if ( d->url.path().left( 2 ) == "//" )
		ok = false;
#endif
	    if ( ok ) {
		TQUrlInfo ui( d->url, ".." );
		ui.setName( ".." );
		ui.setDir( true );
		ui.setFile( false );
		ui.setSymLink( false );
		ui.setSize( 0 );
		TQValueList<TQUrlInfo> lst;
		lst << ui;
		insertEntry( lst, 0 );
	    }
	}
	resortDir();
    } else if ( op->operation() == TQNetworkProtocol::OpGet ) {
    } else if ( op->operation() == TQNetworkProtocol::OpPut ) {
	rereadDir();
	if ( d->progressDia ) {
	    d->ignoreStop = true;
	    d->progressDia->close();
	}
	delete d->progressDia;
	d->progressDia = 0;
    }

#if defined(TQ_WS_WIN)
    if (d->oldPermissionLookup != tqt_ntfs_permission_lookup)
        tqt_ntfs_permission_lookup++;
#endif
}

void TQFileDialog::dataTransferProgress( int bytesDone, int bytesTotal, TQNetworkOperation *op )
{
    if ( !op )
	return;

    TQString label;
    TQUrl u( op->arg( 0 ) );
    if ( u.isLocalFile() ) {
	label = u.path();
    } else {
	label = TQString( "%1 (on %2)" );
	label = label.arg( u.path() ).arg( u.host() );
    }

    if ( !d->progressDia ) {
	if ( bytesDone < bytesTotal) {
	    d->ignoreStop = false;
	    d->progressDia = new TQFDProgressDialog( this, label, bytesTotal );
	    connect( d->progressDia, TQ_SIGNAL( cancelled() ),
		     this, TQ_SLOT( stopCopy() ) );
	    d->progressDia->show();
	} else
	    return;
    }

    if ( d->progressDia ) {
	if ( op->operation() == TQNetworkProtocol::OpGet ) {
	    if ( d->progressDia ) {
		d->progressDia->setReadProgress( bytesDone );
	    }
	} else if ( op->operation() == TQNetworkProtocol::OpPut ) {
	    if ( d->progressDia ) {
		d->progressDia->setWriteLabel( label );
		d->progressDia->setWriteProgress( bytesDone );
	    }
	} else {
	    return;
	}
    }
}

void TQFileDialog::insertEntry( const TQValueList<TQUrlInfo> &lst, TQNetworkOperation *op )
{
    if ( op && op->operation() == TQNetworkProtocol::OpListChildren &&
	 op != d->currListChildren )
	return;
    TQValueList<TQUrlInfo>::ConstIterator it = lst.begin();
    for ( ; it != lst.end(); ++it ) {
	const TQUrlInfo &inf = *it;
	if ( d->mode == DirectoryOnly && !inf.isDir() )
	    continue;
	if ( inf.name() == ".." ) {
	    d->hadDotDot = true;
	    if ( isRoot( d->url ) )
		continue;
#if defined(TQ_WS_WIN)
	    if ( d->url.path().left( 2 ) == "//" )
		continue;
#endif
	} else if ( inf.name() == "." )
	    continue;

#if defined(TQ_WS_WIN)
	// Workaround a Windows bug, '..' is apparantly hidden in directories
	// that are one level away from root
	if ( !bShowHiddenFiles && inf.name() != ".." ) {
	    if ( d->url.isLocalFile() ) {
		TQString file = d->url.path();
		if ( !file.endsWith( "/" ) )
		    file.append( "/" );
	        file += inf.name();
		QT_WA( {
		    if ( GetFileAttributesW( (TCHAR*)file.ucs2() ) & FILE_ATTRIBUTE_HIDDEN )
			continue;
		} , {
		    if ( GetFileAttributesA( file.local8Bit() ) & FILE_ATTRIBUTE_HIDDEN )
			continue;
		} );
	    } else {
		if ( inf.name() != ".." && inf.name()[0] == TQChar('.') )
		    continue;
	    }
	}
#else
 	if ( !bShowHiddenFiles && inf.name() != ".." ) {
 	    if ( inf.name()[ 0 ] == TQChar( '.' ) )
 		continue;
 	}
#endif
	if ( !d->url.isLocalFile() ) {
	    TQFileDialogPrivate::File * i = 0;
	    TQFileDialogPrivate::MCItem *i2 = 0;
	    i = new TQFileDialogPrivate::File( d, &inf, files );
	    i2 = new TQFileDialogPrivate::MCItem( d->moreFiles, i );

	    if ( ( d->mode == ExistingFiles && inf.isDir() ) ||
		( isDirectoryMode( d->mode ) && inf.isFile() ) ) {
		i->setSelectable( false );
		i2->setSelectable( false );
	    }

	    i->i = i2;
	}

	d->sortedList.append( new TQUrlInfo( inf ) );
    }
}

void TQFileDialog::removeEntry( TQNetworkOperation *op )
{
    if ( !op )
	return;

    TQUrlInfo *i = 0;
    TQListViewItemIterator it( files );
    bool ok1 = false, ok2 = false;
    for ( i = d->sortedList.first(); it.current(); ++it, i = d->sortedList.next() ) {
        TQString encName = TQFileDialogPrivate::encodeFileName(
            ( (TQFileDialogPrivate::File*)it.current() )->info.name() );
	if ( encName == op->arg( 0 ) ) {
	    d->pendingItems.removeRef( (TQFileDialogPrivate::File*)it.current() );
	    delete ( (TQFileDialogPrivate::File*)it.current() )->i;
	    delete it.current();
	    ok1 = true;
	}
	if ( i && i->name() == op->arg( 0 ) ) {
	    d->sortedList.removeRef( i );
	    i = d->sortedList.prev();
	    ok2 = true;
	}
	if ( ok1 && ok2 )
	    break;
    }
}

void TQFileDialog::itemChanged( TQNetworkOperation *op )
{
    if ( !op )
	return;

    TQUrlInfo *i = 0;
    TQListViewItemIterator it1( files );
    bool ok1 = false, ok2 = false;
    // first check whether the new file replaces an existing file.
    for ( i = d->sortedList.first(); it1.current(); ++it1, i = d->sortedList.next() ) {
	if ( ( (TQFileDialogPrivate::File*)it1.current() )->info.name() == op->arg( 1 ) ) {
	    delete ( (TQFileDialogPrivate::File*)it1.current() )->i;
	    delete it1.current();
	    ok1 = true;
	}
	if ( i && i->name() == op->arg( 1 ) ) {
	    d->sortedList.removeRef( i );
	    i = d->sortedList.prev();
	    ok2 = true;
	}
	if ( ok1 && ok2 )
	    break;
    }

    i = 0;
    TQListViewItemIterator it( files );
    ok1 = false;
    ok2 = false;
    for ( i = d->sortedList.first(); it.current(); ++it, i = d->sortedList.next() ) {
	if ( ( (TQFileDialogPrivate::File*)it.current() )->info.name() == op->arg( 0 ) ) {
	    ( (TQFileDialogPrivate::File*)it.current() )->info.setName( op->arg( 1 ) );
	    ok1 = true;
	}
	if ( i && i->name() == op->arg( 0 ) ) {
	    i->setName( op->arg( 1 ) );
	    ok2 = true;
	}
	if ( ok1 && ok2 )
	    break;
    }

    resortDir();
}

/*!
  \property TQFileDialog::infoPreview

  \brief whether the file dialog can provide preview information about
  the currently selected file

  The default is false.
*/
bool TQFileDialog::isInfoPreviewEnabled() const
{
    return d->infoPreview;
}

void TQFileDialog::setInfoPreviewEnabled( bool info )
{
    if ( info == d->infoPreview )
	return;
    d->geometryDirty = true;
    d->infoPreview = info;
    updateGeometries();
}


/*!
  \property TQFileDialog::contentsPreview

  \brief whether the file dialog can provide a contents preview of the
  currently selected file

  The default is false.

  \sa setContentsPreview() setInfoPreviewEnabled()
*/
// ### improve the above documentation: how is the preview done, how can I add
// support for customized preview, etc.

bool TQFileDialog::isContentsPreviewEnabled() const
{
    return d->contentsPreview;
}

void TQFileDialog::setContentsPreviewEnabled( bool contents )
{
    if ( contents == d->contentsPreview )
	return;
    d->geometryDirty = true;
    d->contentsPreview = contents;
    updateGeometries();
}


/*!
  Sets the widget to be used for displaying information about the file
  to the widget \a w and a preview of that information to the
  TQFilePreview \a preview.

  Normally you would create a preview widget that derives from both TQWidget and
  TQFilePreview, so you should pass the same widget twice. If you
  don't, you must remember to delete the preview object in order to
  avoid memory leaks.

  \code
    class Preview : public TQLabel, public TQFilePreview
    {
    public:
	Preview( TQWidget *parent=0 ) : TQLabel( parent ) {}

	void previewUrl( const TQUrl &u )
	{
	    TQString path = u.path();
	    TQPixmap pix( path );
	    if ( pix.isNull() )
		setText( "This is not a pixmap" );
	    else
		setText( "This is a pixmap" );
	}
    };

  //...

  int main( int argc, char** argv )
  {
    Preview* p = new Preview;

    TQFileDialog* fd = new TQFileDialog( this );
    fd->setInfoPreviewEnabled( true );
    fd->setInfoPreview( p, p );
    fd->setPreviewMode( TQFileDialog::Info );
    fd->show();
  }

  \endcode

  \sa setContentsPreview(), setInfoPreviewEnabled(), setPreviewMode()

*/

void TQFileDialog::setInfoPreview( TQWidget *w, TQFilePreview *preview )
{
    if ( !w || !preview )
	return;

    if ( d->infoPreviewWidget ) {
	d->preview->removeWidget( d->infoPreviewWidget );
	delete d->infoPreviewWidget;
    }
    d->infoPreviewWidget = w;
    d->infoPreviewer = preview;
    w->reparent( d->preview, 0, TQPoint( 0, 0 ) );
}

/*!
  Sets the widget to be used for displaying the contents of the file
  to the widget \a w and a preview of those contents to the
  TQFilePreview \a preview.

  Normally you would create a preview widget that derives from both TQWidget and
  TQFilePreview, so you should pass the same widget twice. If you
  don't, you must remember to delete the preview object in order to
  avoid memory leaks.

  \code
    class Preview : public TQLabel, public TQFilePreview
    {
    public:
	Preview( TQWidget *parent=0 ) : TQLabel( parent ) {}

	void previewUrl( const TQUrl &u )
	{
	    TQString path = u.path();
	    TQPixmap pix( path );
	    if ( pix.isNull() )
		setText( "This is not a pixmap" );
	    else
		setPixmap( pix );
	}
    };

  //...

  int main( int argc, char** argv )
  {
    Preview* p = new Preview;

    TQFileDialog* fd = new TQFileDialog( this );
    fd->setContentsPreviewEnabled( true );
    fd->setContentsPreview( p, p );
    fd->setPreviewMode( TQFileDialog::Contents );
    fd->show();
  }
  \endcode

  \sa setContentsPreviewEnabled(), setInfoPreview(), setPreviewMode()
*/

void TQFileDialog::setContentsPreview( TQWidget *w, TQFilePreview *preview )
{
    if ( !w || !preview )
	return;

    if ( d->contentsPreviewWidget ) {
	d->preview->removeWidget( d->contentsPreviewWidget );
	delete d->contentsPreviewWidget;
    }
    d->contentsPreviewWidget = w;
    d->contentsPreviewer = preview;
    w->reparent( d->preview, 0, TQPoint( 0, 0 ) );
}

/*!
  Re-sorts the displayed directory.

  \sa rereadDir()
*/

void TQFileDialog::resortDir()
{
    d->mimeTypeTimer->stop();
    d->pendingItems.clear();

    TQFileDialogPrivate::File *item = 0;
    TQFileDialogPrivate::MCItem *item2 = 0;

    d->sortedList.sort();

    if ( files->childCount() > 0 || d->moreFiles->count() > 0 ) {
	d->moreFiles->clear();
	files->clear();
	d->last = 0;
	files->setSorting( -1 );
    }

    TQUrlInfo *i = sortAscending ? d->sortedList.first() : d->sortedList.last();
    for ( ; i; i = sortAscending ? d->sortedList.next() : d->sortedList.prev() ) {
	item = new TQFileDialogPrivate::File( d, i, files );
	item2 = new TQFileDialogPrivate::MCItem( d->moreFiles, item, item2 );
	item->i = item2;
	d->pendingItems.append( item );
	if ( ( d->mode == ExistingFiles && item->info.isDir() ) ||
	    ( isDirectoryMode( d->mode ) && item->info.isFile() ) ) {
	    item->setSelectable( false );
	    item2->setSelectable( false );
	}
    }

    // ##### As the TQFileIconProvider only support TQFileInfo and no
    // TQUrlInfo it can be only used for local files at the moment. In
    // 3.0 we have to change the API of TQFileIconProvider to work on
    // TQUrlInfo so that also remote filesystems can be show mime-type
    // specific icons.
    if ( d->url.isLocalFile() )
	d->mimeTypeTimer->start( 0 );
}

/*!
  Stops the current copy operation.
*/

void TQFileDialog::stopCopy()
{
    if ( d->ignoreStop )
	return;

    d->url.blockSignals( true );
    d->url.stop();
    if ( d->progressDia ) {
	d->ignoreStop = true;
	TQTimer::singleShot( 100, this, TQ_SLOT( removeProgressDia() ) );
    }
    d->url.blockSignals( false );
}

/*!
  \internal
*/

void TQFileDialog::removeProgressDia()
{
    if ( d->progressDia )
	delete d->progressDia;
    d->progressDia = 0;
}

/*!
  \internal
*/

void TQFileDialog::doMimeTypeLookup()
{
    if ( !iconProvider() ) {
	d->pendingItems.clear();
	d->mimeTypeTimer->stop();
	return;
    }

    d->mimeTypeTimer->stop();
    if ( d->pendingItems.count() == 0 ) {
	return;
    }

    TQRect r;
    TQFileDialogPrivate::File *item = d->pendingItems.first();
    if ( item ) {
	TQFileInfo fi;
	if ( d->url.isLocalFile() ) {
	    fi.setFile( TQUrl( d->url.path(), TQFileDialogPrivate::encodeFileName( item->info.name() ) ).path( false ) );
	} else
	    fi.setFile( item->info.name() ); // #####
	const TQPixmap *p = iconProvider()->pixmap( fi );
	if ( p && p != item->pixmap( 0 ) &&
	     ( !item->pixmap( 0 ) || p->serialNumber() != item->pixmap( 0 )->serialNumber() ) &&
	     p != fifteenTransparentPixels ) {
	    item->hasMimePixmap = true;

	    // evil hack to avoid much too much repaints!
	    TQGuardedPtr<TQFileDialog> that( this ); // this may be deleted by an event handler
	    tqApp->processEvents();
	    if ( that.isNull() )
		return;
	    files->setUpdatesEnabled( false );
	    files->viewport()->setUpdatesEnabled( false );
	    if ( item != d->pendingItems.first() )
		return;
	    item->setPixmap( 0, *p );
	    tqApp->processEvents();
	    if ( that.isNull() )
		return;
	    files->setUpdatesEnabled( true );
	    files->viewport()->setUpdatesEnabled( true );

	    if ( files->isVisible() ) {
		TQRect ir( files->itemRect( item ) );
		if ( ir != TQRect( 0, 0, -1, -1 ) ) {
		    r = r.unite( ir );
		}
	    } else {
		TQRect ir( d->moreFiles->itemRect( item->i ) );
		if ( ir != TQRect( 0, 0, -1, -1 ) ) {
		    r = r.unite( ir );
		}
	    }
	}
	if ( d->pendingItems.count() )
	    d->pendingItems.removeFirst();
    }

    if ( d->moreFiles->isVisible() ) {
	d->moreFiles->viewport()->repaint( r, false );
    } else {
	files->viewport()->repaint( r, false );
    }

    if ( d->pendingItems.count() )
	d->mimeTypeTimer->start( 0 );
    else if ( d->moreFiles->isVisible() )
	d->moreFiles->triggerUpdate( true );
}

/*!
  If \a b is true then all the files in the current directory are selected;
  otherwise, they are deselected.
*/

void TQFileDialog::selectAll( bool b )
{
    if ( d->mode != ExistingFiles )
	return;
    d->moreFiles->selectAll( b );
    files->selectAll( b );
}

void TQFileDialog::goBack()
{
    if ( !d->goBack || !d->goBack->isEnabled() )
	return;
    d->history.remove( d->history.last() );
    if ( d->history.count() < 2 )
	d->goBack->setEnabled( false );
    setUrl( d->history.last() );
}

// a class with wonderfully inflexible flexibility. why doesn't it
// just subclass TQWidget in the first place? 'you have to derive your
// preview widget from TQWidget and from this class' indeed.

/*!
  \class TQFilePreview ntqfiledialog.h
  \ingroup misc
  \brief The TQFilePreview class provides file previewing in TQFileDialog.

  This class is an abstract base class which is used to implement
  widgets that can display a preview of a file in a TQFileDialog.

  You must derive the preview widget from both TQWidget and from this
  class. Then you must reimplement this class's previewUrl() function,
  which is called by the file dialog if the preview of a file
  (specified as a URL) should be shown.

  See also TQFileDialog::setPreviewMode(), TQFileDialog::setContentsPreview(),
  TQFileDialog::setInfoPreview(), TQFileDialog::setInfoPreviewEnabled(),
  TQFileDialog::setContentsPreviewEnabled().

  For an example of a preview widget see qt/examples/qdir/qdir.cpp.
*/

/*!
  Constructs the TQFilePreview.
*/

TQFilePreview::TQFilePreview()
{
}

/*!
  \fn void TQFilePreview::previewUrl( const TQUrl &url )

  This function is called by TQFileDialog if a preview
  for the \a url should be shown. Reimplement this
  function to provide file previewing.
*/


#include "qfiledialog.moc"

#endif
