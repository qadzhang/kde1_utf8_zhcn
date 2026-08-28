/****************************************************************************
**
** Implementation of the TQSvgDevice class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the xml module of the TQt GUI Toolkit.
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
*****************************************************************************/

#include <private/qsvgdevice_p.h>

#ifndef TQT_NO_SVG

#include "ntqpainter.h"
#include "ntqpaintdevicemetrics.h"
#include "ntqfile.h"
#include "ntqmap.h"
#include "ntqregexp.h"
#include "ntqvaluelist.h"
#include "ntqtextstream.h"
#include "ntqimage.h"
#include "ntqpixmap.h"

#include <math.h>

const double deg2rad = 0.017453292519943295769;	// pi/180
const char piData[] = "version=\"1.0\" standalone=\"no\"";
const char publicId[] = "-//W3C//DTD SVG 20001102//EN";
const char systemId[] = "http://www.w3.org/TR/2000/CR-SVG-20001102/DTD/svg-20001102.dtd";

struct TQM_EXPORT_SVG ImgElement {
    TQDomElement element;
    TQImage image;
};

struct TQM_EXPORT_SVG PixElement {
    TQDomElement element;
    TQPixmap pixmap;
};

struct TQSvgDeviceState {
    int textx, texty;			// current text position
    int textalign;			// text alignment
};

typedef TQValueList<ImgElement> ImageList;
typedef TQValueList<PixElement> PixmapList;
typedef TQValueList<TQSvgDeviceState> StateList;

class TQSvgDevicePrivate {
public:
    ImageList images;
    PixmapList pixmaps;
    StateList stack;
    int currentClip;

    uint justRestored : 1;

    TQMap<TQString, TQRegion> clipPathTable;
};

enum ElementType {
    InvalidElement = 0,
    AnchorElement,
    CircleElement,
    ClipElement,
    CommentElement,
    DescElement,
    EllipseElement,
    GroupElement,
    ImageElement,
    LineElement,
    PolylineElement,
    PolygonElement,
    PathElement,
    RectElement,
    SvgElement,
    TextElement,
    TitleElement,
    TSpanElement
};

typedef TQMap<TQString,ElementType> TQSvgTypeMap;
static TQSvgTypeMap *qSvgTypeMap=0; // element types
static TQMap<TQString,TQString> *qSvgColMap=0; // recognized color keyword names

/*!
    \class TQSvgDevice qsvgdevice.h
    \brief The TQSvgDevice class provides a paint device for SVG vector graphics.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup xml-tools
    \module XML
    \internal

    SVG is an XML vector graphics format. This class supports the
    loading and saving of SVG files with load() and save(), and the
    rendering of an SVG onto a TQPainter using play(). Use toString()
    to put the SVG into a string.

    \sa TQPaintDevice TQPainter
*/

/*!
    Creates a TQSvgDevice object.
*/

TQSvgDevice::TQSvgDevice()
    : TQPaintDevice( TQInternal::ExternalDevice ),
      pt( 0 )
{
    d = new TQSvgDevicePrivate;
    d->currentClip = 0;
    d->justRestored = false;
}

/*!
    Destroys the TQSvgDevice object and frees the resources it used.
*/

TQSvgDevice::~TQSvgDevice()
{
    delete qSvgTypeMap; qSvgTypeMap = 0;	// static
    delete qSvgColMap; qSvgColMap = 0;
    delete d;
}

/*!
    Loads and parses a SVG from \a dev into the device. Returns true
    on success (i.e. loaded and parsed without error); otherwise
    returns false.
*/

bool TQSvgDevice::load( TQIODevice *dev )
{
    return doc.setContent( dev );
}

/*!
    Renders (replays) the SVG on the \a painter and returns true if
    successful (i.e. it is a valid SVG); otherwise returns false.
*/

bool TQSvgDevice::play( TQPainter *painter )
{
    if ( !painter ) {
#if defined(QT_CHECK_RANGE)
	Q_ASSERT( painter );
#endif
	return false;
    }
    pt = painter;
    pt->setPen( TQt::NoPen ); // SVG default pen and brush
    pt->setBrush( TQt::black );
    if ( doc.isNull() ) {
	tqWarning( "TQSvgDevice::play: No SVG data set." );
	return false;
    }

    TQDomNode svg = doc.namedItem( "svg" );
    if ( svg.isNull() || !svg.isElement() ) {
	tqWarning( "TQSvgDevice::play: Couldn't find any svg element." );
	return false;
    }

    // force transform to be activated in case our sequences
    // are replayed later with a transformed painter
    painter->setWorldXForm( true );

    TQDomNamedNodeMap attr = svg.attributes();
    int x = lenToInt( attr, "x" );
    int y = lenToInt( attr, "y" );
    brect.setX( x );
    brect.setY( y );
    TQString wstr = attr.contains( "width" )
		   ? attr.namedItem( "width" ).nodeValue() : TQString( "100%" );
    TQString hstr = attr.contains( "height" )
		   ? attr.namedItem( "height" ).nodeValue() : TQString( "100%" );
    double width = parseLen( wstr, 0, true );
    double height = parseLen( hstr, 0, false );
    // SVG doesn't respect x and y. But we want a proper bounding rect.
    brect.setWidth( int(width) - x );
    brect.setHeight( int(height) - y );
    painter->setClipRect( brect, TQPainter::CoordPainter );

    if ( attr.contains( "viewBox" ) ) {
	TQRegExp re( TQString::fromLatin1("\\s*(\\S+)\\s*,?\\s*(\\S+)\\s*,?"
					"\\s*(\\S+)\\s*,?\\s*(\\S+)\\s*") );
	if ( re.search( attr.namedItem( "viewBox" ).nodeValue() ) < 0 ) {
	    tqWarning( "TQSvgDevice::play: Invalid viewBox attribute.");
	    return false;
	} else {
	    double x = re.cap( 1 ).toDouble();
	    double y = re.cap( 2 ).toDouble();
	    double w = re.cap( 3 ).toDouble();
	    double h = re.cap( 4 ).toDouble();
	    if ( w < 0 || h < 0 ) {
		tqWarning( "TQSvgDevice::play: Invalid viewBox dimension.");
		return false;
	    } else if ( w == 0 || h == 0 ) {
		return true;
	    }
	    painter->scale( width/w, height/h );
	    painter->translate( -x, -y );
	}
    }

    const struct ElementTable {
	const char *name;
	ElementType type;
    } etab[] = {
	{ "a",        AnchorElement   },
	{ "#comment", CommentElement  },
        { "circle",   CircleElement   },
	{ "clipPath", ClipElement     },
	{ "desc",     DescElement     },
        { "ellipse",  EllipseElement  },
	{ "g",        GroupElement    },
        { "image",    ImageElement    },
        { "line",     LineElement     },
        { "polyline", PolylineElement },
        { "polygon",  PolygonElement  },
        { "path",     PathElement     },
        { "rect",     RectElement     },
        { "svg",      SvgElement      },
        { "text",     TextElement     },
        { "tspan",    TSpanElement    },
	{ "title",    TitleElement    },
	{ 0,          InvalidElement  }
    };
    // initialize only once
    if ( !qSvgTypeMap ) {
	qSvgTypeMap = new TQSvgTypeMap;
	const ElementTable *t = etab;
	while ( t->name ) {
	    qSvgTypeMap->insert( t->name, t->type );
	    t++;
	}
    }

    // initial state
    TQSvgDeviceState st;
    st.textx = st.texty = 0;
    st.textalign = TQt::AlignLeft;
    d->stack.append(st);
    curr = &d->stack.last();
    // 'play' all elements recursively starting with 'svg' as root
    bool b = play( svg );
    d->stack.remove( d->stack.begin() );
    return b;
}

