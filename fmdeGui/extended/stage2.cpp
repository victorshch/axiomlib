#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <iterator>
#include <sstream>
#include <iostream>
#include <deque>

#include <QtDebug>
#include <QMessageBox>

#include "AxiomLibException.h"

#include "ClassSelectForm.h"
#include "SeparateClassDisplay.h"
#include "utilities.h"
#include "plotbrowser.h"
#include "Condition.h"
#include "functors.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "DistanceClusterizer.h"
#include "HeuristicsForm.h"
#include "QtGuiException.h"

#include "stage2.h"

Stage2::Stage2(QWidget *parent, ManagedFuzzyDataController *controller)
	: StageBase(parent, controller, 2)
{
	ui.setupUi(this);
		
	ui.treeWidget_axioms->setColumnCount(6);
	
	ui.treeWidget_axioms->headerItem()->setText(0, encode("Имя."));
	ui.treeWidget_axioms->headerItem()->setText(1, encode("Нештатн."));
	ui.treeWidget_axioms->headerItem()->setText(2, encode("Нормальн."));
	ui.treeWidget_axioms->headerItem()->setText(3, encode("Целев."));
	ui.treeWidget_axioms->headerItem()->setText(4, encode("Отклонение"));
	ui.treeWidget_axioms->headerItem()->setText(5, encode("Встречаемость"));
	//ui.treeWidget_axioms->headerItem()->setText(4, encode(""));	

	filterGroup = new FilterGroup(0, encode("Частотный фильтр"));
	filterGroup->addSlider(encode("Нештатн."))->setChecked(true);
	filterGroup->addSlider(encode("Нормальн."));
	filterGroup->addSlider(encode("Целев."));
	filterGroup->addSlider(encode("Отклонение"));
	filterGroup->addSlider(encode("Встречаемость"));
	
	ui.verticalLayout_axiomFilters->addWidget(filterGroup);
	
	clusteringForm = new ClusteringForm(controller, 0);
	ui.verticalLayout_clustering->addWidget(clusteringForm);
	
	selectionManager = new SelectionManager(0);
	ui.horizontalLayout_selectionManager->addWidget(selectionManager);
	
	m_classSelectForm = new ClassSelectForm(
	            controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getClassNames(),
	            "AllHeuristics_className",
	            this
	            );
	
	ui.horizontalLayout_classSelect->addWidget(m_classSelectForm);
	
	setupParams();
	setupHeuristics();	
	
	auto conditionFunction = [this](const Index& index) {
		return PCondition(
		            new ConditionAxiom(
		                getAxiomByIndex(index), index, index[0]
		                )
		            );
	};
	
	m_separateClassDisplay = new SeparateClassDisplay(controller, 2, 2, ConditionFunction(conditionFunction), ui.treeWidget_axioms, m_classSelectForm, selectionManager, clusteringForm,this);
	connect(m_separateClassDisplay, SIGNAL(buildingNodeForClass(int,QTreeWidgetItem*)), this, SLOT(buildNodeForClass(int,QTreeWidgetItem*)));
	connect(m_separateClassDisplay, SIGNAL(savingClass(int,QTreeWidgetItem*)), this, SLOT(saveClass(int,QTreeWidgetItem*)));
	connect(m_separateClassDisplay, SIGNAL(numberChanged(int)), ui.SpinBox_numberOfAxioms, SLOT(setValue(int)));
	connect(ui.pushButton_showPlots, SIGNAL(clicked()), m_separateClassDisplay, SLOT(displayPlot()));
	
	connect(filterGroup, SIGNAL(sliderSignaled(int,double,bool)), 
			this, SLOT(filterChanged(int,double,bool)));
	
	connect(ui.pushButton_compute, SIGNAL(clicked()), this, SLOT(calculate()));
}

Stage2::~Stage2()
{

}

void Stage2::initializePage() {
	try {
		controller->createStage2();
		init();
		//QMessageBox::information(0, "Information", "Stage 2 initialized");
	} catch(AxiomLib::AxiomLibException exception) {
		QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
	}
}

