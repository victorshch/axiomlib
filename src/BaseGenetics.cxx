/****************************************************************************
*
*			Функции класса BaseGenetics
*
****************************************************************************/

#include "BaseGenetics.h"

using namespace AxiomLib;

// Коструктор класса с заданием начальных параметров по умолчанию
BaseGenetics::BaseGenetics (void) {
	numOfIter = -1; 
	comments = true;
	exitCond = 0;
	axiomSetBaseDir = NULL;
	whereToSave = NULL;
	selectionStrategy = NULL;
	transmuteStrategy = NULL;
	crossoverStrategy = NULL;
	goalStrategy = NULL;
	recognizer = NULL;
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
BaseGenetics::~BaseGenetics (void) {
	if (recognizer != NULL) delete recognizer;
	if (selectionStrategy != NULL) delete selectionStrategy;
	if (transmuteStrategy != NULL) delete transmuteStrategy;
	if (crossoverStrategy != NULL) delete crossoverStrategy;
	if (goalStrategy != NULL) delete goalStrategy;
	if (axiomSetBaseDir != NULL) delete axiomSetBaseDir;
	if (whereToSave != NULL) delete whereToSave;
}


/****************************************************************************
*			BaseGenetics::initFromEnv
*
*	Description:	Инициализация параметров с данными из env
*	Parameters:	const Environment& env - структура с параметрами конфигурации
*	Returns:	0
*	Throws:		AxiomLibException - если конфигурация некорректна
*	Author:		gevor
*	History:
*
****************************************************************************/
int BaseGenetics::initFromEnv(const Environment& env)
{
  // проверяем число операций
  if (env.getIntParamValue(numOfIter, "numOfIter") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : number of iterations has not been set.");

  // инициализируем начальную популяцию
  axiomSetPop.initFromEnv(env);

  // инициализируем распознаватель
  string recognizerName;
  RecognizerFactory rf;
  if (env.getStringParamValue(recognizerName, "Recognizer") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : Recognizer is undefined.");

  recognizer = rf.create(recognizerName);
  recognizer->setParamsFromEnv(env);

  // инициализируем стратегию селекции
  string selectionStrategyName;
  SelectionStrategyFactory ssf;
  if (env.getStringParamValue(selectionStrategyName, "selectClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : selection strategy is undefined.");
  selectionStrategy = ssf.create(selectionStrategyName);
  selectionStrategy->setParamsFromEnv(env);
  selectionStrategy->setComments(comments);

  // инициализируем стратегию мутации
  string transmuteStrategyName;
  TransmuteStrategyFactory tsf;
  if (env.getStringParamValue(transmuteStrategyName, "transClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : mutation strategy is undefined.");
  transmuteStrategy = tsf.create(transmuteStrategyName);
  transmuteStrategy->setParamsFromEnv(env);

  // инициализируем стратегию скрещивания
  string crossoverStrategyName;
  CrossoverStrategyFactory csf;
  if (env.getStringParamValue(crossoverStrategyName, "crossClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : crossover strategy is undefined.");
  crossoverStrategy = csf.create(crossoverStrategyName);
  crossoverStrategy->setParamsFromEnv(env);

  // инициализируем стратегию вычисления целевой функции
  string goalStrategyName;
  GoalStrategyFactory gsf;
  if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : goal strategy is undefined.");
  goalStrategy = gsf.create(goalStrategyName);
  goalStrategy->setParamsFromEnv(env);

  // нинициализация условия досрочного выхожаи из цикла генетического алгоритма
  if (env.getDoubleParamValue (exitCond, "exitCond") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : goal strategy is undefined.");

  // сохраняем путь по которому сохранять лучшую систему аксиом по результатам работы цикла
  std::string temp;
  if (env.getStringParamValue(temp, "AxiomSetBaseDir") >= 0) {
    axiomSetBaseDir = new string;
    *axiomSetBaseDir = temp;
  }
  if (env.getStringParamValue(temp, "saveTo") >= 0) {
    whereToSave = new string;
    *whereToSave = temp;
  }

  return 0;
}


/****************************************************************************
*			BaseGenetics::shortInitFromEnv
*
*	Description:	Инициализация только определенной части параметров с данными из env
*	Parameters:	const Environment& env - структура с параметрами конфигурации
*	Returns:		0
*	Throws:		AxiomLibException - если конфигурация некорректна
*	Author:		gevor, dk
*	History:
*
****************************************************************************/
int BaseGenetics::shortInitFromEnv(const Environment& env)
{
  // инициализируем начальную популяцию
  axiomSetPop.initFromEnv (env);

  // инициализируем распознаватель
  string recognizerName;
  RecognizerFactory rf;
  if (env.getStringParamValue(recognizerName, "Recognizer") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : Recognizer is undefined.");
  recognizer = rf.create(recognizerName);
  recognizer->setNominalParamsFromEnv(env);
  
  // инициализируем стратегию селекции
  string selectionStrategyName;
  SelectionStrategyFactory ssf;
  if (env.getStringParamValue(selectionStrategyName, "selectClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : selection strategy is undefined.");
  selectionStrategy = ssf.create(selectionStrategyName);
  selectionStrategy->setParamsFromEnv (env);
  selectionStrategy->setComments(comments);

  // инициализируем стратегию мутации
  string transmuteStrategyName;
  TransmuteStrategyFactory tsf;
  if (env.getStringParamValue(transmuteStrategyName, "transClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : mutation strategy is undefined.");
  transmuteStrategy = tsf.create(transmuteStrategyName);
  transmuteStrategy->setParamsFromEnv (env);

  // инициализируем стратегию скрещивания
  string crossoverStrategyName;
  CrossoverStrategyFactory csf;
  if (env.getStringParamValue(crossoverStrategyName, "crossClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : crossover strategy is undefined.");
  crossoverStrategy = csf.create(crossoverStrategyName);
  crossoverStrategy->setParamsFromEnv (env);
  
  // инициализируем стратегию вычисления целевой функции
  string goalStrategyName;
  GoalStrategyFactory gsf;
  if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : goal strategy is undefined.");
  goalStrategy = gsf.create(goalStrategyName);
  goalStrategy->setParamsFromEnv (env);
  
  // инициализация условия досрочного выхожаи из цикла генетического алгоритма
  if (env.getDoubleParamValue (exitCond, "exitCond") < 0)
    throw AxiomLibException("BaseGenetics::initFromEnv : goal strategy is undefined.");
  
  return 0;
}


/****************************************************************************
*				BaseGenetics::setDataSet
*
*	Description:	Установка нового набора данных для данного класса подбора системы аксиом
*	Parameters:	newDataSet - новый набор данных
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int BaseGenetics::setDataSet (const DataSet &newDataSet) {
	return recognizer->setDataSet (newDataSet);
}
  

/****************************************************************************
*				BaseGenetics::setParamsOfDataSet
*
*	Description:	Установка вектора указания канонического порядка 
*				параметров набора данных по которым ведется анализ
*	Parameters:	params - устанавливаемый вектор
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int BaseGenetics::setParamsOfDataSet (std::vector<int> &params) {
	return recognizer->setParamsOfDataSet(params);
}


/****************************************************************************
*				BaseGenetics::setNumOfIter
*
*	Description:	Установка нового значения числа итераций при подборе оптимальной системы аксиом
*	Parameters:	newNumOfIter - новое значение числа итераций
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int BaseGenetics::setNumOfIter (const int newNumOfIter) {
	numOfIter = newNumOfIter;
	return 0;
}


/****************************************************************************
*			BaseGenetics::run
*
*	Description:	Запуск ГА
*	Parameters:	-
*	Returns:		0
*	Throws:		AxiomLibException - если произошла ошибка или ГА не
*				был инициализирован
*	Author:		gevor, dk
*	History:
*
****************************************************************************/
int BaseGenetics::run(void) {
	// Классический генетический алгоритм:
	// 0. проверяем, установлены ли параметры
	// В цикле:
	// 1. Прогоняем на существующих системах аксиом распознаватель
	// 2. Вычисляем значение целевой функции по каждой из систем аксиом
	// 3. Селекционируем, Проверяем критерий останова
	// 4. Мутируем
	// 5. Скрещиваем

	// 0. проверка параметров
	if (numOfIter < 0)
		throw AxiomLibException("BaseGenetics::run : number of iterations < 0; probably GA has not been initialized.");
		
	AxiomSetWithStats bestAxiomSet; // заводим переменную дял хранения лучшей системы аксиом за историю
	// Основной Цикл
	for (int gaIter=0; gaIter < numOfIter; gaIter++) {
		if (comments) { 
			cout << "	BaseGenetics: iteration " << gaIter+1 << " out of " << numOfIter << ", pop size = " << axiomSetPop.size() << "\n";
			std::cout.flush();
		}

		// 1. Прогоняем на существующей популяции алгоритм разметки и распознавания, получаем число ошибок первого и второго рода, заполняем статистику.
		recognizer->run(axiomSetPop);
		
		// 2. Прогоняем вычисление H(first, second)
		goalStrategy->compute(axiomSetPop);
		
		// 3. Проводим операцию селекции (на первом цикле, очевидно, бессмысленная)
		if (gaIter != 0) {
			selectionStrategy->select(axiomSetPop);
		}
		
		// Проверяем условие выхода
		if (axiomSetPop.exit(exitCond, bestAxiomSet)) break;
		
		// 4. Мутируем
		transmuteStrategy->transmute(axiomSetPop);
		
		// 5. Скрещиваем    
		crossoverStrategy->crossover(axiomSetPop);
		
		if (comments) cout << "\n";
	}
	// сохраняем лучшую систему аксиом в файл
	if (comments) {
		int  first, second;
		double goal;
		bestAxiomSet.unsafeGetStats (first, second, goal);
		std::cout << "\n	Best Results are:\n	    first and second and goal\n		" << first << "	"  << second << "	 " << goal << "\n";
		std::cout.flush();
	}
	if ( (axiomSetBaseDir != NULL) && (whereToSave != NULL) )
		bestAxiomSet.saveAxiomSetToFile (*axiomSetBaseDir, *whereToSave);

	return 0;
}


/****************************************************************************
*			BaseGenetics::run
*
*	Description:	Запуск генетического алгоритма с записью получившейся 
*				популяции и возвратом наилучшего значения целевой функции
*	Parameters:	asp - переменная для записи значений получившейся популяции
*	Returns:		double - наилучшее полученное значение целевой функции
*	Throws:		AxiomLibException - если произошла ошибка или ГА не
*				был инициализирован
*	Author:		dk
*	History:
*
****************************************************************************/
double BaseGenetics::run(AxiomSetPop &asp) {
	// Классический генетический алгоритм:
	// 0. проверяем, установлены ли параметры
	// В цикле:
	// 1. Прогоняем на существующих системах аксиом распознаватель
	// 2. Вычисляем значение целевой функции по каждой из систем аксиом
	// 3. Селекционируем, Проверяем критерий останова
	// 4. Мутируем
	// 5. Скрещиваем

	// 0. проверка параметров
	if (numOfIter < 0)
		throw AxiomLibException("BaseGenetics::run : number of iterations < 0; probably GA has not been initialized.");
		
	AxiomSetWithStats bestAxiomSet; // заводим переменную дял хранения лучшей системы аксиом за историю
	// Основной Цикл
	for (int gaIter=0; gaIter < numOfIter; gaIter++) {
		//if (comments) cout << "	BaseGenetics: iteration " << gaIter+1 << " out of " << numOfIter << ", pop size = " << axiomSetPop.size() << "\n";
		// 1. Прогоняем на существующей популяции алгоритм разметки и распознавания, получаем число ошибок первого и второго рода, заполняем статистику.
		recognizer->run(axiomSetPop);
		
		// 2. Прогоняем вычисление H(first, second)
		goalStrategy->compute(axiomSetPop);
		
		// 3. Проводим операцию селекции (на первом цикле, очевидно, бессмысленная)
		if (gaIter != 0) {
			selectionStrategy->select(axiomSetPop);
		}
		
		// Проверяем условие выхода
		if (axiomSetPop.exit(exitCond, bestAxiomSet)) break;
		
		// 4. Мутируем
		transmuteStrategy->transmute(axiomSetPop);
		
		// 5. Скрещиваем    
		crossoverStrategy->crossover(axiomSetPop);
		
		if (comments) cout << "\n";
	}
	// запись полученной популяции для возврата
	//asp = axiomSetPop;
	// возвращаем лучшее значение целевой функции
	int  first, second;
	double goal;
	bestAxiomSet.unsafeGetStats (first, second, goal);
	//std::cout << "\n	BG: going out. Press Enter please.\n";
	//std::cin.get();
	return goal;
}


/****************************************************************************
*			BaseGenetics::writeResultsToDisplay
*
*	Description:	Вывод результатов ГА на дисплей
*	Parameters:	0
*	Returns:	0
*	Throws:		AxiomLibException - если произошла ошибка или ГА не
*				был запущен
*	Author:		gevor
*	History:
*
****************************************************************************/
int BaseGenetics::writeResultsToDisplay(void)
{
	return 0;
}
