TEMPLATE	= app
DEPENDPATH      = ../../include
CONFIG		+= console qt warn_on release
HEADERS		=
SOURCES		= msg2tqm.cpp
TARGET		= msg2tqm
REQUIRES=full-config nocrosscompiler

target.path = $$bins.path
INSTALLS += target
