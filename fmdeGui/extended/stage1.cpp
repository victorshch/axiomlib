#include <cmath>

#include <iterator>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>

#include "stage1.h"
#include <QMessageBox>
#include <QTreeWidgetItemIterator>
#include "utilities.h"
#include "plotbrowser.h"
#include "Condition.h"
#include "functors.h"
#include "relaxed_transform.h"
#include "extecbrowser.h"
#include <QPushButton>
#include <QtDebug>
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "DistanceClusterizer.h"
#include "HeuristicsForm.h"

Stage1::Stage1(QWidget *parent, ManagedFuzzyDataController *controller)
	: StageBase(parent, controller, 1), ecs(controller->stage1.bestECForAllAbnormalTypes)
{
	qDebug()<<"Entering Stage1::Stage1()";
	
	ui.setupUi(this);

	ui.treeWidget_ECs->setColumnCount(5);
	
	ui.treeWidget_ECs->headerItem()->setText(0, encode("Имя"));
	ui.treeWidget_ECs->headerItem()->setText(1, encode("Нештатн."));
	ui.treeWidget_ECs->headerItem()->setText(2, encode("Нормальн."));
	ui.treeWidget_ECs->headerItem()->setText(3, encode("Целев."));
	ui.treeWidget_ECs->headerItem()->setText(4, encode("Встречаемость"));
	
        //freqFilterGroupBox = new FreqFilterGroupBox();
	
	m_filterGroup = new FilterGroup(0, "");
	
	m_filterGroup->addSlider(encode("Нештатн."))->setChecked(true);
	m_filterGroup->addSlider(encode("Нормальн."));
	m_filterGroup->addSlider(encode("Целев."));
	m_filterGroup->addSlider(encode("Встречаемость"));
	
	ui.verticalLayout_ecFilters->addWidget(m_filterGroup);
	
	m_clusteringForm = new ClusteringForm(controller, 0);
	
	ui.verticalLayout_clustering->addWidget(m_clusteringForm);
	
	m_selectionManager = new SelectionManager(0);
	
	ui.horizontalLayout_selectionManager->addWidget(m_selectionManager);
	
	m_classSelectForm = new ClassSelectForm(
	            controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getClassNames(),
	            "AllHeuristics_className",
	            0
	            );
	
	ui.horizontalLayout_classSelect->addWidget(m_classSelectForm);
	
	auto conditionFunction = [this](const Index& index) { 
		return PCondition(new ConditionEC(this->getECByIndex(index), index, index[0])); 
	};
	
	qDebug()<<"Configuring SeparateClassDisplay";
	m_separateClassDisplay = new SeparateClassDisplay(controller, 1, 4, 
	                                                  ConditionFunction(conditionFunction),
	                                                  ui.treeWidget_ECs, 
	                                                  m_classSelectForm, 
	                                                  m_selectionManager, 
	                                                  m_clusteringForm,
	                                                  this);
	
	connect(m_separateClassDisplay, SIGNAL(buildingNodeForClass(int,QTreeWidgetItem*)), this, SLOT(buildNodeForClass(int,QTreeWidgetItem*)));
	connect(m_separateClassDisplay, SIGNAL(savingClass(int,QTreeWidgetItem*)), this, SLOT(saveClass(int,QTreeWidgetItem*)));
	connect(m_separateClassDisplay, SIGNAL(numberChanged(int)), ui.SpinBox_numberOfEC, SLOT(setValue(int)));
	connect(ui.pushButton_showPlots, SIGNAL(clicked()), m_separateClassDisplay, SLOT(displayPlot()));
	qDebug()<<"SeparateClassDisplay configured";
	
	setupParams();	
	setupHeuristics();

	connect(ui.pushButton_compute, SIGNAL(clicked(void)), this, SLOT(calculate(void)));
	
//	connect(ui.treeWidget_ECs, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), 
//			this, SLOT(itemClicked(QTreeWidgetItem*,int)));
	
	connect(m_filterGroup, SIGNAL(sliderSignaled(int,double,bool)), 
			this, SLOT(filterItems()));
	
	qDebug()<<"Leaving Stage1::Stage1()";
}

