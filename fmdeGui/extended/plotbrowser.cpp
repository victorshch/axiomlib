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

PlotBrowser::PlotBrowser(ManagedFuzzyDataController *controller, int stage, bool showMarking, QWidget *parent, int dataSetDivision)
	: controller(controller), stage(stage), mShowMarking(showMarking), mMarkingPlot(0), QWidget(parent), dataSetDivision(dataSetDivision) {
	ui.setupUi(this);
	
	qDebug()<<"entering PlotBrowser::PlotBrowser()";
	qDebug()<<"creating trajselectgroupbox";
	trajSelectGroupBox = new TrajSelectGroupBox(0, controller, false, false);
	qDebug()<<"adding to layout";
	ui.verticalLayout_trajselect->addWidget(trajSelectGroupBox);
	
	qDebug()<<"creating legend";
	legend = new SetLegend(0);
	ui.verticalLayout_legend->addWidget(legend, 1);

	qDebug()<<"creating intervalwidget";
	intervalWidget = new IntervalWidget(0, 20, 0);
	
	ui.verticalLayout_interval->addWidget(intervalWidget);
	
	std::vector<std::string> paramNames = controller->paramNames();
	std::vector<int> paramNums;
	controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNums(paramNums);
	
	// проход по размерностям
	for(int i = 0; i < paramNums.size(); i++) {
		VectorPlot * vectorPlot = new VectorPlot(0, paramNames[paramNums[i]].c_str(), legend);
		ui.verticalLayout_plots->addWidget(vectorPlot);

		vectorPlotList.push_back(vectorPlot);
		connect(vectorPlot, SIGNAL(zoomed(QwtDoubleRect)), this, SLOT(onZoomed(QwtDoubleRect)));
	}

	if(mShowMarking) {
		mMarkingPlot = new VectorPlot(0, "Marking");
		mMarkingPlot->setMinimumHeight(100);
		ui.verticalLayout_plots->addWidget(mMarkingPlot);

		connect(mMarkingPlot, SIGNAL(zoomed(QwtDoubleRect)), this, SLOT(onZoomed(QwtDoubleRect)));

		std::vector<std::string> classNames = controller->fuzzyMultiDataLearnAlgorithm.getDataSet().getClassNames();

		for(unsigned i = 0; i < classNames.size(); ++i) {
			VectorPlot* abnormalMarkingPlot = new VectorPlot(0,
				QString("Marking for '%1'").arg(QString::fromStdString(classNames[i])));
			abnormalMarkingPlot->setMinimumHeight(100);

			mAbnormalMarkingPlots.append(
				abnormalMarkingPlot
				);
			ui.verticalLayout_legend->addWidget(abnormalMarkingPlot, 0);
		}

		for(unsigned i = 0; i < classNames.size(); ++i) {
			VectorPlot* vectorPlot = new VectorPlot(0,
				QString("Prelabeling '%1'").arg(QString::fromStdString(classNames[i])));

			mPrelabelingPlots.append(vectorPlot);
			ui.verticalLayout_plots->addWidget(vectorPlot);

			connect(vectorPlot, SIGNAL(zoomed(QwtDoubleRect)), this, SLOT(onZoomed(QwtDoubleRect)));
		}
	}
	
	connect(intervalWidget, SIGNAL(intervalChanged(int,int)), this, SLOT(adjustInterval(int,int)));
	connect(trajSelectGroupBox, SIGNAL(trajSelected(int,int,int)), this, SLOT(replot()));	
}

void PlotBrowser::addCondition(PCondition condition) {
	conditionList.push_back(condition);
}

void PlotBrowser::setAxiomSet(AxiomLib::AxiomExprSetPlus as)
{
	mAs = as;
	for(unsigned i = 0; i < as.axioms.size(); ++i) {
		PCondition condition = PCondition(new ConditionAxiom(*as.axioms[i],
															 makeIndex(i+1),
															 -1));
		addCondition(condition);
	}
}

