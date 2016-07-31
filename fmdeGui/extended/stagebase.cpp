#include "stagebase.h"

#include "logwindow.h"
#include "LogThread.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "AbstractParamsForm.h"
#include "HeuristicsForm.h"
#include "QtGuiException.h"


#include <QMessageBox>
#include <QDebug>

#define MULTI_THREADED

StageBase::StageBase(QWidget *parent, ManagedFuzzyDataController *controller, int stageNumber) :
	QWidget(parent),
	controller(controller),
    m_stageNumber(stageNumber),
    m_currentClass(-1),
    m_classCount(-1)
{
	m_heuristicsMapper = new QSignalMapper(this);
	connect(m_heuristicsMapper, SIGNAL(mapped(QWidget*)), this, SLOT(runHeuristics(QWidget*)));
	
	m_calculatedMapper = new QSignalMapper(this);
	connect(m_calculatedMapper, SIGNAL(mapped(int)), this, SIGNAL(stageCalculated(int)));
	
	m_partialReloadMapper = new QSignalMapper(this);
	connect(m_partialReloadMapper, SIGNAL(mapped(int)), this, SIGNAL(stageRecalculated(int)));
}

void StageBase::runStage() {
	int stageNum = this->stageNumber() + 1;
	ThreadStart threadStart(
		[this, stageNum]() { this->controller->runLevel(stageNum); }
	);

	runThread(threadStart, stageNum);
}
	
void StageBase::runHeuristics() {
	
	int stageNum = this->stageNumber();
	ThreadStart threadStart(
		[this, stageNum]() { this->controller->runHeuristics(stageNum); }
	);

	runThread(threadStart, stageNum, false);	
}

void StageBase::saveParams() {
	qDebug()<<"Saving params. ParamsForm count: "<<m_paramsForms.size();
	
	for(auto it = m_paramsForms.begin(); it != m_paramsForms.end(); ++it) {
		copyParamsToStage(*it);
	}
}

void StageBase::loadParams() {	
	qDebug()<<"Loading params. ParamsForm count: "<<m_paramsForms.size();
	int i = 0;
	for(auto it = m_paramsForms.begin(); it != m_paramsForms.end(); ++it) {
		qDebug()<<"Loading params form "<<i;
		copyParamsFromStage(*it);
		qDebug()<<"Params form "<<i<<" loaded";
		++i;
	}
}
	
void StageBase::refresh() {
	emit stageRefreshing();
	
	loadValuesFromStage();
}
	
void StageBase::reloadValuesFromStage() {
	// По умолчанию -- просто перезагружаем данные
	loadValuesFromStage();
}

void StageBase::exception(AxiomLib::AxiomLibException e) {
	throw e;
}

void StageBase::runHeuristics(QWidget *widget) {
	if(stageNumber() < 1 || stageNumber() > 2) {
		throw std::logic_error("Can't run heuristics for stage "+boost::lexical_cast<std::string>(stageNumber()));
	}
	
	HeuristicsForm* heuristicsForm = dynamic_cast<HeuristicsForm*>(widget);
	
	if(!heuristicsForm) {
		throw std::logic_error("Invalid widget in runHeuristics");
	}
	
	//TODO убрать лишние копирования
	saveValuesToStage();
	copyParamsToStage(heuristicsForm);
	controller->getStage(stageNumber()).save();
	
	qDebug()<<"Running heuristics "<<heuristicsForm->strValue("Heuristics").c_str();
	
	//controller->runHeuristics(stageNumber());
	runHeuristics();
}

int StageBase::stageNumber() const {
	return m_stageNumber;
}

void StageBase::copyParamsToStage(const AbstractParamsForm *paramsForm) {
	std::list<std::string> params = paramsForm->paramNames();
	
	qDebug()<<"Saving params. Params count = "<<params.size();
	
	ManagedFuzzyDataStage& stage = controller->getStage(stageNumber());
	
	for(auto it = params.begin(); it != params.end(); ++it) {
		std::string value = paramsForm->strValue(*it);
		qDebug()<<"Saving param "<<it->c_str()<<" with value "<<value.c_str();
		stage.setParamValue(*it, value);
	}
}

void StageBase::copyParamsFromStage(AbstractParamsForm *paramsForm) {
	std::list<std::string> params = paramsForm->paramNames();
	
	qDebug()<<"Loading params. Params count = "<<params.size();
	
	const ManagedFuzzyDataStage& stage = controller->getStage(stageNumber());
	
	for(auto it = params.begin(); it != params.end(); ++it) {
		std::string value = stage.getParamValue(*it);
		qDebug()<<"Loading param "<<it->c_str()<<" with value "<<value.c_str();
		paramsForm->setStrValue(*it, stage.getParamValue(*it));
	}	
}

void StageBase::registerHeuristicsForm(HeuristicsForm *heuristicsForm) {
	m_heuristicsMapper->setMapping(heuristicsForm, static_cast<QWidget*>(heuristicsForm));
	connect(this, SIGNAL(stageRefreshing()), heuristicsForm, SLOT(enable()), Qt::UniqueConnection);
	connect(heuristicsForm, SIGNAL(executed()), m_heuristicsMapper, SLOT(map()), Qt::UniqueConnection);
		
	registerParamsForm(heuristicsForm);
	
	heuristicsForm->disable();
}

void StageBase::registerParamsForm(AbstractParamsForm *paramsForm) {
	m_paramsForms.push_back(paramsForm);
}

void StageBase::runThread(ThreadStart threadStart, int numStageToRefresh, bool fullReload) {
	qDebug()<<"Entering runThread()";
	
	QObject obj(0);
	LogThread* thread = new LogThread(threadStart, &obj);
	
	QSignalMapper* mapper = fullReload? m_calculatedMapper : m_partialReloadMapper;
	
	mapper->setMapping(thread, numStageToRefresh);
	connect(thread, SIGNAL(finished()), mapper, SLOT(map()));
	connect(thread, SIGNAL(exception(AxiomLib::AxiomLibException)), 
	        this, SLOT(exception(AxiomLib::AxiomLibException)));
	
	qDebug()<<"Starting thread";
	thread->start();	
	qDebug()<<"Exiting runThread()";
}
