TEMPLATE = app
CONFIG	+= qt warn_on
LANGUAGE = C++

SOURCES	+= colorbutton.cpp main.cpp previewframe.cpp previewwidget.cpp mainwindow.cpp paletteeditoradvanced.cpp
HEADERS	+= colorbutton.h previewframe.h previewwidget.h mainwindow.h paletteeditoradvanced.h
FORMS	= mainwindowbase.ui paletteeditoradvancedbase.ui previewwidgetbase.ui
IMAGES	= images/appicon.png

PROJECTNAME	= TQt Configuration
TARGET		= tqtconfig
DESTDIR		= ../../bin

target.path=$$bins.path

INCLUDEPATH	+= .
DBFILE		 = qtconfig.db
REQUIRES=full-config nocrosscompiler !win32*

desktop.path = $$share.path/applications
desktop.files = tqtconfig.desktop

system( cp images/appicon.png tqtconfig.png )

icon.path = $$share.path/pixmaps
icon.files = tqtconfig.png

INSTALLS += target desktop icon
