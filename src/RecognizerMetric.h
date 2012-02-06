/****************************************************************************
*			RecognizerMetric
*
*	Description:	Класс для получения количества ошибок
*				распознавания для популяции систем аксиом 
*				при использовании метрики в качестве меры расстояния
*				между сравниваеми строками разметки
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_RecognizerMetric_HXX
#define __AXIOMS_LIB_RecognizerMetric_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include "Recognizer.h"
#include "DataSet.h"
#include "RefMarkupSetSimple.h"
#include "CompareStatistic.h"
#include "CompareStatisticSimple.h"
#include "CompareClassFactory.h"
#include <string>
#include <vector>

namespace AxiomLib {

class RecognizerMetric : public Recognizer{

 private:
 
  // Вспомогательная функция : Функция добавления разметок к вектору всех эталонных разметок
  inline signed int initRefs (AxiomSetPop& pop, const int asNum, const int param, std::vector <RefMarkupSetSimple> &refer);
  
  // Вспомогательная функция : Функция распознования нештатных ситуаций по данной разметке ряда и эталонным разметкам, кроме того идет подсчет числа ошибок первого и второго рода
  inline signed int recognition (int &first, int &second,  std::vector <std::vector <int> > &markUps, std::vector <int> &corrMarkUp, std::vector < std::vector <RefMarkupSetSimple> > &refs, int numOfTest) const;
  
  // Вспомогательная функция : Функция голосования. Реализаций может быть множество. 
  // В БУДУЩЕМ : СОЗДАТЬ ДИНАМИЧЕСКИ-ПОДКЛЮЧАЕМЫЙ ОБЪЕКТ ДЛЯ ВЫПОЛНЕНИЯ ЭТОЙ ФУНКЦИИ
  inline signed int joinResults (std::vector <int> &result, std::vector < std::vector <int> > &prelimResult, const int numOfClasses) const;

  // Функция очищает содержимое папки
  int clearPath (std::string asName, int asIndex = -1);
  
  // Ищет максимальное значение индекса для которого директория уже не создана
  int searchFreeIndex (const std::string &curPath) const;

  // Функция создает имя файла для текущей записи и сохраняет его во внутренней переменной класса
  int genFileName (int testNum);

  // Функция сохранения ряда и его разметки в файл
  int saveRows (std::vector <double> &row, std::vector<int> &markUp, std::string paramName) const;

  // Функция сохранения ряда и его разметки в файл
  int updateRows (std::vector <int> &corrMarkUp, std::vector<int> &result) const;

  // Функция сохранения ряда и его разметки в файл
  int saveRefs (std::vector <RefMarkupSetSimple> &ref, std::string paramName) const;

 protected:
 
  RefMarkupSetSimple rms; // элемент класса, в котором хранится базовый класс для создания разметок, на основе которого создаются все разметки эталонных траекторий (суть в его параметрах, которые задаются один раз, а не при каждом создании разметки эталнной тракетории)
 
  DataSet dataSet; // рабочий dataSet, пока единственный
  
  std::vector<int> params; // вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
  					  // под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы

  double precision; // переменная для хранения максимально допустимого расстояния между разметками при котором считать их равными
  
  bool comments; // переменная класса для хранения информации о том, выводить ли отладочную информацию встандартный поток вывода

  bool outputToFile; // переменная класса для хранения информации о том, выводить полученный ряды в файл
  std::string whereToSave; // переменная класса для хранения пути, по которому сохранять файлы
  std::string saveDirName; // загатовка для имени директории, в которую сохранять файлы
  // Внутренние переменные класса для хранения текущих имен сохраняемых файлов и директории
  std::string curDir;
  std::string curFile;
 
  CompareStatistic* compareStatistic;
  
 public:
 
  // Стандартный конструктор
  RecognizerMetric (void);

  // Стандартный деструктор - удаление всех динамических объектов класса
  ~RecognizerMetric ();
  
  // Установка нового значения переменной класса comments
  void setComments(const bool newValue) { comments = newValue;}

  // Установка нового значения переменной класса outputToFile
  void setOutputToFile(const bool newValue) { outputToFile = newValue;}
  
  // Установка нового значения переменной класса outputToFile, saveDirName и whereToSave
  void setOutputToFile(const bool newValue, const std::string newWhereToSave, const std::string newSaveDirName) { outputToFile = newValue; whereToSave = newWhereToSave; saveDirName = newSaveDirName; }

  // Функция задания значений параметров класса, в том числе - классов разметки и подсчета ошибок
  int setParamsFromEnv (const Environment& env);
  
  // Функция задания значений строго определенной части параметров класса, в том числе - классов разметки и подсчета ошибок
  int setNominalParamsFromEnv (const Environment& env);

  // Функция установки нового значения параметра precision
  int setPrecision (const double newPrecision);
  
  // Функция установки нового набора данных в текущий класс распознователя
  int setDataSet (const DataSet &newDataSet);
  
  // Функция установки нового значаения переменой класса params
  int setParamsOfDataSet (std::vector<int> &newParams);

  // Функция установки нового класса подсчета статистики
  int setCompareStatistic (CompareStatistic *newCompareStatistic);

  // Функция установки нового класса разметок
  int setRMS (RefMarkupSetSimple &newRMS);

  // Функция возвращает копию данного распознавателя
  Recognizer* copy (void);

  // Функция обучения данного алгоритма распознавания
  double learn (TemplateRecognizer &templateRecognizer);
  
  // Провести разметку рядов в системе аксиом, прогнать распознаватель, посчитать количество ошибок первого и второго рода
  signed int run (AxiomSetPop& pop);
  
  // Провести разметку рядов в системе аксиом,
  // прогнать распознаватель, посчитать количество
  // ошибок 1 и второго рода
  signed int run (AxiomSetPop& pop, std::vector <double> &precs);

  // ! ! ! ЗАГЛУШКА ! ! !
  // Прогон распознавания по заданному набору данных и системе аксиом 
  signed int run (AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, int& resFirst, int& resSecond, const int asIndex = -1) {return 0;}

  // ! ! ! ЗАГЛУШКА ! ! !
  // Прогон распознавания по заданному набору данных и системе аксиом 
  signed int run (AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex = -1) {return 0;}
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_RecognizerMetric_STRATEGY_HXX */
