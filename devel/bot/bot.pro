
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../MindSpace

QT += network

MOC_DIR = ../.build
OBJECTS_DIR = ../.build
RCC_DIR = ../.build
UI_DIR = ../.build


# Input
HEADERS += \
        BotWindow.h \
	../MindSpace/MindSpace.h

SOURCES += main.cpp \
        BotWindow.cpp \
	../MindSpace/MindSpace.cpp


