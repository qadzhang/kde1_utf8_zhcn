/**********************************************************************
**
** Definition of TQCanvas classes
**
** Created : 991211
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the canvas module of the TQt GUI Toolkit.
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

#ifndef TQCANVAS_H
#define TQCANVAS_H

#ifndef QT_H
#include "ntqscrollview.h"
#include "ntqpixmap.h"
#include "ntqptrlist.h"
#include "ntqbrush.h"
#include "ntqpen.h"
#include "ntqvaluelist.h"
#include "ntqpointarray.h"
#endif // QT_H

#if !defined( TQT_MODULE_CANVAS ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_CANVAS )
#define TQM_EXPORT_CANVAS
#define TQM_TEMPLATE_EXTERN_CANVAS
#else
#define TQM_EXPORT_CANVAS TQ_EXPORT
#define TQM_TEMPLATE_EXTERN_CANVAS Q_TEMPLATE_EXTERN
#endif

#ifndef TQT_NO_CANVAS


class TQCanvasSprite;
class TQCanvasPolygonalItem;
class TQCanvasRectangle;
class TQCanvasPolygon;
class TQCanvasEllipse;
class TQCanvasText;
class TQCanvasLine;
class TQCanvasChunk;
class TQCanvas;
class TQCanvasItem;
class TQCanvasView;
class TQCanvasPixmap;

#if defined(Q_TEMPLATEDLL) && ( !defined(QT_MAKEDLL) || defined(TQ_EXPORT_TEMPLATES) )
// MOC_SKIP_BEGIN
TQM_TEMPLATE_EXTERN_CANVAS template class TQM_EXPORT_CANVAS TQValueListIterator< TQCanvasItem* >;
TQM_TEMPLATE_EXTERN_CANVAS template class TQM_EXPORT_CANVAS TQValueList< TQCanvasItem* >;
// MOC_SKIP_END
#endif

class TQM_EXPORT_CANVAS TQCanvasItemList : public TQValueList<TQCanvasItem*> {
public:
    void sort();
    void drawUnique( TQPainter& painter );
    TQCanvasItemList operator+(const TQCanvasItemList &l) const;
};


class TQCanvasItemExtra;

class TQM_EXPORT_CANVAS TQCanvasItem : public TQt
{
public:
    TQCanvasItem(TQCanvas* canvas);
    virtual ~TQCanvasItem();

    double x() const
	{ return myx; }
    double y() const
	{ return myy; }
    double z() const
	{ return myz; } // (depth)

    virtual void moveBy(double dx, double dy);
    void move(double x, double y);
    void setX(double a) { move(a,y()); }
    void setY(double a) { move(x(),a); }
    void setZ(double a) { myz=a; changeChunks(); }

    bool animated() const;
    virtual void setAnimated(bool y);
    virtual void setVelocity( double vx, double vy);
    void setXVelocity( double vx ) { setVelocity(vx,yVelocity()); }
    void setYVelocity( double vy ) { setVelocity(xVelocity(),vy); }
    double xVelocity() const;
    double yVelocity() const;
    virtual void advance(int stage);

    virtual bool collidesWith( const TQCanvasItem* ) const=0;

    TQCanvasItemList collisions(bool exact /* NO DEFAULT */ ) const;

    virtual void setCanvas(TQCanvas*);

    virtual void draw(TQPainter&)=0;

    void show();
    void hide();

    virtual void setVisible(bool yes);
    bool isVisible() const
	{ return (bool)vis; }
    virtual void setSelected(bool yes);
    bool isSelected() const
	{ return (bool)sel; }
    virtual void setEnabled(bool yes);
    bool isEnabled() const
	{ return (bool)ena; }
    virtual void setActive(bool yes);
    bool isActive() const
	{ return (bool)act; }
