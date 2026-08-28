/****************************************************************
**
** Definition of MainWindow class, translation tutorial 3
**
****************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ntqmainwindow.h>

class MainWindow : public TQMainWindow
{
    TQ_OBJECT
public:
    MainWindow( TQWidget *parent = 0, const char *name = 0 );

private slots:
    void about();
    void aboutTQt();
};

#endif
