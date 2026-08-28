/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "showimg.h"
#include "imagetexteditor.h"
#include <ntqmenubar.h>
#include <ntqfiledialog.h>
#include <ntqmessagebox.h>
#include <ntqpopupmenu.h>
#include <ntqlabel.h>
#include <ntqpainter.h>
#include <ntqapplication.h>
#include <ntqclipboard.h>


/*
  In the constructor, we just pass the standard parameters on to
  TQWidget.

  The menu uses a single slot to simplify the process of adding
  more items to the options menu.
*/
ImageViewer::ImageViewer( TQWidget *parent, const char *name, int wFlags )
    : TQWidget( parent, name, wFlags ),
      conversion_flags( PreferDither ),
      helpmsg( 0 )
{
    pickx = -1;
    picky = -1;
    clickx = -1;
    clicky = -1;
    alloc_context = 0;

    menubar = new TQMenuBar(this);
    menubar->setSeparator( TQMenuBar::InWindowsStyle );

    TQStrList fmt = TQImage::outputFormats();
    saveimage = new TQPopupMenu( menubar );
    savepixmap = new TQPopupMenu( menubar );
    for (const char* f = fmt.first(); f; f = fmt.next()) {
	saveimage->insertItem( f );
	savepixmap->insertItem( f );
    }
    connect( saveimage, TQ_SIGNAL(activated(int)), this, TQ_SLOT(saveImage(int)) );
    connect( savepixmap, TQ_SIGNAL(activated(int)), this, TQ_SLOT(savePixmap(int)) );

    file = new TQPopupMenu( menubar );
    menubar->insertItem( "&File", file );
    file->insertItem( "&New window", this,  TQ_SLOT(newWindow()), CTRL+Key_N );
    file->insertItem( "&Open...", this,  TQ_SLOT(openFile()), CTRL+Key_O );
    si = file->insertItem( "Save image", saveimage );
    sp = file->insertItem( "Save pixmap", savepixmap );
    file->insertSeparator();
    file->insertItem( "E&xit", tqApp,  TQ_SLOT(quit()), CTRL+Key_Q );

    edit =  new TQPopupMenu( menubar );
    menubar->insertItem( "&Edit", edit );
    edit->insertItem("&Copy", this, TQ_SLOT(copy()), CTRL+Key_C);
    edit->insertItem("&Paste", this, TQ_SLOT(paste()), CTRL+Key_V);
    edit->insertSeparator();
    edit->insertItem("&Horizontal flip", this, TQ_SLOT(hFlip()), ALT+Key_H);
    edit->insertItem("&Vertical flip", this, TQ_SLOT(vFlip()), ALT+Key_V);
    edit->insertItem("&Rotate 180", this, TQ_SLOT(rot180()), ALT+Key_R);
    edit->insertSeparator();
    edit->insertItem("&Text...", this, TQ_SLOT(editText()));
    edit->insertSeparator();
    t1 = edit->insertItem( "Convert to &1 bit", this, TQ_SLOT(to1Bit()) );
    t8 = edit->insertItem( "Convert to &8 bit", this, TQ_SLOT(to8Bit()) );
    t32 = edit->insertItem( "Convert to &32 bit", this, TQ_SLOT(to32Bit()) );

    options =  new TQPopupMenu( menubar );
    menubar->insertItem( "&Options", options );
    ac = options->insertItem( "AutoColor" );
    co = options->insertItem( "ColorOnly" );
    mo = options->insertItem( "MonoOnly" );
    options->insertSeparator();
    fd = options->insertItem( "DiffuseDither" );
    bd = options->insertItem( "OrderedDither" );
    td = options->insertItem( "ThresholdDither" );
    options->insertSeparator();
    ta = options->insertItem( "ThresholdAlphaDither" );
    ba = options->insertItem( "OrderedAlphaDither" );
    fa = options->insertItem( "DiffuseAlphaDither" );
    options->insertSeparator();
    ad = options->insertItem( "PreferDither" );
    dd = options->insertItem( "AvoidDither" );
    options->insertSeparator();
    ss = options->insertItem( "Smooth scaling" );
    cc = options->insertItem( "Use color context" );
    if ( TQApplication::colorSpec() == TQApplication::ManyColor )
	options->setItemEnabled( cc, false );
    options->setCheckable( true );
    setMenuItemFlags();

    menubar->insertSeparator();

    TQPopupMenu* help = new TQPopupMenu( menubar );
    menubar->insertItem( "&Help", help );
    help->insertItem( "Help!", this, TQ_SLOT(giveHelp()), CTRL+Key_H );

    connect( options, TQ_SIGNAL(activated(int)), this, TQ_SLOT(doOption(int)) );

    status = new TQLabel(this);
    status->setFrameStyle( TQFrame::WinPanel | TQFrame::Sunken );
    status->setFixedHeight( fontMetrics().height() + 4 );

    setMouseTracking( true );
}

