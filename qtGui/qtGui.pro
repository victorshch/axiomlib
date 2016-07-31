# О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫ qtGui

# О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫
TARGET = qtGui

TEMPLATE = app
QT += core \
    gui
DEPENDPATH += .

#MOC_DIR += ./GeneratedFiles/debug
#UI_DIR += ./GeneratedFiles
#RCC_DIR += ./GeneratedFiles

debug {
OBJECTS_DIR = ./obj_debug/
}

release {
OBJECTS_DIR = ./obj_release/
}

# О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫, О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ include'О©╫О©╫ О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫
include(../AxiomLibConfig.pri)
#include(dkNoteDebug.pri)
# include(dkNoteRelease.pri)

### О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ - release О©╫О©╫О©╫ debug ###

#CONFIG += debug
#CONFIG -= release


#INCLUDEPATH += ./GeneratedFiles \
#    ./GeneratedFiles/Debug \
#    .

INCLUDEPATH += $$BOOST_INCLUDE \
    $$OTHER_INCLUDE
LIBS += $$BOOST_LIB \
    $$OTHER_LIB

# О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫ О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫
include(qtGui.pri)

CONFIG -= release
CONFIG += debug
