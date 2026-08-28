# TQt dialogs module

dialogs {
	DIALOGS_P	= dialogs

	HEADERS	+= $$DIALOGS_H/ntqcolordialog.h \
		  $$DIALOGS_H/ntqdialog.h \
		  $$DIALOGS_H/ntqerrormessage.h \
		  $$DIALOGS_H/ntqfiledialog.h \
		  $$DIALOGS_H/ntqfontdialog.h \
		  $$DIALOGS_H/ntqmessagebox.h \
		  $$DIALOGS_H/ntqprogressdialog.h \
		  $$DIALOGS_H/ntqsemimodal.h \
		  $$DIALOGS_H/ntqtabdialog.h \
		  $$DIALOGS_H/ntqwizard.h \
		  $$DIALOGS_H/ntqinputdialog.h

        !embedded:mac:SOURCES  += $$DIALOGS_CPP/qfiledialog_mac.cpp $$DIALOGS_CPP/qcolordialog_mac.cpp
	win32:SOURCES += $$DIALOGS_CPP/qfiledialog_win.cpp
	unix:SOURCES += $$DIALOGS_CPP/qprintdialog.cpp
	unix:HEADERS   += $$DIALOGS_H/ntqprintdialog.h 

	SOURCES += $$DIALOGS_CPP/qcolordialog.cpp \
		  $$DIALOGS_CPP/qdialog.cpp \
		  $$DIALOGS_CPP/qerrormessage.cpp \
		  $$DIALOGS_CPP/qfiledialog.cpp \
		  $$DIALOGS_CPP/qfontdialog.cpp \
		  $$DIALOGS_CPP/qmessagebox.cpp \
		  $$DIALOGS_CPP/qprogressdialog.cpp \
		  $$DIALOGS_CPP/qtabdialog.cpp \
		  $$DIALOGS_CPP/qwizard.cpp \
		  $$DIALOGS_CPP/qinputdialog.cpp
}
