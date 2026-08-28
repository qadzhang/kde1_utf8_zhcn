#ifndef IPCSERVER_H
#define IPCSERVER_H

#include <ntqserversocket.h>

class IpcServer : public TQServerSocket
{
    TQ_OBJECT

public:
    IpcServer( TQ_UINT16 port, TQObject *parent );

    void newConnection( int socket );

signals:
    void receivedText( const TQString& );
    void receivedPixmap( const TQPixmap& );
};

#endif // IPCSERVER_H
