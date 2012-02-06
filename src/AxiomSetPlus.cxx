/****************************************************************************
*
*				Функции структуры AxiomSetPlus
*
****************************************************************************/

#include "AxiomSetPlus.h"

using namespace AxiomLib;

// Конструктор структуры
AxiomSetPlus::AxiomSetPlus (void) {
	axiomSet = NULL;
	goal = -1.0;
	nFirst = -1;
	nSecond = -1;
	axiomProfit.clear();
	goalRatio = 0.0;
	asCrossProb = 0.0;
	axCrossProb = 0.0;
	ecCrossProb = 0.0;
	prTransProb = 0.0;
	asTransProb = 0.0;
	axTransProb = 0.0;
	ecTransProb = 0.0;
}

// Деструктор структуры
AxiomSetPlus::~AxiomSetPlus (void) {
	if (axiomSet != NULL) {
		axiomSet->clear ();
		delete axiomSet;
		axiomSet = NULL;
	}
}

// Оператор присваивания системы аксиом
AxiomSetPlus & AxiomSetPlus::operator= (const AxiomSetPlus &second) {
	if (axiomSet == NULL)
		axiomSet = new AxiomSet;
	if (second.axiomSet != NULL)
		*axiomSet = *(second.axiomSet);
	else {
		delete axiomSet;
		axiomSet = NULL;
	}
	goal = second.goal;
	nFirst = second.nFirst;
	nSecond = second.nSecond;
	axiomProfit = second.axiomProfit;
	// Отношение значений целевой функции
	goalRatio = second.goalRatio;
	// Параметры процедуры скрещивания
	asCrossProb = second.asCrossProb;
	axCrossProb = second.asCrossProb;
	ecCrossProb = second.ecCrossProb;
	// Параметры процедуры мутации
	prTransProb = second.prTransProb;
	asTransProb = second.asTransProb;
	axTransProb = second.axTransProb;
	ecTransProb = second.ecTransProb;
	return *this;
}

// Функция очистки статистики по системе аксиом
int AxiomSetPlus::clearStat (void) {
	goal = -1.0;
	nFirst = -1;
	nSecond = -1;
	axiomProfit.clear();
	goalRatio = 0.0;
	asCrossProb = 0.0;
	axCrossProb = 0.0;
	ecCrossProb = 0.0;
	prTransProb = 0.0;
	asTransProb = 0.0;
	axTransProb = 0.0;
	ecTransProb = 0.0;
	return 0;
}

// Функция очистки содержимого объекта структуры
int AxiomSetPlus::clear (void) {
	if (axiomSet != NULL) {
		axiomSet->clear();
		delete axiomSet;
		axiomSet = NULL;
	}
	goal = -1.0;
	nFirst = -1;
	nSecond = -1;
	axiomProfit.clear();
	goalRatio = 0.0;
	asCrossProb = 0.0;
	axCrossProb = 0.0;
	ecCrossProb = 0.0;
	prTransProb = 0.0;
	asTransProb = 0.0;
	axTransProb = 0.0;
	ecTransProb = 0.0;
	return 0;
}