void Stage2::init() {
	m_separateClassDisplay->buildNodes();
	initFreqFilter();
}

void Stage2::computeIntervals() {
	statAbnormInterval.reset();
	statNormalInterval.reset();
	statVariationInterval.reset();
	statOccurenceInterval.reset();
	diffInterval.reset();
	diffInterval.setMaxMax(1000.0);
	for(unsigned int i = 0; i < allAxioms.size(); i++) {
		for(unsigned int j = 0; j < allAxioms[i].size(); j++) {
			statAbnormInterval.fitLimits(allAxioms[i][j].statAbnorm);
			statNormalInterval.fitLimits(allAxioms[i][j].statNormal);
			statVariationInterval.fitLimits(allAxioms[i][j].statVariation);
			statOccurenceInterval.fitLimits(allAxioms[i][j].statOccurence);
			diffInterval.fitLimits(allAxioms[i][j].goal);
		}
	}
}

void Stage2::initFreqFilterSliders() {
	filterGroup->slider(0)
		->setLimits(statAbnormInterval.min(),
									  statAbnormInterval.max())
		->setDefaults(statAbnormInterval.max(), false)
		->toDefault();
	filterGroup->slider(1)
		->setLimits(statNormalInterval.min(),
									  statNormalInterval.max())
		->setDefaults(statAbnormInterval.min(), true)
		->toDefault();
	filterGroup->slider(2)
		->setLimits(diffInterval.min(),
									  diffInterval.max())
		->setDefaults(diffInterval.max(), false)
		->toDefault();
	filterGroup->slider(3)
		->setLimits(statVariationInterval.min(),
									  statVariationInterval.max())
		->setDefaults(statVariationInterval.min(), true)
		->toDefault();
	filterGroup->slider(4)
		->setLimits(statOccurenceInterval.min(), statOccurenceInterval.max())
		->setDefaults(statOccurenceInterval.min(), true)
		->toDefault();
}

void Stage2::initFreqFilter() {
	computeIntervals();
	
	initFreqFilterSliders();
}

void Stage2::setupParams() {
	ParamsForm* paramsForm = new ParamsForm();
	paramsForm->addParam(StrategyParamSpec(0, 1000, 1, 10, "shiftErrIn"),
	                     encode("Стоимость ошибки, если\n при сдвиге разметок осталась\n область разметки без соответствия\n в другой разметке"));
	paramsForm->addParam(StrategyParamSpec(0.0, 1.0, 0.01, 0.55, "extendMarkUpLensLevel"),
	                     encode("Степень расширения разметок\n во время их поиска (нечеткость поиска разметок)"));
	paramsForm->addParam(StrategyParamSpec(1, 10000, 1, 500, "maxAxiomSetPopSize"),
	                     encode("Максимальное число систем\n аксиом в популяции"));
	paramsForm->addParam(StrategyParamSpec(1, 100, 1, 5, "maxNumberOfSteps"),
	                     encode("Максимальное число итераций\n алгоритма построения систем аксиом"));
	paramsForm->addParam(StrategyParamSpec(0.0, 1000.0, 1.0, 1.0, "bestAxiomSetGoal"),
	                     encode("Требуемое значение целевой функции"));
	paramsForm->addParam(StrategyParamSpec(1, 1000, 1, 20, "numberOfBestAxiomSets"),
	                     encode("Число сохраняемых лучших\n систем аксиом после работы всего алгоритма"));
	paramsForm->addParam(StrategyParamSpec(0.0, 1.0, 0.01, 0.95, "percentBestAxiomSets"),
	                     encode("Доля сохраняемых лучших систем аксиом\n на каждой итерации"));
	
	ui.verticalLayout_params->addWidget(paramsForm);
	
	registerParamsForm(paramsForm);
	
	StrategyFrame* strategyFrame_recognizer = new StrategyFrame("ReducedRecognizer", encode("Распознаватель"));

	QList<QPair<StrategyParamSpec, QString> > paramsList;
	paramsList << qMakePair(StrategyParamSpec(0.0, 1.0, 0.001, 0.125, "Precision"), QString("Precision"))
	           << qMakePair(StrategyParamSpec(0.0, 10.0, 0.01, 1.35, "Stretch"), QString("Stretch"));
	
	strategyFrame_recognizer->addStrategy(paramsList, "ReducedDTW", "Dynamic time warping" );
	
	paramsList.pop_back();
	
	strategyFrame_recognizer->addStrategy(paramsList, "ReducedMetric", encode("Метрика"));
	
	ui.verticalLayout_params->addWidget(strategyFrame_recognizer);
	registerParamsForm(strategyFrame_recognizer);
	
	StrategyFrame* strategyFrame_goal = new StrategyFrame("goalClass", encode("Целевая функция"));
	
	paramsList.clear();
	paramsList << qMakePair(StrategyParamSpec(0.0, 1000.0, 1.0, 1.0, "gcFirst"), QString("gcFirst"))
	           << qMakePair(StrategyParamSpec(0.0, 1000.0, 1.0, 50.0, "gcSecond"), QString("gcSecond"));
	
	strategyFrame_goal->addStrategy(paramsList,
	                                "GoalFunctionPrime", "GoalFunctionPrime");
	
	ui.verticalLayout_params->addWidget(strategyFrame_goal);
	registerParamsForm(strategyFrame_goal);	
	
	registerParamsForm(m_classSelectForm);
}

