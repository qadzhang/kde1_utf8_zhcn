#ifndef REMOTECTRLIMPL_H
#define REMOTECTRLIMPL_H

#include "remotectrl.h"

class TQSocket;

class RemoteCtrlImpl : public RemoteCtrl
{
    TQ_OBJECT

public:
    RemoteCtrlImpl( TQSocket * );

private slots:
    void sendImage();
    void sendText();
    void sendPalette();

private:
    void sendPacket( const TQVariant & );

    TQSocket *socket;
};

#endif // REMOTECTRLIMPL_H
