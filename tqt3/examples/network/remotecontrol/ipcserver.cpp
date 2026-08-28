#include "ipcserver.h"

#include <ntqsocket.h>
#include <ntqvariant.h>
#include <ntqimage.h>
#include <ntqpalette.h>
#include <ntqapplication.h>

class IpcSocket : public TQSocket
{
    TQ_OBJECT

public:
    IpcSocket( TQObject *parent) : TQSocket( parent )
    {
	packetSize = 0;
	connect( this, TQ_SIGNAL(readyRead()), TQ_SLOT(read()) );
    }

signals:
    void receivedText( const TQString& );
    void receivedPixmap( const TQPixmap& );

private slots:
    void read()
    {
	TQ_ULONG bytesAvail = bytesAvailable();
	for ( ;; ) {
	    if ( packetSize == 0 ) {
		TQDataStream ds( this );
		if ( bytesAvail < 4 )
		    return;
		ds >> packetSize;
		bytesAvail -= 4;
	    } else {
		if ( bytesAvail < packetSize )
		    return;
		// read the packet in a byte array to be sure that you don't
		// read too much or too less
		TQByteArray ba( packetSize );
		readBlock( ba.data(), packetSize );
		bytesAvail -= packetSize;
		packetSize = 0;

		TQVariant variant;
		TQDataStream ds( ba, IO_ReadOnly );
		ds >> variant;
		switch ( variant.type() ) {
		    case TQVariant::String:
			emit receivedText( variant.toString() );
			break;
		    case TQVariant::Image:
			emit receivedPixmap( TQPixmap(variant.toImage()) );
			break;
		    case TQVariant::Palette:
			TQApplication::setPalette( variant.toPalette(), true );
			break;
		    default:
			break;
		}
	    }
	}
    }

private:
    TQ_UINT32 packetSize;
};

IpcServer::IpcServer( TQ_UINT16 port, TQObject *parent ) :
    TQServerSocket( 0x7f000001, port, 1, parent )
{
}

void IpcServer::newConnection( int socket )
{
    IpcSocket *s = new IpcSocket( this );
    s->setSocket( socket );
    connect( s, TQ_SIGNAL(receivedText(const TQString&)),
	    TQ_SIGNAL(receivedText(const TQString&)) );
    connect( s, TQ_SIGNAL(receivedPixmap(const TQPixmap&)),
	    TQ_SIGNAL(receivedPixmap(const TQPixmap&)) );
}

#include "ipcserver.moc"
