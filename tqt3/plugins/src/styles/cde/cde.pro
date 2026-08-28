TEMPLATE = lib
TARGET	 = qcdestyle

CONFIG  += qt warn_off release plugin
DESTDIR	 = ../../../styles

HEADERS		= ../../../../include/ntqcdestyle.h
SOURCES		= main.cpp \
		  ../../../../src/styles/qcdestyle.cpp

!contains(styles, motif) {
	HEADERS += ../../../../include/ntqmotifstyle.h
	SOURCES += ../../../../src/styles/qmotifstyle.cpp
}

unix:OBJECTS_DIR	= .obj
win32:OBJECTS_DIR	= obj

target.path += $$plugins.path/styles
INSTALLS += target
