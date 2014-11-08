# -------------------------------------------------
# Project created by QtCreator 2010-04-02T11:28:45
# -------------------------------------------------

CONFIG += static
CONFIG -= release
CONFIG += debug

include(AxiomLibConfig.pri)

QT -= core gui
CONFIG -= qt

INCLUDEPATH += $$MPICH_INCLUDE \
    $$BOOST_INCLUDE \
    $$OTHER_INCLUDE

QT -= core \
    gui
TARGET = AxiomLib
debug { 
    OBJECTS_DIR = obj_debug
    DESTDIR = debug
}
release { 
    OBJECTS_DIR = obj_release
    DESTDIR = release
}
TEMPLATE = lib
DEFINES += AXIOMLIB_LIBRARY

# ##    ###
# windows
win32 { 
    QMAKE_CXXFLAGS_RELEASE += -openmp
    QMAKE_CXXFLAGS_RELEASE += -o2 \
        -DRELEASE
    QMAKE_CXXFLAGS_DEBUG += /ZI \
        /Od
}

# unix
linux-g++|linux-g++-64 { 
    QMAKE_CXXFLAGS += -std=c++0x
    QMAKE_CXXFLAGS_RELEASE += -fopenmp \
        -o2
}
linux-icc|linux-icc-64 { 
    QMAKE_CXXFLAGS += -std=c++0x \
        -no-multibyte-chars \
        -wd913
    QMAKE_CXXFLAGS_RELEASE += -openmp
    QMAKE_LFLAGS_RELEASE += -openmp
}
linux-icc|linux-icc-64 { 
    QMAKE_CXXFLAGS += -std=c++0x \
        -no-multibyte-chars \
        -wd913
    QMAKE_CXXFLAGS_RELEASE += -openmp
    QMAKE_LFLAGS_RELEASE += -openmp
}
win32:#  ,    ,

#  cl   
QMAKE_CXXFLAGS += -D_SCL_SECURE_NO_WARNINGS \
    -D_CRT_SECURE_NO_WARNINGS

