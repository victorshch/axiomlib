/***************************************************************************
*					TemplateManager
*
*	Description:	Класс описывающий алгоритм использования шаблонов для 
*					поиска распознавателя.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Template_Manager_HXX
#define _Template_Manager_HXX

#include "AxiomLibException.h"
#include "AxiomBase.h"
#include "Round.h"
#include "Environment.h"
#include "ExtGenetics.h"
#include "TemplateRecognizer.h"
#include "PreprocessFactory.h"
#include "TemplateGenetics.h"
#include "TemplateGeneticsFactory.h"
#include "DataSet.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace AxiomLib {

class TemplateManager {

 private:
	
	// Минимальное число шаблонов, которое будет формироваться во время обучения
	int numOfTemplates;

	// Вектор шаблонов распознавателей
	std::vector <TemplateRecognizer> templateRecognizers;

	// Исходный набор данных, на котором производится обучение
	DataSet dataSet;

	// Вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
	// Под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> params;

	std::vector < TemplateGenetics* > templateGenetics;

	// Параметря для сохранения лучших решений
	std::string axiomSetBaseDir;
	std::string axiomName;
	std::string whereToSave;
	bool outputToFile;

 protected:

 public:

	// Пустой конструктор: 
	TemplateManager (void);  
  
	// Декструктор, уничтожающий все экземпляры объектов внутри данного
	~TemplateManager (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция создания шаблонов распознавателей
	int generateTemplates (const Environment& env);
	
	// Основная функция класса - запуск подбора распознавателя по шаблону
	int run (void);

	// Функция прогонки распознавателей на заданном наборе данных с выводом результатов в стандартный поток
	int runExecuteOnly (void);

}; // end of class

}; // end of namespace

#endif /* _Template_Manager_HXX */
