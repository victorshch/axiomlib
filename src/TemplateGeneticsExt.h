/****************************************************************************
*					TemplateGeneticsExt
*
*	Description:	Класс, реализующий алгоритм обучения шаблонов распознавателей.
*					Основные моменты: 
*					- Начальная популяция систем аксиом определяется конфигурационным файлом
*					- Подбор различных алгоритмов шаблона ведется максимально независимо
*					- Критерием останова является либо общее число итераций, либо число итераций без улучшений
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef _AXIOMS_LIB_TEMPLATE_GENETICS_EXT_HXX
#define _AXIOMS_LIB_TEMPLATE_GENETICS_EXT_HXX

#include "Environment.h"
#include "AxiomLibException.h"
#include "AxiomSetPlus.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"
#include "CrossoverTpl.h"
#include "TransmuteTpl.h"
#include "DataSet.h"
#include "TemplateGenetics.h"
#include "Round.h"

#include <set>
#include <string>
#include <vector>

namespace AxiomLib {

class TemplateGeneticsExt : public TemplateGenetics {

  private:
	
	// Популяция
	std::vector <AxiomSetPlus> *population;
	// Initial population size (number from environment)
	int popSize;
	
	// Базовые числовые параметры алгоритма обучения шаблонов
	int numOfIter; // максимальное количество итераций алгоритма
	int numOfBadIter; // максимальное число итераций без улучшения целевой функции
	double exitCond; // Переменная для хранения значения целевой функции, по которому возможно досрочное завершение алгоритма обучения шаблонов

	// Базовые числовые параметры генетики подбора систмы аксиом
	int numOfGenIter; // максимальное количество итераций алгоритма
	int numOfBadGenIter; // максимальное число итераций без улучшения целевой функции
	double exitCondGen; // Переменная для хранения значения целевой функции, по которому возможно досрочное завершение генетического алгоритма
	// Параметры селекции
	double percentBest;
	// Параметры алгоритма скрещивания
	double asCrossProbMin;
	double asCrossProbMax;
	double axCrossProbMin;
	double axCrossProbMax;
	double ecCrossProbMin;
	double ecCrossProbMax;
	// Параметры алгоритма мутации
	double prTransProbMin;
	double prTransProbMax;
	double asTransProbMin;
	double asTransProbMax;
	double axTransProbMin;
	double axTransProbMax;
	double ecTransProbMin;
	double ecTransProbMax;

	// Стратегия подсчета целевой функции по ошибкам I и II рода, инициализирующиеся из конфигурационного файла
	GoalStrategy* goalStrategy;

	// Класс для проведения операции скрещивания
	CrossoverTpl crossoverTpl;

	// Класс для проведения операции мутации популяции
	TransmuteTpl transmuteTpl;

  protected:

	// Сортировка вектора целых чисел с помощью меторда пузырька
	  inline void sortVector (std::vector<unsigned int> &vec) const;

  public:

	// Конструктор с заданием параметров по умолчанию
	TemplateGeneticsExt (void);

	// Деструктор - удаляются все созданные в рамках данного класса динамические объекты
	~TemplateGeneticsExt(void);

	// Установка параметров алгоритма обучения
	int initFromEnv (const Environment& env);

	// Операция селекции популяции
	int select (const std::vector<unsigned int> &mask);

	// Операция мутация особей популяции
	int transmute ();

	// Операция скрещивания элементов популяции
	int crossover ();

	// Проверка систем аксиом на тестовой выборке
	inline int recognize (TemplateRecognizer& templateRecognizer);

	// Операция вычисления целевой функции по числу ошибок первого и второго рода для элементов популяции
	inline int compute ();

	// Функция заполнения статистики по элементам популяции на основе данных полученных при распознавании
	inline int fillStatistic (std::vector<unsigned int> &mask);

	// Сортировка вектора масок для элементов популяции по значениям целефой функции
	inline int sort (std::vector<unsigned int> &mask) const;

	// Запуск алгоритма обучения шаблона
	double run(TemplateRecognizer& templateRecognizer);
	
}; // end of class

}; // end of namespace

#endif /* _AXIOMS_LIB_TEMPLATE_GENETICS_EXT_HXX */
