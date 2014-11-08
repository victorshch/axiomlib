#-------------------------------------------------
#
# Project created by QtCreator 2012-02-02T14:11:16
#
#-------------------------------------------------

QT       += core gui

TARGET = fmdeGui
TEMPLATE = app

include(../AxiomLibConfig.pri)

### ������������ - release ��� debug ###

#CONFIG += release
#CONFIG -= debug
CONFIG += debug
CONFIG -= release

unix {
#    QMAKE_CC = gcc-4.5
#    QMAKE_CXX = g++-4.5
}

### ������� �����, ���� ����� ������� ����������� ���� ###
debug {
DESTDIR = ./debug/
OBJECTS_DIR = ./obj_debug/
}
release {
DESTDIR = ./release/
OBJECTS_DIR = ./obj_release/
}

INCLUDEPATH += $$AXIOMLIB_INCLUDE \
    $$QWT_INCLUDE \
    $$MPICH_INCLUDE \
    $$BOOST_INCLUDE \
    $$OTHER_INCLUDE
LIBS += $$AXIOMLIB_LIB \
    $$MPICH_LIB \
    $$QWT_LIB \
    $$BOOST_LIB \
    $$OTHER_LIB


SOURCES += main.cxx\
    extended/vectorplot.cpp \
    extended/valuedsliderwidget.cpp \
    extended/utilities.cpp \
    extended/trajselectgroupbox.cpp \
    extended/thread.cpp \
    extended/StrategyFrame.cxx \
    extended/StdStringHash.cxx \
    extended/stagebase.cpp \
    extended/stage3.cpp \
    extended/stage2.cpp \
    extended/stage1.cpp \
    extended/stage0.cpp \
    extended/slidergroup.cpp \
    extended/signalproxy.cpp \
    extended/SetLegend.cpp \
    extended/SeparateClassDisplay.cxx \
    extended/selectionmanager.cpp \
    extended/plotbrowser.cpp \
    extended/ParamsForm.cxx \
    extended/markupeditor.cpp \
    extended/MainWindowContent.cpp \
    extended/MainWindow.cpp \
    extended/logwindow.cpp \
    extended/LogThread.cxx \
    extended/intervalwidget.cpp \
    extended/interval.cpp \
    extended/Index.cpp \
    extended/HeuristicsForm.cxx \
    extended/filtergroup.cpp \
    extended/extecbrowser.cpp \
    extended/extaxiomsetbrowser.cpp \
    extended/extaxiombrowser.cpp \
    extended/conditionmodel.cpp \
    extended/Condition.cpp \
    extended/clusterizer.cpp \
    extended/clusteringform.cpp \
    extended/ClassSelectForm.cxx \
    extended/asmodel.cpp \
    extended/AbstractParamsForm.cxx \
    extended/abstractasmodel.cpp \
    extended/algorithm_controller/statistics.cpp \
    extended/algorithm_controller/ManagedFuzzyDataStage.cxx \
    extended/algorithm_controller/ManagedFuzzyDataController.cxx \
    extended/as_model/asmodeltreeitemleaf.cpp \
    extended/as_model/asmodeltreeitemcomposite.cpp \
    extended/as_model/ASModelTreeItem.cpp \
    extended/as_model/asmodelroot.cpp \
    extended/as_model/asmodelonemarkup.cpp \
    extended/as_model/asmodeloneaxiom.cpp \
    extended/as_model/asmodelmarkups.cpp \
    extended/as_model/ASModelEC.cxx \
    extended/as_model/ASModelConjunction.cxx \
    extended/as_model/asmodelaxiomset.cpp \
    extended/as_model/asmodelaxioms.cpp \
    extended/condition_model/conditionmodelroot.cpp \
    extended/condition_model/conditionmodelonecondition.cpp \
    extended/model/TreeModel.cxx \
    extended/model/Schema.cxx \
    extended/model/ModelNode.cxx \
    extended/model/DataHandler.cxx \
    extended/model/CheckedDataHandler.cxx \
    extended/fileselectform.cpp \
    extended/configselectwidget.cpp

