#ifndef I18N_H
#define I18N_H

#include <ntqmainwindow.h>

class TQWorkspace;
class TQAction;
class TQPopupMenu;
class Wrapper;


class I18nDemo : public TQMainWindow
{
    TQ_OBJECT

public:
    I18nDemo(TQWidget *, const char * = 0);
    ~I18nDemo();

    void initActions();
    void initMenuBar();

    void showEvent(TQShowEvent *);
    void hideEvent(TQHideEvent *);

    TQWorkspace *workspace;
    TQAction *actionClose, *actionCloseAll, *actionTile, *actionCascade;
    TQPopupMenu *windowMenu, *newMenu;
    Wrapper *lastwrapper;


public slots:
    void newSlot(int);
    void windowSlot(int);
    void windowActivated(TQWidget *);
    void closeSlot();
    void closeAllSlot();
    void tileSlot();
    void cascadeSlot();
    void wrapperDead();
};


#endif // I18N_H
