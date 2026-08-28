unix {
	!xinerama:DEFINES += TQT_NO_XINERAMA
	!xshape:DEFINES += TQT_NO_SHAPE
	!xcursor:DEFINES += TQT_NO_XCURSOR
	!xrandr:DEFINES += TQT_NO_XRANDR
	!xrender:DEFINES += TQT_NO_XRENDER
	!xftfreetype:DEFINES += TQT_NO_XFTFREETYPE
	!xkb:DEFINES += TQT_NO_XKB
	xft2header:DEFINES+=QT_USE_XFT2_HEADER

	PRECOMPILED_HEADER = kernel/qt_pch.h
        
        SOURCES += $$KERNEL_CPP/tqttdeintegration_x11.cpp
        HEADERS += $$KERNEL_H/tqttdeintegration_x11_p.h
}

nas {
	DEFINES     += QT_NAS_SUPPORT
	LIBS	+= -laudio -lXt
}

!x11sm:DEFINES += TQT_NO_SM_SUPPORT
