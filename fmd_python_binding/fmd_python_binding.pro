#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T15:28:49
#
#-------------------------------------------------

QT       -= core gui

CONFIG -= debug
CONFIG += release

TARGET = axiomlib_fmd
TEMPLATE = lib

DEFINES += FMD_PYTHON_BINDING_LIBRARY

SOURCES += fmd_python_binding.cpp

HEADERS += fmd_python_binding.h

include(../AxiomLibConfig.pri)

INCLUDEPATH += $$BOOST_INCLUDE $$MPICH_INCLUDE $$OTHER_INCLUDE

INCLUDEPATH += $$BOOST_PYTHON_INCLUDE

INCLUDEPATH += $$AXIOMLIB_INCLUDE

LIBS += $$AXIOMLIB_LIB

LIBS += $$BOOST_LIB $$MPICH_LIB $$SHARK_LIB $$BOOST_PYTHON_LIB $$OTHER_LIB