void Stage2::setupHeuristics() {
	HeuristicsForm* heuristicsForm = new HeuristicsForm("Heuristics", "AXStatClustering", encode("Кластеризация по статистике"));
	heuristicsForm->addParam(StrategyParamSpec(1, 100, 1, 1, "retain"), encode("Число оставляемых аксиом с одинаковой статистикой"));
	ui.verticalLayout_heuristics->addWidget(heuristicsForm);
	registerHeuristicsForm(heuristicsForm);
	
	heuristicsForm = new HeuristicsForm("Heuristics", "AXOccFilter", encode("Фильтрация по встречаемости"));
	heuristicsForm->addParam(StrategyParamSpec(0.0, 1.0, 0.01, 1.0, "threshold"), encode("Порог встречаемости"));
	ui.verticalLayout_heuristics->addWidget(heuristicsForm);
	registerHeuristicsForm(heuristicsForm);
	
	heuristicsForm = new HeuristicsForm("Heuristics", "AXOccClustering", encode("Кластеризация по точкам выполнения"));
	heuristicsForm->addParam(StrategyParamSpec(2, 10000, 1, 2, "minDistance"), encode("Минимальное расстояние между кластерами"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 3, "maxClusterSize"), encode("Максимальный размер кластера"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 3, "minMaxClusterSize"), encode("Минимальный максимум размера\n кластера на траектории"));
	ui.verticalLayout_heuristics->addWidget(heuristicsForm);
	registerHeuristicsForm(heuristicsForm);
}

void Stage2::filterItems() {
	if(allAxioms.size() == 0)	{
		return;
	}
	
	m_separateClassDisplay->beginUpdateItems();
	
	for(unsigned int i = 0; i < allAxioms.size(); i++) {
		for(unsigned int j = 0; j < allAxioms[i].size(); j++) {
			m_separateClassDisplay->hideItem(makeIndex(i, j), 
			                                 !checkFilter(allAxioms[i][j])
			                                 );
		}
	}
	
	m_separateClassDisplay->endUpdateItems();
}

bool Stage2::checkFilter(const AxiomLib::AxiomExprPlus &ax) {
	return filterGroup->testValue(0,  ax.statAbnorm) &&
			filterGroup->testValue(1, ax.statNormal) &&
			filterGroup->testValue(2, ax.goal) &&
			filterGroup->testValue(4, ax.statOccurence) &&
			filterGroup->testValue(3, ax.statVariation);
}

