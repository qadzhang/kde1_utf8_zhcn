TEMPLATE	= app
TARGET		= qdir

CONFIG		+= qt warn_on release
DEPENDPATH	= ../../include

REQUIRES	= full-config

HEADERS		= ntqdir.h ../dirview/dirview.h
SOURCES		= qdir.cpp ../dirview/dirview.cpp
