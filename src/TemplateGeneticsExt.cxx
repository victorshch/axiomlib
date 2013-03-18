/****************************************************************************
*
*				Функции класса TemplateGeneticsExt
*
****************************************************************************/

#include <boost/lexical_cast.hpp>

#include "Logger.h"

#include "TemplateGeneticsExt.h"

using namespace AxiomLib;

#define debug true
#define bestGoals 1.5 // Особи, со значением целевой функции не более чем во столько раз привосходящим минимальное значение целевой функции, считаются лучшими 
#define epsilon 0.01 // Это параметр для работы с дробными числами

// Коструктор класса с заданием начальных параметров по умолчанию
TemplateGeneticsExt::TemplateGeneticsExt (void) {
	numOfIter = -1; 
	numOfBadIter = -1;
	exitCond = 0;
	numOfGenIter = -1; 
	numOfBadGenIter = -1;
	exitCondGen = 0;
	popSize = -1;
	population = new std::vector <AxiomSetPlus>;
	population->clear();
	goalStrategy = NULL;
	percentBest = 0.75;
	// Параметры алгоритма скрещивания
	asCrossProbMin = 0.0;
	asCrossProbMax = 0.0;
	axCrossProbMin = 0.0;
	axCrossProbMax = 0.0;
	ecCrossProbMin = 0.0;
	ecCrossProbMax = 0.0;
	// Параметры алгоритма мутации
	prTransProbMin = 0.0;
	prTransProbMax = 0.0;
	asTransProbMin = 0.0;
	asTransProbMax = 0.0;
	axTransProbMin = 0.0;
	axTransProbMax = 0.0;
	ecTransProbMin = 0.0;
	ecTransProbMax = 0.0;
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
TemplateGeneticsExt::~TemplateGeneticsExt (void) {
	if (goalStrategy != NULL) delete goalStrategy;
	for (unsigned int i = 0; i < population->size(); i++)
		(*population)[i].clear();
	population->clear();
	delete population;
}


/****************************************************************************
*			TemplateGeneticsExt::initFromEnv
*
*	Description:	Инициализация параметров с данными из env
*	Parameters:	const Environment& env - структура с параметрами конфигурации
*	Returns:	0
*	Throws:		AxiomLibException - если конфигурация некорректна
*	Author:		gevor
*	History:
*
****************************************************************************/
int TemplateGeneticsExt::initFromEnv(const Environment& env) {
	// 1. Устанавливаем значение максимального числа операций генетики и алгоритма обучения шаблонов
	if (env.getIntParamValue(numOfIter, "numOfIter") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : number of iterations has not been set.");
	if (env.getIntParamValue(numOfGenIter, "numOfGenIter") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : number of genetic iterations has not been set.");

	// 2. Устанавливаем число операций без улучшения для алгоритма обучения шаблонов и ГА
	if (env.getIntParamValue(numOfBadIter, "numOfBadIter") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : number of iterations without improvement has not been set.");
	if (env.getIntParamValue(numOfBadGenIter, "numOfBadGenIter") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : number of genetic iterations without improvement has not been set.");

	// 3. Инициализируем начальную популяцию
	if (env.getIntParamValue (popSize, "genPopSize") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : genPopSize is undefined.");
	double presetAxiomSets;
	if (env.getDoubleParamValue(presetAxiomSets, "popPreset") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : popPreset is not set.");
	// Проверка корректности числа систем аксиом
	if ((popSize < 1) || (presetAxiomSets < 0) || (presetAxiomSets > 1))
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : incorrect number of AxiomSets is present.");

	// *.0 Подготовка для установки популяции систем аксиом
	int knownAxioms;
	knownAxioms = round (((double) popSize) * presetAxiomSets);
	//Logger::getInstance()->writeDebug("knownAxioms : " + boost::lexical_cast<std::string>(knownAxioms));
	
	// *.1. Ищем AxiomBase - она должна быть задана, читаем ее
	std::string axiomBaseDir;
	if (env.getStringParamValue(axiomBaseDir, "AxiomBaseDir") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : popPreset > 0 but axiom base dir is not set.");
	AxiomBase axiomBase;
	axiomBase.readAxioms(axiomBaseDir.c_str());
	
	// *.2. ищем базовую директорию с наборами систем аксиом
	std::string axiomSetDir;
	if (env.getStringParamValue(axiomSetDir, "AxiomSetBaseDir") < 0)
	  throw AxiomLibException("TemplateGeneticsExt::initFromEnv : popPreset > 0 but axiom set base dir is not set.");

	// *.3. получаем множество наборов аксиом
	std::set<std::string> axiomSetNames;
	if (env.getStringSetParamValue(axiomSetNames, "AxiomSet") < 0 && knownAxioms > 0) 
	  throw AxiomLibException("TemplateGeneticsExt::initFromEnv : popPreset > 0 but no axiom set is given.");
	
	// *.4. Определяем итератор для хождения по набору названий систем аксиом
	std::set<std::string>::const_iterator curAx = axiomSetNames.begin();
	  
	// *.5. Определяем параметры для систем аксиом, подбираемых случайным образом
	int numOfAxioms;
	int maxECinAxiom;
	if (presetAxiomSets < 1) {
		if (env.getIntParamValue(numOfAxioms, "numOfAxioms") < 0)
			throw AxiomLibException("TemplateGeneticsExt::initFromEnv : numOfAxioms is not set.");
		if (env.getIntParamValue(maxECinAxiom, "maxECinAxiom") < 0)
			throw AxiomLibException("TemplateGeneticsExt::initFromEnv : maxECinAxiom is not set.");
	}

	// *.6 Создаем и определяем системы аксиом по прочитанным ранее параметрам
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : population has not been initialized.");
	for (unsigned int i = 0; i < population->size(); i++)
		(*population)[i].clear();
	population->resize (popSize);
	if (debug) std::cout << "\n	Generated elems of axiomSet population:\n";
	for (int i = 0; i < popSize; i++) {
		// Определяем какую систему аксиом устанавливать [заданную / случайную]
		if (i < knownAxioms) {
			// Значит используем одну из систем аксиом перечисленных в конфигурационном файле
			((*population)[i]).axiomSet = new AxiomSet;
			((*population)[i]).axiomSet->initAxiomSetFromFile(axiomSetDir, *curAx, axiomBase);
			if (debug) std::cout << "	" << i+1 << ". " << (*population)[i].axiomSet->name() << "\n";
			// Сдвигаем итератор по используемым системам аксиом
			++curAx;
			if (curAx == axiomSetNames.end())
				curAx = axiomSetNames.begin();
		} else {
			// Значит используем систему аксиом, сгенерированную случайно
			((*population)[i]).axiomSet = new AxiomSet;
			((*population)[i]).axiomSet->initAxiomSetByRand (numOfAxioms, maxECinAxiom);
			if (debug) std::cout << "	" << i+1 << ". Random Axiom Set\n";
		}
	}
	
	// 4. Устанавливаем параметры операции мутации
	transmuteTpl.setParamsFromEnv (env);
	if (env.getDoubleParamValue(prTransProbMin, "prTransProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : prTransProbMin is not set.");
	if (env.getDoubleParamValue(prTransProbMax, "prTransProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : prTransProbMax is not set.");
	if (env.getDoubleParamValue(asTransProbMin, "asTransProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : asTransProbMin is not set.");
	if (env.getDoubleParamValue(asTransProbMax, "asTransProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : asTransProbMax is not set.");
	if (env.getDoubleParamValue(axTransProbMin, "axTransProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : axTransProbMin is not set.");
	if (env.getDoubleParamValue(axTransProbMax, "axTransProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : axTransProbMax is not set.");
	if (env.getDoubleParamValue(ecTransProbMin, "ecTransProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : ecTransProbMin is not set.");
	if (env.getDoubleParamValue(ecTransProbMax, "ecTransProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : ecTransProbMax is not set.");

	// 5. Инициализируем операцию скрещивания
	crossoverTpl.setParamsFromEnv (env);
	if (env.getDoubleParamValue(asCrossProbMin, "asCrossProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : asCrossProbMin is not set.");
	if (env.getDoubleParamValue(asCrossProbMax, "asCrossProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : asCrossProbMax is not set.");
	if (env.getDoubleParamValue(axCrossProbMin, "axCrossProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : axCrossProbMin is not set.");
	if (env.getDoubleParamValue(axCrossProbMax, "axCrossProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : axCrossProbMax is not set.");
	if (env.getDoubleParamValue(ecCrossProbMin, "ecCrossProbMin") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : ecCrossProbMin is not set.");
	if (env.getDoubleParamValue(ecCrossProbMax, "ecCrossProbMax") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : ecCrossProbMax is not set.");

	// 6. Операция селекции - вытаскиваем необходимые параметры
	if (env.getDoubleParamValue (percentBest, "percentBest") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : param 'percentBest' for selection in GA is not set.");
	if ( (percentBest > 1.0) || (percentBest < 0.0) )
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : param 'percentBest' has a wrong value.");

	// 7. Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : goal strategy is undefined.");
	goalStrategy = gsf.create(goalStrategyName);
	goalStrategy->setParamsFromEnv(env);

	// 8. Инициализация условия досрочного выхода из цикла генетического алгоритма и условия выхода из алгоритма обучения шаблонов
	if (env.getDoubleParamValue (exitCond, "exitCond") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : exit condition for GA is undefined.");
	if (env.getDoubleParamValue (exitCondGen, "exitCondGen") < 0)
		throw AxiomLibException("TemplateGeneticsExt::initFromEnv : exit condition for template learning algorithm is undefined.");

	return 0;
}


/****************************************************************************
*					TemplateGeneticsExt::run
*
*	Description:	Запуск алгоритма обучения шаблона
*	Parameters:		templateRecognizer - шаблон, обучение в рамках которого производится
*	Returns:		0
*	Throws:			AxiomLibException - если произошла ошибка в обучении или ГА не
*					был инициализирован
*	Author:			dk
*	History:
*
****************************************************************************/
double TemplateGeneticsExt::run(TemplateRecognizer& templateRecognizer) {
	// Алгоритм обучения шаблонов:
	// 0. проверяем, установлены ли все необходимые параметры
	// В цикле:
	// 1. Осуществляем с помощью ГА подбор системы аксиом
	// 2. Производим поиск настроек предобработчика
	// 3. Производим поиск настроек распознавателя
	// 4. Проверяем условия выхода из цикла (по общему числу итераций, по числу итераций без улучшений, по значению целевой функции)
	// После цикла:
	// 5. Присваиваем входному шаблону лучшее за всю истрию обучения значение


	// Проверяем корректность начальных параметров
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::run : population has not been initialized.");
	if (population->size() < 1)
		throw AxiomLibException("TemplateGeneticsExt::run : population has not been set.");

	// Так как система аксиом будет подбираться в данной функции, удаляем старую систему аксиом
	/*if (templateRecognizer.axiomSet != NULL) {
		templateRecognizer.axiomSet->clear();
		delete templateRecognizer.axiomSet;
		templateRecognizer.axiomSet = NULL;
	}
	*/

	// Заводим переменную для хранения лучшей системы аксиом за историю
	AxiomSetPlus bestElem;
	TemplateRecognizer bestTemplate;
	
	// Подготовка внутренних переменных для алгоритма обучения и ГА
	int numOfBadIterCount = 0;
	double bestResult = -1.0;
	int numOfBadGenIterCount = 0;
	double bestResultGen = -1.0;
	std::vector <unsigned int> mask;
	double resGoal = -1;
	double tmpGoal = -1;
	double tmpGoalRec = -1;

	// Основной цикл алгоритма обучения шаблона
	for (int baseIter = 0; baseIter < numOfIter; baseIter++) {
		if (debug) cout << "	TemplateGeneticsExt: BA iteration " << baseIter+1 << " out of " << numOfIter << "\n";

		// 1. ГА подбора системы аксиом
		for (int gaIter=0; gaIter < numOfGenIter; gaIter++) {
			if (debug) cout << "		TemplateGeneticsExt: GA iteration " << gaIter+1 << " out of " << numOfGenIter;
			
			// *.1. Проводим Селекцию
			if (gaIter > 0)	
				select (mask);

			// *.2. Скрещиваем
			if (gaIter > 0)
				crossover ();

			// *.3. Мутируем
			if (gaIter > 0)
				transmute ();

			if (debug) cout << ", pop size = " << population->size() << "\n";
			// *.4. Проверяем системы аксиом на тестовой выборке
			recognize (templateRecognizer);
			
			// *.5. Прогоняем вычисление целевой функции H(first, second)
			compute ();

			// *.6. Сортируем системы аксиом по значению целевой функции, результаты записываем в вектор маски
			sort (mask);

			// *.7. Заполняем статистику по элементам популяции на основе данных полученных при распознавании
			fillStatistic(mask);
			
			// *.8. Сохряняем лучший элемент популяции
			if ((gaIter == 0) || ((*population)[mask[0]].goal < bestElem.goal)) {
				bestElem = (*population)[mask[0]];
				numOfBadGenIterCount = 0;
			} else {
				numOfBadGenIterCount++;
			}
			
			if (debug) cout << "		TemplateGeneticsExt: best goal = "<<bestElem.goal
							<<"=("<<bestElem.nFirst<<","<<bestElem.nSecond<<")"<<std::endl;

			// *.9. Проверяем условия выхода из цикла
			if ((numOfBadGenIterCount > numOfBadGenIter) || (bestElem.goal < exitCondGen))
				break;
		}

		// 2. Устанавливаем лучшую систему аксиом в шаблон распознавателя
		if (templateRecognizer.axiomSet == NULL)
			templateRecognizer.axiomSet = new AxiomSet;
		//*(templateRecognizer.axiomSet) = *((*population)[mask[0]].axiomSet);
		*(templateRecognizer.axiomSet) = *(bestElem.axiomSet);

		int e1 = 0, e2 = 0;

		templateRecognizer.recognizer->run();

		// 3. Вызываем алгоритм обучения распознавателя
		tmpGoalRec = templateRecognizer.recognizer->learn (templateRecognizer);

		// 4. Вызываем алгоритм обучения предобработчика
		tmpGoal = templateRecognizer.preprocess->learn (templateRecognizer);

		// Если < 0, значит предобработка не настраивалась
		if (tmpGoal < 0) {
			tmpGoal = tmpGoalRec;
		}
		
//		double tmpGoal2;
//		{
//			int resFirst, resSecond;
//			templateRecognizer.recognizer->run (*bestElem.axiomSet, templateRecognizer.prepDataSet, templateRecognizer.params, resFirst, resSecond);
//			tmpGoal2 = goalStrategy->compute(resFirst, resSecond);
//		}
		
		if (debug) cout << "	This iteration best goal value is " << tmpGoal << std::endl;

		// 5. Обновляем лучшее решение
		if ((baseIter == 0) || (tmpGoal < resGoal)) {
			bestTemplate = templateRecognizer;
			resGoal = tmpGoal;
			numOfBadIterCount = 0;
		} else {
			numOfBadIterCount++;
		}

		// 6. Проверяем условия выхода из цикла
		if ((numOfBadIterCount > numOfBadIter) || (resGoal < exitCond))
			break;
	}

	// Устанавливаем как результат - лучше значение за историю обучения
	templateRecognizer = bestTemplate;

	return resGoal;
}


int TemplateGeneticsExt::select (const std::vector<unsigned int> &mask) {
	// Проверка входных параметров
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::select : population has not been initialized.");
	if (mask.size() != population->size())
		throw AxiomLibException("TemplateGeneticsExt::select : vector of masks is unset.");
	// Определяем число лучших элементов популяции, которые точно выживут
	int numBest;
	numBest = round (percentBest * popSize);
	// Если размер популяции меньше требуемого, то удалять нечего, выходим из функции
	int numToDel;
	numToDel = population->size() - popSize;
	if (numToDel <= 0)
		return 1;
	// Определение элементов популяции, которые будут удалены
	int numToChoose;
	numToChoose = population->size() - numBest;
	// Теперь нужно выбрать из numToChoose число numToDel элементов, которые будут удалены
	std::vector <bool> toDel;
	toDel.resize (numToChoose);
	for (unsigned int i = 0; i < (unsigned int) numToChoose; i++) 
		toDel[i] = true;
	int tempInt;
	int maskSize;
	maskSize = mask.size();
	int numToStore = popSize - numBest;
	std::vector <unsigned int> indexToStore;
	indexToStore.resize(numToStore);
	for (unsigned int j = 0; j < (unsigned int) numToStore; j++) {
		tempInt = round ( ((double) (numToChoose - j - 1)) * ((double) rand()) / ((double) RAND_MAX) );
		while ((tempInt < (int) toDel.size()) && (!toDel[tempInt]))
			tempInt++;
		if (tempInt >= (int) toDel.size())
			throw AxiomLibException("TemplateGeneticsExt::select : internal error - wrong params for select function.");
		toDel[tempInt] = false;
		indexToStore[j] = mask[maskSize - tempInt - 1];
	}
	
	// Удаление выбранных элементов популяции
	std::vector <AxiomSetPlus> *newPop;
	newPop = new std::vector <AxiomSetPlus>;
	newPop->resize(popSize);
	// Копируем лучшие элементы популяции
	for (unsigned int i = 0; i < (unsigned int) numBest; i++)
		(*newPop)[i] = (*population)[mask[i]];
	// Копируем случайные особи из оставшихся
	for (unsigned int i = 0; i < (unsigned int) numToStore; i++) {
		(*newPop)[numBest + i] = (*population)[indexToStore[i]];
	}

	for (unsigned int i = 0; i < population->size(); i++)
		(*population)[i].clear();
	population->clear();
	delete population;

	population = newPop;

	return 0;
}


int TemplateGeneticsExt::transmute () {
	// Проверка начальных параметров
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::transmute : population has not been initialized.");
	if (population->size() < 1) {
		throw AxiomLibException ("Error in TemplateGeneticsExt::transmute: population is not set.");
	}
	transmuteTpl.transmute (*population);
	return 0;
}


int TemplateGeneticsExt::crossover () {
	// Проверка начальных параметров
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::crossover : population has not been initialized.");
	if (population->size() < 2) {
		throw AxiomLibException ("Error in TemplateGeneticsExt::crossover: population is to small.");
	}

	std::vector <AxiomSetPlus> *ret;
	ret = new std::vector <AxiomSetPlus>;
	crossoverTpl.crossover (*population, *ret);
	for (unsigned int i = 0; i < population->size(); i++)
		(*population)[i].clear();
	population->clear();
	delete population;

	population = ret;

	return 0;
}


/****************************************************************************
*					TemplateGeneticsExt::recognize
*
*	Description:	Функция вычисляет число ошибок I и II рода для всех элементов
*					популяции и записывает результат в соответствующие переменные 
*					элементов популяции.
*	Parameters:		templateRecognizer - шаблон, с помощью которого произоводить 
*					оценку числа ошибок
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int TemplateGeneticsExt::recognize (TemplateRecognizer& templateRecognizer) {
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::recognize : population has not been initialized.");
	// Подготовка шаблона распознавателя
	if (templateRecognizer.axiomSet == NULL)
		templateRecognizer.axiomSet = new AxiomSet;
	// Запуск алгоритма предобработки исходных данных
	templateRecognizer.runPreprocessor();
	// В цикле происходит тестировние всех элементов популяции используя данный шаблон
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < (int) population->size(); i++) {
		int resFirst, resSecond;
		templateRecognizer.recognizer->run (*((*population)[i].axiomSet), templateRecognizer.prepDataSet, templateRecognizer.params, resFirst, resSecond, (*population)[i].axiomProfit);
		(*population)[i].nFirst = resFirst;
		(*population)[i].nSecond = resSecond;
	}
	return 0;
}


/****************************************************************************
*					TemplateGeneticsExt::compute
*
*	Description:	Функция вычисляет целевую функцию для элементов популяции по 
*					числу ошибок первого и второго рода
*	Parameters:		-
*	Returns:		0
*	Throws:			AxiomLibException - если не установлен класс подсчета целевой функции
*	Author:			dk
*	History:
*
****************************************************************************/
inline int TemplateGeneticsExt::compute () {
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::compute : population has not been initialized.");
	// Проврека наличия класса подсчета целевой функции по числу ошибок I и II рода
	if (goalStrategy == NULL)
		throw AxiomLibException("TemplateGeneticsExt::compute : goal class is not set.");
	// Вычисление целевой функции для элементов популции
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < (int) population->size(); i++) {		
		(*population)[i].goal = goalStrategy->compute ((*population)[i].nFirst, (*population)[i].nSecond);
	}	
	return 0;
}


/****************************************************************************
*					TemplateGeneticsExt::fillStatistic
*
*	Description:	Заполняем статистику по элементам популяции на основе данных 
*					полученных при распознавании.
*	Parameters:		-
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int TemplateGeneticsExt::fillStatistic (std::vector<unsigned int> &mask) {
	// Проверка входного параметра
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::fillStatistic : population has not been initialized.");
	if (population->size() < 1)
		throw AxiomLibException("TemplateGeneticsExt::fillStatistic : population in not set.");
	if (mask.size() != population->size())
		throw AxiomLibException("TemplateGeneticsExt::fillStatistic : incompatible input parameter.");
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < (int) population->size(); i++) {
		(*population)[i].goalRatio = (double) (epsilon + (*population)[i].goal) / (double) (epsilon + (*population)[mask[0]].goal);
		if ((*population)[i].goalRatio <= bestGoals) {
			// Установка параметров скрещивания
			(*population)[i].asCrossProb = asCrossProbMax;
			(*population)[i].axCrossProb = axCrossProbMax;
			(*population)[i].ecCrossProb = ecCrossProbMax;
			// Установка параметров мутации
			(*population)[i].prTransProb = prTransProbMin;
			(*population)[i].asTransProb = asTransProbMin;
			(*population)[i].axTransProb = axTransProbMin;
			(*population)[i].ecTransProb = ecTransProbMin;
		} else {
			// Установка параметров скрещивания
			(*population)[i].asCrossProb = asCrossProbMin;
			(*population)[i].axCrossProb = axCrossProbMin;
			(*population)[i].ecCrossProb = ecCrossProbMin;
			// Установка параметров мутации
			(*population)[i].prTransProb = prTransProbMax;
			(*population)[i].asTransProb = asTransProbMax;
			(*population)[i].axTransProb = axTransProbMax;
			(*population)[i].ecTransProb = ecTransProbMax;

		}
	}
	return 0;
}


/****************************************************************************
*					TemplateGeneticsExt::sort
*
*	Description:	Функция сортировки элементов популяции по неубыванию значений 
*					целевой функции. При этом сами элементы не меняют своего положения,
*					а результат сортировки записывается в вектор mask, идущий в параметрах.
*	Parameters:		mask - заполняемый вектор, элементы которго указывают индексы элементов 
*					популяции в порядке возрастания целевой функции
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int TemplateGeneticsExt::sort (std::vector<unsigned int> &mask) const {
	if (population == NULL)
		throw AxiomLibException("TemplateGeneticsExt::sort : population has not been initialized.");
	// Подготовка вектора маски
	mask.resize (population->size());
	// собственно сортировка по принципу - размещения в порядке неубывания по goals индексов в mask
	unsigned int j = 0;
	for (unsigned int i = 0; i < population->size(); i++){
		j = 0;
		// поиск размешения
		while ( j < i ) {
			if ((*population)[i].goal < (*population)[mask[j]].goal) {
				break;
			}
			j++;
		}
		// смещение всех, у кого goal меньше
		for (unsigned int k = i; k > j; k--) {
			mask[k] = mask[k-1];
		}
		// собственно размещение индекса
		mask[j] = i;
	}	
	return 0;
}


// Сортировка вектора целых чисел с помощью меторда пузырька
inline void TemplateGeneticsExt::sortVector (std::vector<unsigned int> &vec) const {
	if (vec.size() < 2)
		return;
	unsigned int tempInt;
	for (unsigned int i = vec.size() - 1; i > 0; i--) {
		for (unsigned int j = 0; j < i; j++) {
			if (vec[j] > vec[j+1]) {
				tempInt = vec[j+1];
				vec[j+1] = vec[i];
				vec[i] = tempInt;
			}
		}
	}
}
