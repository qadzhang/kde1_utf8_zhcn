/****************************************************************
**
** Definition of MainWindow class, translation tutorial 2
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
};

#endif
