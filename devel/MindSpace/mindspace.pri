VPATH += $$PWD
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

#FORMS += 

include(graph/graph.pri)
	
SOURCES += \
	QStorableObject.cpp \
	MindSpace.cpp \
	MindNode.cpp \
	MindLink.cpp

HEADERS += \
	QStorableObject.h \
	MindSpace.h \
	MindNode.h \
	MindLink.h