#ifndef TQT_NO_COMPAT
    bool visible() const
	{ return (bool)vis; }
    bool selected() const
	{ return (bool)sel; }
    bool enabled() const
	{ return (bool)ena; }
    bool active() const
	{ return (bool)act; }
#endif

    enum RttiValues {
	Rtti_Item = 0,
	Rtti_Sprite = 1,
	Rtti_PolygonalItem = 2,
	Rtti_Text = 3,
	Rtti_Polygon = 4,
	Rtti_Rectangle = 5,
	Rtti_Ellipse = 6,
	Rtti_Line = 7,
	Rtti_Spline = 8
    };

    virtual int rtti() const;
    static int RTTI;

    virtual TQRect boundingRect() const=0;
    virtual TQRect boundingRectAdvanced() const;

    TQCanvas* canvas() const
	{ return cnv; }

protected:
    void update() { changeChunks(); }

private:
    // For friendly subclasses...

    friend class TQCanvasPolygonalItem;
    friend class TQCanvasSprite;
    friend class TQCanvasRectangle;
    friend class TQCanvasPolygon;
    friend class TQCanvasEllipse;
    friend class TQCanvasText;
    friend class TQCanvasLine;

    virtual TQPointArray chunks() const;
    virtual void addToChunks();
    virtual void removeFromChunks();
    virtual void changeChunks();
    virtual bool collidesWith( const TQCanvasSprite*,
			       const TQCanvasPolygonalItem*,
			       const TQCanvasRectangle*,
			       const TQCanvasEllipse*,
			       const TQCanvasText* ) const = 0;
    // End of friend stuff

    TQCanvas* cnv;
    static TQCanvas* current_canvas;
    double myx,myy,myz;
    TQCanvasItemExtra *ext;
    TQCanvasItemExtra& extra();
    uint ani:1;
    uint vis:1;
    uint val:1;
    uint sel:1;
    uint ena:1;
    uint act:1;
};


class TQCanvasData;

class TQM_EXPORT_CANVAS TQCanvas : public TQObject
{
    TQ_OBJECT
public:
    TQCanvas( TQObject* parent = 0, const char* name = 0 );
    TQCanvas(int w, int h);
    TQCanvas( TQPixmap p, int h, int v, int tilewidth, int tileheight );

    virtual ~TQCanvas();

    virtual void setTiles( TQPixmap tiles, int h, int v,
			   int tilewidth, int tileheight );
    virtual void setBackgroundPixmap( const TQPixmap& p );
    TQPixmap backgroundPixmap() const;

    virtual void setBackgroundColor( const TQColor& c );
    TQColor backgroundColor() const;

    virtual void setTile( int x, int y, int tilenum );
    int tile( int x, int y ) const
	{ return grid[x+y*htiles]; }

    int tilesHorizontally() const
	{ return htiles; }
    int tilesVertically() const
	{ return vtiles; }

    int tileWidth() const
	{ return tilew; }
    int tileHeight() const
	{ return tileh; }

    virtual void resize(int width, int height);
    int width() const
	{ return awidth; }
    int height() const
	{ return aheight; }
    TQSize size() const
	{ return TQSize(awidth,aheight); }
    TQRect rect() const
	{ return TQRect( 0, 0, awidth, aheight ); }
    bool onCanvas( int x, int y ) const
	{ return x>=0 && y>=0 && x<awidth && y<aheight; }
    bool onCanvas( const TQPoint& p ) const
	{ return onCanvas(p.x(),p.y()); }
    bool validChunk( int x, int y ) const
	{ return x>=0 && y>=0 && x<chwidth && y<chheight; }
    bool validChunk( const TQPoint& p ) const
	{ return validChunk(p.x(),p.y()); }

    int chunkSize() const
	{ return chunksize; }
    virtual void retune(int chunksize, int maxclusters=100);