/*!
    Returns the SVG as a single string of XML.
*/
TQString TQSvgDevice::toString() const
{
    if ( doc.isNull() )
	return TQString();

    return doc.toString();
}

/*!
    Saves the SVG to \a fileName.
*/

bool TQSvgDevice::save( const TQString &fileName )
{
    // guess svg id from fileName
    TQString svgName = fileName.endsWith( ".svg" ) ?
		      fileName.left( fileName.length()-4 ) : fileName;

    // now we have the info about name and dimensions available
    TQDomElement root = doc.documentElement();
    root.setAttribute( "id", svgName );
    // the standard doesn't take respect x and y. But we want a
    // proper bounding rect. We make width and height bigger when
    // writing out and subtract x and y when reading in.
    root.setAttribute( "x", brect.x() );
    root.setAttribute( "y", brect.y() );
    root.setAttribute( "width", brect.width() + brect.x() );
    root.setAttribute( "height", brect.height() + brect.y() );

    // ... and know how to name any image files to be written out
    int icount = 0;
    ImageList::Iterator iit = d->images.begin();
    for ( ; iit != d->images.end(); ++iit ) {
	TQString href = TQString( "%1_%2.png" ).arg( svgName ).arg( icount );
	(*iit).image.save( href, "PNG" );
	(*iit).element.setAttribute( "xlink:href", href );
	icount++;
    }
    PixmapList::Iterator pit = d->pixmaps.begin();
    for ( ; pit != d->pixmaps.end(); ++pit ) {
	TQString href = TQString( "%1_%2.png" ).arg( svgName ).arg( icount );
	(*pit).pixmap.save( href, "PNG" );
	(*pit).element.setAttribute( "xlink:href", href );
	icount++;
    }

    TQFile f( fileName );
    if ( !f.open ( IO_WriteOnly ) )
	return false;
    TQTextStream s( &f );
    s.setEncoding( TQTextStream::UnicodeUTF8 );
    s << doc;

    return true;
}

/*!
    \overload

    \a dev is the device to use for saving.
*/

bool TQSvgDevice::save( TQIODevice *dev )
{
#if defined(CHECK_RANGE)
    if ( !d->images.isEmpty() || !d->pixmaps.isEmpty() )
	tqWarning( "TQSvgDevice::save: skipping external images" );
#endif

    TQTextStream s( dev );
    s.setEncoding( TQTextStream::UnicodeUTF8 );
    s << doc;

    return true;
}

/*!
    \fn TQRect TQSvgDevice::boundingRect() const

    Returns the bounding rectangle of the SVG.
*/

/*!
    Sets the bounding rectangle of the SVG to rectangle \a r.
*/

void TQSvgDevice::setBoundingRect( const TQRect &r )
{
    brect = r;
}

/*!
    Internal implementation of the virtual TQPaintDevice::metric()
    function.

    \warning Use the TQPaintDeviceMetrics class instead.

    A TQSvgDevice has the following hard coded values: dpi=72,
    numcolors=16777216 and depth=24. \a m is the metric to get.
*/

int TQSvgDevice::metric( int m ) const
{
    int val;
    switch ( m ) {
    case TQPaintDeviceMetrics::PdmWidth:
	val = brect.width();
	break;
    case TQPaintDeviceMetrics::PdmHeight:
	val = brect.height();
	break;
    case TQPaintDeviceMetrics::PdmWidthMM:
	val = int(25.4/72.0*brect.width());
	break;
    case TQPaintDeviceMetrics::PdmHeightMM:
	val = int(25.4/72.0*brect.height());
	break;
    case TQPaintDeviceMetrics::PdmDpiX:
	val = 72;
	break;
    case TQPaintDeviceMetrics::PdmDpiY:
	val = 72;
	break;
    case TQPaintDeviceMetrics::PdmNumColors:
	val = 16777216;
	break;
    case TQPaintDeviceMetrics::PdmDepth:
	val = 24;
	break;
    default:
	val = 0;
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQSvgDevice::metric: Invalid metric command" );
#endif
    }
    return val;
}

/*!
    \internal

    Records painter commands and stores them in the TQDomDocument doc.
*/

