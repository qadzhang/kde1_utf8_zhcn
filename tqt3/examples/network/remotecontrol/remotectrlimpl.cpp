#include "remotectrlimpl.h"

#include <ntqpushbutton.h>
#include <ntqlineedit.h>
#include <ntqsocket.h>
#include <ntqfiledialog.h>
#include <ntqcolordialog.h>
#include <ntqimage.h>

RemoteCtrlImpl::RemoteCtrlImpl( TQSocket *s )
{
    socket = s;
    connect( sImage, TQ_SIGNAL(clicked()), TQ_SLOT(sendImage()) );
    connect( sText, TQ_SIGNAL(clicked()), TQ_SLOT(sendText()) );
    connect( sPalette, TQ_SIGNAL(clicked()), TQ_SLOT(sendPalette()) );
}

void RemoteCtrlImpl::sendPacket( const TQVariant &v )
{
    TQByteArray ba;
    TQDataStream varDs( ba, IO_WriteOnly );
    varDs << v;

    TQDataStream ds( socket );
    ds << (TQ_UINT32) ba.size();
    socket->writeBlock( ba.data(), ba.size() );
}

void RemoteCtrlImpl::sendImage()
{
    TQString imageName = TQFileDialog::getOpenFileName( TQString::null,
	    "Images (*.png *.xpm *.jpg)", this );
    TQImage image( imageName );
    if ( !image.isNull() ) {
	sendPacket( image );
    }
}

void RemoteCtrlImpl::sendText()
{
    sendPacket( textToSend->text() );
}

void RemoteCtrlImpl::sendPalette()
{
    TQColor col = TQColorDialog::getColor( white, this );
    if ( col.isValid() ) {
	sendPacket( TQPalette(col,col) );
    }
}
