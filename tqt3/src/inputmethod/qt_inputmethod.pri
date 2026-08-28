# TQt inputmetod module

inputmethod {
	INPUTMETHOD_P = inputmethod
	HEADERS +=$$INPUTMETHOD_H/ntqinputcontextfactory.h \
		  $$INPUTMETHOD_P/qinputcontextinterface_p.h \
		  $$INPUTMETHOD_H/ntqinputcontextplugin.h
	SOURCES +=$$INPUTMETHOD_CPP/qinputcontextfactory.cpp \
		  $$INPUTMETHOD_CPP/qinputcontextplugin.cpp
}