ImageViewer::~ImageViewer()
{
    if ( alloc_context )
	TQColor::destroyAllocContext( alloc_context );
    if ( other == this )
	other = 0;
}

/*
  This function modifies the conversion_flags when an options menu item
  is selected, then ensures all menu items are up to date, and reconverts
  the image if possibly necessary.
*/
void ImageViewer::doOption(int item)
{
    if ( item == ss || item == cc ) {
	// Toggle
	bool newbool = !options->isItemChecked(item);
	options->setItemChecked(item, newbool);
	// And reconvert...
	reconvertImage();
	repaint(image.hasAlphaBuffer());	// show image in widget
	return;
    }

    if ( options->isItemChecked( item ) ) return; // They are all radio buttons

    int ocf = conversion_flags;

    if ( item == ac ) {
	conversion_flags = ( conversion_flags & ~ColorMode_Mask ) | AutoColor;
    } else if ( item == co ) {
	conversion_flags = ( conversion_flags & ~ColorMode_Mask ) | ColorOnly;
    } else if ( item == mo ) {
	conversion_flags = ( conversion_flags & ~ColorMode_Mask ) | MonoOnly;
    } else if ( item == fd ) {
	conversion_flags = ( conversion_flags & ~Dither_Mask ) | DiffuseDither;
    } else if ( item == bd ) {
	conversion_flags = ( conversion_flags & ~Dither_Mask ) | OrderedDither;
    } else if ( item == td ) {
	conversion_flags = ( conversion_flags & ~Dither_Mask ) | ThresholdDither;
    } else if ( item == ta ) {
	conversion_flags = ( conversion_flags & ~AlphaDither_Mask ) | ThresholdAlphaDither;
    } else if ( item == fa ) {
	conversion_flags = ( conversion_flags & ~AlphaDither_Mask ) | DiffuseAlphaDither;
    } else if ( item == ba ) {
	conversion_flags = ( conversion_flags & ~AlphaDither_Mask ) | OrderedAlphaDither;
    } else if ( item == ad ) {
	conversion_flags = ( conversion_flags & ~DitherMode_Mask ) | PreferDither;
    } else if ( item == dd ) {
	conversion_flags = ( conversion_flags & ~DitherMode_Mask ) | AvoidDither;
    }

    if ( ocf != conversion_flags ) {
	setMenuItemFlags();
	// And reconvert...
	reconvertImage();
	repaint(image.hasAlphaBuffer());	// show image in widget
    }
}

/*
  Set the options menu to reflect the conversion_flags value.
*/
void ImageViewer::setMenuItemFlags()
{
    // File
    bool valid_image = pm.size() != TQSize( 0, 0 );
    file->setItemEnabled( si, valid_image );
    file->setItemEnabled( sp, valid_image );

    // Edit
    edit->setItemEnabled( t1, image.depth() != 1 );
    edit->setItemEnabled( t8, image.depth() != 8 );
    edit->setItemEnabled( t32, image.depth() != 32 );

    // Options
    bool may_need_color_dithering =
	    !valid_image
	|| image.depth() == 32 && TQPixmap::defaultDepth() < 24;
    bool may_need_dithering = may_need_color_dithering
	 || image.depth() > 1 && options->isItemChecked(mo)
	 || image.depth() > 1 && TQPixmap::defaultDepth() == 1;
    bool has_alpha_mask = !valid_image || image.hasAlphaBuffer();

    options->setItemEnabled( fd, may_need_dithering );
    options->setItemEnabled( bd, may_need_dithering );
    options->setItemEnabled( td, may_need_dithering );

    options->setItemEnabled( ta, has_alpha_mask );
    options->setItemEnabled( fa, has_alpha_mask );
    options->setItemEnabled( ba, has_alpha_mask );

    options->setItemEnabled( ad, may_need_color_dithering );
    options->setItemEnabled( dd, may_need_color_dithering );

    options->setItemChecked( ac, (conversion_flags & ColorMode_Mask) == AutoColor );
    options->setItemChecked( co, (conversion_flags & ColorMode_Mask) == ColorOnly );
    options->setItemChecked( mo, (conversion_flags & ColorMode_Mask) == MonoOnly );
    options->setItemChecked( fd, (conversion_flags & Dither_Mask) == DiffuseDither );
    options->setItemChecked( bd, (conversion_flags & Dither_Mask) == OrderedDither );
    options->setItemChecked( td, (conversion_flags & Dither_Mask) == ThresholdDither );
    options->setItemChecked( ta, (conversion_flags & AlphaDither_Mask) == ThresholdAlphaDither );
    options->setItemChecked( fa, (conversion_flags & AlphaDither_Mask) == DiffuseAlphaDither );
    options->setItemChecked( ba, (conversion_flags & AlphaDither_Mask) == OrderedAlphaDither );
    options->setItemChecked( ad, (conversion_flags & DitherMode_Mask) == PreferDither );
    options->setItemChecked( dd, (conversion_flags & DitherMode_Mask) == AvoidDither );
}

