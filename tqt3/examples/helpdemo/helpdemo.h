#ifndef HELPDEMO_H
#define HELPDEMO_H

#include <ntqptrdict.h>

#include "helpdemobase.h"

class TQAssistantClient;
class TQPopupMenu;

class HelpDemo : public HelpDemoBase
{
    TQ_OBJECT

public:
    HelpDemo( TQWidget *parent = 0, const char *name = 0 );
    ~HelpDemo();

protected:
    void contextMenuEvent( TQContextMenuEvent *e );

private slots:
    void setAssistantArguments();
    void openAssistant();
    void closeAssistant();
    void displayPage();
    void showAssistantErrors( const TQString &err );
    void assistantOpened();
    void assistantClosed();
    void showHelp();

private:
    TQWidget* lookForWidget();
    void showHelp( TQWidget *w );

    TQPtrDict<char> widgets;
    TQAssistantClient *assistant;
    TQPopupMenu *menu;

};

#endif
