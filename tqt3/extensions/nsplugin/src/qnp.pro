TEMPLATE = lib
TARGET	 = qnp

CONFIG  -= dll
CONFIG  += qt x11 release staticlib
DESTDIR	 = ../../../lib
VERSION	 = 0.4

SOURCES		= qnp.cpp
unix:HEADERS   += ntqnp.h
win32:HEADERS	= ../../../include/ntqnp.h
win32:LIBS     += -lqtmain
MOC_DIR		= .
DESTINCDIR	= ../../../include