void ImageViewer::updateStatus()
{
    if ( pm.size() == TQSize( 0, 0 ) ) {
	if ( !filename.isEmpty() )
	    status->setText("Could not load image");
	else
	    status->setText("No image - select Open from File menu.");
    } else {
	TQString message, moremsg;
	message.sprintf("%dx%d", image.width(), image.height());
	if ( pm.size() != pmScaled.size() ) {
	    moremsg.sprintf(" [%dx%d]", pmScaled.width(),
		pmScaled.height());
	    message += moremsg;
	}
	moremsg.sprintf(", %d bits ", image.depth());
	message += moremsg;
	if (image.valid(pickx,picky)) {
	    moremsg.sprintf("(%d,%d)=#%0*x ",
			  pickx, picky,
			  image.hasAlphaBuffer() ? 8 : 6,
			  image.pixel(pickx,picky));
	    message += moremsg;
	}
	if ( image.numColors() > 0 ) {
	    if (image.valid(pickx,picky)) {
		moremsg.sprintf(", %d/%d colors", image.pixelIndex(pickx,picky),
		    image.numColors());
	    } else {
		moremsg.sprintf(", %d colors", image.numColors());
	    }
	    message += moremsg;
	}
	if ( image.hasAlphaBuffer() ) {
	    if ( image.depth() == 8 ) {
		int i;
		bool alpha[256];
		int nalpha=0;

		for (i=0; i<256; i++)
		    alpha[i] = false;

		for (i=0; i<image.numColors(); i++) {
		    int alevel = image.color(i) >> 24;
		    if (!alpha[alevel]) {
			alpha[alevel] = true;
			nalpha++;
		    }
		}
		moremsg.sprintf(", %d alpha levels", nalpha);
	    } else {
		// Too many pixels to bother counting.
		moremsg = ", 8-bit alpha channel";
	    }
	    message += moremsg;
	}
	status->setText(message);
    }
}

/*
  This function saves the image.
*/
void ImageViewer::saveImage( int item )
{
    const char* fmt = saveimage->text(item);
    TQString savefilename = TQFileDialog::getSaveFileName(TQString::null, TQString::null,
					this, filename);
    if ( !savefilename.isEmpty() )
	if ( !image.save( savefilename, fmt ) )
	    TQMessageBox::warning( this, "Save failed", "Error saving file" );
}

/*
  This function saves the converted image.
*/
void ImageViewer::savePixmap( int item )
{
    const char* fmt = savepixmap->text(item);
    TQString savefilename = TQFileDialog::getSaveFileName(TQString::null,
					TQString::null, this, filename);
    if ( !savefilename.isEmpty() )
	if ( !pmScaled.save( savefilename, fmt ) )
	    TQMessageBox::warning( this, "Save failed", "Error saving file" );
}


void ImageViewer::newWindow()
{
    ImageViewer* that = new ImageViewer(0, "new window", WDestructiveClose);
    that->options->setItemChecked( that->cc, useColorContext() );
    that->show();
}

/*
  This function is the slot for processing the Open menu item.
*/
void ImageViewer::openFile()
{
    TQString newfilename = TQFileDialog::getOpenFileName( TQString::null,
							TQString::null,
							this );
    if ( !newfilename.isEmpty() ) {
	loadImage( newfilename ) ;
	repaint();			// show image in widget
    }
}