# ##   ###
SOURCES += src/TreatmentSample.cxx \
    src/TreatmentFactory.cxx \
    src/TransmuteTpl.cxx \
    src/TransmuteStrategyFactory.cxx \
    src/TransmutePrecision.cxx \
    src/TestTS.cxx \
    src/TemplateRecognizer.cxx \
    src/TemplateManager.cxx \
    src/TemplateGeneticsFactory.cxx \
    src/TemplateGeneticsExt.cxx \
    src/StartStrategy.cxx \
    src/SimpleTransmuteStrategy.cxx \
    src/SimpleSelectionStrategy.cxx \
    src/SimpleCrossoverStrategy.cxx \
    src/SelectionStrategyFactory.cxx \
    src/RefTrajFinder.cxx \
    src/RefMarkupSimple.cxx \
    src/RefMarkupSetSimple.cxx \
    src/RefMarkupSet.cxx \
    src/RefMarkup.cxx \
    src/ReducedRecognizerMetric.cxx \
    src/ReducedRecognizerFactory.cxx \
    src/ReducedRecognizerDTW.cxx \
    src/RecognizerNeuralNet.cxx \
    src/RecognizerNet.cxx \
    src/RecognizerMetric.cxx \
    src/RecognizerFactory.cxx \
    src/RecognizerExtend.cxx \
    src/QnaryPercWrapper.cxx \
    src/QnaryPercSingle.cxx \
    src/QnaryPerc.cxx \
    src/PrimeGoalStrategy.cxx \
    src/PreprocessSqueeze.cxx \
    src/PreprocessSimple.cxx \
    src/PreprocessFactory.cxx \
    src/PopElemExt.cxx \
    src/PercWrapper.cxx \
    src/PercSingle.cxx \
    src/PercSettings.cxx \
    src/NeuralMultiplexer.cxx \
    src/NeuralDistRejecter.cxx \
    src/NeuralDataHolder.cxx \
    src/NeuralCommonSettings.cxx \
    src/NeuralCommon.cxx \
    src/NeuralCoin.cxx \
    src/NeuralCoding.cxx \
    src/NeuralCoderGeneric.cxx \
    src/NeuralAuxSupplier.cxx \
    src/NeuralActFuncStr.cxx \
    src/Nets.cxx \
    src/Net.cxx \
    src/IntervalSet.cxx \
    src/GoalStrategyFactory.cxx \
    src/GaOps.cxx \
    src/FuzzyMultiDataLearnAlgorithm.cxx \
    src/FuzzyMultiData_ElemConds.cxx \
    src/FuzzyMultiData_AxiomSets.cxx \
    src/FuzzyMultiData_Axioms.cxx \
    src/FuzzyDataLearnAlgorithm.cxx \
    src/FullFuzzyDataSet.cxx \
    src/ExtGenetics.cxx \
    src/Environment.cxx \
    src/EnvDataSet.cxx \
    src/ElemCondPlusStat.cxx \
    src/ElemCondPlus.cxx \
    src/ElemConditionsFactory.cxx \
    src/DataSet.cxx \
    src/CrossoverTry.cxx \
    src/CrossoverTpl.cxx \
    src/CrossoverStrategyFactory.cxx \
    src/CompareStatisticSimple.cxx \
    src/CompareClassFactory.cxx \
    src/CheckName.cxx \
    src/Check.cxx \
    src/BaseGenetics.cxx \
    src/AxiomSetWithStats.cxx \
    src/AxiomSetPop.cxx \
    src/AxiomSetPlus.cxx \
    src/AxiomSetBase.cxx \
    src/AxiomSet.cxx \
    src/AxiomLibException.cxx \
    src/AxiomExprSetPlus.cxx \
    src/AxiomExprSet.cxx \
    src/AxiomExprPlus.cxx \
    src/AxiomExpr.cxx \
    src/AxiomBase.cxx \
    src/Axiom.cxx \
    src/elem_conditions/Volatility.cxx \
    src/elem_conditions/SecDerivativeIncreaseRight.cxx \
    src/elem_conditions/SecDerivativeIncreaseLeft.cxx \
    src/elem_conditions/SecDerivativeDecreaseRight.cxx \
    src/elem_conditions/SecDerivativeDecreaseLeft.cxx \
    src/elem_conditions/Integral.cxx \
    src/elem_conditions/IncreaseRightElemCondition.cxx \
    src/elem_conditions/IncreaseLeftElemCondition.cxx \
    src/elem_conditions/ElemCondition.cxx \
    src/elem_conditions/DerivativeIncreaseRight.cxx \
    src/elem_conditions/DerivativeIncreaseLeft.cxx \
    src/elem_conditions/DerivativeDecreaseRight.cxx \
    src/elem_conditions/DerivativeDecreaseLeft.cxx \
    src/elem_conditions/DecreaseRightElemCondition.cxx \
    src/elem_conditions/DecreaseLeftElemCondition.cxx \
    src/elem_conditions/BetweenTwoLevels.cxx \
    src/FANN/fann_train_data.c \
    src/FANN/fann_train.c \
    src/FANN/fann_io.c \
    src/FANN/fann_error.c \
    src/FANN/fann_cascade.c \
    src/FANN/fann.c \
    src/FANN/doublefann.c \
    src/DataSetBase.cxx \
    src/DataSetDivision.cxx \
    src/SatPointSet.cpp \
    src/Logger.cxx \
    src/FuzzyMultiDataExt/Heuristics.cxx \
    src/FuzzyMultiDataExt/ECTypeStage.cxx \
    src/FuzzyMultiDataExt/AXStage.cxx \
    src/FuzzyMultiDataExt/ASStage.cxx \
    src/FuzzyMultiDataExtAlgorithm.cxx \
    src/FuzzyMultiDataExt/HeuristicsSet.cxx \
    src/FuzzyMultiDataExt/HeuristicsFactory.cxx \
    src/FuzzyMultiDataExt/AXStatClustering.cxx \
    src/FuzzyMultiDataExt/StatClustering.cxx \
    src/FuzzyMultiDataExt/ECStatClustering.cxx \
    src/FuzzyMultiDataExt/ReducedSatPointSet.cxx \
    src/FuzzyMultiDataExt/ECOccFilter.cxx \
    src/FuzzyMultiDataExt/AXOccFilter.cxx \
    src/FuzzyMultiDataExt/ECTypeSelection.cxx \
    src/FuzzyMultiDataExt/OccClustering.cxx \
    src/FuzzyMultiDataExt/ECOccClustering.cxx \
    src/FuzzyMultiDataExt/AXOccClustering.cxx \
    src/FuzzyMultiDataExt/StatVector.cxx \
    src/RecognizerImplBase.cxx \
    src/RecognizerReducedAdapter.cxx \
    src/ReducedRecognizerDistance.cxx \
    src/TrajectorySampleDistance.cxx \
    src/LabelingStrategy.cxx \
    src/LabelingStrategyFactory.cxx \
    src/LabelingStrategySimple.cxx \
    src/optimization/FunctionOneVariable.cxx \
    src/optimization/Minimizer.cxx \
    src/optimization/GoldenSectionSearch.cxx \
    src/optimization/GridSearch.cxx \
    src/optimization/MinimizerFactory.cxx \
    src/optimization/HybridGridSearch.cxx \
    src/MultiMarking/dtwmetric.cxx \
    src/RecognizerMultimarkup.cxx \
    src/MultiMarking/MetricsBetweenSets1.cxx \
    src/MultiMarking/MetricsBetweenSets2.cxx \
    src/MultiMarking/MetricsMatrix.cxx \
    src/MultiMarking/MetricsHamming.cxx \
    src/MultiMarking/MetricsEuclidean.cxx \
    src/MultiMarking/MetricsEqual.cxx \
    src/MultiMarking/multimarks.cxx \
    src/MultiMarking/Dice.cxx \
    src/MultiMarking/Jaccard.cxx \
    src/Recognizer.cxx \
    src/MultiMarking/AsymmHamming.cpp \
    src/MultiMarking/AsymmBetweenSets.cpp \
    src/MultiMarking/WeakEqual.cpp \
    src/MultiMarking/Priority.cpp  \
    src/FuzzyMultiDataExt/ASStageSimple.cxx \
    src/FuzzyDataSet_old.cxx \
    src/FuzzyDataSet.cxx \
    src/elem_conditions/Variance.cxx \
    src/FuzzyMultiDataExt/AXStageSimple.cpp \
    src/FuzzyMultiDataExt/AXStageTree.cpp \
    src/FuzzyMultiDataExt/ECStage.cxx \
    src/FuzzyMultiDataExt/ECStageSimple.cxx \
    src/FuzzyMultiDataExt/ClusteringRealizations/ClusteringModel.cxx \
    src/FuzzyMultiDataExt/ClusteringRealizations/KMeansClusteringModel.cpp \
    src/FuzzyMultiDataExt/ElemCondClustering.cxx \
    src/FuzzyMultiDataExt/ECClusteringConfiguration.cxx \
    src/FuzzyMultiDataExt/ECStageClustering.cxx \
    src/FuzzyMultiDataExt/AXStageTrivial.cpp \
    src/FuzzyMultiDataExt/ClusteringFeatures/ClusteringFeature.cxx \
    src/elem_conditions/Export.cxx \
    src/FuzzyMultiDataExt/ASStageGenetic.cpp \
    src/FuzzyMultiDataExt/ECStageDiscretization.cpp \
    src/FuzzyMultiDataExt/ElemCondMean.cxx
