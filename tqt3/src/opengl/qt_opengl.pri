# TQt opengl module

opengl {
	HEADERS += $$OPENGL_H/ntqgl.h \
		   $$OPENGL_H/ntqglcolormap.h
	SOURCES	+= $$OPENGL_CPP/qgl.cpp \
		   $$OPENGL_CPP/qglcolormap.cpp
	x11 {
		HEADERS += $$OPENGL_H/qgl_x11_p.h
		SOURCES += $$OPENGL_CPP/qgl_x11.cpp
	}
	else:mac:SOURCES += $$OPENGL_CPP/qgl_mac.cpp
	else:win32:SOURCES += $$OPENGL_CPP/qgl_win.cpp

	dlopen_opengl:DEFINES+=QT_DLOPEN_OPENGL
}


