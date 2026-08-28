TEMPLATE	= app
CONFIG		+= console qt warn_on release
HEADERS		=
SOURCES		= tqtmergetr.cpp
TARGET		= tqtmergetr
REQUIRES=full-config nocrosscompiler

target.path = $$bins.path
INSTALLS += target