/*
  This function loads an image from a file and resizes the widget to
  exactly fit the image size. If the file was not found or the image
  format was unknown it will resize the widget to fit the errorText
  message (see above) displayed in the current font.

  Returns true if the image was successfully loaded.
*/

bool ImageViewer::loadImage( const TQString& fileName )
{
    filename = fileName;
    bool ok = false;
    if ( !filename.isEmpty() ) {
	TQApplication::setOverrideCursor( waitCursor ); // this might take time
	ok = image.load(filename, 0);
	pickx = -1;
	clickx = -1;
	if ( ok )
	    ok = reconvertImage();
	if ( ok ) {
	    setCaption( filename );			// set window caption
	    int w = pm.width();
	    int h = pm.height();

	    const int reasonable_width = 128;
	    if ( w < reasonable_width ) {
		// Integer scale up to something reasonable
		int multiply = ( reasonable_width + w - 1 ) / w;
		w *= multiply;
		h *= multiply;
	    }

	    h += menubar->heightForWidth(w) + status->height();
	    resize( w, h );				// we resize to fit image
	} else {
	    pm.resize(0,0);				// couldn't load image
	    update();
	}
	TQApplication::restoreOverrideCursor();	// restore original cursor
    }
    updateStatus();
    setMenuItemFlags();
    return ok;
}

bool ImageViewer::reconvertImage()
{
    bool success = false;

    if ( image.isNull() ) return false;

    if ( alloc_context ) {
	TQColor::destroyAllocContext( alloc_context );
	alloc_context = 0;
    }
    if ( useColorContext() ) {
	alloc_context = TQColor::enterAllocContext();
	// Clear the image to hide flickering palette
	TQPainter painter(this);
	painter.eraseRect(0, menubar->heightForWidth( width() ), width(), height());
    }

    TQApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( pm.convertFromImage(image, conversion_flags) )
    {
	pmScaled = TQPixmap();
	scale();
	resize( width(), height() );
	success = true;				// load successful
    } else {
	pm.resize(0,0);				// couldn't load image
    }
    updateStatus();
    setMenuItemFlags();
    TQApplication::restoreOverrideCursor();	// restore original cursor

    if ( useColorContext() )
	TQColor::leaveAllocContext();

    return success;				// true if loaded OK
}

bool ImageViewer::smooth() const
{
    return options->isItemChecked(ss);
}

bool ImageViewer::useColorContext() const
{
    return options->isItemChecked(cc);
}

/*
  This functions scales the pixmap in the member variable "pm" to fit the
  widget size and  puts the resulting pixmap in the member variable "pmScaled".
*/

void ImageViewer::scale()
{
    int h = height() - menubar->heightForWidth( width() ) - status->height();

    if ( image.isNull() ) return;

    TQApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( width() == pm.width() && h == pm.height() )
    {						// no need to scale if widget
	pmScaled = pm;				// size equals pixmap size
    } else {
	if (smooth()) {
	    pmScaled.convertFromImage(image.smoothScale(width(), h),
		conversion_flags);
	} else {
	    TQWMatrix m;				// transformation matrix
	    m.scale(((double)width())/pm.width(),// define scale factors
		    ((double)h)/pm.height());
	    pmScaled = pm.xForm( m );		// create scaled pixmap
	}
    }
    TQApplication::restoreOverrideCursor();	// restore original cursor
}

/*
  The resize event handler, if a valid pixmap was loaded it will call
  scale() to fit the pixmap to the new widget size.
*/

void ImageViewer::resizeEvent( TQResizeEvent * )
{
    status->setGeometry(0, height() - status->height(),
			width(), status->height());

    if ( pm.size() == TQSize( 0, 0 ) )		// we couldn't load the image
	return;

    int h = height() - menubar->heightForWidth( width() ) - status->height();
    if ( width() != pmScaled.width() || h != pmScaled.height())
    {						// if new size,
	scale();				// scale pmScaled to window
	updateStatus();
    }
    if ( image.hasAlphaBuffer() )
	erase();
}

bool ImageViewer::convertEvent( TQMouseEvent* e, int& x, int& y)
{
    if ( pm.size() != TQSize( 0, 0 ) ) {
	int h = height() - menubar->heightForWidth( width() ) - status->height();
	int nx = e->x() * image.width() / width();
	int ny = (e->y()-menubar->heightForWidth( width() )) * image.height() / h;
	if (nx != x || ny != y ) {
	    x = nx;
	    y = ny;
	    updateStatus();
	    return true;
	}
    }
    return false;
}

