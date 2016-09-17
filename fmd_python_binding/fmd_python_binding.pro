#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T15:28:49
#
#-------------------------------------------------

QT       -= core gui

TARGET = fmd_python_binding
TEMPLATE = lib

DEFINES += FMD_PYTHON_BINDING_LIBRARY

SOURCES += fmd_python_binding.cpp

HEADERS += fmd_python_binding.h\
        fmd_python_binding_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
