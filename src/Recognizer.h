/****************************************************************************
*			Recognizer
*
*	Description:	Абстрактный класс для получения количества ошибок
*				распознавания для популяции систем аксиом
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Recognizer_HXX
#define __AXIOMS_LIB_Recognizer_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include "DataSet.h"
#include "RefMarkupSet.h"
#include "CompareStatistic.h"
#include "CompareStatisticSimple.h"
#include "CompareClassFactory.h"
#include "TrajectorySampleDistance.h"
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>

//#include "undefs.h"

//#include <limits.h>

//#include "Defines.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

#include "omp.h"

namespace AxiomLib {

class TemplateRecognizer;

class Recognizer {

 private:
 
 protected:
  
 public:
 
  // Стандартный конструктор - пустой
  // Recognizer (void) {}

  // Стандартный деструктор - пустой
  virtual ~Recognizer (void) {}
  
  // Установка нового значения переменной класса comments
  virtual void setComments(const bool newValue) = 0;

  // Установка нового значения переменной класса outputToFile
  virtual void setOutputToFile(const bool newValue) = 0;

  // Функция задания значений параметров класса, в том числе - классов разметки и подсчета ошибок
  virtual int setParamsFromEnv (const Environment& env) = 0;
  
  // Функция задания значений строго определенной части параметров класса, в том числе - классов разметки и подсчета ошибок
  virtual int setNominalParamsFromEnv (const Environment& env) = 0;
  
  // Функция установки нового набора данных в текущий класс распознователя
  virtual int setDataSet (const DataSet &newDataSet) = 0;
  
  // Функция установки нового значаения переменой класса params
  virtual int setParamsOfDataSet (std::vector<int> &newParams) = 0;

  // Функция возвращает копию распознавателя
  virtual Recognizer* copy (void) = 0;

  // Функция обучения алгоритма распознавания
  virtual double learn (TemplateRecognizer &templateRecognizer) = 0;
	
	virtual double learn(TemplateRecognizer &templateRecognizer, int& resFirst, int& resSecond, std::vector<double>& stat, int asIndex = -1);
  
  // Провести разметку рядов в системе аксиом,
  // прогнать распознаватель, посчитать количество
  // ошибок 1 и второго рода
  virtual signed int run(AxiomSetPop& pop) = 0;
  
  virtual signed int run (AxiomSetPop& pop, std::vector <double> &precs) = 0;

  virtual signed int run (AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, int& resFirst, int& resSecond, const int asIndex = -1) = 0;

  virtual signed int run (AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex = -1) = 0;
	
  virtual double countErrors(std::vector<TrajectorySampleDistance> distances, const std::vector<std::vector<int> >& corrLabelings, int &resFirst, int &resSecond);
	
};

// end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_RecognizerExtend_STRATEGY_HXX */
