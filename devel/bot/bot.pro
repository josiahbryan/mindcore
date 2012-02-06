
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . \
	../MindSpace	

QT += network opengl

MOC_DIR = ../.build
OBJECTS_DIR = ../.build
RCC_DIR = ../.build
UI_DIR = ../.build

# Rebuild the types files from MindSpace.types
system(cd ../MindSpace && perl mindspace-types.pl)

# The node/link library
include(../MindSpace/mindspace.pri)

# The viewer widget and friends
include(../mspace-viewer/mspace-viewer.pri)

# Input
HEADERS += \
        BotWindow.h \
        SimpleBotEnv.h \
        SimpleBotAgent.h \
	AgentSubsystems.h

SOURCES += main.cpp \
        BotWindow.cpp \
        SimpleBotEnv.cpp \
        SimpleBotAgent.cpp \
        AgentSubsystems.cpp