    bool sameChunk(int x1, int y1, int x2, int y2) const
	{ return x1/chunksize==x2/chunksize && y1/chunksize==y2/chunksize; }
    virtual void setChangedChunk(int i, int j);
    virtual void setChangedChunkContaining(int x, int y);
    virtual void setAllChanged();
    virtual void setChanged(const TQRect& area);
    virtual void setUnchanged(const TQRect& area);

    // These call setChangedChunk.
    void addItemToChunk(TQCanvasItem*, int i, int j);
    void removeItemFromChunk(TQCanvasItem*, int i, int j);
    void addItemToChunkContaining(TQCanvasItem*, int x, int y);
    void removeItemFromChunkContaining(TQCanvasItem*, int x, int y);

    TQCanvasItemList allItems();
    TQCanvasItemList collisions( const TQPoint&) const;
    TQCanvasItemList collisions( const TQRect&) const;
    TQCanvasItemList collisions( const TQPointArray& pa, const TQCanvasItem* item,
				bool exact) const;

    void drawArea(const TQRect&, TQPainter* p, bool double_buffer=false);

    // These are for TQCanvasView to call
    virtual void addView(TQCanvasView*);
    virtual void removeView(TQCanvasView*);
    void drawCanvasArea(const TQRect&, TQPainter* p=0, bool double_buffer=true);
    void drawViewArea( TQCanvasView* view, TQPainter* p, const TQRect& r, bool dbuf );

    // These are for TQCanvasItem to call
    virtual void addItem(TQCanvasItem*);
    virtual void addAnimation(TQCanvasItem*);
    virtual void removeItem(TQCanvasItem*);
    virtual void removeAnimation(TQCanvasItem*);

    virtual void setAdvancePeriod(int ms);
    virtual void setUpdatePeriod(int ms);

    virtual void setDoubleBuffering(bool y);

signals:
    void resized();

public slots:
    virtual void advance();
    virtual void update();

protected:
    virtual void drawBackground(TQPainter&, const TQRect& area);
    virtual void drawForeground(TQPainter&, const TQRect& area);

private:
    void init(int w, int h, int chunksze=16, int maxclust=100);

    TQCanvasChunk& chunk(int i, int j) const;
    TQCanvasChunk& chunkContaining(int x, int y) const;

    TQRect changeBounds(const TQRect& inarea);
    void drawChanges(const TQRect& inarea);

    TQPixmap offscr;
    int awidth,aheight;
    int chunksize;
    int maxclusters;
    int chwidth,chheight;
    TQCanvasChunk* chunks;

    TQCanvasData* d;

    void initTiles(TQPixmap p, int h, int v, int tilewidth, int tileheight);
    ushort *grid;
    ushort htiles;
    ushort vtiles;
    ushort tilew;
    ushort tileh;
    bool oneone;
    TQPixmap pm;
    TQTimer* update_timer;
    TQColor bgcolor;
    bool debug_redraw_areas;
    bool dblbuf;

    friend void qt_unview(TQCanvas* c);

#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQCanvas( const TQCanvas & );
    TQCanvas &operator=( const TQCanvas & );
#endif
};

class TQCanvasViewData;

class TQM_EXPORT_CANVAS TQCanvasView : public TQScrollView
{
    TQ_OBJECT
public:

    TQCanvasView(TQWidget* parent=0, const char* name=0, WFlags f=0);
    TQCanvasView(TQCanvas* viewing, TQWidget* parent=0, const char* name=0, WFlags f=0);
    ~TQCanvasView();

    TQCanvas* canvas() const
	{ return viewing; }
    void setCanvas(TQCanvas* v);

    const TQWMatrix &worldMatrix() const;
    const TQWMatrix &inverseWorldMatrix() const;
    bool setWorldMatrix( const TQWMatrix & );

protected:
    void drawContents( TQPainter*, int cx, int cy, int cw, int ch );
    TQSize sizeHint() const;

private:
    void drawContents( TQPainter* );
    TQCanvas* viewing;
    TQCanvasViewData* d;
    friend void qt_unview(TQCanvas* c);

private slots:
    void cMoving(int,int);
    void updateContentsSize();

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQCanvasView( const TQCanvasView & );
    TQCanvasView &operator=( const TQCanvasView & );
#endif
};


