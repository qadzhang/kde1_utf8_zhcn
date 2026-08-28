#ifndef __kclipboard_h__
#define __kclipboard_h__

#include <qclipbrd.h>
#include <qobject.h>
#include <qapp.h>
#include <qbuffer.h>
#include <qwidget.h>
#include <qstring.h>
#include <qstrlist.h>

class KClipboard : public QObject, public QBuffer
{
  Q_OBJECT
public:
  KClipboard();
  ~KClipboard();
   
  bool open( int _mode );
  bool open( int _mode, const char *_format );
  void close();
    
  void clear();

  const char *format();

  /////////////////////////
  // Convenience functions
  /////////////////////////
  void setURLList( QStrList& _urls );
  bool urlList( QStrList& _urls );
  void setText( const char *_text );
  const char* text();
  QByteArray octetStream();
  void setOctetStream( QByteArray& _arr );

  static KClipboard* self();

  // TQt3 迁移：x11Event 由 KApplication::x11EventFilter 直接调用（public）；
  // s_pSelf 供其判空转发（原经 Qt1 Event_Clipboard 事件投递，无需外部访问）
  bool x11Event( XEvent *xevent );
  static KClipboard* s_pSelf;

signals:
  void ownerChanged();
    
protected:

  void fetchData();
  void setOwner();
  bool isOwner();
  bool isEmpty();
  QWidget* makeOwner();

  // TQt3 迁移：KApplication::x11EventFilter 凭此静态实例判空转发 X11
  // selection 事件（原先经 Qt1 Event_Clipboard 事件投递，无需访问）

private:
  bool m_bOwner;
  QWidget *m_pOwner;
  bool m_bEmpty;
  QString m_strFormat;
  int m_mimeTypeLen;
};

#endif
