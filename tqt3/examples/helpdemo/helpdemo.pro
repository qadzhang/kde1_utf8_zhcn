TEMPLATE = app

CONFIG	+= qt warn_on
LIBS    += -ltqassistantclient
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

REQUIRES = full-config

SOURCES += helpdemo.cpp main.cpp
HEADERS += helpdemo.h
FORMS	 = helpdemobase.ui
