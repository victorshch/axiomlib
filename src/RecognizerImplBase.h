/****************************************************************************
*			RecognizerImplBase
*
*	Description:	Промежуточный класс в иерархии потомков Recognizer. В
*					него вынесена логика подсчета ошибок и статистики,
*					потомкам остается реализовать только логику вычисления
*					расстояний до эталонных траекторий
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef RECOGNIZERIMPLBASE_H
#define RECOGNIZERIMPLBASE_H

#include <boost/shared_ptr.hpp>

#include "Recognizer.h"
#include "CompareStatistic.h"
#include "GoalStrategy.h"
#include "LabelingStrategy.h"
#include "TrajectorySampleDistance.h"

namespace AxiomLib {

class RecognizerImplBase : public Recognizer
{
private:
	bool m_comments;
	bool m_outputToFile;
	
	boost::shared_ptr<CompareStatistic> compareStatistic;
	boost::shared_ptr<GoalStrategy> goalStrategy;
	boost::shared_ptr<LabelingStrategy> labelingStrategy;
	
	static int coutAbnormalSegments(const vector<int> &labeling, int &abnormalSegmentsNum);
	static void countStat(std::vector<double>& stat, int resFirst, int resSecond, int abnormalSegmentsNum,
						  const std::vector<bool>& refStat, const std::vector<bool> &testStat);
	
	// Функция строит маркировки тестовых траекторий датасета, т. е. указывает для каждой из них участки нештатного поведения и их типы
	void performLabeling(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, 
								 std::vector<std::vector<int> >& result, 
								 // какие аксиомы срабатывали при разметке эталонных траекторий
								 std::vector<bool> &refAxiomUsage, 
								 // какие аксиомы срабатывали при разметке тестовых траекторий
								 std::vector<bool> &testAxiomUsage);
	
protected:
	bool comments();
	bool outputToFile();
	
public:
	RecognizerImplBase();
	
	// Конструктор копирования
	RecognizerImplBase(const RecognizerImplBase& other);
	
	// Установка нового значения переменной класса comments
    virtual void setComments(const bool newValue);
  
    // Установка нового значения переменной класса outputToFile
    virtual void setOutputToFile(const bool newValue);
  
    // Функция задания значений параметров класса, в том числе - классов разметки и подсчета ошибок
    virtual int setParamsFromEnv (const Environment& env);
    
    // Функция задания значений строго определенной части параметров класса, в том числе - классов разметки и подсчета ошибок
    virtual int setNominalParamsFromEnv (const Environment& env);
    
	//ЗАГЛУШКА
    // Функция установки нового набора данных в текущий класс распознователя
    virtual int setDataSet (const DataSet &newDataSet);
    
	//ЗАГЛУШКА
    // Функция установки нового значаения переменой класса params
    virtual int setParamsOfDataSet (std::vector<int> &newParams);
    
	//ЗАГЛУШКА
    virtual signed int run (AxiomSetPop& pop);
    
	//ЗАГЛУШКА
    virtual signed int run (AxiomSetPop& pop, std::vector <double> &precs);
  
	// Провести разметку рядов в системе аксиом,
    // прогнать распознаватель, посчитать количество
    // ошибок 1-го и 2-го рода
    virtual signed int run (AxiomSet& axiomSet, DataSet& dataSet, 
							std::vector<int>& params, int& resFirst, int& resSecond, 
							const int asIndex = -1);
  
	// Провести разметку рядов в системе аксиом,
    // прогнать распознаватель, посчитать количество
    // ошибок 1-го и 2-го рода, посчитать статистику
    virtual signed int run (AxiomSet& axiomSet, DataSet& dataSet, 
							std::vector<int>& params, int& resFirst, int& resSecond, 
							std::vector<double> &stat, const int asIndex = -1);
	
	virtual double countErrors(std::vector<TrajectorySampleDistance> distances, 
							const std::vector<std::vector<int> >& corrLabelings, int &resFirst, int &resSecond);
	// Функция обучения алгоритма распознавания
    virtual double learn (TemplateRecognizer &templateRecognizer);
	
	//---------------------------Чистые виртуальные функции-------------------------------------
protected:
	
	// Вычисление расстояний от участков разметок тестовых траекторий до разметок эталонных траекторий
	virtual void computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, 
								  // расстояния от участков разметок тестовых траекторий до разметок эталонных траекторий
								  std::vector<TrajectorySampleDistance>& result, 
								  // какие аксиомы срабатывали при разметке эталонных траекторий
								  std::vector<bool>& refAxiomUsage, 
								  // какие аксиомы срабатывали при разметке тестовых траекторий
								  std::vector<bool>& testAxiomUsage) = 0;
public:
	
	// Задание параметров класса-потомка
	virtual int initFromEnv (const Environment& env) = 0;
	
	// Функция возвращает копию распознавателя
	// Рекомендуется реализовывать вызовом конструктора копирования
    virtual Recognizer* copy (void) = 0;
};

}

#endif // RECOGNIZERIMPLBASE_H
