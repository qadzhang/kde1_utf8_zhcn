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

#include <ntqvariant.h>

#include "pixmapchooser.h"
#include "formwindow.h"
#if defined(DESIGNER) && !defined(RESOURCE)
#include "pixmapfunction.h"
#endif
#include "metadatabase.h"
#include "mainwindow.h"
#include "pixmapcollectioneditor.h"
#include "pixmapcollection.h"
#include "project.h"

#include <ntqapplication.h>
#include <ntqimage.h>
#include <ntqpainter.h>
#include <ntqlabel.h>
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include <ntqiconview.h>

#if defined(DESIGNER)
static ImageIconProvider *imageIconProvider = 0;
#endif

PixmapView::PixmapView( TQWidget *parent )
    : TQScrollView( parent )
{
    viewport()->setBackgroundMode( PaletteBase );
}

void PixmapView::setPixmap( const TQPixmap &pix )
{
    pixmap = pix;
    resizeContents( pixmap.size().width(), pixmap.size().height() );
    viewport()->repaint( false );
}

void PixmapView::drawContents( TQPainter *p, int cx, int cy, int cw, int ch )
{
    p->fillRect( cx, cy, cw, ch, colorGroup().brush( TQColorGroup::Base ) );
    p->drawPixmap( 0, 0, pixmap );
}

void PixmapView::previewUrl( const TQUrl &u )
{
    if ( u.isLocalFile() ) {
	TQString path = u.path();
	TQPixmap pix( path );
	if ( !pix.isNull() )
	    setPixmap( pix );
    } else {
	tqWarning( "Previewing remote files not supported." );
    }
}

static void buildImageFormatList( TQString &filter, TQString &all )
{
    all = tqApp->translate( "qChoosePixmap", "All Pixmaps (" );
    for ( uint i = 0; i < TQImageIO::outputFormats().count(); i++ ) {
	TQString outputFormat = TQImageIO::outputFormats().at( i );
	TQString outputExtension;
	if ( outputFormat != "JPEG" )
	   outputExtension = outputFormat.lower();
	else
	    outputExtension = "jpg;*.jpeg";
	filter += tqApp->translate( "qChoosePixmap", "%1-Pixmaps (%2)\n" ).
		  arg( outputFormat ).arg( "*." + outputExtension);
	all += "*." + outputExtension + ";";
    }
    filter.prepend( all + tqApp->translate( "qChoosePixmap", ")\n" ) );
    filter += tqApp->translate( "qChoosePixmap", "All Files (*)" );

}
TQStringList qChoosePixmaps( TQWidget *parent )
{
    if ( !imageIconProvider && !TQFileDialog::iconProvider() )
	TQFileDialog::setIconProvider( ( imageIconProvider = new ImageIconProvider ) );

    TQString filter;
    TQString all;
    buildImageFormatList( filter, all );

    TQFileDialog fd( TQString::null, filter, parent, 0, true );
    fd.setMode( TQFileDialog::ExistingFiles );
    fd.setContentsPreviewEnabled( true );
    PixmapView *pw = new PixmapView( &fd );
    fd.setContentsPreview( pw, pw );
    fd.setViewMode( TQFileDialog::List );
    fd.setPreviewMode( TQFileDialog::Contents );
    fd.setCaption( tqApp->translate( "qChoosePixmap", "Choose Images..." ) );
    if ( fd.exec() == TQDialog::Accepted )
	return fd.selectedFiles();
    return TQStringList();
}

TQPixmap qChoosePixmap( TQWidget *parent, FormWindow *fw, const TQPixmap &old, TQString *fn )
{
#if defined(DESIGNER)
    if ( !fw || fw->savePixmapInline() ) {
	if ( !imageIconProvider && !TQFileDialog::iconProvider() )
	    TQFileDialog::setIconProvider( ( imageIconProvider = new ImageIconProvider ) );

	TQString filter;
	TQString all;
	buildImageFormatList( filter, all );

	TQFileDialog fd( TQString::null, filter, parent, 0, true );
	fd.setContentsPreviewEnabled( true );
	PixmapView *pw = new PixmapView( &fd );
	fd.setContentsPreview( pw, pw );
	fd.setViewMode( TQFileDialog::List );
	fd.setPreviewMode( TQFileDialog::Contents );
	fd.setCaption( tqApp->translate( "qChoosePixmap", "Choose a Pixmap..." ) );
	if ( fd.exec() == TQDialog::Accepted ) {
	    TQPixmap pix( fd.selectedFile() );
	    if ( fn )
		*fn = fd.selectedFile();
	    MetaDataBase::setPixmapArgument( fw, pix.serialNumber(), fd.selectedFile() );
	    return pix;
	}
    } else if ( fw && fw->savePixmapInProject() ) {
	PixmapCollectionEditor dia( parent, 0, true );
	dia.setProject( fw->project() );
	dia.setChooserMode( true );
	dia.setCurrentItem( MetaDataBase::pixmapKey( fw, old.serialNumber() ) );
	if ( dia.exec() == TQDialog::Accepted ) {
	    TQPixmap pix( fw->project()->pixmapCollection()->pixmap( dia.viewPixmaps->currentItem()->text() ) );
	    MetaDataBase::setPixmapKey( fw, pix.serialNumber(), dia.viewPixmaps->currentItem()->text() );
	    return pix;
	}
    } else {
	PixmapFunction dia( parent, 0, true );
	TQObject::connect( dia.helpButton, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );
	dia.labelFunction->setText( fw->pixmapLoaderFunction() + "(" );
	dia.editArguments->setText( MetaDataBase::pixmapArgument( fw, old.serialNumber() ) );
	dia.editArguments->setFocus();
	if ( dia.exec() == TQDialog::Accepted ) {
	    TQPixmap pix;
	    // we have to force the pixmap to get a new and unique serial number. Unfortunately detatch() doesn't do that
	    pix.convertFromImage( TQPixmap::fromMimeSource( "designer_image.png" ).convertToImage() );

	    MetaDataBase::setPixmapArgument( fw, pix.serialNumber(), dia.editArguments->text() );
	    return pix;
	}
    }
#else
    Q_UNUSED( parent );
    Q_UNUSED( fw );
    Q_UNUSED( old );
#endif
    return TQPixmap();
}

ImageIconProvider::ImageIconProvider( TQWidget *parent, const char *name )
    : TQFileIconProvider( parent, name ), imagepm( TQPixmap::fromMimeSource( "designer_image.png" ) )
{
    fmts = TQImage::inputFormats();
}

ImageIconProvider::~ImageIconProvider()
{
}

const TQPixmap * ImageIconProvider::pixmap( const TQFileInfo &fi )
{
    TQString ext = fi.extension().upper();
    if ( fmts.contains( ext ) ) {
	return &imagepm;
    } else {
	return TQFileIconProvider::pixmap( fi );
    }
}
