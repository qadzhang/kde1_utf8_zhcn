
#include "kdropsite.moc"
#include <kapp.h>
#include <kiconloader.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qdragobject.h>
#include <qimage.h>


KDropSite::KDropSite( QWidget * parent ) : QObject( parent ), QDropSite( parent )
{
  //tqDebug("KDropSite constructor");
}

void KDropSite::dragMoveEvent( QDragMoveEvent *e )
{
  //tqDebug("dragMove");
  emit dragMove(e);
}

void KDropSite::dragEnterEvent( QDragEnterEvent *e )
{
  //tqDebug("dragEnter");
  emit dragEnter(e);
}

void KDropSite::dragLeaveEvent( QDragLeaveEvent *e )
{
  //tqDebug("dragLeave");
  emit dragLeave(e);
}

void KDropSite::dropEvent( QDropEvent *e )
{
  //tqDebug("drop");
  emit dropAction(e);
}