void Stage2::filterChanged(int /*id*/, double /*value*/, bool /*direction*/) {
	filterItems();
}

void Stage2::saveValuesToStage() {
	saveParams();
	
	m_separateClassDisplay->saveCurrentClass();
}

void Stage2::loadValuesFromStage() {
	
	loadParams();
	//controller->stage2.setCalculated(true);
	//qDebug()<<"Loading stage 2 - initializing\n";
	init();
	//qDebug()<<"Done\n";
	//initializePage();
}

void Stage2::buildNodeForClass(int classNo, QTreeWidgetItem *abnormalTypeItem) {
	const auto& axiomsForClass = controller->stage2.bestAxioms[classNo];
	
	for(unsigned int j = 0; j < axiomsForClass.size(); j++) {
		QTreeWidgetItem *item2 = new QTreeWidgetItem();
		item2->setText(0, axiomsForClass[j].nameOfAxiomExpr.c_str());
		item2->setFlags(
						Qt::ItemIsUserCheckable | Qt::ItemIsEnabled
						);
		if(controller->stage2.bestAxiomsSelected[classNo][j])
		{
			item2->setCheckState(0, Qt::Checked);
		}
		else
		{
			item2->setCheckState(0, Qt::Unchecked);
		}
		item2->setData(1, Qt::DisplayRole, QVariant::fromValue(axiomsForClass[j].statAbnorm));
		item2->setData(2, Qt::DisplayRole, QVariant::fromValue(axiomsForClass[j].statNormal));
		item2->setData(3, Qt::DisplayRole, QVariant::fromValue(axiomsForClass[j].goal));
		item2->setData(4, Qt::DisplayRole, QVariant::fromValue(axiomsForClass[j].statVariation));
		item2->setData(5, Qt::DisplayRole, QVariant::fromValue(axiomsForClass[j].statOccurence));
		item2->setData(0, Qt::UserRole, QVariant::fromValue(
				makeIndex(classNo, j)
				)
			);
		
		//qDebug()<<"statOccurence: "<<allAxioms[i][j].statOccurence;
		
		abnormalTypeItem->addChild(item2);
	}	
}

void Stage2::saveClass(int classNo, QTreeWidgetItem *currentAbnormalTypeItem) {
	auto& axiomsSelected = controller->stage2.bestAxiomsSelected[classNo];
	
	int trueCount = 0;
	for(unsigned int j = 0; j < axiomsSelected.size(); j++) {
		axiomsSelected[j] = currentAbnormalTypeItem->child(j)->checkState(0) == Qt::Checked && 
		   !currentAbnormalTypeItem->child(j)->isHidden();
		if(axiomsSelected[j]) {
			trueCount++;
		}
	}
	
	QtGuiException::assertCondition(trueCount>0,
			"No axioms selected for abnormal type " + boost::lexical_cast<std::string>(classNo));	
}

void Stage2::calculate() {
	try {
		qDebug()<<"saving value to stage...";
		saveValuesToStage();
		qDebug()<<"applying stage...";
		controller->applyStage2();
		qDebug()<<"running...";

		runStage();
	} catch(AxiomLib::AxiomLibException exception) {
		displayException(exception);
	}
}

void Stage2::reloadSelection() {
	m_separateClassDisplay->beginUpdateItems();
	
	const auto& axioms = controller->stage2.bestAxiomsSelected;
	for(int classNo = 0; classNo < axioms.size(); ++classNo) {
		for(int i = 0; i < axioms[classNo].size(); ++i) {
			m_separateClassDisplay->setSelection(makeIndex(classNo, i), axioms[classNo][i]);
		}
	}
	
	m_separateClassDisplay->endUpdateItems();
}

AxiomLib::AxiomExprPlus Stage2::getAxiomByIndex(const Index &index) const {
	return controller->stage2.bestAxioms[index[0]][index[1]];
}
