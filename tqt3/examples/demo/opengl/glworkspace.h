#ifndef GLWORKSPACE_H
#define GLWORKSPACE_H

#include <ntqmainwindow.h>

class TQWorkspace;
class TQPrinter;

class GLWorkspace : public TQMainWindow
{
    TQ_OBJECT
public:
    GLWorkspace( TQWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~GLWorkspace();

protected:
    void setupSceneActions();

protected slots:
    void newWirebox();
    void newGear();
    void newTexture();
    void newNurbs();
    void filePrint( int x, int y );
    void filePrintWindowRes();
    void filePrintLowRes();
    void filePrintMedRes();
    void filePrintHighRes();
    bool filePrintSetup();
    void fileClose();

private:
    TQWorkspace *workspace;
    TQPrinter *printer;
};

#endif //GLWORKSPACE_H