HEADERS  += \
    extended/VectorQwtAdapter.h \
    extended/vectorplot.h \
    extended/valuedsliderwidget.h \
    extended/utilities.h \
    extended/trajselectgroupbox.h \
    extended/thread.h \
    extended/StrategyFrame.h \
    extended/StdStringHash.h \
    extended/stagebase.h \
    extended/stage3.h \
    extended/stage2.h \
    extended/stage1.h \
    extended/stage0.h \
    extended/slidergroup.h \
    extended/signalproxy.h \
    extended/SetLegend.h \
    extended/SeparateClassDisplay.h \
    extended/selectionmanager.h \
    extended/relaxed_transform.h \
    extended/QtGuiException.h \
    extended/qdebugstream.h \
    extended/plotbrowser.h \
    extended/ParamsForm.h \
    extended/MarkupQwtAdapter.h \
    extended/markupeditor.h \
    extended/MainWindowContent.h \
    extended/MainWindow.h \
    extended/logwindow.h \
    extended/LogThread.h \
    extended/intervalwidget.h \
    extended/interval.h \
    extended/Index.h \
    extended/HeuristicsForm.h \
    extended/functors.h \
    extended/filtergroup.h \
    extended/extecbrowser.h \
    extended/extaxiomsetbrowser.h \
    extended/extaxiombrowser.h \
    extended/conditionmodel.h \
    extended/Condition.h \
    extended/ColorManager.h \
    extended/clusterizer.h \
    extended/clusteringform.h \
    extended/ClickableFrame.h \
    extended/ClassSelectForm.h \
    extended/CheckBox.h \
    extended/asmodel.h \
    extended/AbstractParamsForm.h \
    extended/abstractasmodel.h \
    extended/algorithm_controller/vectortypes.h \
    extended/algorithm_controller/statistics.h \
    extended/algorithm_controller/ManagedFuzzyDataUtility.h \
    extended/algorithm_controller/ManagedFuzzyDataStage3.h \
    extended/algorithm_controller/ManagedFuzzyDataStage2.h \
    extended/algorithm_controller/ManagedFuzzyDataStage1.h \
    extended/algorithm_controller/ManagedFuzzyDataStage0.h \
    extended/algorithm_controller/ManagedFuzzyDataStage.h \
    extended/algorithm_controller/ManagedFuzzyDataController.h \
    extended/algorithm_controller/forwarddecls.h \
    extended/algorithm_controller/EnvironmentSerialization.h \
    extended/as_model/asmodeltreeitemleaf.h \
    extended/as_model/asmodeltreeitemcomposite.h \
    extended/as_model/ASModelTreeItem.h \
    extended/as_model/asmodelroot.h \
    extended/as_model/asmodelonemarkup.h \
    extended/as_model/asmodeloneaxiom.h \
    extended/as_model/asmodelmarkups.h \
    extended/as_model/ASModelEC.h \
    extended/as_model/ASModelConjunction.h \
    extended/as_model/asmodelaxiomset.h \
    extended/as_model/asmodelaxioms.h \
    extended/condition_model/conditionmodelroot.h \
    extended/condition_model/conditionmodelonecondition.h \
    extended/model/TreeModel.h \
    extended/model/Schema.h \
    extended/model/ModelNode.h \
    extended/model/DataHandler.h \
    extended/model/CheckedDataHandler.h \
    extended/fileselectform.h \
    extended/configselectwidget.h

FORMS += \
    extended/valuedsliderwidget.ui \
    extended/trajselectgroupbox.ui \
    extended/StrategyFrame.ui \
    extended/stage3.ui \
    extended/stage2.ui \
    extended/stage1.ui \
    extended/stage0.ui \
    extended/slidergroup.ui \
    extended/selectionmanager.ui \
    extended/plotbrowser.ui \
    extended/ParamsForm.ui \
    extended/markupeditor.ui \
    extended/mainwindowcontent.ui \
    extended/MainWindow.ui \
    extended/logwindow.ui \
    extended/intervalwidget.ui \
    extended/HeuristicsForm.ui \
    extended/extecbrowser.ui \
    extended/extaxiomsetbrowser.ui \
    extended/extaxiombrowser.ui \
    extended/clusteringform.ui \
    extended/ClassSelectForm.ui
