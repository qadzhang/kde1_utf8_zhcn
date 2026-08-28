TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= tqvfb.h tqvfbview.h tqvfbratedlg.h qanimationwriter.h \
		  gammaview.h skin.h
SOURCES		= tqvfb.cpp tqvfbview.cpp tqvfbratedlg.cpp \
		  main.cpp qanimationwriter.cpp skin.cpp
INTERFACES	= config.ui
IMAGES		= images/logo.png
TARGET		= tqvfb
INCLUDEPATH	+= $$QT_SOURCE_TREE/src/3rdparty/libpng $$QT_SOURCE_TREE/src/3rdparty/zlib
DEPENDPATH	= ../../include