Stage1::~Stage1()
{

}

void Stage1::initializePage() {
	if(controller->getStage() > 0) {
		if(!controller->stage1.calculated()) {
			controller->createStage1();
		}
		try {
			init();
		//QMessageBox::information(0, "Information", "Stage 1 initialized");
		} catch(AxiomLib::AxiomLibException exception) {
			QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
		}
	}
}

void Stage1::init() {
	m_selectionManager->clear();
	
	buildTree();
	initFreqFilter();
}

void Stage1::initFreqFilterSliders() {
	//qDebug()<<"initializing slider 0";
	m_filterGroup->slider(0)
			->setLimits(
				statAbnormInterval.min(),
				statAbnormInterval.max()
				);
	//qDebug()<<"setting defaults";
	m_filterGroup->slider(0)->setDefaults(statAbnormInterval.max(), false);
	//qDebug()<<"toDefault()";
	m_filterGroup->slider(0)->toDefault();
	
	//qDebug()<<"initializing slider 1";
	m_filterGroup->slider(1)
			->setLimits(
				statNormalInterval.min(),
				statNormalInterval.max()
				)
			->setDefaults(statNormalInterval.min(), true)
			->toDefault();
	
	//qDebug()<<"initializing slider 2";
	m_filterGroup->slider(2)
			->setLimits(
				diffInterval.min(),
				diffInterval.max()
				)
			->setDefaults(diffInterval.max(), false)
			->toDefault();
	//qDebug()<<"initializing slider 3";
	m_filterGroup->slider(3)
			->setLimits(
				occurrenceInterval.min(),
				occurrenceInterval.max()
				)
			->setDefaults(occurrenceInterval.max(), true)
			->toDefault();
}

void Stage1::initFreqFilter() {	
	computeIntervals();	
	
	initFreqFilterSliders();
}

void Stage1::cleanupPage() {

}

bool Stage1::validatePage() {
	return true;
}

void Stage1::filterItems() {
	//qDebug()<<"filtering items in stage1...";
	
	if(ecs.size() == 0)	{
		return;
	}
	
	m_separateClassDisplay->beginUpdateItems();
	
	for(unsigned int i = 0; i < ecs.size(); i++) {
		for(unsigned int j = 0; j < ecs[i].size(); j++) {
			for(unsigned int k = 0; k < ecs[i][j].size(); k++) {
				for(unsigned int l = 0; l < ecs[i][j][k].size(); l++) {
					if(!keepItem(ecs[i][j][k][l])) {
						m_separateClassDisplay->hideItem(makeIndex(i, j, k, l), true);
					} else {
						m_separateClassDisplay->hideItem(makeIndex(i, j, k, l), false);
					}
				}
			}
		}
	}	
	
	m_separateClassDisplay->endUpdateItems();
}

bool Stage1::keepItem(const AxiomLib::ElemCondPlusStat &ec) {
	return m_filterGroup->testValue(0, ec.statAbnorm) &&
			m_filterGroup->testValue(1, ec.statNormal) &&
			m_filterGroup->testValue(2, ec.goal) &&
			m_filterGroup->testValue(3, ec.statOccurence);
}

void Stage1::buildTree() {
	m_separateClassDisplay->buildNodes();
}

void Stage1::initItem(QTreeWidgetItem *item, const QString &text, 
					  const AxiomLib::ElemCondPlusStat &ecPlus, bool checked) {
	item->setText(0, text);
	item->setFlags(
			Qt::ItemIsUserCheckable | Qt::ItemIsEnabled
			);
	
	item->setCheckState(0, checked ?  Qt::Checked : Qt::Unchecked);
	
	item->setData(1, Qt::DisplayRole, QVariant::fromValue(
			ecPlus.statAbnorm
			)
		);
	
	item->setData(2, Qt::DisplayRole, QVariant::fromValue(
			ecPlus.statNormal
			)
		);

	item->setData(3, Qt::DisplayRole, QVariant::fromValue(
			ecPlus.goal
			)
		);
	item->setData(4, Qt::DisplayRole, ecPlus.statOccurence);
}

