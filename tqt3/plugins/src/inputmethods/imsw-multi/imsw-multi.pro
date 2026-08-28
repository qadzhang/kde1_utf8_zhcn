TEMPLATE = lib
TARGET   = qimsw-multi
DESTDIR  = ../../../inputmethods

INCLUDEPATH += .
CONFIG      += qt warn_on plugin
target.path += $$plugins.path/inputmethods
INSTALLS    += target

# Input
HEADERS += qmultiinputcontext.h \
           qmultiinputcontextplugin.h
SOURCES += qmultiinputcontext.cpp \
           qmultiinputcontextplugin.cpp