bool TQSvgDevice::cmd ( int c, TQPainter *painter, TQPDevCmdParam *p )
{
    pt = painter;

    if ( c == PdcBegin ) {
	TQDomImplementation domImpl;
	TQDomDocumentType docType = domImpl.createDocumentType( "svg",
							       publicId,
							       systemId );
	doc = domImpl.createDocument( "http://www.w3.org/2000/svg",
				      "svg", docType );
	doc.insertBefore( doc.createProcessingInstruction( "xml", piData ),
			  doc.firstChild() );
	current = doc.documentElement();
	d->images.clear();
	d->pixmaps.clear();
	dirtyTransform = dirtyStyle = false; // ###
	return true;
    } else if ( c == PdcEnd ) {
	return true;
    }

    TQDomElement e;
    TQString str;
    TQRect rect;
    TQPointArray a;
    int i, width, height, x, y;
    switch ( c ) {
    case PdcNOP:
	break;
    case PdcMoveTo:
	curPt = *p[0].point;
	break;
    case PdcLineTo:
	e = doc.createElement( "line" );
	e.setAttribute( "x1", curPt.x() );
	e.setAttribute( "y1", curPt.y() );
	e.setAttribute( "x2", p[0].point->x() );
	e.setAttribute( "y2", p[0].point->y() );
	break;
    case PdcDrawPoint:
    case PdcDrawLine:
	e = doc.createElement( "line" );
	e.setAttribute( "x1", p[0].point->x() );
	e.setAttribute( "y1", p[0].point->y() );
	i = ( c == PdcDrawLine ) ? 1 : 0;
	e.setAttribute( "x2", p[i].point->x() );
	e.setAttribute( "y2", p[i].point->y() );
	break;
    case PdcDrawRect:
    case PdcDrawRoundRect:
	e = doc.createElement( "rect" );
	x = p[0].rect->x();
	y = p[0].rect->y();
	width = p[0].rect->width();
	height = p[0].rect->height();
	if ( width < 0 ) {
	    width = -width;
	    x -= width - 1;
	}
	if ( height < 0 ) {
	    height = -height;
	    y -= height - 1;
	}
	e.setAttribute( "x", x );
	e.setAttribute( "y", y );
	e.setAttribute( "width", width );
	e.setAttribute( "height", height );
	if ( c == PdcDrawRoundRect ) {
	    e.setAttribute( "rx", (p[1].ival*p[0].rect->width())/200 );
	    e.setAttribute( "ry", (p[2].ival*p[0].rect->height())/200 );
	}
	break;
    case PdcDrawEllipse:
	rect = *p[0].rect;
	if ( rect.width() == rect.height() ) {
	    e = doc.createElement( "circle" );
	    double cx = rect.x() + (rect.width() / 2.0);
	    double cy = rect.y() + (rect.height() / 2.0);
	    e.setAttribute( "cx", cx );
	    e.setAttribute( "cy", cy );
	    e.setAttribute( "r", cx - rect.x() );
	} else {
	    e = doc.createElement( "ellipse" );
    	    double cx = rect.x() + (rect.width() / 2.0);
	    double cy = rect.y() + (rect.height() / 2.0);
	    e.setAttribute( "cx", cx );
	    e.setAttribute( "cy", cy );
	    e.setAttribute( "rx", cx - rect.x() );
	    e.setAttribute( "ry", cy - rect.y() );
	}
	break;
    case PdcDrawArc:
    case PdcDrawPie:
    case PdcDrawChord: {
	rect = *p[0].rect;
	double a = (double)p[1].ival / 16.0 * deg2rad;
	double al = (double)p[2].ival / 16.0 * deg2rad;
	double rx = rect.width() / 2.0;
	double ry = rect.height() / 2.0;
	double x0 = (double)rect.x() + rx;
	double y0 = (double)rect.y() + ry;
	double x1 = x0 + rx*cos(a);
	double y1 = y0 - ry*sin(a);
	double x2 = x0 + rx*cos(a+al);
	double y2 = y0 - ry*sin(a+al);
	int large = TQABS( al ) > ( 180.0 * deg2rad ) ? 1 : 0;
	int sweep = al < 0.0 ? 1 : 0;
	if ( c == PdcDrawPie )
	    str = TQString( "M %1 %2 L %3 %4 " ).arg( x0 ).arg( y0 )
		  .arg( x1 ).arg( y1 );
	else
	    str = TQString( "M %1 %2 " ).arg( x1 ).arg( y1 );
	str += TQString( "A %1 %2 %3 %4 %5 %6 %7" )
	       .arg( rx ).arg( ry ).arg( a/deg2rad ). arg( large ).arg( sweep )
	       .arg( x2 ).arg( y2 );
	if ( c != PdcDrawArc )
	    str += "z";
	e = doc.createElement( "path" );
	e.setAttribute( "d", str );
    }
	break;
    case PdcDrawLineSegments:
	{
	    a = *p[0].ptarr;
	    uint end = a.size() / 2;
	    for (uint i = 0; i < end; i++) {
		e = doc.createElement( "line" );
		e.setAttribute( "x1", a[int(2*i)].x() );
		e.setAttribute( "y1", a[int(2*i)].y() );
		e.setAttribute( "x2", a[int(2*i+1)].x() );
		e.setAttribute( "y2", a[int(2*i+1)].y() );
		if ( i < end - 1 ) // The last one will be done at the end
		    appendChild( e, c );
	    }
	}
	break;
    case PdcDrawPolyline:
    case PdcDrawPolygon:
	{
	    a = *p[0].ptarr;
	    e = doc.createElement( ( c == PdcDrawPolyline ) ?
				   "polyline" : "polygon" );
	    for (uint i = 0; i < a.size(); i++) {
		TQString tmp;
		tmp.sprintf( "%d %d ", a[ (int)i ].x(), a[ (int)i ].y() );
		str += tmp;
	    }
	    e.setAttribute( "points", str.stripWhiteSpace() );
	}
	break;
#ifndef TQT_NO_BEZIER
    case PdcDrawCubicBezier:
	a = *p[0].ptarr;
	e = doc.createElement( "path" );
	str.sprintf( "M %d %d C %d %d %d %d %d %d", a[0].x(), a[0].y(),
		     a[1].x(), a[1].y(), a[2].x(), a[2].y(),
		     a[3].x(), a[3].y() );
	e.setAttribute( "d", str );
	break;
#endif
    case PdcDrawText2:
	e = doc.createElement( "text" );
	if ( p[0].point->x() )
	    e.setAttribute( "x", p[0].point->x() );
	if ( p[0].point->y() )
	    e.setAttribute( "y", p[0].point->y() );
	e.appendChild( doc.createTextNode( *p[1].str ) );
	break;
    case PdcDrawText2Formatted: {
	e = doc.createElement( "text" );
	const TQRect *r = p[0].rect;
	int tf = p[1].ival;
	int x, y;
	// horizontal text alignment
	if ( ( tf & TQt::AlignHCenter ) != 0 ) {
	    x = r->x() + r->width() / 2;
	    e.setAttribute( "text-anchor", "middle" );
	} else if ( ( tf & TQt::AlignRight ) != 0 ) {
	    x = r->right();
	    e.setAttribute( "text-anchor", "end" );
	} else {
	    x = r->x();
	}
	// vertical text alignment
	if ( ( tf & TQt::AlignVCenter ) != 0 )
	    y = r->y() + ( r->height() + painter->fontMetrics().ascent() ) / 2;
	else if ( ( tf & TQt::AlignBottom ) != 0 )
	    y = r->bottom();
	else
	    y = r->y() + painter->fontMetrics().ascent();
	if ( x )
	    e.setAttribute( "x", x );
	if ( y )
	    e.setAttribute( "y", y );
	e.appendChild( doc.createTextNode( *p[2].str ) );
    }
	break;
    case PdcDrawPixmap:
    case PdcDrawImage:
	e = doc.createElement( "image" );
	e.setAttribute( "x", p[0].rect->x() );
	e.setAttribute( "y", p[0].rect->y() );
	e.setAttribute( "width", p[0].rect->width() );
	e.setAttribute( "height", p[0].rect->height() );
	if ( c == PdcDrawImage ) {
	    ImgElement ie;
	    ie.element = e;
	    ie.image = *p[1].image;
	    d->images.append( ie );
	} else {
	    PixElement pe;
	    pe.element = e;
	    pe.pixmap = *p[1].pixmap;
	    d->pixmaps.append( pe );
	}
	// saving to disk and setting the xlink:href attribute will be
	// done later in save() once we now the svg document name.
	break;
    case PdcSave:
	e = doc.createElement( "g" );
	break;
    case PdcRestore:
	current = current.parentNode();
	dirtyTransform = !pt->worldMatrix().isIdentity();
        d->justRestored = true;
	// ### reset dirty flags
	break;
    case PdcSetBkColor:
    case PdcSetBkMode:
    case PdcSetROP:
    case PdcSetBrushOrigin:
    case PdcSetFont:
    case PdcSetPen:
    case PdcSetBrush:
	dirtyStyle = true;
	break;
    case PdcSetTabStops:
	// ###
	break;
    case PdcSetTabArray:
	// ###
	break;
    case PdcSetVXform:
    case PdcSetWindow:
    case PdcSetViewport:
    case PdcSetWXform:
    case PdcSetWMatrix:
    case PdcSaveWMatrix:
    case PdcRestoreWMatrix:
	dirtyTransform = true;
	break;
    case PdcSetClip:
	// ###
	break;
    case PdcSetClipRegion:
	{
            // We skip the clip after restore, since restoring the clip is done automatically by
            // the viewer as part of the tree structure. It doesn't hurt to write the region
            // out, but it doubles the number of clipregions defined in the final svg.
            if (d->justRestored) {
                d->justRestored = false;
                return true;
            }

            TQMemArray<TQRect> rects = p[0].rgn->rects();
            if (rects.count() == 0)
                return true;
	    d->currentClip++;
	    e = doc.createElement( "clipPath" );
	    e.setAttribute( "id", TQString("clip%1").arg(d->currentClip) );
            for (int i=0; i<(int)rects.count(); ++i) {
                TQDomElement ce = doc.createElement("rect");
                ce.setAttribute( "x", rects.at(i).x() );
                ce.setAttribute( "y", rects.at(i).y() );
                ce.setAttribute( "width", rects.at(i).width() );
                ce.setAttribute( "height", rects.at(i).height() );
                e.appendChild(ce);
	    }
	    break;
	}
    default:
#if defined(CHECK_RANGE)
	tqWarning( "TQSVGDevice::cmd: Invalid command %d", c );
#endif
	break;
    }

    appendChild( e, c );

    return true;
}

