
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../MindSpace

QT += network opengl

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

include(../MindSpace/mindspace.pri)

# Input
HEADERS += \
        MSpaceViewerWidget.h 

SOURCES += main.cpp \
        MSpaceViewerWidget.cpp