void PlotBrowser::replot() const {
	legend->clear();
	
	std::vector<std::vector<double> > *multiTSRaw = new std::vector<std::vector<double> >();
	int currentClass = trajSelectGroupBox->currentClass();
	int currentMultiTS = trajSelectGroupBox->currentTS();
	controller->getMultiTS(
			currentClass,
			trajSelectGroupBox->currentTS(),
			*multiTSRaw,
				dataSetDivision
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
		currentPlot->addClippingMarkers();
	}
	
	// Отмечаем выполнение условий
	ColorManager colorManager;
	std::vector<int> markup;
	std::vector<int> marking;
	if(multiTS->size() > 0) {
		marking.resize((*multiTS)[0].size(), 0);
	} else {
		qDebug() << "PlotBrowser::replot(): Warning: empty MultiTS";
	}
	for(int i = 0; i < conditionList.size(); i++) {
		Condition *currentCondition = conditionList[i].get();
		if(currentClass >= 0
				&& currentCondition->abnormalType() != currentClass
				// if condition's abnormal type is -1, we show it on all trajectories
				&& currentCondition->abnormalType() != -1) {
			continue;
		}
		int numSat = currentCondition->markUp(*multiTS, markup);
		if(marking.size() != markup.size()) {
			marking.resize(markup.size());
		}
		for(unsigned j = 0; j < marking.size(); ++j) {
			if(markup[j]) {
				marking[j] = i + 1;
			}
		}
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
	
	if(mShowMarking) {
		mMarkingPlot->addMarking(marking);
	}

	legend->commit();
	
	for(int i = 0; i < vectorPlotList.size(); i++) {
		vectorPlotList[i]->commit();
	}

	if(mShowMarking) {
		mMarkingPlot->commit();
	}
	
	AxiomLib::IntInterval range = controller->getRange(currentClass, currentMultiTS);
	intervalWidget->setRange(range.left(), range.right());
	
	AxiomLib::IntInterval interval = controller->getStage(stage).clippingIntervals->
									 getClippingInterval(currentClass, currentMultiTS);
	intervalWidget->setInterval(interval.left(), interval.right());

	if(mShowMarking && mAs.markUps.size()) {
		for(int i = 0; i < mAbnormalMarkingPlots.size(); ++i) {
			mAbnormalMarkingPlots[i]->clear();
			mAbnormalMarkingPlots[i]->addMarking(
						mAs.markUps[i]
						);
			mAbnormalMarkingPlots[i]->commit();
		}

		boost::shared_ptr<AxiomLib::ReducedRecognizer> recognizer = controller->createReducedRecognizer();

		for(int i = 0; i < mPrelabelingPlots.size(); ++i) {
			std::vector<double> prelabeling;
			recognizer->run(marking, mAs.markUps[i], prelabeling);

			mPrelabelingPlots[i]->clear();
			mPrelabelingPlots[i]->addPrelabelingPlot(prelabeling);
			double precision;
			if(!controller->env.getDoubleParamValue(precision, "precision")) {
				mPrelabelingPlots[i]->addHorizontalLine(precision, "Precision");
			}
		}
	}
}

#define EPS 0.00001
#define ISZERO(d) (d<EPS && d>-EPS)

void PlotBrowser::onZoomed(const QwtDoubleRect &rect)
{
	VectorPlot* source = dynamic_cast<VectorPlot*>(sender());
	if(!source) {
		qDebug() << "PlotBrowser::onZoomed(): invalid sender";
		return;
	}

	qDebug() << "Entering PlotBrowser::onZoomed(), rect: " << rect;

	QwtDoubleRect normalizedRect = rect;

	normalizedRect.setTop(ISZERO(source->boundingRect().top()) ?
							0 :
							rect.top() / source->boundingRect().top());
	normalizedRect.setBottom(ISZERO(source->boundingRect().bottom()) ?
							 0 :
							 rect.bottom() / source->boundingRect().bottom());

	foreach(VectorPlot* vectorPlot, vectorPlotList) {
		if(vectorPlot != sender()) {
			QwtDoubleRect scaledRect = normalizedRect;
			scaledRect.setBottom(scaledRect.bottom() * vectorPlot->boundingRect().bottom());
			scaledRect.setTop(scaledRect.top() * vectorPlot->boundingRect().top());
			vectorPlot->blockSignals(true);
			vectorPlot->zoom(scaledRect);
			vectorPlot->blockSignals(false);
		}
	}

	if(mMarkingPlot && sender() != mMarkingPlot) {
		QwtDoubleRect scaledRect = normalizedRect;
		scaledRect.setBottom(-1);
		scaledRect.setTop(1);
		mMarkingPlot->blockSignals(true);
		mMarkingPlot->zoom(scaledRect);
		mMarkingPlot->blockSignals(false);
	}
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
