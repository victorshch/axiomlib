CONFIG -= console

include(../AxiomLibConfig.pri)

QT -= core gui
CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += $$BOOST_INCLUDE $$MPICH_INCLUDE $$OTHER_INCLUDE

INCLUDEPATH += $$AXIOMLIB_INCLUDE

LIBS += $$AXIOMLIB_LIB

LIBS += $$BOOST_LIB $$MPICH_LIB $$SHARK_LIB $$OTHER_LIB

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

unix {
    LIBS += -lmpl -lopa
}

win32 {
#МСФМН ДКЪ vc++10
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
