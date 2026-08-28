/****************************************************************************
**
** Definition of TQDragObject
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQDRAGOBJECT_H
#define TQDRAGOBJECT_H

class TQWidget;
class TQTextDragPrivate;
class TQDragObjectData;
class TQStoredDragData;
class TQImageDragData;

#ifndef QT_H
#include "ntqobject.h"
#include "ntqimage.h"
#include "ntqstrlist.h"
#include "ntqcolor.h"
#endif // QT_H

#ifndef TQT_NO_MIME

class TQ_EXPORT TQDragObject: public TQObject, public TQMimeSource {
    TQ_OBJECT
public:
    TQDragObject( TQWidget * dragSource = 0, const char * name = 0 );
    virtual ~TQDragObject();

#ifndef TQT_NO_DRAGANDDROP
    bool drag();
    bool dragMove();
    void dragCopy();
    void dragLink();

    virtual void setPixmap(TQPixmap);
    virtual void setPixmap(TQPixmap, const TQPoint& hotspot);
    TQPixmap pixmap() const;
    TQPoint pixmapHotSpot() const;
#endif

    TQWidget * source();
    static TQWidget * target();

    static void setTarget(TQWidget*);

#ifndef TQT_NO_DRAGANDDROP
    enum DragMode { DragDefault, DragCopy, DragMove, DragLink, DragCopyOrMove };

protected:
    virtual bool drag(DragMode);
#endif

private:
    TQDragObjectData * d;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDragObject( const TQDragObject & );
    TQDragObject &operator=( const TQDragObject & );
#endif
};

class TQ_EXPORT TQStoredDrag: public TQDragObject {
    TQ_OBJECT
    TQStoredDragData * d;

public:
    TQStoredDrag( const char * mimeType,
		 TQWidget * dragSource = 0, const char * name = 0 );
    ~TQStoredDrag();

    virtual void setEncodedData( const TQByteArray & );

    const char * format(int i) const;
    virtual TQByteArray encodedData(const char*) const;

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQStoredDrag( const TQStoredDrag & );
    TQStoredDrag &operator=( const TQStoredDrag & );
#endif
};

class TQ_EXPORT TQTextDrag: public TQDragObject {
    TQ_OBJECT
    TQTextDragPrivate* d;
public:
    TQTextDrag( const TQString &,
	       TQWidget * dragSource = 0, const char * name = 0 );
    TQTextDrag( TQWidget * dragSource = 0, const char * name = 0 );
    ~TQTextDrag();

    virtual void setText( const TQString &);
    virtual void setSubtype( const TQCString &);

    const char * format(int i) const;
    virtual TQByteArray encodedData(const char*) const;

    static bool canDecode( const TQMimeSource* e );
    static bool decode( const TQMimeSource* e, TQString& s );
    static bool decode( const TQMimeSource* e, TQString& s, TQCString& subtype );

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQTextDrag( const TQTextDrag & );
    TQTextDrag &operator=( const TQTextDrag & );
#endif
};

class TQ_EXPORT TQImageDrag: public TQDragObject {
    TQ_OBJECT
    TQImage img;
    TQStrList ofmts;
    TQImageDragData* d;

public:
    TQImageDrag( TQImage image, TQWidget * dragSource = 0, const char * name = 0 );
    TQImageDrag( TQWidget * dragSource = 0, const char * name = 0 );
    ~TQImageDrag();

    virtual void setImage( TQImage image );

    const char * format(int i) const;
    virtual TQByteArray encodedData(const char*) const;

    static bool canDecode( const TQMimeSource* e );
    static bool decode( const TQMimeSource* e, TQImage& i );
    static bool decode( const TQMimeSource* e, TQPixmap& i );

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQImageDrag( const TQImageDrag & );
    TQImageDrag &operator=( const TQImageDrag & );
#endif
};


class TQ_EXPORT TQUriDrag: public TQStoredDrag {
    TQ_OBJECT

public:
    TQUriDrag( TQStrList uris, TQWidget * dragSource = 0, const char * name = 0 );
    TQUriDrag( TQWidget * dragSource = 0, const char * name = 0 );
    ~TQUriDrag();

    void setFilenames( const TQStringList & fnames ) { setFileNames( fnames ); }
    void setFileNames( const TQStringList & fnames );
    void setUnicodeUris( const TQStringList & uuris );
    virtual void setUris( TQStrList uris );

    static TQString uriToLocalFile(const char*);
    static TQCString localFileToUri(const TQString&);
    static TQString uriToUnicodeUri(const char*);
    static TQCString unicodeUriToUri(const TQString&);
    static bool canDecode( const TQMimeSource* e );
    static bool decode( const TQMimeSource* e, TQStrList& i );
    static bool decodeToUnicodeUris( const TQMimeSource* e, TQStringList& i );
    static bool decodeLocalFiles( const TQMimeSource* e, TQStringList& i );

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQUriDrag( const TQUriDrag & );
    TQUriDrag &operator=( const TQUriDrag & );
#endif
};

class TQ_EXPORT TQColorDrag : public TQStoredDrag
{
    TQ_OBJECT
    TQColor color;

public:
    TQColorDrag( const TQColor &col, TQWidget *dragsource = 0, const char *name = 0 );
    TQColorDrag( TQWidget * dragSource = 0, const char * name = 0 );
    void setColor( const TQColor &col );

    static bool canDecode( TQMimeSource * );
    static bool decode( TQMimeSource *, TQColor &col );

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQColorDrag( const TQColorDrag & );
    TQColorDrag &operator=( const TQColorDrag & );
#endif
};

#ifndef TQT_NO_COMPAT
typedef TQUriDrag TQUrlDrag;
#endif

#ifndef TQT_NO_DRAGANDDROP

// TQDragManager is not part of the public API.  It is defined in a
// header file simply so different .cpp files can implement different
// member functions.
//

class TQ_EXPORT TQDragManager: public TQObject {
    TQ_OBJECT

private:
    TQDragManager();
    ~TQDragManager();
    // only friend classes can use TQDragManager.
    friend class TQDragObject;
    friend class TQDragMoveEvent;
    friend class TQDropEvent;
    friend class TQApplication;

    bool eventFilter( TQObject *, TQEvent * );
    void timerEvent( TQTimerEvent* );

    bool drag( TQDragObject *, TQDragObject::DragMode );

    void cancel( bool deleteSource = true );
    void move( const TQPoint & );
    void drop();
    void updatePixmap();
    void updatePixmap( const TQPoint& cursorPos );

private:
    TQDragObject * object;
    bool updateMode( ButtonState newstate );
    void updateCursor();
#if defined(TQ_WS_X11)
    void createCursors();
#endif

    TQWidget * dragSource;
    TQWidget * dropWidget;
    bool beingCancelled;
    bool restoreCursor;
    bool willDrop;

    TQPixmap *pm_cursor;
    int n_cursor;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDragManager( const TQDragManager & );
    TQDragManager &operator=( const TQDragManager & );
#endif
};

#endif

#endif // TQT_NO_MIME

#endif // TQDRAGOBJECT_H