/*!
    \internal

    Appends the child and applys any style and transformation.

*/

void TQSvgDevice::appendChild( TQDomElement &e, int c )
{
    if ( !e.isNull() ) {
	current.appendChild( e );
	if ( c == PdcSave )
	    current = e;
	// ### optimize application of attributes utilizing <g>
	if ( c == PdcSetClipRegion ) {
	    TQDomElement ne;
	    ne = doc.createElement( "g" );
	    ne.setAttribute( "style", TQString("clip-path:url(#clip%1)").arg(d->currentClip) );
	    current.appendChild( ne );
	    current = ne;
	} else {
	    if ( dirtyStyle )		// only reset when entering
		applyStyle( &e, c );	// or leaving a <g> tag
	    if ( dirtyTransform && e.tagName() != "g" ) {
		// same as above but not for <g> tags
		applyTransform( &e );
		if ( c == PdcSave )
		    dirtyTransform = false;
	    }
	}
    }
}


/*!
    \internal

    Push the current drawing attributes on a stack.

    \sa restoreAttributes()
*/

void TQSvgDevice::saveAttributes()
{
    pt->save();
    // copy old state
    TQSvgDeviceState st( *curr );
    d->stack.append( st );
    curr = &d->stack.last();
}

/*!
    \internal

    Pop the current drawing attributes off the stack.

    \sa saveAttributes()
*/

void TQSvgDevice::restoreAttributes()
{
    pt->restore();
    Q_ASSERT( d->stack.count() > 1 );
    d->stack.remove( d->stack.fromLast() );
    curr = &d->stack.last();
}

/*!
    \internal

    Evaluate \a node, drawing on \a p. Allows recursive calls.
*/

