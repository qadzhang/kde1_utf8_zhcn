#include <ntqapplication.h>
#include <ntqwidget.h>
#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqbitmap.h>
#include <ntqfile.h>

#include <stdlib.h>

class MoveMe : public TQWidget
{
public:
    MoveMe( TQWidget *parent=0, const char *name=0, WFlags f = 0)
	:TQWidget(parent,name, f) {}

protected:
    void mousePressEvent( TQMouseEvent *);
    void mouseMoveEvent( TQMouseEvent *);
private:
    TQPoint clickPos;
};

void MoveMe::mousePressEvent( TQMouseEvent *e )
{
    clickPos = e->pos();
}

void MoveMe::mouseMoveEvent( TQMouseEvent *e )
{
    move( e->globalPos() - clickPos );
}



int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    TQString fn="tux.png";

    if ( argc >= 2 )
	fn = argv[1];

    if ( ! TQFile::exists( fn ) )
	exit( 1 );

    TQImage img( fn );
    TQPixmap p;
    p.convertFromImage( img );
    if ( !p.mask() )
	if ( img.hasAlphaBuffer() ) {
	    TQBitmap bm;
	    bm = img.createAlphaMask();
	    p.setMask( bm );
	} else {
	    TQBitmap bm;
	    bm = img.createHeuristicMask();
	    p.setMask( bm );
	}
    MoveMe w(0,0,TQt::WStyle_Customize|TQt::WStyle_NoBorder);
    w.setBackgroundPixmap( p );
    w.setFixedSize( p.size() );
    if ( p.mask() )
	w.setMask( *p.mask() );
    w.show();
    a.setMainWidget(&w);


    return a.exec();

}
