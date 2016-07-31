/****************************************************************************
*			RecognizerExtend
*
*	Description:	Класс для получения количества ошибок
*				распознавания для популяции систем аксиом
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_RecognizerExtend_HXX
#define __AXIOMS_LIB_RecognizerExtend_HXX

#include "Environment.h"
#include "AxiomSetPop.h"
#include "Recognizer.h"
#include "DataSet.h"
#include "RefMarkupSet.h"
#include "CompareStatistic.h"
#include "CompareStatisticSimple.h"
#include "CompareClassFactory.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"
#include <string>
#include <vector>

namespace AxiomLib {

class TemplateRecognizer;

class RecognizerExtend : public Recognizer {

 private:
 
  // Вспомогательная функция : Функция добавления расширенных эталонных разметок к вектору разметок
  inline signed int initRefs (AxiomSetPop& pop, const int asNum, const int param, std::vector <RefMarkupSet> &refer) const;

  inline signed int initRefs (AxiomSet& axiomSet, const int param, std::vector <RefMarkupSet> &refer, std::vector <bool> &refStat) const;

  // Вспомогательная процедура обновление вектора
  inline void updateVec (std::vector <bool> &vec, std::vector <bool> &newVec) const;

  // Вспомогательная процедура для подсчета статистики по результатам распознавания
  inline void countStat (std::vector <double> &stat, int resFirst, int resSecond, int resShouldBe, std::vector <bool> &refStat, std::vector <bool> &rowStat) const;
  
  // Вспомогательная функция : Функция распознования нештатных ситуаций по данной разметке ряда и эталонным разметкам, кроме того идет подсчет числа ошибок первого и второго рода
  inline signed int recognition (int &first, int &second,  std::vector <std::vector <int> > &markUps, std::vector <int> &corrMarkUp, std::vector < std::vector <RefMarkupSet> > &refs, int numOfTest) const;
  
  // Вспомогательная функция : Функция голосования
  inline signed int joinResults (std::vector <int> &result, std::vector < std::vector <int> > &prelimResult, const int numOfClasses) const;

  // Анализ одного теста - возвращает общее число траекторий нештатного поведения в данном тесте
  inline signed int analizeTest (std::vector <int> &corrMarkUp, int &res);

  // Вспомогательная функция обучения - рекурсивно улучшает значение precision
  double sublearn (TemplateRecognizer &templateRecognizer, std::vector <double> &curRes, std::vector <double> &curPrecision, std::vector < std::vector <signed short> > &comparisonTable);

  // Вспомогательная функция для алгоритма обучения - изменение векторов согласно направлению изменения параметра precision
  inline void manageSets (unsigned int direction, std::vector <double> &curRes, std::vector <double> &curPrecision) const;

  // Вспомогательная функция для алгоритма обучения - выбор лучшего направления для изменения по сравнительной таблице результатов
  inline unsigned int chooseDirection (std::vector < std::vector <signed short> > &compTable) const;

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
  int saveRefs (std::vector <RefMarkupSet> &ref, std::string paramName) const;

 protected:
 
  DataSet dataSet; // рабочий dataSet, пока единственный
  
  std::vector<int> params; // вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
  					  // под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы

  double precision; // переменная класса относящаяся к параметрам разметки временных рядов
  
  bool comments; // переменная класса для хранения информации о том, выводить ли отладочную информацию встандартный поток вывода

  bool outputToFile; // переменная класса для хранения информации о том, выводить полученный ряды в файл
  std::string whereToSave; // переменная класса для хранения пути, по которому сохранять файлы
  std::string saveDirName; // загатовка для имени директории, в которую сохранять файлы
  // Внутренние переменные класса для хранения текущих имен сохраняемых файлов и директории
  std::string curDir; 
  std::string curFile;
 
  CompareStatistic* compareStatistic; // Класс подсчета числа ошибок I и II рода на тестовой выборке
  
  GoalStrategy* goalStrategy; // Стратегия подсчета целевой функции по ошибкам I и II рода

 public:
 
  // Конструктор класса с заданием значений по умолчанию
  RecognizerExtend (void);

  // Деструктор класса с уничтожением всех созданных динамических объектов
  ~RecognizerExtend (void);
  
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
  
  // Функция установки нового набора данных в текущий класс распознователя
  int setDataSet (const DataSet &newDataSet);
  
  // Функция установки нового значения параметра precision
  int setPrecision (const double newPrecision);
  
  // Функция установки нового значаения переменой класса params
  int setParamsOfDataSet (std::vector<int> &newParams);

  // Функция установки нового класса подсчета статистики
  int setCompareStatistic (CompareStatistic *newCompareStatistic);

  // Функция возвращает копию данного распознавателя
  Recognizer* copy (void);

  // Функция обучения данного алгоритма распознавания
  double learn (TemplateRecognizer &templateRecognizer);
  
  // Провести разметку рядов в системе аксиом,
  // прогнать распознаватель, посчитать количество
  // ошибок 1 и второго рода
  signed int run (AxiomSetPop& pop);
  
  // Провести разметку рядов в системе аксиом,
  // прогнать распознаватель, посчитать количество
  // ошибок 1 и второго рода
  signed int run (AxiomSetPop& pop, std::vector <double> &precs);

  // Прогон распознавания по заданному набору данных и системе аксиом 
  signed int run (AxiomSet& axiomSet, DataSet& extDataSet, std::vector<int>& extParams, int& resFirst, int& resSecond, const int asIndex = -1);

  // Прогон распознавания по заданному набору данных и системе аксиом с подсчетом статистики по задействованным аксиомам
  signed int run (AxiomSet& axiomSet, DataSet& extDataSet, std::vector<int>& extParams, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex = -1);
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_RecognizerExtend_STRATEGY_HXX */