class TQM_EXPORT_CANVAS TQCanvasPixmap : public TQPixmap
{
public:
#ifndef TQT_NO_IMAGEIO
    TQCanvasPixmap(const TQString& datafilename);
#endif
    TQCanvasPixmap(const TQImage& image);
    TQCanvasPixmap(const TQPixmap&, const TQPoint& hotspot);
    ~TQCanvasPixmap();

    int offsetX() const
	{ return hotx; }
    int offsetY() const
	{ return hoty; }
    void setOffset(int x, int y) { hotx = x; hoty = y; }

private:
#if defined(TQ_DISABLE_COPY)
    TQCanvasPixmap( const TQCanvasPixmap & );
    TQCanvasPixmap &operator=( const TQCanvasPixmap & );
#endif
    void init(const TQImage&);
    void init(const TQPixmap& pixmap, int hx, int hy);

    friend class TQCanvasSprite;
    friend class TQCanvasPixmapArray;
    friend bool qt_testCollision(const TQCanvasSprite* s1, const TQCanvasSprite* s2);

    int hotx,hoty;

    TQImage* collision_mask;
};


class TQM_EXPORT_CANVAS TQCanvasPixmapArray
{
public:
    TQCanvasPixmapArray();
#ifndef TQT_NO_IMAGEIO
    TQCanvasPixmapArray(const TQString& datafilenamepattern, int framecount=0);
#endif
    // this form is deprecated
    TQCanvasPixmapArray(TQPtrList<TQPixmap>, TQPtrList<TQPoint> hotspots);

    TQCanvasPixmapArray(TQValueList<TQPixmap>, TQPointArray hotspots = TQPointArray() );
    ~TQCanvasPixmapArray();

#ifndef TQT_NO_IMAGEIO
    bool readPixmaps(const TQString& datafilenamepattern, int framecount=0);
    bool readCollisionMasks(const TQString& filenamepattern);
#endif

    // deprecated
    bool operator!(); // Failure check.
    bool isValid() const;

    TQCanvasPixmap* image(int i) const
	{ return img ? img[i] : 0; }
    void setImage(int i, TQCanvasPixmap* p);
    uint count() const
	{ return (uint)framecount; }

private:
#if defined(TQ_DISABLE_COPY)
    TQCanvasPixmapArray( const TQCanvasPixmapArray & );
    TQCanvasPixmapArray &operator=( const TQCanvasPixmapArray & );
#endif
#ifndef TQT_NO_IMAGEIO
    bool readPixmaps(const TQString& datafilenamepattern, int framecount, bool maskonly);
#endif

    void reset();
    int framecount;
    TQCanvasPixmap** img;
};


class TQM_EXPORT_CANVAS TQCanvasSprite : public TQCanvasItem
{
public:
    TQCanvasSprite(TQCanvasPixmapArray* array, TQCanvas* canvas);

    void setSequence(TQCanvasPixmapArray* seq);

    virtual ~TQCanvasSprite();

    void move(double x, double y);
    virtual void move(double x, double y, int frame);
    void setFrame(int);
    enum FrameAnimationType { Cycle, Oscillate };
    virtual void setFrameAnimation(FrameAnimationType=Cycle, int step=1, int state=0);
    int frame() const
	{ return frm; }
    int frameCount() const
	{ return images->count(); }

    int rtti() const;
    static int RTTI;

    bool collidesWith( const TQCanvasItem* ) const;

    TQRect boundingRect() const;

    // is there a reason for these to be protected? Lars
//protected:

    int width() const;
    int height() const;

    int leftEdge() const;
    int topEdge() const;
    int rightEdge() const;
    int bottomEdge() const;

