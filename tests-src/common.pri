CONFIG -= console

include(../../AxiomLibConfig.pri)

QT -= core gui
CONFIG -= qt

INCLUDEPATH += $$BOOST_INCLUDE $$MPICH_INCLUDE $$OTHER_INCLUDE

INCLUDEPATH += $$AXIOMLIB_INCLUDE

LIBS += $$BOOST_LIB $$MPICH_LIB $$OTHER_LIB

LIBS += $$AXIOMLIB_LIB

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
