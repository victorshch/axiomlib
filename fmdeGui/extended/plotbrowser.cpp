#include "plotbrowser.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QtDebug>
#include <list>
#include <algorithm>
#include "ColorManager.h"
#include "SetLegend.h"
#include "boost/foreach.hpp"

#include <QtDebug>

#include "algorithm_controller/ManagedFuzzyDataController.h"

PlotBrowser::PlotBrowser(ManagedFuzzyDataController *controller, int stage, QWidget *parent) 
	: controller(controller), stage(stage), QWidget(parent) {
	ui.setupUi(this);
	
	qDebug()<<"entering PlotBrowser::PlotBrowser()";
	qDebug()<<"creating trajselectgroupbox";
	trajSelectGroupBox = new TrajSelectGroupBox(0, controller, false, false);
	qDebug()<<"adding to layout";
	ui.verticalLayout_trajselect->addWidget(trajSelectGroupBox);
	
	qDebug()<<"creating legend";
	legend = new SetLegend(0);
	ui.verticalLayout_legend->addWidget(legend);

	qDebug()<<"creating intervalwidget";
	intervalWidget = new IntervalWidget(0, 20, 0);
	
	ui.verticalLayout_interval->addWidget(intervalWidget);
	
	std::vector<std::string> paramNames = controller->paramNames();
	std::vector<int> paramNums;
	controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNums(paramNums);
	
	//qDebug()<<"Plotbrowser - num dimensions: "<<controller->numDimensions();
	
	// проход по размерностям
	for(int i = 0; i < paramNums.size(); i++) {
		VectorPlot * vectorPlot = new VectorPlot(0, paramNames[paramNums[i]].c_str(), legend);
		ui.verticalLayout_plots->addWidget(vectorPlot);

		vectorPlotList.push_back(vectorPlot);
	}
	
	connect(intervalWidget, SIGNAL(intervalChanged(int,int)), this, SLOT(adjustInterval(int,int)));
	connect(trajSelectGroupBox, SIGNAL(trajSelected(int,int,int)), this, SLOT(replot()));
	
}

void PlotBrowser::addCondition(PCondition condition) {
	conditionList.push_back(condition);
}

//class FindById {
//	Index *id;
//public:
//	FindById(Index *id): id(id) {}
//	bool operator()(const PCondition &condition) {
//		return condition->identifier()->compare(id);
//	}
//};

//void PlotBrowser::removeCondition(Index *identifier) {
//	QList<PCondition>::iterator it = 
//			std::remove_if(conditionList.begin(), conditionList.end(), FindById(identifier));
//	conditionList.erase(it, conditionList.end());
//}

void PlotBrowser::replot() const {
	legend->clear();
	
	std::vector<std::vector<double> > *multiTSRaw = new std::vector<std::vector<double> >();
	int currentClass = trajSelectGroupBox->currentClass();
	int currentMultiTS = trajSelectGroupBox->currentTS();
	controller->getMultiTS(
			currentClass,
			trajSelectGroupBox->currentTS(),
			*multiTSRaw
			);
	PCMultiVector multiTS = PCMultiVector(multiTSRaw);
	std::vector<int> paramNums;
	controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNums(paramNums);
	// Строим графики рядов
	for(int i = 0; i < vectorPlotList.size(); i++) {
		VectorPlot *currentPlot = vectorPlotList[i];
		currentPlot->clear();
		currentPlot->addLinePlot(
				VectorQwtAdapter(
						multiTS,
						paramNums[i]
						)
				);
	}
	
	// Отмечаем выполнение условий
	ColorManager colorManager;
	std::vector<int> markup;
	for(int i = 0; i < conditionList.size(); i++) {
		Condition *currentCondition = conditionList[i].get();
		if(currentClass >= 0 && currentCondition->abnormalType() != currentClass) {
			continue;
		}
		int numSat = currentCondition->markUp(*multiTS, markup);
		QString nameExt = QString("%1 [%2]").arg(currentCondition->name().c_str(), QString::number(numSat));
		for(std::set<int>::const_iterator it = currentCondition->dimensions().begin();
			it != currentCondition->dimensions().end(); ++it) {
			int dimension = *it;
			auto it2 = std::find(paramNums.begin(), paramNums.end(), dimension);
			if(it2 == paramNums.end()) {
				continue;
			}
			int index = std::distance(paramNums.begin(), it2);
			vectorPlotList[index]->addDotPlot(
					MarkupQwtAdapter(
							multiTS, dimension, markup
							), 
					colorManager.getColor(),
					nameExt,
					true
					);
		}
	}
	
	legend->commit();
	
	for(int i = 0; i < vectorPlotList.size(); i++) {
		vectorPlotList[i]->commit();
	}
	
	AxiomLib::IntInterval range = controller->getRange(currentClass, currentMultiTS);
	intervalWidget->setRange(range.left(), range.right());
	
	AxiomLib::IntInterval interval = controller->getStage(stage).clippingIntervals->
									 getClippingInterval(currentClass, currentMultiTS);
	intervalWidget->setInterval(interval.left(), interval.right());	
}

void PlotBrowser::setInterval(int left, int right) {
	qDebug()<<"Entering PlotBrowser::setInterval()";
	intervalWidget->setInterval(left, right);
	qDebug()<<"Leaving PlotBrowser::setInterval()";
}

void PlotBrowser::adjustInterval(int left, int right) {
	for(int i = 0; i < vectorPlotList.size(); i++) {
		vectorPlotList[i]->adjustMarkers(left, right);
	}
	controller->getStage(stage).clippingIntervals->setClippingInterval(
			AxiomLib::IntInterval(left, right),
			trajSelectGroupBox->currentClass(),
			trajSelectGroupBox->currentTS()
			);	
}

void PlotBrowser::displayMultiTS(int /*classNo*/, int /*multiTSNo*/) {
	replot();
}

PlotBrowser::~PlotBrowser()
{

}
