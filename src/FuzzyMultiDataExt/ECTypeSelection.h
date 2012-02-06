/****************************************************************************
*				ECTypeSelection.h
*
*	Description: Механизм задания используемых типов ЭУ, оформлен как эвристика
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef ECTYPESELECTION_H
#define ECTYPESELECTION_H

#include <set>
#include <string>

#include "Heuristics.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class ECTypeSelection : public Heuristics
{
public:
    ECTypeSelection();
	
	// Имя эвристики
	virtual std::string name() const;
	
	// Стадия, на которой применяется эвристика
	virtual int stage() const;
	
	// Виртуальный конструктор копирования
	virtual ECTypeSelection* clone() const;
	
	// Запуск эвристики на алгоритме
	virtual void run(FuzzyMultiDataExtAlgorithm& algorithm) const;
	
protected:	

	void initFromEnvImpl(const Environment &env);

private:
	enum SelectionPolicy { Exclusive = 0, /* Используем все кроме указанных в конфиге */ 
		                   Inclusive = 1 /* Используем только указанные в конфиге */ };
	
	struct compare {
		bool operator()(const std::pair<bool, std::string>& p1, const std::pair<bool, std::string>& p2) const;
	};
	
	SelectionPolicy m_selectionPolicy;
	std::set<std::pair<bool, std::string>, compare> m_ecTypes;
	
	void selectExclusive(FuzzyMultiDataExtAlgorithm& algorithm) const;
	void selectInclusive(FuzzyMultiDataExtAlgorithm& algorithm) const;
};

}

}

#endif // ECTYPESELECTION_H