HEADERS += src/undefs.h \
    src/TreatmentSample.h \
    src/TreatmentFactory.h \
    src/Treatment.h \
    src/TransmuteTpl.h \
    src/TransmuteStrategyFactory.h \
    src/TransmuteStrategy.h \
    src/TransmutePrecision.h \
    src/TestTS.h \
    src/TemplateRecognizer.h \
    src/TemplateManager.h \
    src/TemplateGeneticsFactory.h \
    src/TemplateGeneticsExt.h \
    src/TemplateGenetics.h \
    src/StringPlus.h \
    src/StartStrategy.h \
    src/SimpleTransmuteStrategy.h \
    src/SimpleSelectionStrategy.h \
    src/SimpleCrossoverStrategy.h \
    src/SelectionStrategyFactory.h \
    src/SelectionStrategy.h \
    src/Round.h \
    src/RefTrajFinder.h \
    src/RefMarkupSimple.h \
    src/RefMarkupSetSimple.h \
    src/RefMarkupSet.h \
    src/RefMarkup.h \
    src/ReducedRecognizerMetric.h \
    src/ReducedRecognizerFactory.h \
    src/ReducedRecognizerDTW.h \
    src/ReducedRecognizer.h \
    src/RecognizerQnaryPercSingle.h \
    src/RecognizerQnaryPercMulti.h \
    src/RecognizerPercSingle.h \
    src/RecognizerPercMulti.h \
    src/RecognizerNeuralNet.h \
    src/RecognizerNet.h \
    src/RecognizerMetric.h \
    src/RecognizerFactory.h \
    src/RecognizerExtend.h \
    src/Recognizer.h \
    src/QnaryPercWrapper.h \
    src/QnaryPercSingle.h \
    src/QnaryPercMulti.h \
    src/QnaryPerc.h \
    src/QnaryBase.h \
    src/PrimeGoalStrategy.h \
    src/PreprocessSqueeze.h \
    src/PreprocessSimple.h \
    src/PreprocessFactory.h \
    src/Preprocess.h \
    src/PopElemExt.h \
    src/PercWrapper.h \
    src/PercSingle.h \
    src/PercSettings.h \
    src/PercMulti.h \
    src/NeuralMultiplexer.h \
    src/NeuralFANNHeader.h \
    src/NeuralDistRejecter.h \
    src/NeuralDataHolder.h \
    src/NeuralConcepts.h \
    src/NeuralCommonSettings.h \
    src/NeuralCommon.h \
    src/NeuralCoin.h \
    src/NeuralCoderGeneric.h \
    src/NeuralAuxSupplier.h \
    src/NeuralActFuncStr.h \
    src/Nets.h \
    src/Net.h \
    src/MultiTS.h \
    src/IntInterval.h \
    src/IntervalSet.h \
    src/GoalStrategyFactory.h \
    src/GoalStrategy.h \
    src/GaOps.h \
    src/FuzzyMultiDataLearnAlgorithm.h \
    src/FuzzyDataLearnAlgorithm.h \
    src/FullFuzzyDataSet.h \
    src/ExtGenetics.h \
    src/Environment.h \
    src/EnvDataSet.h \
    src/ElemCondPlusStruture.h \
    src/ElemCondPlusStat.h \
    src/ElemCondPlus.h \
    src/ElemConditionsFactory.h \
    src/ECStatistics.h \
    src/Defines.h \
    src/DataSet.h \
    src/CrossoverTry.h \
    src/CrossoverTpl.h \
    src/CrossoverStrategyFactory.h \
    src/CrossoverStrategy.h \
    src/CompareStatisticSimple.h \
    src/CompareStatistic.h \
    src/CompareClassFactory.h \
    src/CheckName.h \
    src/Check.h \
    src/BaseGenetics.h \
    src/AxiomStructure.h \
    src/AxiomStatistics.h \
    src/AxiomSetWithStats.h \
    src/AxiomSetStructure.h \
    src/AxiomSetStatistics.h \
    src/AxiomSetPop.h \
    src/AxiomSetPlus.h \
    src/AxiomSetBase.h \
    src/AxiomSet.h \
    src/AxiomLibException.h \
    src/AxiomExprStructure.h \
    src/AxiomExprSetStructure.h \
    src/AxiomExprSetPlus.h \
    src/AxiomExprSet.h \
    src/AxiomExprPlus.h \
    src/AxiomExpr.h \
    src/AxiomBase.h \
    src/Axiom.h \
    src/AllTreatments.h \
    src/AllTransmute.h \
    src/AllTemplateGenetics.h \
    src/AllSelection.h \
    src/AllReducedRecognizers.h \
    src/AllRecognizers.h \
    src/AllPreprocess.h \
    src/AllGoalFunctions.h \
    src/AllCrossover.h \
    src/AllCompareStatistic.h \
    src/all_axioms.h \
    src/elem_conditions/Volatility.h \
    src/elem_conditions/SecDerivativeIncreaseRight.h \
    src/elem_conditions/SecDerivativeIncreaseLeft.h \
    src/elem_conditions/SecDerivativeDecreaseRight.h \
    src/elem_conditions/SecDerivativeDecreaseLeft.h \
    src/elem_conditions/Integral.h \
    src/elem_conditions/IncreaseRightElemCondition.h \
    src/elem_conditions/IncreaseLeftElemCondition.h \
    src/elem_conditions/ElemCondition.h \
    src/elem_conditions/DerivativeIncreaseRight.h \
    src/elem_conditions/DerivativeIncreaseLeft.h \
    src/elem_conditions/DerivativeDecreaseRight.h \
    src/elem_conditions/DerivativeDecreaseLeft.h \
    src/elem_conditions/DecreaseRightElemCondition.h \
    src/elem_conditions/DecreaseLeftElemCondition.h \
    src/elem_conditions/BetweenTwoLevels.h \
    src/FANN/fann_train.h \
    src/FANN/fann_io.h \
    src/FANN/fann_internal.h \
    src/FANN/fann_error.h \
    src/FANN/fann_data.h \
    src/FANN/fann_cpp.h \
    src/FANN/fann_cascade.h \
    src/FANN/fann_activation.h \
    src/FANN/fann.h \
    src/FANN/doublefann.h \
    src/FANN/config.h \
    src/FANN/compat_time.h \
    src/elem_conditions/Export.h \
    src/DataSetBase.h \
    src/DataSetDivision.h \
    src/DistanceClusterizer.h \
    src/SatPointSet.h \
    src/Logger.h \
    src/FuzzyMultiDataExt/Heuristics.h \
    src/FuzzyMultiDataExt/ECTypeStage.h \
    src/FuzzyMultiDataExt/Common.h \
    src/FuzzyMultiDataExt/AXStage.h \
    src/FuzzyMultiDataExt/ASStage.h \
    src/FuzzyMultiDataExt/ForwardDeclarations.h \
    src/FuzzyMultiDataExtAlgorithm.h \
    src/FuzzyMultiDataExt/HeuristicsSet.h \
    src/FuzzyMultiDataExt/HeuristicsFactory.h \
    src/FuzzyMultiDataExt/AXStatClustering.h \
    src/FuzzyMultiDataExt/StatClustering.h \
    src/FuzzyMultiDataExt/ECStatClustering.h \
    src/FuzzyMultiDataExt/ReducedSatPointSet.h \
    src/FuzzyMultiDataExt/ECOccFilter.h \
    src/FuzzyMultiDataExt/AXOccFilter.h \
    src/FuzzyMultiDataExt/ECTypeSelection.h \
    src/FuzzyMultiDataExt/OccClustering.h \
    src/FuzzyMultiDataExt/ECOccClustering.h \
    src/FuzzyMultiDataExt/AXOccClustering.h \
    src/Common.h \
    src/FuzzyMultiDataExt/StatVector.h \
    src/RecognizerImplBase.h \
    src/RecognizerReducedAdapter.h \
    src/ReducedRecognizerDistance.h \
    src/TrajectorySampleDistance.h \
    src/LabelingStrategy.h \
    src/LabelingStrategyFactory.h \
    src/LabelingStrategySimple.h \
    src/optimization/FunctionOneVariable.h \
    src/optimization/Minimizer.h \
    src/optimization/GoldenSectionSearch.h \
    src/optimization/GridSearch.h \
    src/optimization/MinimizerFactory.h \
    src/optimization/HybridGridSearch.h \
    src/MultiMarking/dtwmetric.h \
    src/MultiMarking/multimarks.h \
    src/RecognizerMultimarkup.h \
    src/MultiMarking/MetricsBetweenSets1.h \
    src/MultiMarking/MetricsMatrix.h \
    src/MultiMarking/MetricsHamming.h \
    src/MultiMarking/MetricsEuclidean.h \
    src/MultiMarking/MetricsBetweenSets2.h \
    src/MultiMarking/MetricsEqual.h \
    src/MultiMarking/Dice.h \
    src/MultiMarking/Jaccard.h \
    src/MultiMarking/AsymmHamming.h \
    src/MultiMarking/AsymmBetweenSets.h \
    src/MultiMarking/WeakEqual.h \
    src/FuzzyMultiDataExt/ASStageSimple.h \
    src/FuzzyDataSet_old.h \
    src/FuzzyDataSet.h \
    src/elem_conditions/Variance.h \
    src/FuzzyMultiDataExt/AXStageSimple.h \
    src/FuzzyMultiDataExt/AXStageTree.h \
    src/FuzzyMultiDataExt/ECStageSimple.h \
    src/FuzzyMultiDataExt/ECStage.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/MinValueFeature.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/MaxValueFeature.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/ClusteringFeatureFactory.h \
    src/FuzzyMultiDataExt/ClusteringRealizations/KMeansClusteringModel.h \
    src/FuzzyMultiDataExt/ClusteringRealizations/ClusteringModel.h \
    src/FuzzyMultiDataExt/ElemCondClustering.h \
    src/FuzzyMultiDataExt/ECStageClustering.h \
    src/FuzzyMultiDataExt/ECClusteringConfiguration.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/ClusteringFeature.h \
    src/FuzzyMultiDataExt/AXStageTrivial.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/StandardDeviationFeature.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/RegressionErrorFeature.h \
    src/FuzzyMultiDataExt/ClusteringFeatures/RegressionAngleFeature.h \
    src/FuzzyMultiDataExt/ASStageGenetic.h \
    src/FuzzyMultiDataExt/GeneticUtils/RouletteWheelSelection.h \
    src/FuzzyMultiDataExt/GeneticUtils/ProbabilityMapping.h \
    src/FuzzyMultiDataExt/ECStageDiscretization.h \
	src/FuzzyMultiDataExt/ElemCondMean.h \
    src/prettyprint.hpp
