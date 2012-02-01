
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../MindSpace

QT += network opengl

MOC_DIR = ../.build
OBJECTS_DIR = ../.build
RCC_DIR = ../.build
UI_DIR = ../.build

include(../MindSpace/mindspace.pri)
include(../mspace-viewer/mspace-viewer.pri)

# Input
HEADERS += \
        BotWindow.h \
        SimpleBotEnv.h \
        SimpleBotAgent.h

SOURCES += main.cpp \
        BotWindow.cpp \
        SimpleBotEnv.cpp \
        SimpleBotAgent.cpp


