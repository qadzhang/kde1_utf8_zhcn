/**********************************************************************
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

void PixmapCollectionEditor::init()
{
    project = 0;
    setChooserMode( false );
}

void PixmapCollectionEditor::destroy()
{
}

void PixmapCollectionEditor::addPixmap()
{
    if ( !project )
	return;

    TQString f;
    TQStringList pixmaps = qChoosePixmaps( this );
    if ( pixmaps.isEmpty() )
	return;

    TQString lastName;
    for ( TQStringList::ConstIterator it = pixmaps.begin(); it != pixmaps.end(); ++it ) {
	TQPixmap pm( *it );
	if ( pm.isNull() )
	    continue;
	PixmapCollection::Pixmap pixmap;
	pixmap.pix = pm;
	TQFileInfo fi ( *it );
	pixmap.name = fi.fileName();
	pixmap.absname = fi.filePath();
	if ( !project->pixmapCollection()->addPixmap( pixmap, false ) )
	    continue;
	lastName = pixmap.name;
    }

    updateView();
    TQIconViewItem *item = viewPixmaps->findItem( lastName );
    if ( item ) {
	viewPixmaps->setCurrentItem( item );
	viewPixmaps->ensureItemVisible( item );
    }

}

void PixmapCollectionEditor::removePixmap()
{
    if ( !project || !viewPixmaps->currentItem() )
	return;
    project->pixmapCollection()->removePixmap( viewPixmaps->currentItem()->text() );
    updateView();
}

void PixmapCollectionEditor::updateView()
{
    if ( !project )
	return;

    viewPixmaps->clear();

    TQValueList<PixmapCollection::Pixmap> pixmaps = project->pixmapCollection()->pixmaps();
    for ( TQValueList<PixmapCollection::Pixmap>::Iterator it = pixmaps.begin(); it != pixmaps.end(); ++it ) {
	// #### might need to scale down the pixmap
	TQIconViewItem *item = new TQIconViewItem( viewPixmaps, (*it).name, scaledPixmap( (*it).pix ) );
	//item->setRenameEnabled( true ); // this will be a bit harder to implement
	item->setDragEnabled( false );
	item->setDropEnabled( false );
    }
    viewPixmaps->setCurrentItem( viewPixmaps->firstItem() );
    currentChanged( viewPixmaps->firstItem() );
}

void PixmapCollectionEditor::currentChanged( TQIconViewItem * i )
{
    buttonOk->setEnabled( !!i );
}

void PixmapCollectionEditor::setChooserMode( bool c )
{
    chooser = c;
    if ( chooser ) {
	buttonClose->hide();
	buttonOk->show();
	buttonCancel->show();
	buttonOk->setEnabled( false );
	buttonOk->setDefault( true );
	connect( viewPixmaps, TQ_SIGNAL( doubleClicked( TQIconViewItem * ) ), buttonOk, TQ_SIGNAL( clicked() ) );
	connect( viewPixmaps, TQ_SIGNAL( returnPressed( TQIconViewItem * ) ), buttonOk, TQ_SIGNAL( clicked() ) );
	setCaption( tr( "Choose an Image" ) );
    } else {
	buttonClose->show();
	buttonOk->hide();
	buttonCancel->hide();
	buttonClose->setDefault( true );
    }
    updateView();
}

void PixmapCollectionEditor::setCurrentItem( const TQString & name )
{
    TQIconViewItem *i = viewPixmaps->findItem( name );
    if ( i ) {
	viewPixmaps->setCurrentItem( i );
	currentChanged( i );
    }
}

void PixmapCollectionEditor::setProject( Project * pro )
{
    project = pro;
    updateView();
}

TQPixmap PixmapCollectionEditor::scaledPixmap( const TQPixmap & p )
{
    TQPixmap pix( p );
    if ( pix.width() < 50 && pix.height() < 50 )
	return pix;
    TQImage img;
    img = pix;
    img = img.smoothScale( 50, 50 );
    pix.convertFromImage( img );
    return pix;
}
