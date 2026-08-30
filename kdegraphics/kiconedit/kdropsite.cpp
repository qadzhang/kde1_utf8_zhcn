
#include "kdropsite.h"
#include <kapp.h>
#include <kiconloader.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qdragobject.h>
#include <qimage.h>


KDropSite::KDropSite( QWidget * parent ) : QObject( parent ), QDropSite( parent )
{
  tqWarning("KDropSite constructor");
}

void KDropSite::dragMoveEvent( QDragMoveEvent *e )
{
  //tqWarning("dragMove");
  emit dragMove(e);
}

void KDropSite::dragEnterEvent( QDragEnterEvent *e )
{
  //tqWarning("dragEnter");
  emit dragEnter(e);
}

void KDropSite::dragLeaveEvent( QDragLeaveEvent *e )
{
  //tqWarning("dragLeave");
  emit dragLeave(e);
}

void KDropSite::dropEvent( QDropEvent *e )
{
  //tqWarning("drop");
  emit dropAction(e);
}



#include "kdropsite.moc" /* [KDE1 Revival 2026] TQt3 moc linkage */