    int leftEdge(int nx) const;
    int topEdge(int ny) const;
    int rightEdge(int nx) const;
    int bottomEdge(int ny) const;
    TQCanvasPixmap* image() const
	{ return images->image(frm); }
    virtual TQCanvasPixmap* imageAdvanced() const;
    TQCanvasPixmap* image(int f) const
	{ return images->image(f); }
    virtual void advance(int stage);

public:
    void draw(TQPainter& painter);

private:
#if defined(TQ_DISABLE_COPY)
    TQCanvasSprite( const TQCanvasSprite & );
    TQCanvasSprite &operator=( const TQCanvasSprite & );
#endif
    void addToChunks();
    void removeFromChunks();
    void changeChunks();

    int frm;
    ushort anim_val;
    uint anim_state:2;
    uint anim_type:14;
    bool collidesWith( const TQCanvasSprite*,
		       const TQCanvasPolygonalItem*,
		       const TQCanvasRectangle*,
		       const TQCanvasEllipse*,
		       const TQCanvasText* ) const;

    friend bool qt_testCollision( const TQCanvasSprite* s1,
				  const TQCanvasSprite* s2 );

    TQCanvasPixmapArray* images;
};

class TQPolygonalProcessor;

class TQM_EXPORT_CANVAS TQCanvasPolygonalItem : public TQCanvasItem
{
public:
    TQCanvasPolygonalItem(TQCanvas* canvas);
    virtual ~TQCanvasPolygonalItem();

    bool collidesWith( const TQCanvasItem* ) const;

    virtual void setPen(TQPen p);
    virtual void setBrush(TQBrush b);

    TQPen pen() const
	{ return pn; }
    TQBrush brush() const
	{ return br; }

    virtual TQPointArray areaPoints() const=0;
    virtual TQPointArray areaPointsAdvanced() const;
    TQRect boundingRect() const;

    int rtti() const;
    static int RTTI;

protected:
    void draw(TQPainter &);
    virtual void drawShape(TQPainter &) = 0;

    bool winding() const;
    void setWinding(bool);

    void invalidate();
    bool isValid() const
	{ return (bool)val; }

private:
    void scanPolygon( const TQPointArray& pa, int winding,
		      TQPolygonalProcessor& process ) const;
    TQPointArray chunks() const;

    bool collidesWith( const TQCanvasSprite*,
		       const TQCanvasPolygonalItem*,
		       const TQCanvasRectangle*,
		       const TQCanvasEllipse*,
		       const TQCanvasText* ) const;

    TQBrush br;
    TQPen pn;
    uint wind:1;
};


class TQM_EXPORT_CANVAS TQCanvasRectangle : public TQCanvasPolygonalItem
{
public:
    TQCanvasRectangle(TQCanvas* canvas);
    TQCanvasRectangle(const TQRect&, TQCanvas* canvas);
    TQCanvasRectangle(int x, int y, int width, int height, TQCanvas* canvas);

    ~TQCanvasRectangle();

    int width() const;
    int height() const;
    void setSize(int w, int h);
    TQSize size() const
	{ return TQSize(w,h); }
    TQPointArray areaPoints() const;
    TQRect rect() const
	{ return TQRect(int(x()),int(y()),w,h); }

    bool collidesWith( const TQCanvasItem* ) const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(TQPainter &);
    TQPointArray chunks() const;

private:
    bool collidesWith(   const TQCanvasSprite*,
			 const TQCanvasPolygonalItem*,
			 const TQCanvasRectangle*,
			 const TQCanvasEllipse*,
			 const TQCanvasText* ) const;

    int w, h;
};


class TQM_EXPORT_CANVAS TQCanvasPolygon : public TQCanvasPolygonalItem
{
public:
    TQCanvasPolygon(TQCanvas* canvas);
    ~TQCanvasPolygon();
    void setPoints(TQPointArray);
    TQPointArray points() const;
    void moveBy(double dx, double dy);

