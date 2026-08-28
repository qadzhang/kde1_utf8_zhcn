TEMPLATE = subdirs

CONFIG  += ordered

SUBDIRS	=  uic \
	   uilib \
	   designer \
	   app \
	   tools/tqtcreatecw \
	   tools/tqtconv2ui

dll:SUBDIRS *=  editor plugins
shared:SUBDIRS *=  editor plugins
REQUIRES=full-config nocrosscompiler
