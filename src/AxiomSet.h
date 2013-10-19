/****************************************************************************
*			Axiom Set 
*
*	Description:	Класс описания системы аксиом
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_SET_HXX
#define __AXIOMS_LIB_AXIOM_SET_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "AxiomBase.h"
#include "AxiomSetBase.h"
#include "AxiomSetStructure.h"
#include "AxiomStructure.h"
#include "all_axioms.h"
#include "Axiom.h"
#include "Environment.h"
#include "EnvDataSet.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "./MultiMarking/multimarks.h"
namespace AxiomLib {

class AxiomSet {

 private:

 protected:

	// Установка другом класс скрещивания для элементов популяции
	friend class CrossoverTpl;

	// Установка другом класс мутации для элементов популяции
	friend class TransmuteTpl;
 
    // Аксиомы входящие в систему в порядке уменьшения приоритета 
    std::vector<Axiom *> axioms;
    
    // Имя класса системы аксиом (соответствует пересекающимся классам слабой структурной идентичности)
    std::string nameOfAxiomSet;
    
    // Имя используемого банка аксиом
    std::string nameOfAxiomBank;
    
    // Контрольный бит для операции скрещивания при подборе исключительно параметров - если он выставлен - то проведение 
    // операции скрещивания параметров систем аксиом проходит в ускоренном режиме без дополнительных проверок структуры
    int crossoverControlBit;
    
 public:
 
    // Пустой конструктор с заданием имени по default'у
    AxiomSet(void);  
  
    // Пустой конструктор с заданием имени в параметре
    AxiomSet(const std::string sName);
 
    // Деструктор удаляющий все созданные в рамках данной системы аксиом динамические объекты
    ~AxiomSet(void);

    // Функция удаления всех аксиом из системы аксиом
    void clear (void);
    
    // Функция возвращающая имя системы аксиом
    std::string name (void) const;

	// Функция возвращающая число аксиом в системе
	int size (void) const;
    
    // Функция установки значения контрольного бита скрещивания
    int setCrossoverControlBit (const int newCrossoverControlBit);
    
    // Функция возвращает текущее значение контрольного бита скрещивания
    int getCrossoverControlBit (void) const;

    // Функиця разметки ряда
    int enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end);

    // Функиця разметки ряда с подсчетом статистики
    int enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat);

    // for MultiMarking
    int enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end);

    // for MultiMarking
    int enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat);

    // Функция задания аксиом системы случайным обоазом
    signed int initAxiomSetByRand (const int numOfAxioms, const int maxECinAxiom);
    
    // Функция задания аксиом системы строго определенным образом (задание имен элементарных условий входящих в каждую аксиому в параметрах)
    signed int initAxiomSetByNames (const std::vector < std::vector < std::string > > &axiomEC, const std::vector <std::string>& axiomNames);

    // Функуия задания аксиом системы строго определенным образом (задание номеров элементарных условий входящих в каждую аксиому в параметрах)
    signed int initAxiomSetByNums (const std::vector < std::vector < int > > &axiomEC, const std::vector <std::string>& axiomNames);
    
    // Функция задания аксиом системы строго определенным образом (по конфигурации из файла)
    signed int initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName, AxiomBase &axiomBase);
    
    // Функция создания новой системы аксиом путем копирования и упорядычевания аксиом из входного вектора
    signed int initAxiomSetByCopy (const std::vector <Axiom*> &axiomVec, double levelOfSub);
    
    // Функция сохранения системы аксиом в файл
    signed int saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, int first = -1, int second = -1);
    
    // Функция сохранения системы аксиом в файл
    signed int saveAxiomSetToFile (Environment &env, EnvDataSet &envDataSet, int first = -1, int second = -1);

	// Функция сохраняет структуру данной системы аксиом в AxiomSetStructures
	signed int createAxiomSetStructure (AxiomSetStructure &ass) const;
    
    // Функция задания параметров системы аксиом
    signed int setAxiomParameters (const std::map < std::string , std::map <std::string, std::map <std::string, std::string> > >& axiomParams);
    
    // Функция возвращает значения установленных параметров системы аксиом
    signed int getAxiomParameters (std::map < std::string , std::map <std::string, std::map <std::string, std::string> > > &axiomParams);
    
    // Функция устанавливает значение параметра по номеру аксиомы в векторе аксиом, по номеру элементарного условия в наборе аксиом, по имени параметра
    signed int setParamValue (double param, int axiomNum, int ecNum, std::string &paramName);
    
    // Функция возвращает значение параметра по номеру аксиомы в векторе аксиом, по номеру элементарного условия в наборе аксиом, по имени параметра
    signed int getParamValue (double &param, int axiomNum, int ecNum, std::string &paramName);
    
    // Функция возвращает число элементов в векторе axioms данного набора аксиом
    signed int getNumOfAxioms (void) const;
    
    // Функция возвращает аксиому с заданным номером в векторе аксиом данной системы
    Axiom& getAxiom (int i) const;
    
    // Функция изменяет аксиому с заданным номером  векторе аксиом
    signed int setAxiom (Axiom& axiom, const int i);
    
    // Функция циклического сдвига систем аксиом
    signed int slide (const int i, const int j);
    
    // Оператор присваивания системы аксиом
    AxiomSet & operator= (const AxiomSet &second);
	
	AxiomSet(const AxiomSet &second);
    
    // Проверка сильной (заменить на слабую!!!) структурной идентичности двух наборов аксиом
    signed int checkWeakEquivalence(const AxiomSet& second) const;

	// Transmute selected axiom
	signed int transmuteAxiom (const int axiomNum, double ectl, double atl);

	void initAxiomWeights();

	std::vector<double> getAxiomWeights() const;

	// Normalizes weights
	void setAxiomWeights(const std::vector<double>& w);
    
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_AXIOM_SET_HXX */