bool TQSvgDevice::play( const TQDomNode &node )
{
    saveAttributes();

	ElementType t = (*qSvgTypeMap)[ node.nodeName() ];

	if ( t == LineElement && pt->pen().style() == TQt::NoPen ) {
	    TQPen p = pt->pen();
	    p.setStyle( TQt::SolidLine );
	    pt->setPen( p );
	}
	TQDomNamedNodeMap attr = node.attributes();
	if ( attr.contains( "style" ) )
	    setStyle( attr.namedItem( "style" ).nodeValue() );
	// ### might have to exclude more elements from transform
	if ( t != SvgElement && attr.contains( "transform" ) )
	    setTransform( attr.namedItem( "transform" ).nodeValue() );
	uint i = attr.length();
	if ( i > 0 ) {
	    TQPen pen = pt->pen();
	    TQFont font = pt->font();
	    while ( i-- ) {
		TQDomNode n = attr.item( i );
		TQString a = n.nodeName();
		TQString val = n.nodeValue().lower().stripWhiteSpace();
		setStyleProperty( a, val, &pen, &font, &curr->textalign );
	    }
	    pt->setPen( pen );
	    pt->setFont( font );
	}

	int x1, y1, x2, y2, rx, ry, w, h;
	double cx1, cy1, crx, cry;
	switch ( t ) {
	case CommentElement:
	    // ignore
	    break;
	case RectElement:
	    rx = ry = 0;
	    x1 = lenToInt( attr, "x" );
	    y1 = lenToInt( attr, "y" );
	    w = lenToInt( attr, "width" );
	    h = lenToInt( attr, "height" );
	    if ( w == 0 || h == 0 )	// prevent div by zero below
		break;
	    x2 = (int)attr.contains( "rx" ); // tiny abuse of x2 and y2
	    y2 = (int)attr.contains( "ry" );
	    if ( x2 )
		rx = lenToInt( attr, "rx" );
	    if ( y2 )
		ry = lenToInt( attr, "ry" );
	    if ( x2 && !y2 )
		ry = rx;
	    else if ( !x2 && y2 )
		rx = ry;
	    rx = int(200.0*double(rx)/double(w));
	    ry = int(200.0*double(ry)/double(h));
	    pt->drawRoundRect( x1, y1, w, h, rx, ry );
	    break;
	case CircleElement:
	    cx1 = lenToDouble( attr, "cx" ) + 0.5;
	    cy1 = lenToDouble( attr, "cy" ) + 0.5;
	    crx = lenToDouble( attr, "r" );
	    pt->drawEllipse( (int)(cx1-crx), (int)(cy1-crx), (int)(2*crx), (int)(2*crx) );
	    break;
	case EllipseElement:
	    cx1 = lenToDouble( attr, "cx" ) + 0.5;
	    cy1 = lenToDouble( attr, "cy" ) + 0.5;
	    crx = lenToDouble( attr, "rx" );
	    cry = lenToDouble( attr, "ry" );
	    pt->drawEllipse( (int)(cx1-crx), (int)(cy1-cry), (int)(2*crx), (int)(2*cry) );
	    break;
	case LineElement:
	    {
		x1 = lenToInt( attr, "x1" );
		x2 = lenToInt( attr, "x2" );
		y1 = lenToInt( attr, "y1" );
		y2 = lenToInt( attr, "y2" );
		TQPen p = pt->pen();
		w = p.width();
		p.setWidth( (unsigned int)(w * (TQABS(pt->worldMatrix().m11()) + TQABS(pt->worldMatrix().m22())) / 2) );
		pt->setPen( p );
		pt->drawLine( x1, y1, x2, y2 );
		p.setWidth( w );
		pt->setPen( p );
	    }
	    break;
	case PolylineElement:
	case PolygonElement:
	    {
		TQString pts = attr.namedItem( "points" ).nodeValue();
		pts = pts.simplifyWhiteSpace();
		TQStringList sl = TQStringList::split( TQRegExp( TQString::fromLatin1("[ ,]") ), pts );
		TQPointArray ptarr( (uint)sl.count() / 2);
		for ( int i = 0; i < (int)sl.count() / 2; i++ ) {
		    double dx = sl[2*i].toDouble();
		    double dy = sl[2*i+1].toDouble();
		    ptarr.setPoint( i, int(dx), int(dy) );
		}
  		if ( t == PolylineElement ) {
		    if ( pt->brush().style() != TQt::NoBrush ) {
			TQPen pn = pt->pen();
			pt->setPen( TQt::NoPen );
			pt->drawPolygon( ptarr );
			pt->setPen( pn );
		    }
		    pt->drawPolyline( ptarr ); // ### closes when filled. bug ?
  		} else {
		    pt->drawPolygon( ptarr );
		}
	    }
	    break;
	case SvgElement:
	case GroupElement:
	case AnchorElement:
	    {
		TQDomNode child = node.firstChild();
		while ( !child.isNull() ) {
		    play( child );
		    child = child.nextSibling();
		}
	    }
	    break;
	case PathElement:
	    drawPath( attr.namedItem( "d" ).nodeValue() );
	    break;
	case TSpanElement:
	case TextElement:
	    {
		if ( attr.contains( "x" ) )
		     curr->textx = lenToInt( attr, "x" );
		if ( attr.contains( "y" ) )
		     curr->texty = lenToInt( attr, "y" );
		if ( t == TSpanElement ) {
		    curr->textx += lenToInt( attr, "dx" );
		    curr->texty += lenToInt( attr, "dy" );
		}
		// backup old colors
		TQPen pn = pt->pen();
		TQColor pcolor = pn.color();
		TQColor bcolor = pt->brush().color();
		TQDomNode c = node.firstChild();
		while ( !c.isNull() ) {
		    if ( c.isText() ) {
			// we have pen and brush reversed for text drawing
			pn.setColor( bcolor );
			pt->setPen( pn );
			TQString text = c.toText().nodeValue();
			text = text.simplifyWhiteSpace(); // ### 'preserve'
			w = pt->fontMetrics().width( text );
			if ( curr->textalign == TQt::AlignHCenter )
			    curr->textx -= w / 2;
			else if ( curr->textalign == TQt::AlignRight )
			    curr->textx -= w;
			pt->drawText( curr->textx, curr->texty, text );
			// restore pen
			pn.setColor( pcolor );
			pt->setPen( pn );
			curr->textx += w;
		    } else if ( c.isElement() &&
				c.toElement().tagName() == "tspan" ) {
			play( c );

		    }
		    c = c.nextSibling();
		}
		if ( t == TSpanElement ) {
		    // move current text position in parent text element
		    StateList::Iterator it = --d->stack.fromLast();
		    (*it).textx = curr->textx;
		    (*it).texty = curr->texty;
		}
	    }
	    break;
	case ImageElement:
	    {
		x1 = lenToInt( attr, "x" );
		y1 = lenToInt( attr, "y" );
		w = lenToInt( attr, "width" );
		h = lenToInt( attr, "height" );
		TQString href = attr.namedItem( "xlink:href" ).nodeValue();
		// ### catch references to embedded .svg files
		TQPixmap pix;
		if ( !pix.load( href ) ) {
		    tqWarning( "TQSvgDevice::play: Couldn't load image %s", href.latin1() );
		    break;
		}
		pt->drawPixmap( TQRect( x1, y1, w, h ), pix );
	    }
	    break;
	case DescElement:
	case TitleElement:
	    // ignored for now
	    break;
	case ClipElement:
	    {
		TQDomNode child = node.firstChild();
                TQRegion region;
                while (!child.isNull()) {
                    TQDomNamedNodeMap childAttr = child.attributes();
                    if ( child.nodeName() == "rect" ) {
                        TQRect r;
                        r.setX(lenToInt( childAttr, "x" ));
                        r.setY(lenToInt( childAttr, "y" ));
                        r.setWidth(lenToInt( childAttr, "width" ));
                        r.setHeight(lenToInt( childAttr, "height" ));
                        region |= r;
                    } else if ( child.nodeName() == "ellipse" ) {
                        TQRect r;
                        int x = lenToInt( childAttr, "cx" );
                        int y = lenToInt( childAttr, "cy" );
                        int width = lenToInt( childAttr, "rx" );
                        int height = lenToInt( childAttr, "ry" );
                        r.setX( x - width );
                        r.setY( y - height );
                        r.setWidth( width * 2 );
                        r.setHeight( height * 2 );
                        TQRegion rgn( r, TQRegion::Ellipse );
                        region |= rgn;
                    }
                    child = child.nextSibling();
                }
                // Store the region in a named map so that it can be used when the
                // group node is entered.
                TQString idString = attr.namedItem("id").nodeValue();
                if (!idString.isEmpty())
                    d->clipPathTable[idString] = region;
                break;
	    }
	case InvalidElement:
	    tqWarning( "TQSvgDevice::play: unknown element type %s",
		      node.nodeName().latin1() );
	    break;
	};

	restoreAttributes();

    return true;
}

/*!
    \internal

    Parses a CSS2-compatible color specification. Either a keyword or
    a numerical RGB specification like #ff00ff or rgb(255,0,50%).
*/

TQColor TQSvgDevice::parseColor( const TQString &col )
{
    static const struct ColorTable {
	const char *name;
	const char *rgb;
    } coltab[] = {
	{ "black",   "#000000" },
	{ "silver",  "#c0c0c0" },
	{ "gray",    "#808080" },
	{ "white",   "#ffffff" },
	{ "maroon",  "#800000" },
	{ "red",     "#ff0000" },
	{ "purple",  "#800080" },
	{ "fuchsia", "#ff00ff" },
	{ "green",   "#008000" },
	{ "lime",    "#00ff00" },
	{ "olive",   "#808000" },
	{ "yellow",  "#ffff00" },
	{ "navy",    "#000080" },
	{ "blue",    "#0000ff" },
	{ "teal",    "#008080" },
	{ "aqua",    "#00ffff" },
	// ### the latest spec has more
	{ 0,         0         }
    };

    // initialize color map on first use
    if ( !qSvgColMap ) {
	qSvgColMap = new TQMap<TQString, TQString>;
	const struct ColorTable *t = coltab;
	while ( t->name ) {
	    qSvgColMap->insert( t->name, t->rgb );
	    t++;
	}
    }

    // a keyword ?
    if ( qSvgColMap->contains ( col ) )
	return TQColor( (*qSvgColMap)[ col ] );
    // in rgb(r,g,b) form ?
    TQString c = col;
    c.replace( TQRegExp( TQString::fromLatin1("\\s*") ), "" );
    TQRegExp reg( TQString::fromLatin1("^rgb\\((\\d+)(%?),(\\d+)(%?),(\\d+)(%?)\\)$") );
    if ( reg.search( c ) >= 0 ) {
	int comp[3];
	for ( int i = 0; i < 3; i++ ) {
	    comp[ i ] = reg.cap( 2*i+1 ).toInt();
	    if ( !reg.cap( 2*i+2 ).isEmpty() )		// percentage ?
		comp[ i ] = int((double(255*comp[ i ])/100.0));
	}
	return TQColor( comp[ 0 ], comp[ 1 ], comp[ 2 ] );
    }

    // check for predefined TQt color objects, #RRGGBB and #RGB
    return TQColor( col );
}

