/***************************************************************************
*					TemplateRecognizer
*
*	Description:	Класс, описывающий шаблон распознавателя, который включает 
*					алгоритм предобработки, систему аксиом, алгоритм поиска
*					подстрок.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Template_Recognizer_HXX
#define _Template_Recognizer_HXX

#include "AxiomLibException.h"
#include "Round.h"
#include "Environment.h"
#include "DataSet.h"
#include "Preprocess.h"
#include "AxiomSet.h"
#include "Recognizer.h"
#include "RefTrajFinder.h"

#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class TemplateRecognizer {

 private:

 protected:

 public:

	// Элементы шаблона поиска:
	// Класс предобработки 
	Preprocess* preprocess; 

	// Система аксиом
	AxiomSet* axiomSet;

	// Класс распознавателя
	Recognizer* recognizer;

	// Алгоритм выделения эталонных траекторий
	RefTrajFinder* refTrajFinder;

	// Другие элементы класса: 
	// Набор временных рядов
	DataSet dataSet;

	// Набор временных рядов - после обработки алгоритмом предобработки
	DataSet prepDataSet;

	// Вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
	// Под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> params;

	/**********************************************************************************/
	// Функции класса:
	// Конструктор класса с заданием всех переменных по умолчанию
	TemplateRecognizer (void);  
  
	// Декструктор, уничтожающий все экземпляры объектов внутри данного
	~TemplateRecognizer (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция установки нового набора данных в текущий класс распознователя
	int setDataSet (const DataSet &newDataSet);

	// Функция установки нового значаения переменой класса, которая указывает какие ряды из набора данных использовать
	int setParamsOfDataSet (std::vector<int> &newParams);

	// Основная функция класса - запуск работы распознавателя
	int run (int &resFirst, int &resSecond, const int asIndex = -1);

	// Запуск работы распознавателя с выводом всех разметок в файлы
	int runWithOutput (int &resFirst, int &resSecond, const int asIndex = -1);

	// Запуск алгоритма предобработки исходного набора данных
	int runPreprocessor (void);

	// Вызывает только алгоритм распознавания по уже предобработанному набору данных
	int runRecognizer (int &resFirst, int &resSecond);

	// Вызывает только алгоритм распознавания с подсчетом статистики по уже предобработанному набору данных
	int runRecognizer (int &resFirst, int &resSecond, std::vector<double> &stat);

	// Функция обучения шаблона
	int learn (void);

	// Оператор присваивания шаблонов
    TemplateRecognizer & operator= (const TemplateRecognizer &second);

}; // end of class

}; // end of namespace

#endif /* _Template_Recognizer_HXX */
