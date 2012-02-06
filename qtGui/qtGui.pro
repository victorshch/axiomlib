# Файл проекта qtGui

# имя исполняемого файла
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

DEFINES += "BOOST_FILESYSTEM_VERSION=2"

# Включение файла, в котором задаются параметры include'ов и библиотек
include(../AxiomLibConfig.pri)
#include(dkNoteDebug.pri)
# include(dkNoteRelease.pri)

### Конфигурация - release или debug ###

CONFIG += release
CONFIG -= debug
#CONFIG += debug
#CONFIG -= release


#INCLUDEPATH += ./GeneratedFiles \
#    ./GeneratedFiles/Debug \
#    .

INCLUDEPATH += $$BOOST_INCLUDE \
    $$OTHER_INCLUDE
LIBS += $$BOOST_LIB \
    $$OTHER_LIB

# Включение файла со списком файлов проекта
include(qtGui.pri)