//void Stage1::itemClicked(QTreeWidgetItem *item, int /*column*/) {
//	//QMessageBox::information(0, "information", "Item clicked");
//	IndexQuadruple *indexQuadruple = dynamic_cast<IndexQuadruple *>(
//			item->data(0, Qt::UserRole).value<QObject*>());
	
//	AxiomLib::ElemCondPlusStat ecPlus = ecs[indexQuadruple->indices.get<0>()]
//										[indexQuadruple->indices.get<1>()]
//										[indexQuadruple->indices.get<2>()]
//										[indexQuadruple->indices.get<3>()];
	
//	std::string &ecName = ecPlus.name;
	
//	ExtECBrowser *ecBrowser = new ExtECBrowser(0, controller, ecPlus,
//											   ecName.c_str(),
//											   indexQuadruple
//											   );
	
	
//	connect(ecBrowser, SIGNAL(ECChanged(AxiomLib::ElemCondPlus,const QObject*)), 
//			this, SLOT(ECChanged(AxiomLib::ElemCondPlus,const QObject*)));
//	ecBrowser->show();
//}

//void Stage1::ECChanged(AxiomLib::ElemCondPlus ec, const QObject *identifier) {
//	const IndexQuadruple *indices = dynamic_cast<const IndexQuadruple *>(identifier);
//	if(indices) {
		
//		//qDebug()<<"ECTypeName: "<<ec.elemCondition->name().c_str();
		
//		//qDebug()<<"getting indices...";
		
//		int i = indices->indices.get<0>();
//		int j = indices->indices.get<1>();
//		int k = indices->indices.get<2>();
//		int l = indices->indices.get<3>();
		
//		//qDebug()<<"saving values to stage...";
		
//		ecs[i][j][k][l] = AxiomLib::ElemCondPlusStat(ec);
		
//		//qDebug()<<"calculating matter func...";
		
////		controller->fuzzyMultiDataLearnAlgorithm.matterECFunc(
////				ecs[i][j][k][l],
////				ecs[i][j][k][l].dimension,
////				i
////				);
//		//TODO: remove function
		
//		saveValuesToStage();
		
//		buildTree();
//		initFreqFilter();
//	}
//}

void Stage1::setupParams() {
	ParamsForm* paramsForm = new ParamsForm();
	
	paramsForm->addParam(StrategyParamSpec(1, 10000, 1, 200, "maxAxiomPopSize"), 
	                     encode("Максимальное число аксиом в популяции"));
	paramsForm->addParam(StrategyParamSpec(1, 100, 1, 8, "axiomAlgIterNum"), 
	                     encode("Максимальное число итераций алгоритма поиска аксиом"));
	paramsForm->addParam(StrategyParamSpec(10, 1000000, 1, 100000, "maxCostForAxiom"), 
	                     encode("Максимальное значение целевой функции для аксиомы"));
	paramsForm->addParam(StrategyParamSpec(0.01, 1.0, 0.01, 0.85, "percentBestAxioms"), 
	                     encode("Сохраняемая доля лучших аксиом"));
	paramsForm->addParam(StrategyParamSpec(0.0, 1e6, 0.01, 0.0, "axiomGoalOccurenceWeight"), 
	                     encode("Вес встречаемости в целевой функции"));
	
	ui.verticalLayout_params->addWidget(paramsForm);
	
	registerParamsForm(paramsForm);	
	
	registerParamsForm(m_classSelectForm);
}

