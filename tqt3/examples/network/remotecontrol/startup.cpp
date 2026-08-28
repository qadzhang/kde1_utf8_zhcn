#include "startup.h"
#include "remotectrlimpl.h"
#include "maindialog.h"
#include "ipcserver.h"

#include <ntqsocket.h>
#include <ntqlabel.h>

static const TQ_UINT16 ipcPort = 54923;

StartUp::StartUp()
{
    remoteCtrl = 0;
    mainDialog = 0;

    socket = new TQSocket( this );
    connect( socket, TQ_SIGNAL(connected()), TQ_SLOT(startRemoteCtrl()) );
    connect( socket, TQ_SIGNAL(error(int)), TQ_SLOT(startMainDialog()) );
    socket->connectToHost( "localhost", ipcPort );
}

StartUp::~StartUp()
{
    delete remoteCtrl;
    delete mainDialog;
}

void StartUp::startRemoteCtrl()
{
    remoteCtrl = new RemoteCtrlImpl( socket );
    remoteCtrl->show();
}

void StartUp::startMainDialog()
{
    mainDialog = new MainDialog();
    mainDialog->show();

    IpcServer *server = new IpcServer( ipcPort, this );

    connect( server, TQ_SIGNAL(receivedText(const TQString&)),
	    mainDialog->description, TQ_SLOT(setText(const TQString&)) );
    connect( server, TQ_SIGNAL(receivedPixmap(const TQPixmap&)),
	    mainDialog->image, TQ_SLOT(setPixmap(const TQPixmap&)) );
}
