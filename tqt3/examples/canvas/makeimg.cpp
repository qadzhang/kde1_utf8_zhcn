#include <ntqimage.h>
#include <ntqcolor.h>

static inline int blendComponent( int v, int av, int s, int as )
{
    //shadow gets a color inversely proportional to the
    //alpha value
    s = (s*(255-as))/255;
    //then do standard blending
    return as*s + av*v -(av*as*s)/255;  
}
    
static inline TQRgb blendShade( TQRgb v, TQRgb s )
{
    //pick a number: shadow is 1/3 of object
    int as = tqAlpha(s)/3; 
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
    TQImage *img;

    img = new TQImage( "in.png" );
    int w,h;
    int y;
    img->setAlphaBuffer( true );
    *img = img->convertDepth( 32 );
    w = img->width();
    h = img->height();
#if 0
    for ( y = 0; y < h; y ++ ) {
	uint *line = (uint*)img->scanLine( y );
	for ( int x = 0; x < w; x++ ) {
	    uint pixel = line[x];
	    int r = tqRed(pixel);
	    int g = tqGreen(pixel);
	    int b = tqBlue(pixel);
	    int min = TQMIN( r, TQMIN( g, b ) );
	    int max = TQMAX( r, TQMAX( g, b ) );
	    r -= min;
	    g -= min;
	    b -= min;
	    if ( max !=min ) {
		r = (r*255)/(max-min);
		g = (g*255)/(max-min);
		b = (b*255)/(max-min);
	    }
	    int a = 255-min;
	    a -=  (max-min)/3; //hack more transparency for colors.
	    line[x] = tqRgba( r, g, b, a );
	}
    }
#endif    
    *img = img->smoothScale( w/2, h/2 );

    tqDebug( "saving out.png");
    img->save( "out.png", "PNG" );
    
    w = img->width();
    h = img->height();
    
    TQImage *img2 = new TQImage( w, h, 32 );
    img2->setAlphaBuffer( true );
    for ( y = 0; y < h; y++ ) {
	for ( int x = 0; x < w; x++ ) {
	    TQRgb shader = img->pixel( x, y );

	    int as = tqAlpha(shader)/3;

	    int r = (tqRed(shader)*(255-as))/255;
	    int g = (tqGreen(shader)*(255-as))/255;
	    int b = (tqBlue(shader)*(255-as))/255;

	    img2->setPixel( x, y, tqRgba(r,g,b,as) ); 
	}
    }

    img2->save( "outshade.png", "PNG" );

}
