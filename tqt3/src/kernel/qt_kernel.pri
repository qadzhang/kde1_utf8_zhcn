# TQt kernel module

kernel {
	KERNEL_P	= kernel
	HEADERS += $$KERNEL_H/ntqabstractlayout.h \
		  $$KERNEL_H/ntqaccel.h \
		  $$KERNEL_P/qucomextra_p.h \
		  $$KERNEL_H/ntqapplication.h \
		  $$KERNEL_P/qapplication_p.h \
		  $$KERNEL_H/ntqasyncimageio.h \
		  $$KERNEL_H/ntqasyncio.h \
		  $$KERNEL_H/ntqbitmap.h \
		  $$KERNEL_H/ntqbrush.h \
		  $$KERNEL_H/ntqclipboard.h \
		  $$KERNEL_H/ntqcolor.h \
		  $$KERNEL_P/qcolor_p.h \
		  $$KERNEL_H/ntqconnection.h \
		  $$KERNEL_H/ntqcursor.h \
		  $$KERNEL_H/ntqdesktopwidget.h \
		  $$KERNEL_H/ntqdragobject.h \
		  $$KERNEL_H/ntqdrawutil.h \
		  $$KERNEL_H/ntqdropsite.h \
		  $$KERNEL_H/ntqevent.h \
		  $$KERNEL_H/ntqeventloop.h \
		  $$KERNEL_P/qeventloop_p.h \
		  $$KERNEL_P/qeventloop_glib_p.h \
		  $$KERNEL_H/ntqfocusdata.h \
		  $$KERNEL_H/ntqfont.h \
		  $$KERNEL_P/qfontdata_p.h \
		  $$KERNEL_H/ntqfontinfo.h \
		  $$KERNEL_H/ntqfontmetrics.h \
		  $$KERNEL_H/ntqguardedptr.h \
		  $$KERNEL_H/ntqgif.h \
		  $$KERNEL_H/ntqiconset.h \
		  $$KERNEL_H/ntqimage.h \
		  $$KERNEL_P/qimageformatinterface_p.h \
		  $$KERNEL_H/ntqimageformatplugin.h \
		  $$KERNEL_H/ntqkeycode.h \
		  $$KERNEL_H/ntqkeysequence.h \
		  $$KERNEL_H/ntqlayout.h \
		  $$KERNEL_P/qlayoutengine_p.h \
		  $$KERNEL_H/ntqtranslator.h \
		  $$KERNEL_H/ntqmetaobject.h \
		  $$KERNEL_H/ntqmime.h \
		  $$KERNEL_H/ntqmovie.h \
		  $$KERNEL_H/ntqnamespace.h \
		  $$KERNEL_H/ntqnetworkprotocol.h \
		  $$KERNEL_H/ntqobject.h \
 		  $$KERNEL_H/ntqobjectcleanuphandler.h \
		  $$KERNEL_H/ntqobjectdefs.h \
		  $$KERNEL_H/ntqobjectdict.h \
		  $$KERNEL_H/ntqobjectlist.h \
		  $$KERNEL_H/ntqpaintdevice.h \
		  $$KERNEL_H/ntqpainter.h \
		  $$KERNEL_P/qpainter_p.h \
		  $$KERNEL_H/ntqpalette.h \
		  $$KERNEL_H/ntqpaintdevicemetrics.h \
		  $$KERNEL_H/ntqpen.h \
		  $$KERNEL_H/ntqpicture.h \
		  $$KERNEL_H/ntqpixmap.h \
		  $$KERNEL_H/ntqpixmapcache.h \
		  $$KERNEL_H/ntqpointarray.h \
		  $$KERNEL_H/ntqpoint.h \
		  $$KERNEL_H/ntqpolygonscanner.h \
		  $$KERNEL_H/ntqprinter.h \
		  $$KERNEL_H/ntqprocess.h \
		  $$KERNEL_H/ntqrect.h \
		  $$KERNEL_H/ntqregion.h \
		  $$KERNEL_H/ntqsessionmanager.h \
		  $$KERNEL_H/ntqsignal.h \
		  $$KERNEL_H/ntqsignalmapper.h \
		  $$KERNEL_H/ntqsignalslotimp.h \
		  $$KERNEL_H/ntqsize.h \
		  $$KERNEL_H/ntqsizegrip.h \
		  $$KERNEL_H/ntqsizepolicy.h \
		  $$KERNEL_H/ntqsocketnotifier.h \
		  $$KERNEL_H/ntqsound.h \
		  $$KERNEL_H/ntqstyle.h \
		  $$KERNEL_H/ntqstylesheet.h \
		  $$KERNEL_H/ntqthread.h \
		  $$KERNEL_H/ntqtimer.h \
		  $$KERNEL_H/ntqurl.h \
		  $$KERNEL_H/ntqlocalfs.h \
		  $$KERNEL_H/ntqurloperator.h \
		  $$KERNEL_H/ntqurlinfo.h \
		  $$KERNEL_H/ntqwidget.h \
		  $$KERNEL_H/ntqwidgetintdict.h \
		  $$KERNEL_H/ntqwidgetlist.h \
		  $$KERNEL_H/ntqwindowdefs.h \
		  $$KERNEL_H/ntqwmatrix.h \
		  $$KERNEL_H/ntqvariant.h \
		  $$KERNEL_P/qrichtext_p.h \
		  $$KERNEL_P/qinternal_p.h \
		  $$KERNEL_H/ntqgplugin.h \
		  $$KERNEL_H/ntqsimplerichtext.h \
		  $$KERNEL_CPP/qscriptengine_p.h \
		  $$KERNEL_CPP/qtextengine_p.h \
		  $$KERNEL_CPP/qfontengine_p.h \
		  $$KERNEL_CPP/qtextlayout_p.h

	inputmethod {
		unix:x11 {
			HEADERS += $$KERNEL_H/ntqinputcontext.h
		} else {
			HEADERS += $$KERNEL_P/qinputcontext_p.h
		}
	}

		  glibmainloop {
		  		HEADERS+=$$KERNEL_P/qeventloop_glib_p.h
		  } 
		  
	win32:SOURCES += $$KERNEL_CPP/qapplication_win.cpp \
		  $$KERNEL_CPP/qclipboard_win.cpp \
		  $$KERNEL_CPP/qcolor_win.cpp \
		  $$KERNEL_CPP/qcursor_win.cpp \
		  $$KERNEL_CPP/qdesktopwidget_win.cpp \
		  $$KERNEL_CPP/qdnd_win.cpp \
		  $$KERNEL_CPP/qeventloop_win.cpp \
		  $$KERNEL_CPP/qfont_win.cpp \
		  $$KERNEL_CPP/qinputcontext_win.cpp \
		  $$KERNEL_CPP/qmime_win.cpp \
		  $$KERNEL_CPP/qpixmap_win.cpp \
		  $$KERNEL_CPP/qprinter_win.cpp \
		  $$KERNEL_CPP/qprocess_win.cpp \
		  $$KERNEL_CPP/qpaintdevice_win.cpp \
		  $$KERNEL_CPP/qpainter_win.cpp \
		  $$KERNEL_CPP/qregion_win.cpp \
		  $$KERNEL_CPP/qsound_win.cpp \
		  $$KERNEL_CPP/qthread_win.cpp \
		  $$KERNEL_CPP/qwidget_win.cpp \
		  $$KERNEL_CPP/qole_win.c \
		  $$KERNEL_CPP/qfontengine_win.cpp

	unix:x11 {
	      SOURCES += $$KERNEL_CPP/qapplication_x11.cpp \
		          $$KERNEL_CPP/qclipboard_x11.cpp \
			  $$KERNEL_CPP/qcolor_x11.cpp \
			  $$KERNEL_CPP/qcursor_x11.cpp \
			  $$KERNEL_CPP/qdnd_x11.cpp \
			  $$KERNEL_CPP/qdesktopwidget_x11.cpp \
			  $$KERNEL_CPP/qfont_x11.cpp \
			  $$KERNEL_CPP/qinputcontext.cpp \
			  $$KERNEL_CPP/qinputcontext_x11.cpp \
			  $$KERNEL_CPP/qmotifdnd_x11.cpp \
			  $$KERNEL_CPP/qpixmap_x11.cpp \
			  $$KERNEL_CPP/qpaintdevice_x11.cpp \
			  $$KERNEL_CPP/qpainter_x11.cpp \
			  $$KERNEL_CPP/qregion_x11.cpp \
			  $$KERNEL_CPP/qsound_x11.cpp \
			  $$KERNEL_CPP/qwidget_x11.cpp \
			  $$KERNEL_CPP/qwidgetcreate_x11.cpp \
		          $$KERNEL_CPP/qfontengine_x11.cpp
             glibmainloop {
                  SOURCES += $$KERNEL_CPP/qeventloop_x11_glib.cpp
             } else {
                  SOURCES += $$KERNEL_CPP/qeventloop_x11.cpp
             }

	}

	!x11:mac {
	    exists(qsound_mac.cpp):SOURCES += $$KERNEL_CPP/qsound_mac.cpp
	    else:SOURCES += $$KERNEL_CPP/qsound_qws.cpp
	}
        !embedded:!x11:mac {
	      SOURCES += $$KERNEL_CPP/qapplication_mac.cpp \
		          $$KERNEL_CPP/qclipboard_mac.cpp \
			  $$KERNEL_CPP/qcolor_mac.cpp \
			  $$KERNEL_CPP/qcursor_mac.cpp \
			  $$KERNEL_CPP/qmime_mac.cpp \
			  $$KERNEL_CPP/qdnd_mac.cpp \
			  $$KERNEL_CPP/qdesktopwidget_mac.cpp \
			  $$KERNEL_CPP/qpixmap_mac.cpp \
			  $$KERNEL_CPP/qprinter_mac.cpp \
			  $$KERNEL_CPP/qpaintdevice_mac.cpp \
			  $$KERNEL_CPP/qpainter_mac.cpp \
			  $$KERNEL_CPP/qregion_mac.cpp \
			  $$KERNEL_CPP/qwidget_mac.cpp \
			  $$KERNEL_CPP/qeventloop_mac.cpp \
			  $$KERNEL_CPP/qfont_mac.cpp \
			  $$KERNEL_CPP/qfontengine_mac.cpp
             DEFINES += QMAC_ONE_PIXEL_LOCK
        } else:unix {
	   SOURCES += $$KERNEL_CPP/qprinter_unix.cpp \
	              $$KERNEL_CPP/qpsprinter.cpp
           glibmainloop {
              SOURCES += $$KERNEL_CPP/qeventloop_unix_glib.cpp
           } else {
	      SOURCES += $$KERNEL_CPP/qeventloop_unix.cpp
           }

        }
	unix:SOURCES += $$KERNEL_CPP/qprocess_unix.cpp \
		        $$KERNEL_CPP/qthread_unix.cpp

	SOURCES += $$KERNEL_CPP/qabstractlayout.cpp \
		  $$KERNEL_CPP/qucomextra.cpp \
		  $$KERNEL_CPP/qaccel.cpp \
		  $$KERNEL_CPP/qapplication.cpp \
		  $$KERNEL_CPP/qasyncimageio.cpp \
		  $$KERNEL_CPP/qasyncio.cpp \
		  $$KERNEL_CPP/qbitmap.cpp \
		  $$KERNEL_CPP/qclipboard.cpp \
		  $$KERNEL_CPP/qcolor.cpp \
		  $$KERNEL_CPP/qcolor_p.cpp \
		  $$KERNEL_CPP/qconnection.cpp \
		  $$KERNEL_CPP/qcursor.cpp \
		  $$KERNEL_CPP/qdragobject.cpp \
		  $$KERNEL_CPP/qdrawutil.cpp \
		  $$KERNEL_CPP/qdropsite.cpp \
		  $$KERNEL_CPP/qevent.cpp \
		  $$KERNEL_CPP/qeventloop.cpp \
		  $$KERNEL_CPP/qfocusdata.cpp \
		  $$KERNEL_CPP/qfont.cpp \
		  $$KERNEL_CPP/qfontdatabase.cpp \
		  $$KERNEL_CPP/qguardedptr.cpp \
		  $$KERNEL_CPP/qiconset.cpp \
		  $$KERNEL_CPP/qimage.cpp \
		  $$KERNEL_CPP/qimageformatplugin.cpp \
		  $$KERNEL_CPP/qkeysequence.cpp \
		  $$KERNEL_CPP/qlayout.cpp \
		  $$KERNEL_CPP/qlayoutengine.cpp \
		  $$KERNEL_CPP/qtranslator.cpp \
		  $$KERNEL_CPP/qmetaobject.cpp \
		  $$KERNEL_CPP/qmime.cpp \
		  $$KERNEL_CPP/qmovie.cpp \
		  $$KERNEL_CPP/qnetworkprotocol.cpp \
		  $$KERNEL_CPP/qobject.cpp \
		  $$KERNEL_CPP/qobjectcleanuphandler.cpp \
		  $$KERNEL_CPP/qpainter.cpp \
		  $$KERNEL_CPP/qpalette.cpp \
		  $$KERNEL_CPP/qpaintdevicemetrics.cpp \
		  $$KERNEL_CPP/qpicture.cpp \
		  $$KERNEL_CPP/qpixmap.cpp \
		  $$KERNEL_CPP/qpixmapcache.cpp \
		  $$KERNEL_CPP/qpointarray.cpp \
		  $$KERNEL_CPP/qpoint.cpp \
		  $$KERNEL_CPP/qpolygonscanner.cpp \
		  $$KERNEL_CPP/qprinter.cpp \
		  $$KERNEL_CPP/qprocess.cpp \
		  $$KERNEL_CPP/qrect.cpp \
		  $$KERNEL_CPP/qregion.cpp \
		  $$KERNEL_CPP/qsignal.cpp \
		  $$KERNEL_CPP/qsignalmapper.cpp \
		  $$KERNEL_CPP/qsize.cpp \
		  $$KERNEL_CPP/qsizegrip.cpp \
		  $$KERNEL_CPP/qstyle.cpp \
		  $$KERNEL_CPP/qsocketnotifier.cpp \
		  $$KERNEL_CPP/qsound.cpp \
		  $$KERNEL_CPP/qstylesheet.cpp \
		  $$KERNEL_CPP/qthread.cpp \
		  $$KERNEL_CPP/qtimer.cpp \
		  $$KERNEL_CPP/qurl.cpp \
		  $$KERNEL_CPP/qlocalfs.cpp \
		  $$KERNEL_CPP/qurloperator.cpp \
		  $$KERNEL_CPP/qurlinfo.cpp \
		  $$KERNEL_CPP/qwidget.cpp \
		  $$KERNEL_CPP/qwmatrix.cpp \
		  $$KERNEL_CPP/qvariant.cpp \
		  $$KERNEL_CPP/qrichtext.cpp \
		  $$KERNEL_CPP/qinternal.cpp \
		  $$KERNEL_CPP/qrichtext_p.cpp \
		  $$KERNEL_CPP/qgplugin.cpp \
		  $$KERNEL_CPP/qsimplerichtext.cpp \
		  $$KERNEL_CPP/qscriptengine.cpp \
		  $$KERNEL_CPP/qtextlayout.cpp \
		  $$KERNEL_CPP/qtextengine.cpp

	unix:HEADERS   += $$KERNEL_P/qpsprinter_p.h \
			  $$KERNEL_H/ntqfontdatabase.h

	embedded:SOURCES += $$KERNEL_CPP/qsharedmemory_p.cpp \
		  	    $$KERNEL_CPP/qfontengine_qws.cpp

	accessibility {
	      HEADERS += $$KERNEL_H/ntqaccessible.h
	      SOURCES += $$KERNEL_CPP/qaccessible.cpp

	      !embedded:!x11:mac:SOURCES += $$KERNEL_CPP/qaccessible_mac.cpp
	      else:win32:SOURCES += $$KERNEL_CPP/qaccessible_win.cpp
	}
}
