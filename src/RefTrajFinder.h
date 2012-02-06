/***************************************************************************
*					RefTrajFinder
*
*	Description:	Класс, описывающий алгоритм выделения эталонных траекторий 
*					в наборе данных.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Ref_Traj_Finder_HXX
#define _Ref_Traj_Finder_HXX

#include "AxiomLibException.h"
#include "Round.h"
#include "Environment.h"
#include "DataSet.h"

#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class RefTrajFinder {

 private:

	// Другие элементы класса: 
	// Набор временных рядов
	DataSet dataSet;

	// Вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
	// Под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> params;
	
 protected:

 public:

	// Конструктор класса с заданием всех переменных по умолчанию
	RefTrajFinder (void);  
  
	// Декструктор, уничтожающий все экземпляры объектов внутри данного
	~RefTrajFinder (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция установки нового набора данных в данный класс
	int setDataSet (const DataSet &newDataSet);

	// Функция установки нового значаения переменой класса, которая указывает какие ряды из набора данных использовать
	int setParamsOfDataSet (std::vector<int> &newParams);

	// Функция возвращает копию данного класса
	RefTrajFinder* copy (void);

	// Основная функция класса - запуск работы алгоритма выделения траекторий
	int run (void);

}; // end of class

}; // end of namespace

#endif /* _Ref_Traj_Finder_HXX */
