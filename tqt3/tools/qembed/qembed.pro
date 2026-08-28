CONFIG += console qt warn_on release
SOURCES = qembed.cpp
TARGET  = tqembed
REQUIRES=full-config nocrosscompiler

target.path = $$bins.path
INSTALLS += target
