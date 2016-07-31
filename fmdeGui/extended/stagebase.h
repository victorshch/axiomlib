#ifndef STAGEBASE_H
#define STAGEBASE_H

#include "StdStringHash.h"

#include <QList>
#include <QSignalMapper>
#include <QWidget>

#include "algorithm_controller/forwarddecls.h"
#include "AxiomLibException.h"
#include "ParamsForm.h"
#include "StrategyFrame.h"
#include "ClassSelectForm.h"

#include "thread.h"

class HeuristicsForm;

class StageBase : public QWidget
{
    Q_OBJECT
public:
	explicit StageBase(QWidget *parent, ManagedFuzzyDataController * controller, int stageNumber);

signals:
	void stageCalculated(int);
	void stageRecalculated(int);
	
	void stageRefreshing();
public slots:
	void exception(AxiomLib::AxiomLibException e);
	
	void runHeuristics(QWidget* heuristicsForm);
	//void saveParams(QWidget* paramsForm);
	
	void refresh();
public slots:
	// Загрузка новых значений из стадии в интерфейс
	virtual void loadValuesFromStage() = 0;
	// Подгрузка измененного выбора ЭУ/аксиом (используется с эвристиками)
	virtual void reloadValuesFromStage();
	// Сохранение значений, заданных пользователем в интерфейсе, в стадию
	virtual void saveValuesToStage() = 0;
protected:
	ManagedFuzzyDataController * controller;
	
	void runStage();
	void runHeuristics();
	
	void saveParams();
	void loadParams();
	
	void registerHeuristicsForm(HeuristicsForm* heuristicsForm);
	void registerParamsForm(AbstractParamsForm* paramsForm);
	
	int stageNumber() const;

private:
	void copyParamsToStage(const AbstractParamsForm* paramsForm);
	void copyParamsFromStage(AbstractParamsForm* paramsForm);	
	
	const int m_stageNumber;
	
	int m_currentClass;
	int m_classCount;
	
	QSignalMapper* m_heuristicsMapper;
	QSignalMapper* m_calculatedMapper;
	QSignalMapper* m_partialReloadMapper;
	
	QList<AbstractParamsForm*> m_paramsForms;
	
	void runThread(ThreadStart threadStart, int stageToRefresh, bool fullReload = true);
};

#endif // STAGEBASE_H
