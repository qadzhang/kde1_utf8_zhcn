#include "kclipboard.h"

#include <qdatetm.h>
#define  GC GC_QQQ
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>  

#include <cassert>
#include <iostream>
#include <ctype.h>
#include <string.h>

// Here we depend on Qt not to change their implementation!!!
// TQt3 迁移：qt_clipboard/qt_x_clipboardtime 是 Qt1 内部全局符号，TQt3 改由
// TQApplication::clipboard() 静态访问器管理——原「顶替 Qt 剪贴板对象位」的
// 注册逻辑删除（KClipboard 自行管理 X11 selection，两者并存互不顶替）

KClipboard* KClipboard::s_pSelf = 0L;

KClipboard* KClipboard::self()
{
  if ( s_pSelf == 0L )
    s_pSelf = new KClipboard;
  
  return s_pSelf;
}

KClipboard::KClipboard()
{
  if ( s_pSelf )
  {
    std::cerr << "You may only open one KClipboard at once" << std::endl;
    assert( 0 );
  }
  
  m_bOwner = false;
  m_pOwner = 0L;
  m_bEmpty = true;    
  m_mimeTypeLen = 0;
}

KClipboard::~KClipboard()
{
}
   
bool KClipboard::open( int _mode )
{
  return open( _mode, "application/octet-stream" );
}

bool KClipboard::open( int _mode, const char *_format )
{
  if ( _mode != IO_ReadOnly && _mode != ( IO_WriteOnly | IO_Truncate ) && _mode != IO_WriteOnly )
  {    
    std::cerr << "KClipboard: Wrong flags in call for Ken" << std::endl;
    assert( 0 );
  }
    
  if ( _mode == IO_WriteOnly )
    _mode |= IO_Truncate;

  if ( _mode == ( IO_WriteOnly | IO_Truncate ) )
  {
    m_bEmpty = false;
    m_strFormat = _format;

    std::cerr << "Fuck ya too" << std::endl;

    QBuffer::open( _mode );
    if ( strcmp( _format, "application/octet-stream" ) != 0L && 
	 strcmp( _format, "text/plain" ) != 0L ) 
    {
      m_mimeTypeLen = strlen( _format ) + 1;
      writeBlock( _format, m_mimeTypeLen );
    }
    else
      m_mimeTypeLen = 0;
    
    std::cerr << "2 Fuck ya too" << std::endl;

    return true;
  }
  else if ( ( _mode & IO_ReadOnly ) == IO_ReadOnly )
  {
    // printf("isOwner %i\n", ( isOwner() ? 1:0 ) );
    // printf("isEmpty %i\n", ( isEmpty() ? 1:0 ) );
    if ( !isOwner() )
      fetchData();
      
    if ( m_strFormat != _format && strcmp( _format, "application/octet-stream" ) != 0L )
      return false;

    if ( !QBuffer::open( _mode ) )
      return false;
    at( m_mimeTypeLen );

    return true;
  }
  else
    assert( 0 );
}

const char* KClipboard::format()
{
  if ( !isOwner() )
    fetchData();

  return m_strFormat;  
}
  
void KClipboard::close()
{
  if ( ( mode() & IO_WriteOnly ) == IO_WriteOnly )
    setOwner();    
 
  QBuffer::close();
 
  if ( ( mode() & IO_ReadOnly ) == IO_ReadOnly && !isOwner() )
    clear();
}
    
void KClipboard::clear()
{
  m_bEmpty = true;
  m_strFormat = "";
  
  buffer().resize( 0 );
  
  if ( isOwner() )
    setOwner();
}

bool KClipboard::isEmpty()
{
  return m_bEmpty;
}

bool KClipboard::isOwner()
{
  return m_bOwner;
}

void KClipboard::setOwner()
{
  if ( isOwner() )
    return;

  // printf("Setting owner\n");
    
  QWidget *owner = makeOwner();
  Window win = owner->winId();
  Display *dpy = owner->x11Display();
    
  XSetSelectionOwner( dpy, XA_PRIMARY, win, CurrentTime );
  if ( XGetSelectionOwner( dpy, XA_PRIMARY ) != win )
  {
    std::cerr <<  "KClipboard::setOwner: Cannot set X11 selection owner" << std::endl;
    return;
  }                            

  m_bOwner = true;
}

void KClipboard::fetchData()
{
  // printf("Getting data\n");
    
  if ( isOwner() )
    return;

  // printf("Doing it really!\n");
    
  clear();
    
  QWidget *owner = makeOwner();
  Window   win   = owner->winId();
  Display *display   = owner->x11Display();

  if ( XGetSelectionOwner(display,XA_PRIMARY) == None )
    return;

  Atom prop = XInternAtom( display, "QT_SELECTION", FALSE );
  XConvertSelection( display, XA_PRIMARY, XA_STRING, prop, win, CurrentTime );

  /** DEBUG code */
  XFlush( display );
  /** End DEBUG code */

  XEvent xevent;

  /** DEBUG code */
  QTime started = QTime::currentTime();
  while ( TRUE )
  {
    if ( XCheckTypedWindowEvent(display,win,SelectionNotify,&xevent) )
      break;
    QTime now = QTime::currentTime();
    if ( started > now )
      started = now;
    if ( started.msecsTo(now) > 5000 )
    {
      return;
    }
  }
  /** End Debug code */

  win  = xevent.xselection.requestor;
  prop = xevent.xselection.property;

  int nread = 0;
  Atom type;
  ulong nitems, bytes_after;
  int format;
  uchar *result;

  QBuffer::open( IO_WriteOnly | IO_Truncate );

  bool first = true;
  
  do 
  {
    int n = XGetWindowProperty( display, win, prop, nread/4, 1024, TRUE,
				AnyPropertyType, &type, &format, &nitems,
				&bytes_after, &result );
    if ( n != Success || type != XA_STRING )
      break;

    if ( first )
    {
      first = false;

      unsigned int i;
      for( i = 0; i < nitems; i++ )
	if ( result[i] == 0 )
	  break;
      
      if ( i < nitems )
      {
	m_mimeTypeLen = i + 1;
	m_strFormat = (const char*)result;
      }
      else
      {  
	m_strFormat = "";
      }
    }
    
    writeBlock( (const char*)result, nitems );
    nread += nitems;

    XFree( (char *)result );
    
  } while ( bytes_after > 0 );

  // printf("#################### READ %i bytes\n", nread );
    
  QBuffer::close();

  if ( m_strFormat.isEmpty() )
  {
    // Find non printable characters
    QByteArray ba = buffer();
    const char *d = ba.data();
    int len = ba.size();
    for( int j = 0; j < len; j++ )
    {
      if ( !isprint( d[j] ) && d[j] != '\n' && d[j] != '\r' && d[j] != '\t' )
      {
	m_strFormat = "application/octet-stream";
	return;
      }
    }
    
    m_strFormat = "text/plain";  
  }
  
  return;
}

