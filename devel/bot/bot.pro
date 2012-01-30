
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
	../MindSpace/MindSpace.h \
	MindSpaceGraphWidget.h \
	MindSpaceGraphEdge.h \
	MindSpaceGraphNode.h

SOURCES += main.cpp \
        BotWindow.cpp \
	../MindSpace/MindSpace.cpp \
	MindSpaceGraphWidget.cpp \
	MindSpaceGraphEdge.cpp \
	MindSpaceGraphNode.cpp


