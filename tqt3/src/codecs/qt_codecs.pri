# TQt codecs module

!bigcodecs:DEFINES += TQT_NO_BIG_CODECS
tools {
	CODECS_P		= codecs
	HEADERS += \
#$$CODECS_H/qasmocodec.h \
		$$CODECS_H/ntqbig5codec.h \
		  $$CODECS_H/ntqeucjpcodec.h \
		  $$CODECS_H/ntqeuckrcodec.h \
		  $$CODECS_P/qisciicodec_p.h \
		  $$CODECS_H/ntqgb18030codec.h \
		  $$CODECS_H/ntqjiscodec.h \
		  $$CODECS_H/ntqjpunicode.h \
		  $$CODECS_H/ntqrtlcodec.h \
		  $$CODECS_H/ntqsjiscodec.h \
		  $$CODECS_H/ntqtextcodec.h \
		  $$CODECS_H/ntqtsciicodec.h \
		  $$CODECS_H/ntqutfcodec.h \
		  $$CODECS_P/qtextcodecinterface_p.h \
		  $$CODECS_H/ntqtextcodecfactory.h \
		  $$CODECS_H/ntqtextcodecplugin.h

	SOURCES += \
#$$CODECS_CPP/qasmocodec.cpp \
		$$CODECS_CPP/qbig5codec.cpp \
		  $$CODECS_CPP/qeucjpcodec.cpp \
		  $$CODECS_CPP/qeuckrcodec.cpp \
		  $$CODECS_CPP/qisciicodec.cpp \
		  $$CODECS_CPP/qgb18030codec.cpp \
		  $$CODECS_CPP/qjiscodec.cpp \
		  $$CODECS_CPP/qjpunicode.cpp \
		  $$CODECS_CPP/qrtlcodec.cpp \
		  $$CODECS_CPP/qsjiscodec.cpp \
		  $$CODECS_CPP/qtextcodec.cpp \
		  $$CODECS_CPP/qtsciicodec.cpp \
		  $$CODECS_CPP/qutfcodec.cpp \
		  $$CODECS_CPP/qtextcodecfactory.cpp \
		  $$CODECS_CPP/qtextcodecplugin.cpp

	x11:SOURCES += $$CODECS_CPP/qfontcncodec.cpp  \
	               $$CODECS_CPP/qfonthkcodec.cpp  \
	               $$CODECS_CPP/qfontjpcodec.cpp  \
		       $$CODECS_CPP/qfontkrcodec.cpp  \
		       $$CODECS_CPP/qfontlaocodec.cpp \
		       $$CODECS_CPP/qfonttwcodec.cpp
	x11:HEADERS += $$CODECS_P/qfontcodecs_p.h
}