//   Modified for the KDE1 Revival Project, 2026
//   Maintainer: <维护者姓名> <邮箱>
//   Modifications written with GLM-5.3 (Z.ai)
//   （TQt3 底座迁移手术：Qt1 用 Q_CUSTOM_EVENT/Event_Clipboard 把 X11 selection
//     事件包成 QCustomEvent 投递到本类 event()；TQt3 不再产生该自定义事件——
//     改由 KApplication::x11EventFilter 直接把原始 XEvent 转发到本方法，
//     内部逻辑（SelectionNotify/SelectionRequest/SelectionClear 处理）原样保留）
bool KClipboard::x11Event( XEvent *xevent )
{
  Display *display = qt_xdisplay();

  switch ( xevent->type )
  {
  case SelectionNotify:
    // printf("NOTIFY\n");
    m_bOwner = false;
    clear();
    break;

  case SelectionRequest:
    // printf("REQUEST\n");
    {
      // printf("Sending %i bytes\n",size());
	    
      XEvent xev;
      XSelectionRequestEvent *xreqev = &xevent->xselectionrequest;
      xev.xselection.type = SelectionNotify;
      xev.xselection.display = xreqev->display;
      xev.xselection.requestor = xreqev->requestor;
      xev.xselection.selection = xreqev->selection;
      xev.xselection.target = xreqev->target;
      xev.xselection.property = None;
      xev.xselection.time = xreqev->time;
      
      if ( xreqev->target == XA_STRING )
      {
	XChangeProperty ( display, xreqev->requestor, xreqev->property, XA_STRING, 8,
                                  PropModeReplace, (uchar *)buffer().data(), buffer().size() );
	xev.xselection.property = xreqev->property;
      }
      XSendEvent( display, xreqev->requestor, False, 0, &xev );
    }
  break;

  case SelectionClear:                    // new selection owner
    // printf("CLEAR\n");
    m_bOwner = false;
    clear();
    emit ownerChanged();
    break;

  }

  return true;
}                                              

QWidget* KClipboard::makeOwner()
{
  // Fake some clipboard owner
  if ( m_pOwner )  
    return m_pOwner;
  if ( qApp->mainWidget() )
    m_pOwner = qApp->mainWidget();
  else                     
    m_pOwner = new QWidget( 0L );
  return m_pOwner;
}            

void KClipboard::setURLList( QStrList& _urls )
{
  open( IO_WriteOnly | IO_Truncate, "url/url" );
  
  const char *s;
  for( s = _urls.first(); s != 0L; s = _urls.next() )
  {
    if ( s == _urls.getLast() )
      writeBlock( s, strlen( s ) );
    else
      writeBlock( s, strlen( s ) + 1 );
  }
  
  close();
}

bool KClipboard::urlList( QStrList& _urls)
{
  if ( !isOwner() )
    fetchData();

  if ( m_strFormat != "url/url" )
    return false;

  QByteArray ba = octetStream();
  char* d = ba.data();
  unsigned int c = 0;

  while ( c < ba.size() )
  {
    unsigned int start = c;
    // Find zero or end
    while ( d[c] != 0 && c < ba.size() )
      c++;

    if ( c < ba.size() )
    {
      _urls.append( d + start );
      // Skip zero
      c++;
    }
    else
    {
      char* s = new char[ c - start + 1 ];
      memcpy( s, d + start, c - start );
      s[ c - start ] = '\0';
      _urls.append( s );
      delete []s;
    }
  }

  return true;
}

void KClipboard::setText( const char *_text )
{
  open( IO_WriteOnly | IO_Truncate, "text/plain" );  

  writeBlock( _text, strlen( _text ) );
  
  close();
}

const char* KClipboard::text()
{
  if ( !isOwner() )
    fetchData();

 if ( m_strFormat != "text/plain" )
    return 0L;

  QByteArray ba = buffer();
  return ba.data() + m_mimeTypeLen;
}

void KClipboard::setOctetStream( QByteArray& _arr )
{
  open( IO_WriteOnly | IO_Truncate );  

  writeBlock( _arr.data(), _arr.size() );
  
  close();
}

QByteArray KClipboard::octetStream()
{
  if ( !isOwner() )
    fetchData();

  QByteArray ba;
  ba.duplicate( buffer().data() + m_mimeTypeLen, buffer().size() - m_mimeTypeLen );
  
  return ba;
}

#include "kclipboard.moc"