void Stage1::setupHeuristics() {
	HeuristicsForm* heuristicsForm = new HeuristicsForm("Heuristics", "ECStatClustering", encode("Кластеризация по статистике"));
	heuristicsForm->addParam(StrategyParamSpec(1, 100, 1, 1, "retain"), encode("Число оставляемых ЭУ с одинаковой статистикой"));
	ui.verticalLayout_heuristics->addWidget(heuristicsForm);
	registerHeuristicsForm(heuristicsForm);
	
	heuristicsForm = new HeuristicsForm("Heuristics", "ECOccFilter", encode("Фильтрация по встречаемости"));
	heuristicsForm->addParam(StrategyParamSpec(0.01, 1.0, 0.01, 0.2, "offset"), encode("Отступ от максимальной встречаемости"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 100, "requiredSaturation"), encode("Требуемое насыщение"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 200, "maxSaturation"), encode("Максимальное насыщение"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 200, "maxToSelect"), encode("Максимальное число выбираемых ЭУ"));
	ui.verticalLayout_heuristics->addWidget(heuristicsForm);
	registerHeuristicsForm(heuristicsForm);
	
	heuristicsForm = new HeuristicsForm("Heuristics", "ECOccClustering", encode("Кластеризация по точкам выполнения"));
	heuristicsForm->addParam(StrategyParamSpec(2, 10000, 1, 2, "minDistance"), encode("Минимальное расстояние между кластерами"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 3, "maxClusterSize"), encode("Максимальный размер кластера"));
	heuristicsForm->addParam(StrategyParamSpec(0, 10000, 1, 3, "minMaxClusterSize"), encode("Минимальный максимум размера\n кластера на траектории"));
	ui.verticalLayout_heuristics->addWidget(heuristicsForm);
	registerHeuristicsForm(heuristicsForm);
}

void Stage1::saveClass(int classNo, QTreeWidgetItem *classItem) {
	auto& selected = controller->stage1.abnormalTypesSelected;
	
	//auto& ecs = controller->stage1.bestECForAllAbnormalTypes;
	
//	for(unsigned int j = 0; j < classItem->ch; j++) {
//		for(unsigned int k = 0; k < ecs[classNo][j].size(); k++) {
//			for(unsigned int l = 0; l < ecs[classNo][j][k].size(); l++) {
//				QTreeWidgetItem* currentItem = classItem->child(j)->child(k)->child(l);
//				selected[classNo][j][k][l] = 
//					currentItem->checkState(0) == Qt::Checked && 
//					!currentItem->isHidden();
//			}
//		}
//	}
	
	QList<Index> selection = m_separateClassDisplay->getSelection(classNo);

	std::for_each(selection.begin(), selection.end(), [&selected](const Index& index){ 
	              selected[index[0]][index[1]][index[2]][index[3]] = true;
	              });
}

void Stage1::buildNodeForClass(int classNo, QTreeWidgetItem *abnormalTypeItem) {
	ECMultiVector &container = controller->stage1.bestECForAllAbnormalTypes;
	std::vector<std::string> paramNames = controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNames();
	std::vector<int> paramNums = controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNums();
	
	for(unsigned int j = 0; j < container[classNo].size(); j++) {
		QTreeWidgetItem *item2 = new QTreeWidgetItem();
		QString dimName;
		if(paramNums.size() > j && paramNames.size() > paramNums[j] && paramNums[j] >= 0) {
			dimName = QString::fromStdString(
						paramNames[paramNums[j]]);
		} else {
			dimName = QString::number(j);
		}
		item2->setText(0, encode("Разм. ")+dimName);
		abnormalTypeItem->addChild(item2);
		for(unsigned int k = 0; k < container[classNo][j].size(); k++) {
			if(container[classNo][j][k].size()==0) {
				//qDebug()<<"continuing with container["<<classNo<<"]["<<j<<"]["<<k<<"].size()==0";
//				QTreeWidgetItem *item3 = new QTreeWidgetItem();
//				item3->setText(0, "hidden");
//				item2->addChild(item3);
//				item3->setHidden(true);
				continue;
			}
			//QString ecTypeName = getECTypeName(container[i][j][k][0].c_str());
			QTreeWidgetItem *item3 = new QTreeWidgetItem();
			item3->setText(0, QString::fromStdString(
								getFullECTypeName(container[classNo][j][k][0])
							));
			item2->addChild(item3);
			for(unsigned int l = 0; l < container[classNo][j][k].size(); l++) {
				QTreeWidgetItem *item4 = new QTreeWidgetItem();
				
				initItem(
						item4,
						container[classNo][j][k][l].name.c_str(),
						container[classNo][j][k][l],
						controller->stage1.abnormalTypesSelected[classNo][j][k][l]
						);
				
				item4->setData(0, Qt::UserRole, QVariant::fromValue(
							makeIndex(classNo, j, k, l)
						));
				
				item3->addChild(item4);
			}
			item3->setExpanded(true);
		}
		item2->setExpanded(true);
	}	
}

