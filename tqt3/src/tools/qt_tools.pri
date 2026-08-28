# TQt tools module

tools {
	TOOLS_P		= tools
	HEADERS +=  $$TOOLS_H/ntqmemarray.h \
		  $$TOOLS_H/ntqasciicache.h \
		  $$TOOLS_H/ntqasciidict.h \
		  $$TOOLS_H/ntqbitarray.h \
		  $$TOOLS_H/ntqbuffer.h \
		  $$TOOLS_H/ntqcache.h \
		  $$TOOLS_H/ntqcleanuphandler.h \
		  $$TOOLS_P/qcomponentfactory_p.h \
		  $$TOOLS_P/qcomlibrary_p.h \
		  $$TOOLS_H/ntqcstring.h \
		  $$TOOLS_H/ntqdatastream.h \
		  $$TOOLS_H/ntqdatetime.h \
		  $$TOOLS_H/ntqdeepcopy.h \
		  $$TOOLS_H/ntqdict.h \
		  $$TOOLS_H/ntqdir.h \
		  $$TOOLS_P/qdir_p.h \
		  $$TOOLS_H/ntqfile.h \
		  $$TOOLS_P/qfiledefs_p.h \
		  $$TOOLS_H/ntqfileinfo.h \
		  $$TOOLS_H/ntqgarray.h \
		  $$TOOLS_H/ntqgcache.h \
		  $$TOOLS_H/ntqgdict.h \
		  $$TOOLS_H/ntqgeneric.h \
		  $$TOOLS_H/ntqglist.h \
		  $$TOOLS_H/ntqglobal.h \
		  $$TOOLS_P/qgpluginmanager_p.h \
		  $$TOOLS_H/ntqgvector.h \
		  $$TOOLS_H/ntqintcache.h \
		  $$TOOLS_H/ntqintdict.h \
		  $$TOOLS_H/ntqiodevice.h \
		  $$TOOLS_H/ntqlibrary.h \
		  $$TOOLS_P/qlibrary_p.h \
		  $$TOOLS_H/ntqlocale.h \
		  $$TOOLS_P/qlocale_p.h \
		  $$TOOLS_H/ntqptrlist.h \
		  $$TOOLS_H/ntqmap.h \
		  $$TOOLS_H/ntqmutex.h \
		  $$TOOLS_P/qmutex_p.h \
		  $$TOOLS_P/qmutexpool_p.h \
		  $$TOOLS_P/qpluginmanager_p.h \
		  $$TOOLS_H/ntqptrcollection.h \
		  $$TOOLS_H/ntqptrdict.h \
		  $$TOOLS_H/ntqptrqueue.h \
		  $$TOOLS_H/ntqregexp.h \
		  $$TOOLS_H/ntqsemaphore.h \
		  $$TOOLS_H/ntqsettings.h \
		  $$TOOLS_P/qsettings_p.h \
		  $$TOOLS_H/ntqshared.h \
		  $$TOOLS_H/ntqsortedlist.h \
		  $$TOOLS_H/ntqptrstack.h \
		  $$TOOLS_H/ntqstring.h \
		  $$TOOLS_H/ntqstringlist.h \
		  $$TOOLS_H/ntqstrlist.h \
		  $$TOOLS_H/ntqstrvec.h \
		  $$TOOLS_H/ntqtextstream.h \
		  $$TOOLS_P/qthreadinstance_p.h \
		  $$TOOLS_H/ntqthreadstorage.h\
		  $$TOOLS_P/qunicodetables_p.h \
		  $$TOOLS_H/ntqptrvector.h \
	          $$TOOLS_H/ntqvaluelist.h \
		  $$TOOLS_H/ntqvaluestack.h \
		  $$TOOLS_H/ntqvaluevector.h \
		  $$TOOLS_H/ntqwaitcondition.h \
		  $$TOOLS_P/qcom_p.h \
		  $$TOOLS_P/qucom_p.h \
		  $$TOOLS_H/ntquuid.h

	win32:SOURCES += $$TOOLS_CPP/qdir_win.cpp \
	 	  $$TOOLS_CPP/qfile_win.cpp \
		  $$TOOLS_CPP/qfileinfo_win.cpp \
		  $$TOOLS_CPP/qlibrary_win.cpp \
		  $$TOOLS_CPP/qsettings_win.cpp \
		  $$TOOLS_CPP/qmutex_win.cpp \
		  $$TOOLS_CPP/qwaitcondition_win.cpp \
		  $$TOOLS_CPP/qthreadstorage_win.cpp \
		  $$TOOLS_CPP/qcriticalsection_p.cpp

	wince-* {
		SOURCES -= $$TOOLS_CPP/qdir_win.cpp \
			   $$TOOLS_CPP/qfile_win.cpp \
			   $$TOOLS_CPP/qfileinfo_win.cpp
		SOURCES += $$TOOLS_CPP/qdir_wce.cpp \
			   $$TOOLS_CPP/qfile_wce.cpp \
			   $$TOOLS_CPP/qfileinfo_wce.cpp
	}

        offmac:SOURCES += $$TOOLS_CPP/qdir_mac.cpp \
		  $$TOOLS_CPP/qfile_mac.cpp \
		  $$TOOLS_CPP/qfileinfo_mac.cpp
	else:unix:SOURCES += $$TOOLS_CPP/qdir_unix.cpp \
		  $$TOOLS_CPP/qfile_unix.cpp \
		  $$TOOLS_CPP/qfileinfo_unix.cpp \
		  $$TOOLS_CPP/qmutex_unix.cpp \
		  $$TOOLS_CPP/qthreadstorage_unix.cpp \
		  $$TOOLS_CPP/qwaitcondition_unix.cpp

        mac:!x11:!embedded:SOURCES += $$TOOLS_CPP/qsettings_mac.cpp
	mac {
		SOURCES+=3rdparty/dlcompat/dlfcn.c
		INCLUDEPATH+=3rdparty/dlcompat
	    }
	unix:SOURCES += $$TOOLS_CPP/qlibrary_unix.cpp

	SOURCES += $$TOOLS_CPP/qbitarray.cpp \
		  $$TOOLS_CPP/qbuffer.cpp \
		  $$TOOLS_CPP/qcomponentfactory.cpp \
		  $$TOOLS_CPP/qcomlibrary.cpp \
		  $$TOOLS_CPP/qcstring.cpp \
		  $$TOOLS_CPP/qdatastream.cpp \
		  $$TOOLS_CPP/qdatetime.cpp \
		  $$TOOLS_CPP/qdeepcopy.cpp \
		  $$TOOLS_CPP/qdir.cpp \
		  $$TOOLS_CPP/qfile.cpp \
		  $$TOOLS_CPP/qfileinfo.cpp \
		  $$TOOLS_CPP/qgarray.cpp \
		  $$TOOLS_CPP/qgcache.cpp \
		  $$TOOLS_CPP/qgdict.cpp \
		  $$TOOLS_CPP/qglist.cpp \
		  $$TOOLS_CPP/qglobal.cpp \
		  $$TOOLS_CPP/qgpluginmanager.cpp \
		  $$TOOLS_CPP/qgvector.cpp \
		  $$TOOLS_CPP/qiodevice.cpp \
		  $$TOOLS_CPP/qlibrary.cpp \
		  $$TOOLS_CPP/qlocale.cpp \
		  $$TOOLS_CPP/qmap.cpp \
		  $$TOOLS_CPP/qmutexpool.cpp \
		  $$TOOLS_CPP/qptrcollection.cpp \
		  $$TOOLS_CPP/qregexp.cpp \
		  $$TOOLS_CPP/qstring.cpp \
		  $$TOOLS_CPP/qsemaphore.cpp \
		  $$TOOLS_CPP/qsettings.cpp \
		  $$TOOLS_CPP/qstringlist.cpp \
		  $$TOOLS_CPP/qtextstream.cpp \
		  $$TOOLS_CPP/qunicodetables.cpp \
		  $$TOOLS_CPP/qucom.cpp \
		  $$TOOLS_CPP/quuid.cpp
}

