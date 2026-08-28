# TQt styles module

styles {
	STYLES_P = styles
	HEADERS +=$$STYLES_H/ntqstylefactory.h \
		  $$STYLES_P/qstyleinterface_p.h \
		  $$STYLES_H/ntqstyleplugin.h \
		  $$STYLES_H/ntqcommonstyle.h
	SOURCES +=$$STYLES_CPP/qstylefactory.cpp \
		  $$STYLES_CPP/qstyleplugin.cpp \
		  $$STYLES_CPP/qcommonstyle.cpp

	contains( styles, all ) {
		styles += mac cde motifplus sgi platinum compact interlace windows motif
	}

	x11|embedded|!macx-*:styles -= mac
	contains( styles, mac ) {
		HEADERS +=$$STYLES_H/qmacstyle_mac.h \
                          $$STYLES_H/qmacstylepixmaps_mac_p.h
		SOURCES +=$$STYLES_CPP/qmacstyle_mac.cpp
		HEADERS *= $$STYLES_CPP/qaquastyle_p.h 
		SOURCES *= $$STYLES_CPP/qaquastyle_p.cpp 

		!contains( styles, windows ) {
			message( mac requires windows )
			styles += windows
		}
        }
	else:DEFINES += TQT_NO_STYLE_MAC

	#embedded|!macx-*:styles -= aqua
	contains( styles, aqua ) {
		HEADERS += $$STYLES_H/qaquastyle.h 
		SOURCES += $$STYLES_CPP/qaquastyle.cpp 
		HEADERS *= $$STYLES_CPP/qaquastyle_p.h 
		SOURCES *= $$STYLES_CPP/qaquastyle_p.cpp 

		!contains( styles, windows ) {
			message( aqua requires windows )
			styles += windows
		}
	}
	else:DEFINES += TQT_NO_STYLE_AQUA

	contains( styles, cde ) {
		HEADERS +=$$STYLES_H/ntqcdestyle.h
		SOURCES +=$$STYLES_CPP/qcdestyle.cpp

		!contains( styles, motif ) {
			message( cde requires motif )
			styles += motif
		}
	}
	else:DEFINES += TQT_NO_STYLE_CDE

	contains( styles, motifplus ) {
		HEADERS +=$$STYLES_H/ntqmotifplusstyle.h
		SOURCES +=$$STYLES_CPP/qmotifplusstyle.cpp
		!contains( styles, motif ) {
			message( motifplus requires motif )
			styles += motif
		}
	}
	else:DEFINES += TQT_NO_STYLE_MOTIFPLUS

	contains( styles, interlace ) {
		HEADERS +=$$STYLES_H/ntqinterlacestyle.h
		SOURCES +=$$STYLES_CPP/qinterlacestyle.cpp
		!contains( styles, windows ) {
			message( interlace requires windows )
			styles += windows
		}
	}
	else:DEFINES += TQT_NO_STYLE_INTERLACE

	contains( styles, platinum ) {
		HEADERS +=$$STYLES_H/ntqplatinumstyle.h
		SOURCES +=$$STYLES_CPP/qplatinumstyle.cpp
		!contains( styles, windows ) {
			message( platinum requires windows )
			styles += windows
		}
	}
	else:DEFINES += TQT_NO_STYLE_PLATINUM

	contains( styles, windowsxp ) {
		HEADERS +=$$STYLES_H/qwindowsxpstyle.h
		SOURCES +=$$STYLES_CPP/qwindowsxpstyle.cpp
		!contains( styles, windowsxp ) {
			message( windowsxp requires windows )
			styles += windows
		}
        }
        else:DEFINES += TQT_NO_STYLE_WINDOWSXP

	contains( styles, sgi ) {
		HEADERS +=$$STYLES_H/ntqsgistyle.h
		SOURCES +=$$STYLES_CPP/qsgistyle.cpp
		!contains( styles, motif ) {
			message( sgi requires motif )
			styles += motif
		}
	}
	else:DEFINES += TQT_NO_STYLE_SGI

	contains( styles, compact ) {
		HEADERS +=$$STYLES_H/ntqcompactstyle.h
		SOURCES +=$$STYLES_CPP/qcompactstyle.cpp
		!contains( styles, windows ) {
			message( compact requires windows )
			styles += windows
		}
	}
	else:DEFINES += TQT_NO_STYLE_COMPACT

        wince-*:styles += pocketpc
	contains( styles, pocketpc ) {
		HEADERS +=$$STYLES_H/qpocketpcstyle_wce.h
		SOURCES +=$$STYLES_CPP/qpocketpcstyle_wce.cpp
		
		!contains( styles, windows ) {
			message( pocketpc requires windows )
			styles += windows
		}
	}
        else:DEFINES += TQT_NO_STYLE_POCKETPC
        
	contains( styles, windows ) {
		HEADERS +=$$STYLES_H/ntqwindowsstyle.h
		SOURCES +=$$STYLES_CPP/qwindowsstyle.cpp
	}
	else:DEFINES += TQT_NO_STYLE_WINDOWS

	contains( styles, motif ) {
		HEADERS +=$$STYLES_H/ntqmotifstyle.h
		SOURCES +=$$STYLES_CPP/qmotifstyle.cpp
	}
	else:DEFINES += TQT_NO_STYLE_MOTIF
}
