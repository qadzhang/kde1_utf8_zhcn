/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SHOWIMG_H
#define SHOWIMG_H

#include <ntqwidget.h>
#include <ntqimage.h>


class TQLabel;
class TQMenuBar;
class TQPopupMenu;

class ImageViewer : public TQWidget
{
    TQ_OBJECT
public:
    ImageViewer( TQWidget *parent=0, const char *name=0, int wFlags=0 );
    ~ImageViewer();
    bool	loadImage( const TQString& );
protected:
    void	paintEvent( TQPaintEvent * );
    void	resizeEvent( TQResizeEvent * );
    void	mousePressEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );

private:
    void	scale();
    int		conversion_flags;
    bool	smooth() const;
    bool	useColorContext() const;
    int		alloc_context;
    bool	convertEvent( TQMouseEvent* e, int& x, int& y );
    TQString     filename;
    TQImage	image;			// the loaded image
    TQPixmap	pm;			// the converted pixmap
    TQPixmap	pmScaled;		// the scaled pixmap

    TQMenuBar   *menubar;
    TQPopupMenu  *file;
    TQPopupMenu   *saveimage;
    TQPopupMenu   *savepixmap;
    TQPopupMenu  *edit;
    TQPopupMenu  *options;

    TQWidget    *helpmsg;
    TQLabel     *status;
    int         si, sp, ac, co, mo, fd, bd, // Menu item ids
		td, ta, ba, fa, au, ad, dd,
		ss, cc, t1, t8, t32;
    void	updateStatus();
    void	setMenuItemFlags();
    bool 	reconvertImage();
    int		pickx, picky;
    int		clickx, clicky;
    bool	may_be_other;
    static ImageViewer* other;
    void	setImage(const TQImage& newimage);

private slots:
    void	to1Bit();
    void	to8Bit();
    void	to32Bit();
    void	toBitDepth(int);

    void	copy();
    void	paste();

    void	hFlip();
    void	vFlip();
    void	rot180();

    void	editText();

    void	newWindow();
    void	openFile();
    void	saveImage(int);
    void	savePixmap(int);
    void	giveHelp();
    void	doOption(int);
    void	copyFrom(ImageViewer*);
};


#endif // SHOWIMG_H