/*!
    \internal

    Parse a <length> datatype consisting of a number followed by an
    optional unit specifier. Can be used for type <coordinate> as
    well. For relative units the value of \a horiz will determine
    whether the horizontal or vertical dimension will be used.
*/

double TQSvgDevice::parseLen( const TQString &str, bool *ok, bool horiz ) const
{
    TQRegExp reg( TQString::fromLatin1("([+-]?\\d*\\.*\\d*[Ee]?[+-]?\\d*)(em|ex|px|%|pt|pc|cm|mm|in|)$") );
    if ( reg.search( str ) == -1 ) {
	tqWarning( "TQSvgDevice::parseLen: couldn't parse %s ", str.latin1() );
	if ( ok )
	    *ok = false;
	return 0.0;
    }

    double dbl = reg.cap( 1 ).toDouble();
    TQString u = reg.cap( 2 );
    if ( !u.isEmpty() && u != "px" ) {
	TQPaintDeviceMetrics m( pt->device() );
	if ( u == "em" ) {
	    TQFontInfo fi( pt->font() );
	    dbl *= fi.pixelSize();
	} else if ( u == "ex" ) {
	    TQFontInfo fi( pt->font() );
	    dbl *= 0.5 * fi.pixelSize();
	} else if ( u == "%" )
	    dbl *= (horiz ? pt->window().width() : pt->window().height())/100.0;
	else if ( u == "cm" )
	    dbl *= m.logicalDpiX() / 2.54;
	else if ( u == "mm" )
	    dbl *= m.logicalDpiX() / 25.4;
	else if ( u == "in" )
	    dbl *= m.logicalDpiX();
	else if ( u == "pt" )
	    dbl *= m.logicalDpiX() / 72.0;
	else if ( u == "pc" )
	    dbl *= m.logicalDpiX() / 6.0;
	else
	    tqWarning( "TQSvgDevice::parseLen: Unknown unit %s",  u.latin1() );
    }
    if ( ok )
	*ok = true;
    return dbl;
}

/*!
    \internal

    Returns the length specified in attribute \a attr in \a map. If
    the specified attribute doesn't exist or can't be parsed \a def is
    returned.
*/

int TQSvgDevice::lenToInt( const TQDomNamedNodeMap &map, const TQString &attr,
			  int def ) const
{
    if ( map.contains( attr ) ) {
	bool ok;
	double dbl = parseLen( map.namedItem( attr ).nodeValue(), &ok );
	if ( ok )
	    return tqRound( dbl );
    }
    return def;
}

double TQSvgDevice::lenToDouble( const TQDomNamedNodeMap &map, const TQString &attr,
				int def ) const
{
    if ( map.contains( attr ) ) {
	bool ok;
	double d = parseLen( map.namedItem( attr ).nodeValue(), &ok );
	if ( ok )
	    return d;
    }
    return def;
}

void TQSvgDevice::setStyleProperty( const TQString &prop, const TQString &val,
				   TQPen *pen, TQFont *font, int *talign )
{
    if ( prop == "stroke" ) {
	if ( val == "none" ) {
	    pen->setStyle( TQt::NoPen );
	} else {
	    pen->setColor( parseColor( val ));
	    if ( pen->style() == TQt::NoPen )
		pen->setStyle( TQt::SolidLine );
	    if ( pen->width() == 0 )
		pen->setWidth( 1 );
	}
    } else if ( prop == "stroke-width" ) {
	double w = parseLen( val );
	if ( w > 0.0001 )
	    pen->setWidth( int(w) );
	else
	    pen->setStyle( TQt::NoPen );
    } else if ( prop == "stroke-linecap" ) {
	if ( val == "butt" )
	    pen->setCapStyle( TQt::FlatCap );
	else if ( val == "round" )
	    pen->setCapStyle( TQt::RoundCap );
	else if ( val == "square" )
	    pen->setCapStyle( TQt::SquareCap );
    } else if ( prop == "stroke-linejoin" ) {
	if ( val == "miter" )
	    pen->setJoinStyle( TQt::MiterJoin );
	else if ( val == "round" )
	    pen->setJoinStyle( TQt::RoundJoin );
	else if ( val == "bevel" )
	    pen->setJoinStyle( TQt::BevelJoin );
    } else if ( prop == "stroke-dasharray" ) {
        if ( val == "18,6" )
	    pen->setStyle( TQt::DashLine );
	else if ( val == "3" )
	    pen->setStyle( TQt::DotLine );
	else if ( val == "9,6,3,6" )
	    pen->setStyle( TQt::DashDotLine );
	else if ( val == "9,3,3" )
	    pen->setStyle( TQt::DashDotDotLine );
        else
            pen->setStyle( TQt::DotLine );
    } else if ( prop == "fill" ) {
	if ( val == "none" )
	    pt->setBrush( TQt::NoBrush );
	else
	    pt->setBrush( parseColor( val ) );
    } else if ( prop == "font-size" ) {
	font->setPointSizeFloat( float(parseLen( val )) );
    } else if ( prop == "font-family" ) {
	font->setFamily( val );
    } else if ( prop == "font-style" ) {
	if ( val == "normal" )
	    font->setItalic( false );
	else if ( val == "italic" )
	    font->setItalic( true );
	else
	    tqWarning( "TQSvgDevice::setStyleProperty: unhandled "
		      "font-style: %s", val.latin1() );
    } else if ( prop == "font-weight" ) {
	int w = font->weight();
	// no exact equivalents so we have to "round" a little bit
	if ( val == "100" || val == "200" )
	    w = TQFont::Light;
	if ( val == "300" || val == "400" || val == "normal" )
	    w = TQFont::Normal;
	else if ( val == "500" || val == "600" )
	    w = TQFont::DemiBold;
	else if ( val == "700" || val == "bold" || val == "800" )
	    w = TQFont::Bold;
	else if ( val == "900" )
	    w = TQFont::Black;
	font->setWeight( w );
    } else if ( prop == "text-anchor" ) {
	if ( val == "middle" )
	    *talign = TQt::AlignHCenter;
	else if ( val == "end" )
	    *talign = TQt::AlignRight;
	else
	    *talign = TQt::AlignLeft;
    } else if ( prop == "clip-path" ) {
        if (val.startsWith("url(#")) {
            TQString clipName = val.mid(5, val.length() - 6);
            if (!clipName.isEmpty()) {
                TQRegion clipRegion = d->clipPathTable[clipName];
                if (!clipRegion.isEmpty())
                    pt->setClipRegion(pt->clipRegion() & clipRegion, TQPainter::CoordPainter);
            }
        }
    }
}