    TQPointArray areaPoints() const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(TQPainter &);
    TQPointArray poly;
};


class TQM_EXPORT_CANVAS TQCanvasSpline : public TQCanvasPolygon
{
public:
    TQCanvasSpline(TQCanvas* canvas);
    ~TQCanvasSpline();

    void setControlPoints(TQPointArray, bool closed=true);
    TQPointArray controlPoints() const;
    bool closed() const;

    int rtti() const;
    static int RTTI;

private:
    void recalcPoly();
    TQPointArray bez;
    bool cl;
};


class TQM_EXPORT_CANVAS TQCanvasLine : public TQCanvasPolygonalItem
{
public:
    TQCanvasLine(TQCanvas* canvas);
    ~TQCanvasLine();
    void setPoints(int x1, int y1, int x2, int y2);

    TQPoint startPoint() const
	{ return TQPoint(x1,y1); }
    TQPoint endPoint() const
	{ return TQPoint(x2,y2); }

    int rtti() const;
    static int RTTI;

    void setPen(TQPen p);
    void moveBy(double dx, double dy);

protected:
    void drawShape(TQPainter &);
    TQPointArray areaPoints() const;

private:
    int x1,y1,x2,y2;
};


class TQM_EXPORT_CANVAS TQCanvasEllipse : public TQCanvasPolygonalItem
{

public:
    TQCanvasEllipse( TQCanvas* canvas );
    TQCanvasEllipse( int width, int height, TQCanvas* canvas );
    TQCanvasEllipse( int width, int height, int startangle, int angle,
		    TQCanvas* canvas );

    ~TQCanvasEllipse();

    int width() const;
    int height() const;
    void setSize(int w, int h);
    void setAngles(int start, int length);
    int angleStart() const
	{ return a1; }
    int angleLength() const
	{ return a2; }
    TQPointArray areaPoints() const;

    bool collidesWith( const TQCanvasItem* ) const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(TQPainter &);

private:
    bool collidesWith( const TQCanvasSprite*,
		       const TQCanvasPolygonalItem*,
		       const TQCanvasRectangle*,
		       const TQCanvasEllipse*,
		       const TQCanvasText* ) const;
    int w, h;
    int a1, a2;
};


class TQCanvasTextExtra;

class TQM_EXPORT_CANVAS TQCanvasText : public TQCanvasItem
{
public:
    TQCanvasText(TQCanvas* canvas);
    TQCanvasText(const TQString&, TQCanvas* canvas);
    TQCanvasText(const TQString&, TQFont, TQCanvas* canvas);

    virtual ~TQCanvasText();

    void setText( const TQString& );
    void setFont( const TQFont& );
    void setColor( const TQColor& );
    TQString text() const;
    TQFont font() const;
    TQColor color() const;

    void moveBy(double dx, double dy);

    int textFlags() const
	{ return flags; }
    void setTextFlags(int);

    TQRect boundingRect() const;

    bool collidesWith( const TQCanvasItem* ) const;

    int rtti() const;
    static int RTTI;

protected:
    virtual void draw(TQPainter&);

private:
#if defined(TQ_DISABLE_COPY)
    TQCanvasText( const TQCanvasText & );
    TQCanvasText &operator=( const TQCanvasText & );
#endif
    void addToChunks();
    void removeFromChunks();
    void changeChunks();

    void setRect();
    TQRect brect;
    TQString txt;
    int flags;
    TQFont fnt;
    TQColor col;
    TQCanvasTextExtra* extra;

    bool collidesWith(   const TQCanvasSprite*,
			 const TQCanvasPolygonalItem*,
			 const TQCanvasRectangle*,
			 const TQCanvasEllipse*,
			 const TQCanvasText* ) const;
};

#define Q_DEFINED_QCANVAS
#include "ntqwinexport.h"
#endif // TQT_NO_CANVAS

#endif // TQCANVAS_H
