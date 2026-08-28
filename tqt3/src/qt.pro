# TQt project file
TEMPLATE	= lib
TARGET		= qt
embedded:TARGET	= qte
VERSION		= 3.5
DESTDIR		= $$QMAKE_LIBDIR_TQT
DLLDESTDIR	= ../bin

CONFIG		+= qt warn_on depend_includepath
CONFIG          += qmake_cache

win32:!shared:CONFIG += staticlib

linux-*:version_script {
   QMAKE_LFLAGS += -Wl,--version-script=libtqt.map
   TARGETDEPS += libtqt.map
}

KERNEL_CPP	= kernel
CANVAS_CPP      = canvas
WIDGETS_CPP	= widgets
SQL_CPP	        = sql
TABLE_CPP	= table
DIALOGS_CPP	= dialogs
NETWORK_CPP	= network
OPENGL_CPP	= opengl
TOOLS_CPP	= tools
CODECS_CPP	= codecs
WORKSPACE_CPP	= workspace
INPUTMETHOD_CPP	= inputmethod
XML_CPP	        = xml
STYLES_CPP	= styles
EMBEDDED_CPP	= embedded

win32 {
	contains(QT_PRODUCT,qt-internal) {
		SQL_H		= $$SQL_CPP
		KERNEL_H	= $$KERNEL_CPP
		WIDGETS_H	= $$WIDGETS_CPP
		TABLE_H		= $$TABLE_CPP
		DIALOGS_H	= $$DIALOGS_CPP
		NETWORK_H	= $$NETWORK_CPP
		OPENGL_H	= $$OPENGL_CPP
		TOOLS_H		= $$TOOLS_CPP
		CODECS_H	= $$CODECS_CPP
		WORKSPACE_H	= $$WORKSPACE_CPP
                #INPUTMETHOD_H 	= $$INPUTMETHOD_CPP
		XML_H		= $$XML_CPP
		CANVAS_H	= $$CANVAS_CPP
		STYLES_H	= $$STYLES_CPP
	} else {
		WIN_ALL_H = ../include
		SQL_H		= $$WIN_ALL_H
		KERNEL_H	= $$WIN_ALL_H
		WIDGETS_H	= $$WIN_ALL_H
		TABLE_H		= $$WIN_ALL_H
		DIALOGS_H	= $$WIN_ALL_H
		NETWORK_H	= $$WIN_ALL_H
		OPENGL_H	= $$WIN_ALL_H
		TOOLS_H		= $$WIN_ALL_H
		CODECS_H	= $$WIN_ALL_H
		WORKSPACE_H	= $$WIN_ALL_H
                #INPUTMETHOD_H 	= $$WIN_ALL_H
		XML_H		= $$WIN_ALL_H
		CANVAS_H	= $$WIN_ALL_H
		STYLES_H	= $$WIN_ALL_H
		CONFIG 		-= incremental
	}

	CONFIG	+= zlib
	INCLUDEPATH += tmp
	!staticlib {
	    DEFINES+=QT_MAKEDLL
	    exists(qt.rc):RC_FILE = qt.rc
	}
}

unix {
	CANVAS_H	= $$CANVAS_CPP
	KERNEL_H	= $$KERNEL_CPP
	WIDGETS_H	= $$WIDGETS_CPP
	SQL_H		= $$SQL_CPP
	TABLE_H		= $$TABLE_CPP
	DIALOGS_H	= $$DIALOGS_CPP
	NETWORK_H	= $$NETWORK_CPP
	OPENGL_H	= $$OPENGL_CPP
	TOOLS_H		= $$TOOLS_CPP
	CODECS_H	= $$CODECS_CPP
	WORKSPACE_H	= $$WORKSPACE_CPP
        INPUTMETHOD_H 	= $$INPUTMETHOD_CPP
	XML_H		= $$XML_CPP
	STYLES_H	= $$STYLES_CPP
	!embedded:!mac:CONFIG	   += x11 x11inc
}

aix-g++ {
	QMAKE_CFLAGS   += -mminimal-toc
	QMAKE_CXXFLAGS += -mminimal-toc
}

