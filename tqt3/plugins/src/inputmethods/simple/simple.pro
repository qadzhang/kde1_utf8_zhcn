TEMPLATE = lib
TARGET = qsimple
DESTDIR  = ../../../inputmethods

INCLUDEPATH += .
CONFIG      += qt warn_on plugin
target.path += $$plugins.path/inputmethods
INSTALLS    += target

# Input
HEADERS += qsimpleinputcontext.h \
           qsimpleinputcontextplugin.h
SOURCES += qsimpleinputcontext.cpp \
           qsimpleinputcontextplugin.cpp \
           composetable.cpp
