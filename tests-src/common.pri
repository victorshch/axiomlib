CONFIG -= console

include(../../AxiomLibConfig.pri)

DEFINES += "BOOST_FILESYSTEM_VERSION=2"

QT -= core gui
CONFIG -= qt

INCLUDEPATH += $$BOOST_INCLUDE $$MPICH_INCLUDE $$OTHER_INCLUDE

INCLUDEPATH += $$AXIOMLIB_INCLUDE

LIBS += $$AXIOMLIB_LIB

LIBS += $$BOOST_LIB $$MPICH_LIB $$OTHER_LIB

unix {
    LIBS += -lmpl -lopa
}

win32 {
#нужно для vc++10
QMAKE_LFLAGS += /entry:mainCRTStartup
}

debug {
OBJECTS_DIR = ./obj/debug
DESTDIR = ../bin/debug
}
release {
OBJECTS_DIR = ./obj/release
DESTDIR = ../bin/release
}
