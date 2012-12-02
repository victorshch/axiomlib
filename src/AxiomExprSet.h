/****************************************************************************
*					AxiomExprSet 
*
*	Description:	Класс описания системы аксиом на основе выражений он 
*					элементарных условий.
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_EXPR_SET_HXX
#define __AXIOMS_LIB_AXIOM_EXPR_SET_HXX

#include "./elem_conditions/ElemCondition.h"
#include "all_axioms.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "AxiomBase.h"
#include "AxiomSetBase.h"
#include "AxiomExpr.h"
#include "AxiomExprSetStructure.h"
#include "AxiomExprStructure.h"
#include "Environment.h"
#include "EnvDataSet.h"
#include "./MultiMarking/multimarks.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace AxiomLib {

class AxiomExprSet {

 private:

	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive &archive, const unsigned int /*version*/) {
		archive & BOOST_SERIALIZATION_NVP(nameOfAxiomSet);
		archive & BOOST_SERIALIZATION_NVP(nameOfAxiomBank);
		archive & BOOST_SERIALIZATION_NVP(axioms);
	}
	
	// Установка другом класс скрещивания для элементов популяции
	friend class CrossoverTpl;

	// Установка другом класс мутации для элементов популяции
	friend class TransmuteTpl;

 protected:
    
    // Имя класса системы аксиом (соответствует пересекающимся классам слабой структурной идентичности)
    std::string nameOfAxiomSet;
    
    // Имя используемого банка аксиом
    std::string nameOfAxiomBank;
    
 public:

	 // Аксиомы входящие в систему в порядке уменьшения приоритета 
    std::vector < AxiomExpr* > axioms;

	//		Функции класса:
 
    // Пустой конструктор с заданием имени по default'у
    AxiomExprSet(void);  
  
    // Пустой конструктор с заданием имени в параметре
    AxiomExprSet(const std::string sName);
	
	// Конструктор копирования
	AxiomExprSet(const AxiomExprSet &other);
 
    // Деструктор удаляющий все созданные в рамках данной системы аксиом динамические объекты
    ~AxiomExprSet(void);
	
    // Функция удаления всех аксиом из системы аксиом
    void clear (void);
    
    // Функция возвращающая имя системы аксиом
    std::string name (void) const;

	// Функция устанавливает имя системы аксиом
	void setName(const std::string& value) { nameOfAxiomSet = value; }

	// Функция возвращающая число аксиом в системе
	int size (void) const;

	// Добавляет аксиому к существующему ветору аксиом в данной системе
	void addAxiom (const AxiomExpr &newAxiom);

	// Добавляет аксиомы к существующему ветору аксиом в данной системе
	void addAxioms (std::vector <AxiomExpr*> &newAxiomExpr);

	// Устанавливает новый вектор аксиом
	void setAxioms (std::vector <AxiomExpr*> &newAxiomExpr);
    
	// Функиця разметки ряда
	int enter (std::vector<int>& marked, const std::vector < std::vector<double> > &row, const unsigned long begin, const unsigned long end) const;

    // Функиця разметки ряда
    int enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end);

	// Функиця разметки ряда с подсчетом статистики
    int enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat);

    // Функция разметки ряда в случае разметки подмножествами аксиом
    int enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat);

    // Оператор присваивания системы аксиом
    AxiomExprSet& operator= (const AxiomExprSet &second);

	// Функция сохраняет структуру данной системы аксиом в AxiomExprSetStructures
	int createAxiomExprSetStructure (AxiomExprSetStructure &aess) const;
	
	// Функция задания системы аксиом строго определенным образом (по конфигурации из файла)
    int initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName);
	
	// Функция задания системы аксиом строго определенным образом (по конфигурации из файла)
    int initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName, const std::vector<std::string> &paramNames);

	// Функция задания системы аксиом по данным из структуры с описанием системы аксиом
	int initAxiomSetFromStructure (AxiomExprSetStructure &aess);

	// Функция задания системы аксиом случайным образом
	int initAxiomSetByRand (const int numOfAxioms, const int maxECinAxiom);

	// Функция задания системы аксиом случайным образом с указанием размерности набора данных для каждого условия из заданного диапазона
	int initAxiomSetByRand (const int numOfAxioms, const int maxECinAxiom, const int dimensionMin, const int dimensionMax);

	// Функция сохранения системы аксиом в файл
	int saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, int first = -1, int second = -1) const;

	// Функция сохранения системы аксиом в файл (с указанием размерости исходных данных для каждого элементарного условия)
	int saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, std::vector <std::string> &dataSetParams, int first = -1, int second = -1) const;
        
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_AXIOM_EXPR_SET_HXX */