void Stage1::saveValuesToStage()  {
	qDebug()<<"Entering saveValuesToStage()";
	qDebug()<<"Saving params...";
	
	saveParams();
	
	qDebug()<<"Reseting ECs selection...";
	
	auto& selected = controller->stage1.abnormalTypesSelected;
	TransformMultiVectorHelper<3>::forEach(selected, 
	                                       [](std::vector<bool>& v) { std::fill(v.begin(), v.end(), false); });

	qDebug()<<"Saving ECs...";
	
	m_separateClassDisplay->saveCurrentClass();
	
	qDebug()<<"Exiting saveValuesToStage()";
}

void Stage1::loadValuesFromStage() {
	loadParams();
	
	controller->stage1.setCalculated(true);
	qDebug()<<"Loading stage - initializing\n";
	init();
	qDebug()<<"Done\n";
	//initializePage();
}

void Stage1::computeIntervals()
{	
	qDebug()<<"Computing intervals...";
	statNormalInterval.reset();
	TransformMultiVectorHelper<4>::forEach(ecs, 
										   boost::lambda::bind(&DoubleInterval::fitLimits, boost::ref(statNormalInterval), 
												boost::lambda::bind(&AxiomLib::ElemCondPlusStat::statNormal, 
													 boost::lambda::_1)));
	qDebug()<<statNormalInterval.toString().c_str();
	
	statAbnormInterval.reset();
	TransformMultiVectorHelper<4>::forEach(ecs, 
										   boost::lambda::bind(&DoubleInterval::fitLimits, boost::ref(statAbnormInterval), 
												boost::lambda::bind(&AxiomLib::ElemCondPlusStat::statAbnorm, 
													 boost::lambda::_1)));	
	qDebug()<<statAbnormInterval.toString().c_str();
	
	diffInterval.reset();
	diffInterval.setMaxMax(1000.0);
	TransformMultiVectorHelper<4>::forEach(ecs, 
										   boost::lambda::bind(&DoubleInterval::fitLimits, boost::ref(diffInterval), 
																	boost::lambda::bind(&AxiomLib::ElemCondPlusStat::goal, 
																		 boost::lambda::_1)));
	qDebug()<<diffInterval.toString().c_str();
	
	occurrenceInterval.reset();
	TransformMultiVectorHelper<4>::forEach(ecs, 
										   boost::lambda::bind(&DoubleInterval::fitLimits, boost::ref(occurrenceInterval), 
															   boost::lambda::bind(&AxiomLib::ElemCondPlusStat::statOccurence, 
																		 boost::lambda::_1)));
	qDebug()<<occurrenceInterval.toString().c_str();
	
}

void Stage1::calculate() {
	try {
		saveValuesToStage();
	} catch(AxiomLib::AxiomLibException exception) {
		displayException(exception);
		return;
	}
	controller->applyStage1();

	runStage();
}

void Stage1::reloadSelection() {
	QList<Index> selection;
	
	for(int classNo = 0; classNo < controller->stage1.abnormalTypesSelected.size(); classNo++) {
		const auto& currentClassSelection = controller->stage1.abnormalTypesSelected[classNo];
		for(int i = 0; i < currentClassSelection.size(); ++i) {
			for(int j = 0; j < currentClassSelection[i].size(); ++j) {
				for(int k = 0; k < currentClassSelection[i][j].size(); ++k) {
					if(currentClassSelection[i][j][k]) {
						selection.append(makeIndex(classNo, i, j, k));
					}
				}
			}
		}
	}
	
	m_separateClassDisplay->applySelection(selection, false);
}

const AxiomLib::ElemCondPlusStat & Stage1::getECByIndex(Index index) const {
	qDebug()<<"Entering getECByIndex()";
	return controller->stage1.bestECForAllAbnormalTypes[index[0]][index[1]][index[2]][index[3]];
	qDebug()<<"Leaving getECByIndex()";
}