void TQSvgDevice::setStyle( const TQString &s )
{
    TQStringList rules = TQStringList::split( TQChar(';'), s );

    TQPen pen = pt->pen();
    TQFont font = pt->font();

    TQStringList::ConstIterator it = rules.begin();
    for ( ; it != rules.end(); it++ ) {
	int col = (*it).find( ':' );
	if ( col > 0 ) {
	    TQString prop = (*it).left( col ).simplifyWhiteSpace();
	    TQString val = (*it).right( (*it).length() - col - 1 );
	    val = val.lower().stripWhiteSpace();
	    setStyleProperty( prop, val, &pen, &font, &curr->textalign );
	}
    }

    pt->setPen( pen );
    pt->setFont( font );
}

void TQSvgDevice::setTransform( const TQString &tr )
{
    TQString t = tr.simplifyWhiteSpace();

    TQRegExp reg( TQString::fromLatin1("\\s*([\\w]+)\\s*\\(([^\\(]*)\\)") );
    int index = 0;
    while ( (index = reg.search(t, index)) >= 0 ) {
	TQString command = reg.cap( 1 );
	TQString params = reg.cap( 2 );
	TQStringList plist = TQStringList::split( TQRegExp(TQString::fromLatin1("[,\\s]")), params );
	if ( command == "translate" ) {
	    double tx = 0, ty = 0;
	    tx = plist[0].toDouble();
	    if ( plist.count() >= 2 )
		ty = plist[1].toDouble();
	    pt->translate( tx, ty );
	} else if ( command == "rotate" ) {
	    pt->rotate( plist[0].toDouble() );
	} else if ( command == "scale" ) {
	    double sx, sy;
	    sx = sy = plist[0].toDouble();
	    if ( plist.count() >= 2 )
		sy = plist[1].toDouble();
	    pt->scale( sx, sy );
	} else if ( command == "matrix" && plist.count() >= 6 ) {
	    double m[ 6 ];
	    for (int i = 0; i < 6; i++)
		m[ i ] = plist[ i ].toDouble();
	    TQWMatrix wm( m[ 0 ], m[ 1 ], m[ 2 ],
			 m[ 3 ], m[ 4 ], m[ 5 ] );
	    pt->setWorldMatrix( wm, true );
	} else if ( command == "skewX" ) {
	    pt->shear( 0.0, tan( plist[0].toDouble() * deg2rad ) );
	} else if ( command == "skewY" ) {
	    pt->shear( tan( plist[0].toDouble() * deg2rad ), 0.0 );
	}

	// move on to next command
	index += reg.matchedLength();
    }
}

void TQSvgDevice::drawPath( const TQString &data )
{
    double x0 = 0, y0 = 0;		// starting point
    double x = 0, y = 0;		// current point
    double controlX = 0, controlY = 0;	// last control point for curves
    TQPointArray path( 500 );		// resulting path
    TQValueList<int> subIndex;		// start indices for subpaths
    TQPointArray quad( 4 ), bezier;	// for curve calculations
    int pcount = 0;			// current point array index
    uint idx = 0;			// current data position
    int mode = 0, lastMode = 0;		// parser state
    bool relative = false;		// e.g. 'h' vs. 'H'
    TQString commands( "MZLHVCSTQTA" );	// recognized commands
    int cmdArgs[] = { 2, 0, 2, 1, 1, 6, 4, 4, 2, 7 };	// no of arguments
    TQRegExp reg( TQString::fromLatin1("\\s*,?\\s*([+-]?\\d*\\.?\\d*)") );	// floating point

    subIndex.append( 0 );
    // detect next command
    while ( idx < data.length() ) {
	TQChar ch = data[ (int)idx++ ];
	if ( ch.isSpace() )
	    continue;
	TQChar chUp = ch.upper();
	int cmd = commands.find( chUp );
	if ( cmd >= 0 ) {
	    // switch to new command mode
	    mode = cmd;
	    relative = ( ch != chUp );		// e.g. 'm' instead of 'M'
	} else {
	    if ( mode && !ch.isLetter() ) {
		cmd = mode;			// continue in previous mode
		idx--;
	    } else {
		tqWarning( "TQSvgDevice::drawPath: Unknown command" );
		return;
	    }
	}

	// read in the required number of arguments
	const int maxArgs = 7;
	double arg[ maxArgs ];
	int numArgs = cmdArgs[ cmd ];
	for ( int i = 0; i < numArgs; i++ ) {
	    int pos = reg.search( data, idx );
	    if ( pos == -1 ) {
		tqWarning( "TQSvgDevice::drawPath: Error parsing arguments" );
		return;
	    }
	    arg[ i ] = reg.cap( 1 ).toDouble();
	    idx = pos + reg.matchedLength();
	};

	// process command
	double offsetX = relative ? x : 0;	// correction offsets
	double offsetY = relative ? y : 0;	// for relative commands
	switch ( mode ) {
	case 0:					// 'M' move to
	    if ( x != x0 || y != y0 )
		path.setPoint( pcount++, int(x0), int(y0) );
	    x = x0 = arg[ 0 ] + offsetX;
	    y = y0 = arg[ 1 ] + offsetY;
	    subIndex.append( pcount );
	    path.setPoint( pcount++, int(x0), int(y0) );
	    mode = 2;				// -> 'L'
	    break;
	case 1:					// 'Z' close path
	    path.setPoint( pcount++, int(x0), int(y0) );
	    x = x0;
	    y = y0;
	    mode = 0;
	    break;
	case 2:					// 'L' line to
	    x = arg[ 0 ] + offsetX;
	    y = arg[ 1 ] + offsetY;
	    path.setPoint( pcount++, int(x), int(y) );
	    break;
	case 3:					// 'H' horizontal line
	    x = arg[ 0 ] + offsetX;
	    path.setPoint( pcount++, int(x), int(y) );
	    break;
	case 4:					// 'V' vertical line
	    y = arg[ 0 ] + offsetY;
	    path.setPoint( pcount++, int(x), int(y) );
	    break;
#ifndef TQT_NO_BEZIER
	case 5:					// 'C' cubic bezier curveto
	case 6:					// 'S' smooth shorthand
	case 7:					// 'Q' quadratic bezier curves
	case 8: {				// 'T' smooth shorthand
	    quad.setPoint( 0, int(x), int(y) );
	    // if possible, reflect last control point if smooth shorthand
	    if ( mode == 6 || mode == 8 ) { 	// smooth 'S' and 'T'
		bool cont = mode == lastMode ||
		     ( mode == 6 && lastMode == 5 ) || 	// 'S' and 'C'
		     ( mode == 8 && lastMode == 7 );	// 'T' and 'Q'
		x = cont ? 2*x-controlX : x;
		y = cont ? 2*y-controlY : y;
		quad.setPoint( 1, int(x), int(y) );
		quad.setPoint( 2, int(x), int(y) );
	    }
	    for ( int j = 0; j < numArgs/2; j++ ) {
		x = arg[ 2*j   ] + offsetX;
		y = arg[ 2*j+1 ] + offsetY;
		quad.setPoint( j+4-numArgs/2, int(x), int(y) );
	    }
	    // remember last control point for next shorthand
	    controlX = quad[ 2 ].x();
	    controlY = quad[ 2 ].y();
	    // transform quadratic into cubic Bezier
	    if ( mode == 7 || mode == 8 ) {	// cubic 'Q' and 'T'
		int x31 = quad[0].x()+int(2.0*(quad[2].x()-quad[0].x())/3.0);
		int y31 = quad[0].y()+int(2.0*(quad[2].y()-quad[0].y())/3.0);
		int x32 = quad[2].x()+int(2.0*(quad[3].x()-quad[2].x())/3.0);
		int y32 = quad[2].y()+int(2.0*(quad[3].y()-quad[2].y())/3.0);
		quad.setPoint( 1, x31, y31 );
		quad.setPoint( 2, x32, y32 );
	    }
	    // calculate points on curve
	    bezier = quad.cubicBezier();
	    // reserve more space if needed
	    if ( bezier.size() > path.size() - pcount )
		path.resize( path.size() - pcount + bezier.size() );
	    // copy
	    for ( int k = 0; k < (int)bezier.size(); k ++ )
		path.setPoint( pcount++, bezier[ k ] );
	    break;
	}
#endif // TQT_NO_BEZIER
	case 9:					// 'A' elliptical arc curve
	    // ### just a straight line
	    x = arg[ 5 ] + offsetX;
	    y = arg[ 6 ] + offsetY;
	    path.setPoint( pcount++, int(x), int(y) );
	    break;
	};
	lastMode = mode;
	// array almost full ? expand for next loop
	if ( pcount >= (int)path.size() - 4 )
	    path.resize( 2 * path.size() );
    }

    subIndex.append( pcount );			// dummy marking the end
    if ( pt->brush().style() != TQt::NoBrush ) {
	// fill the area without stroke first
	if ( x != x0 || y != y0 )
	    path.setPoint( pcount++, int(x0), int(y0) );
	TQPen pen = pt->pen();
	pt->setPen( TQt::NoPen );
	pt->drawPolygon( path, false, 0, pcount );
	pt->setPen( pen );
    }
    // draw each subpath stroke seperately
    TQValueListConstIterator<int> it = subIndex.begin();
    int start = 0;
    while ( it != subIndex.fromLast() ) {
	int next = *++it;
	// ### always joins ends if first and last point coincide.
	// ### 'Z' can't have the desired effect
	pt->drawPolyline( path, start, next-start );
	start = next;
    }
}

