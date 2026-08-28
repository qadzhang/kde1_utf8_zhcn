TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		=
SOURCES		= main.cpp
INTERFACES	=
TARGET		= maketqpf
REQUIRES=full-config nocrosscompiler

target.path = $$bins.path
INSTALLS += target