embedded {
	EMBEDDED_H	= $$EMBEDDED_CPP
}

DEPENDPATH += ;$$NETWORK_H;$$KERNEL_H;$$WIDGETS_H;$$INPUTMETHOD_H;$$SQL_H;$$TABLE_H;$$DIALOGS_H;
DEPENDPATH += $$OPENGL_H;$$TOOLS_H;$$CODECS_H;$$WORKSPACE_H;$$XML_H;
DEPENDPATH += $$CANVAS_H;$$STYLES_H
embedded:DEPENDPATH += ;$$EMBEDDED_H

thread {
	TARGET = tqt-mt
	DEFINES += TQT_THREAD_SUPPORT
}

!cups:DEFINES += TQT_NO_CUPS

!nis:DEFINES += TQT_NO_NIS

largefile {
	unix:!darwin:DEFINES += _LARGEFILE_SOURCE _LARGE_FILES _FILE_OFFSET_BITS=64
}

#here for compatability, should go away ####
include($$KERNEL_CPP/qt_compat.pri)

#platforms
x11:include($$KERNEL_CPP/qt_x11.pri)
mac:include($$KERNEL_CPP/qt_mac.pri)
win32:include($$KERNEL_CPP/qt_win.pri)
embedded:include($$KERNEL_CPP/qt_qws.pri)

#modules
include($$KERNEL_CPP/qt_kernel.pri)
include($$WIDGETS_CPP/qt_widgets.pri)
include($$DIALOGS_CPP/qt_dialogs.pri)
include($$WORKSPACE_CPP/qt_workspace.pri)
include($$INPUTMETHOD_CPP/qt_inputmethod.pri)
include($$NETWORK_CPP/qt_network.pri)
include($$CANVAS_CPP/qt_canvas.pri)
include($$TABLE_CPP/qt_table.pri)
include($$XML_CPP/qt_xml.pri)
include($$OPENGL_CPP/qt_opengl.pri)
include($$SQL_CPP/qt_sql.pri)
include($$KERNEL_CPP/qt_gfx.pri)
include($$TOOLS_CPP/qt_tools.pri)
include($$CODECS_CPP/qt_codecs.pri)
include($$STYLES_CPP/qt_styles.pri)
embedded:include($$EMBEDDED_CPP/qt_embedded.pri)

# qconfig.cpp
exists($$QT_BUILD_TREE/src/tools/qconfig.cpp) {
    SOURCES += $$QT_BUILD_TREE/src/tools/qconfig.cpp
}

#install directives
include(qt_install.pri)
!staticlib:PRL_EXPORT_DEFINES += QT_SHARED

unix {
   CONFIG     += create_libtool create_pc
   QMAKE_PKGCONFIG_LIBDIR = $$target.path
   QMAKE_PKGCONFIG_INCDIR = $$headers.path
   QMAKE_PKGCONFIG_PLUGINS = $$plugins.path
}

wince-* {
	CONFIG -= incremental
	message( ...removing plugin stuff... (not permanent) )
	HEADERS -= $$TOOLS_CPP/qcomlibrary.h \
		   $$KERNEL_CPP/ntqgplugin.h \
		   $$KERNEL_CPP/ntqimageformatplugin.h \
		   $$STYLES_CPP/ntqstyleplugin.h \
		   $$CODECS_CPP/ntqtextcodecplugin.h \
		   $$WIDGETS_CPP/ntqwidgetplugin.h

	SOURCES -= $$TOOLS_CPP/qcomlibrary.cpp \
		   $$KERNEL_CPP/qgplugin.cpp \
		   $$KERNEL_CPP/qimageformatplugin.cpp \
		   $$STYLES_CPP/qstyleplugin.cpp \
		   $$CODECS_CPP/qtextcodecplugin.cpp \
		   $$WIDGETS_CPP/qwidgetplugin.cpp 
}

glibmainloop {
	DEFINES += QT_USE_GLIBMAINLOOP
	QMAKE_CFLAGS += $$QMAKE_CFLAGS_GLIB
	QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_GLIB
        LIBS += $$QMAKE_LIBS_GLIB
}

