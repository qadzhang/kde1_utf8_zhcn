#ifndef STARTUP_H
#define STARTUP_H

#include <ntqobject.h>

class TQSocket;
class RemoteCtrlImpl;
class MainDialog;

class StartUp : public TQObject
{
    TQ_OBJECT

public:
    StartUp();
    ~StartUp();

private slots:
    void startRemoteCtrl();
    void startMainDialog();

private:
    TQSocket *socket;
    RemoteCtrlImpl *remoteCtrl;
    MainDialog *mainDialog;
};

#endif // STARTUP_H
