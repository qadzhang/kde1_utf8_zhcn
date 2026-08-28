# TQt network module

network {
	HEADERS += $$NETWORK_H/ntqdns.h \
		    $$NETWORK_H/ntqftp.h \
		    $$NETWORK_H/ntqhttp.h \
		    $$NETWORK_H/ntqhostaddress.h \
		    $$NETWORK_H/ntqnetwork.h \
		    $$NETWORK_H/ntqserversocket.h \
		    $$NETWORK_H/ntqsocket.h \
		    $$NETWORK_H/ntqsocketdevice.h
	NETWORK_SOURCES	= $$NETWORK_CPP/qdns.cpp \
		    $$NETWORK_CPP/qftp.cpp \
		    $$NETWORK_CPP/qhttp.cpp \
		    $$NETWORK_CPP/qhostaddress.cpp \
		    $$NETWORK_CPP/qnetwork.cpp \
		    $$NETWORK_CPP/qserversocket.cpp \
		    $$NETWORK_CPP/qsocket.cpp \
		    $$NETWORK_CPP/qsocketdevice.cpp
	unix:NETWORK_SOURCES += $$NETWORK_CPP/qsocketdevice_unix.cpp
	win32:NETWORK_SOURCES += $$NETWORK_CPP/qsocketdevice_win.cpp
	SOURCES    += $$NETWORK_SOURCES
}