void TQSvgDevice::applyStyle( TQDomElement *e, int c ) const
{
    // ### do not write every attribute each time
    TQColor pcol = pt->pen().color();
    TQColor bcol = pt->brush().color();
    TQString s;
    if ( c == PdcDrawText2 || c == PdcDrawText2Formatted ) {
	// TQPainter has a reversed understanding of pen/stroke vs.
	// brush/fill for text
	s += TQString( "fill:rgb(%1,%2,%3);" )
	     .arg( pcol.red() ).arg( pcol.green() ).arg( pcol.blue() );
	s += TQString( "stroke-width:0;" );
	TQFont f = pt->font();
	TQFontInfo fi( f );
	s += TQString( "font-size:%1;" ).arg( fi.pointSize() );
	s += TQString( "font-style:%1;" )
	     .arg( f.italic() ? "italic" : "normal" );
	// not a very scientific distribution
	TQString fw;
	if ( f.weight() <= TQFont::Light )
	    fw = "100";
	else if ( f.weight() <= TQFont::Normal )
	    fw = "400";
	else if ( f.weight() <= TQFont::DemiBold )
	    fw = "600";
	else if ( f.weight() <= TQFont::Bold )
	    fw = "700";
	else if ( f.weight() <= TQFont::Black )
	    fw = "800";
	else
	    fw = "900";
	s += TQString( "font-weight:%1;" ).arg( fw );
	s += TQString( "font-family:%1;" ).arg( f.family() );
    } else {
	s += TQString( "stroke:rgb(%1,%2,%3);" )
	     .arg( pcol.red() ).arg( pcol.green() ).arg( pcol.blue() );
	double pw = pt->pen().width();
	if ( pw == 0 && pt->pen().style() != TQt::NoPen )
	    pw = 0.9;
	if ( c == PdcDrawLine )
	    pw /= (TQABS(pt->worldMatrix().m11()) + TQABS(pt->worldMatrix().m22())) / 2.0;
	s += TQString( "stroke-width:%1;" ).arg( pw );
	if ( pt->pen().style() == TQt::DashLine )
	    s+= TQString( "stroke-dasharray:18,6;" );
	else if ( pt->pen().style() == TQt::DotLine )
	    s+= TQString( "stroke-dasharray:3;" );
	else if ( pt->pen().style() == TQt::DashDotLine )
	    s+= TQString( "stroke-dasharray:9,6,3,6;" );
	else if ( pt->pen().style() == TQt::DashDotDotLine )
	    s+= TQString( "stroke-dasharray:9,3,3;" );
	if ( pt->brush().style() == TQt::NoBrush || c == PdcDrawPolyline ||
	     c == PdcDrawCubicBezier )
	    s += "fill:none;"; // TQt polylines use no brush, neither do Beziers
	else
	    s += TQString( "fill:rgb(%1,%2,%3);" )
		 .arg( bcol.red() ).arg( bcol.green() ).arg( bcol.blue() );
    }
    e->setAttribute( "style", s );
}

void TQSvgDevice::applyTransform( TQDomElement *e ) const
{
    TQWMatrix m = pt->worldMatrix();

    TQString s;
    bool rot = ( m.m11() != 1.0 || m.m12() != 0.0 ||
		 m.m21() != 0.0 || m.m22() != 1.0 );
    if ( !rot && ( m.dx() != 0.0 || m.dy() != 0.0 ) )
	s = TQString( "translate(%1,%2)" ).arg( m.dx() ).arg( m.dy() );
    else if ( rot ) {
	if ( m.m12() == 0.0 && m.m21() == 0.0 &&
	     m.dx() == 0.0 && m.dy() == 0.0 )
	    s = TQString( "scale(%1,%2)" ).arg( m.m11() ).arg( m.m22() );
	else
	    s = TQString( "matrix(%1,%2,%3,%4,%5,%6)" )
		.arg( m.m11() ).arg( m.m12() )
		.arg( m.m21() ).arg( m.m22() )
		.arg( m.dx() ).arg( m.dy() );
    }
    else
	return;

    e->setAttribute( "transform", s );
}

#endif // TQT_NO_SVG
