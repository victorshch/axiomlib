/****************************************************************************
*				Heuristics.h
*
*	Description: Базовый класс для эвристик
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef FUZZYMULTIDATAEXT_HEURISTICS_H
#define FUZZYMULTIDATAEXT_HEURISTICS_H

#include <string>

#include "../Environment.h"

#include "ForwardDeclarations.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class Heuristics
{
public:
    Heuristics();
	
	// Инициализация из окружения
	void initFromEnv(const Environment& env);
	
	// Порядок -- по нему сортируются эвристики, и применяются в этом порядке
	int order() const;
	
	// Имя эвристики
	virtual std::string name() const = 0;
	
	// Стадия, на которой применяется эвристика
	virtual int stage() const = 0;
	
	// Виртуальный конструктор копирования
	virtual Heuristics* clone() const = 0;
	
	// Запуск эвристики на алгоритме
	virtual void run(FuzzyMultiDataExtAlgorithm& algorithm) const;
	
	static std::string getParamName(const std::string& heuristicsName, const std::string& paramName);
private:
	int m_order;

	// Имена классов, на которых запускать эвристику
	std::vector<std::string> m_classNames;
protected:
	// Возможность установить свой order по умолчанию для конкретной эвристики
	void setOrder(int value);
	
	virtual void initFromEnvImpl(const Environment& env) = 0;
	
	virtual void run(FuzzyMultiDataExtAlgorithm& algorithm, int classNo) const;
	
	std::string getParamName(const std::string& name) const;
};

};

};

#endif // FUZZYMULTIDATAEXT_HEURISTICS_H