void ImageViewer::mousePressEvent( TQMouseEvent *e )
{
    may_be_other = convertEvent(e, clickx, clicky);
}

void ImageViewer::mouseReleaseEvent( TQMouseEvent * )
{
    if ( may_be_other )
	other = this;
}

/*
  Record the pixel position of interest.
*/
void ImageViewer::mouseMoveEvent( TQMouseEvent *e )
{
    if (convertEvent(e,pickx,picky)) {
	updateStatus();
	if ((e->state()&LeftButton)) {
	    may_be_other = false;
	    if ( clickx >= 0 && other) {
		copyFrom(other);
	    }
	}
    }
}

/*
  Draws the portion of the scaled pixmap that needs to be updated or prints
  an error message if no legal pixmap has been loaded.
*/

void ImageViewer::paintEvent( TQPaintEvent *e )
{
    if ( pm.size() != TQSize( 0, 0 ) ) {		// is an image loaded?
	TQPainter painter(this);
	painter.setClipRect(e->rect());
	painter.drawPixmap(0, menubar->heightForWidth( width() ), pmScaled);
    }
}


/*
  Explain anything that might be confusing.
*/
void ImageViewer::giveHelp()
{
    if (!helpmsg) {
	TQString helptext =
	    "<b>Usage:</b> <tt>showimg [-m] <i>filename ...</i></tt>"
	    "<blockquote>"
		"<tt>-m</tt> - use <i>ManyColor</i> color spec"
	    "</blockquote>"
	    "<p>Supported input formats:"
	    "<blockquote>";
	helptext += TQImage::inputFormatList().join(", ");
	helptext += "</blockquote>";

	helpmsg = new TQMessageBox( "Help", helptext,
	    TQMessageBox::Information, TQMessageBox::Ok, 0, 0, 0, 0, false );
    }
    helpmsg->show();
    helpmsg->raise();
}

void ImageViewer::copyFrom(ImageViewer* s)
{
    if ( clickx >= 0 ) {
	int dx = clickx;
	int dy = clicky;
	int sx = s->clickx;
	int sy = s->clicky;
	int sw = TQABS(clickx - pickx)+1;
	int sh = TQABS(clicky - picky)+1;
	if ( clickx > pickx ) {
	    dx = pickx;
	    sx -= sw-1;
	}
	if ( clicky > picky ) {
	    dy = picky;
	    sy -= sh-1;
	}
	bitBlt( &image, dx, dy, &s->image, sx, sy, sw, sh );
	reconvertImage();
	repaint( image.hasAlphaBuffer() );
    }
}
ImageViewer* ImageViewer::other = 0;

void ImageViewer::hFlip()
{
    setImage(image.mirror(true,false));
}

void ImageViewer::vFlip()
{
    setImage(image.mirror(false,true));
}

void ImageViewer::rot180()
{
    setImage(image.mirror(true,true));
}

void ImageViewer::copy()
{
#ifndef TQT_NO_MIMECLIPBOARD
    TQApplication::clipboard()->setImage(image); // Less information loss
#endif
}

void ImageViewer::paste()
{
#ifndef TQT_NO_MIMECLIPBOARD
    TQImage p = TQApplication::clipboard()->image();
    if ( !p.isNull() ) {
	filename = "pasted";
	setImage(p);
    }
#endif
}

void ImageViewer::setImage(const TQImage& newimage)
{
    image = newimage;

    pickx = -1;
    clickx = -1;
    setCaption( filename );			// set window caption
    int w = image.width();
    int h = image.height();
    if ( !w )
	return;

    const int reasonable_width = 128;
    if ( w < reasonable_width ) {
	// Integer scale up to something reasonable
	int multiply = ( reasonable_width + w - 1 ) / w;
	w *= multiply;
	h *= multiply;
    }

    h += menubar->heightForWidth(w) + status->height();
    resize( w, h );				// we resize to fit image

    reconvertImage();
    repaint( image.hasAlphaBuffer() );

    updateStatus();
    setMenuItemFlags();
}

void ImageViewer::editText()
{
    ImageTextEditor editor(image,this);
    editor.exec();
}

void ImageViewer::to1Bit()
{
    toBitDepth(1);
}

void ImageViewer::to8Bit()
{
    toBitDepth(8);
}

void ImageViewer::to32Bit()
{
    toBitDepth(32);
}

void ImageViewer::toBitDepth(int d)
{
    image = image.convertDepth(d);
    reconvertImage();
    repaint( image.hasAlphaBuffer() );
}
