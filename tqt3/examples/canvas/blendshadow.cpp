#include <ntqimage.h>
#include <ntqcolor.h>

static inline int blendComponent( int v, int av, int s, int as )
{
    return as*s + av*v -(av*as*s)/255;  
}
    
static inline TQRgb blendShade( TQRgb v, TQRgb s )
{
    //shadow image is already reduced and blurred
    int as = tqAlpha(s); 
    int av = tqAlpha(v);
    if ( as == 0 || av == 255 )
	return v;

    int a = as + av -(as*av)/255;
    
    int r = blendComponent( tqRed(v),av, tqRed(s), as)/a;
    int g = blendComponent( tqGreen(v),av, tqGreen(s), as)/a;
    int b = blendComponent( tqBlue(v),av, tqBlue(s), as)/a;

    return tqRgba(r,g,b,a);
}

 

int main( int*, char**)
{
    TQImage image( "out.png" );
    image.convertDepth( 32 );
    TQImage shade( "outshade.png" );
    shade.convertDepth( 32 );
    int dx = 10;
    int dy = 5;
	
    int w = image.width();
    int h = image.height();
	
    TQImage img( w+dx, h+dy, 32 );
    img.setAlphaBuffer( true );

    for ( int y = 0; y < h+dy; y++ ) {
	for ( int x = 0; x < w+dx; x++ ) {
	    TQRgb sh =  (x<dx||y<dy) ? 0 : shade.pixel( x-dx, y-dy );
	    TQRgb pixel = (x<w&y<h) ? image.pixel( x, y ) : 0;
	    img.setPixel( x, y, blendShade( pixel, sh ) ); 
	}
    }
    img.save("blend.png", "PNG" );
}


