/****************************************************************************
*			stage0
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/

#include "stage0.h"
#include "relaxed_transform.h"
#include <QMessageBox>
#include <QDebug>

#include <boost/thread.hpp>

#include "ParamsForm.h"

#include "algorithm_controller/ManagedFuzzyDataController.h"

#include "plotbrowser.h"

#include "thread.h"
#include "logwindow.h"

Stage0::Stage0(QWidget *parent, ManagedFuzzyDataController * controller): StageBase(parent ,controller, 0)
{
	ui.setupUi(this);

	paramsForm = new ParamsForm();
	
	paramsForm->addParam(StrategyParamSpec(1, 100, 1, 3, "leftLimit"), encode("Левая граница окрестности"));
	paramsForm->addParam(StrategyParamSpec(1, 100, 1, 3, "rightLimit"), encode("Правая граница окрестности"));
	paramsForm->addParam(StrategyParamSpec(0.1, 100.0, 0.1, 1.5, "reserve"), encode("Фактор расширения пространства параметров"));
	paramsForm->addParam(StrategyParamSpec(2, 1000, 1, 150, "numOfLevels"), encode("Число разбиений сетки"));
	paramsForm->addParam(StrategyParamSpec(1, 200, 1, 10, "numBestECs"), 
	                     encode("Минимальное сохраняемое число ЭУ для каждого типа ЭУ, размерности и типа нештатного поведения"));
	paramsForm->addParam(StrategyParamSpec(0.0, 100000.0, 1.0, 0.0, "ECGoalOccurenceWeight"), encode("Вес встречаемости в целевой функции"));
//	paramsForm->addParam(StrategyParamSpec(QString(), QString(""), QString(), QString(), "ECNameTemplate"),
//	                     encode("Шаблон имен файлов ЭУ"));
	
	ui.verticalLayout_params->addWidget(paramsForm);
	
	registerParamsForm(paramsForm);
	
	connect(ui.pushButton_selectAll, SIGNAL(clicked(void)), this, SLOT(selectAll()));
	connect(ui.pushButton_cancelAll, SIGNAL(clicked(void)), this, SLOT(deselectAll()));
	connect(ui.pushButton_calculate, SIGNAL(clicked(void)), this, SLOT(calculate()));
	connect(ui.pushButton_plots, SIGNAL(clicked()), this, SLOT(displayPlots()));
}

Stage0::~Stage0()
{

}

bool Stage0::validatePage() {
	return controller->stage0.calculated();
}

void Stage0::cleanupPage() {


}

void Stage0::saveValuesToStage() {
	
	saveParams();
	
	std::vector<bool> checkedECs;
	checkedECs.reserve(allECTypes.size());
	for(int i = 0; i < (int) allECTypes.size(); i++) {
		if(ui.listWidget_ECTypes->item(i)->checkState() == Qt::Checked) {
			checkedECs.push_back(true);
		} else {
			checkedECs.push_back(false);
		}
	}

//	copyTransformed(newEcs,
//					controller->stage0.initialECTypes,
//					ManagedFuzzyDataController::getSpecByECType
//			);
	controller->stage0.checkedECTypes = checkedECs;
}

void Stage0::loadValuesFromStage() {

	loadParams();
	
	for(int i = 0; i < ui.listWidget_ECTypes->count(); i++) {
		ui.listWidget_ECTypes->item(i)->setCheckState(Qt::Unchecked);
	}

	int ecTypesCount = controller->stage0.initialECTypes.size();
	allECTypes = controller->stage0.initialECTypes;
	std::vector<AxiomLib::ElemCondPlus> &ecs = allECTypes;
	
	ui.listWidget_ECTypes->clear();
	
	for(int i = 0; i < ecTypesCount; i++)
	{
		QString name = QString(ecs[i].elemCondition->name().c_str());
		if(ecs[i].sign == false) {
			name.prepend("not ");
		}
		
		QListWidgetItem *item = new QListWidgetItem(name, ui.listWidget_ECTypes);
		item->setFlags(
				Qt::ItemIsUserCheckable | Qt::ItemIsEnabled
				);
		item->setTextAlignment(Qt::AlignRight);
		
		item->setCheckState(
			controller->stage0.checkedECTypes[i] ?
				Qt::Checked : 
				Qt::Unchecked
			);
	}
}

void Stage0::initializePage() {

	controller->createStage0();
	
	loadValuesFromStage();
}

void Stage0::selectAll() {
	int count = ui.listWidget_ECTypes->count();
	for(int i = 0; i < count; i++) {
		QListWidgetItem *item = ui.listWidget_ECTypes->item(i);
		item->setCheckState(Qt::Checked);
	}
}

void Stage0::deselectAll() {
	int count = ui.listWidget_ECTypes->count();
	for(int i = 0; i < count; i++) {
		QListWidgetItem *item = ui.listWidget_ECTypes->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}

void Stage0::calculate() {
	saveValuesToStage();

	controller->applyStage0();
	
	this->runStage();
}

void Stage0::displayPlots() {
	PlotBrowser *plotBrowser = new PlotBrowser(controller, 0);
	
	plotBrowser->setAttribute(Qt::WA_DeleteOnClose, true);
	plotBrowser->setWindowModality(Qt::ApplicationModal);
	
	plotBrowser->replot();
	
	plotBrowser->show();
}
