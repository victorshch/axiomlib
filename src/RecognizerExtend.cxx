/****************************************************************************
*				RecognizerExtend.cxx
*
*	Description:	Функции класса RecognizerExtend - Реализация
*	Author:			gevor, dk
*	History:		
*
****************************************************************************/
#include <limits.h>

#include "RecognizerExtend.h"
#include "TemplateRecognizer.h"

using namespace AxiomLib;

#define noValue -1.0
#define precMinValue 0.001
#define precMaxValue 0.999
#define precCentralValue 0.3 // Этот параметр определяет откуда с точностью до precDelta начинать поиск
#define precDelta 0.25
#define precMinDelta 0.005
#define epsilon 0.01

// Конструктор класса с заданием значений по умолчанию
RecognizerExtend::RecognizerExtend (void) { 
	comments = false;
	outputToFile = false;
	whereToSave = "./";
	saveDirName = "axiomSet_";
	curDir.clear();
	curFile.clear();
	precision = 0.0; 
	compareStatistic = NULL; 
	goalStrategy = NULL;
}

// Деструктор класса с уничтожением всех созданных динамических объектов
RecognizerExtend::~RecognizerExtend (void) { 
	if (compareStatistic != NULL) delete compareStatistic; 
	if (goalStrategy != NULL) delete goalStrategy;
}

/****************************************************************************
*                       RecognizerExtend::setParamsFromEnv
*
*       Description:    Инициализировать параметры данными из env
*       Parameters:     const Environment& env - среда
*       Returns:        0
*       Throws:         AxiomLibException - если нет какого-топараметра
*       Author:         gevor
*       History:
*
****************************************************************************/
int RecognizerExtend::setParamsFromEnv (const Environment& env) {
	// инициализируем dataset
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("RecognizerExtend::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("RecognizerExtend::setParamsFromEnv : data set is undefined.");

	// считываем необходимые для данного класса параметры о наборе данных
	EnvDataSet envDataSet;
	envDataSet.readConfigFile (datasetDir, datasetName);
	// установка корректного обозначения NullStr - обозначение остутсвия в данной точке ряда какого либо нештатного поведения
	dataSet.setNullStr (envDataSet);
	// собственно считываем набор данных - заводим его во внутреннее представление
	dataSet.readDataSet(datasetDir, datasetName);
	// восстанавливаем в данном классе вектор индексов параметров в каноническом представленнии по которым вести поиск нештатых ситуаций
	dataSet.getParamNums(params, env, envDataSet);

	// установка параметров относящикся к MarkUp
	if (env.getDoubleParamValue(precision, "precision") < 0)
		throw AxiomLibException("RecognizerExtend::setParamsFromEnv : precision is undefined.");
 	//std::cout << "\n Precision set to " << precision << " !\n";
	// создание класса подсчета статистики
	std::string compClassName;
	CompareClassFactory ccf;
	if (env.getStringParamValue(compClassName, "compareClass") < 0)
		throw AxiomLibException("RecognizerExtend::setParamsFromEnv : compare-class is undefined.");
	compareStatistic = ccf.create(compClassName);
	compareStatistic->setParamsFromEnv(env);

	// Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") >= 0) {
		goalStrategy = gsf.create(goalStrategyName);
		goalStrategy->setParamsFromEnv(env);
	}

	// информация о сохранении файлов
	env.getStringParamValue(whereToSave, "PathToSaveResults");
	env.getStringParamValue (saveDirName, "saveTo");

	std::string str;
	env.getStringParamValue (str, "outputMarkUpToFile");
	if ((str == "true") || (str == "True") || (str == "TRUE"))
		outputToFile = true;	

	return 0;
}


/****************************************************************************
*                       RecognizerExtend::setNominalParamsFromEnv
*
*       Description:	Инициализировать только основные параметры данными из env
*       Parameters:	const Environment& env - среда
*       Returns:		0
*       Throws:		AxiomLibException - если нет какого-то параметра
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerExtend::setNominalParamsFromEnv (const Environment& env) {
	// установка параметров относящикся к разметке
	if (env.getDoubleParamValue(precision, "precision") < 0)
		throw AxiomLibException("RecognizerExtend::setParamsFromEnv : precision is undefined.");
 
	// создание класса подсчета статистики
	std::string compClassName;
	CompareClassFactory ccf;
	if (env.getStringParamValue(compClassName, "compareClass") < 0)
		throw AxiomLibException("RecognizerExtend::setParamsFromEnv : compare-class is undefined.");
	compareStatistic = ccf.create(compClassName);
	compareStatistic->setParamsFromEnv(env);

	// информация о сохранении файлов
	env.getStringParamValue(whereToSave, "PathToSaveResults");
	env.getStringParamValue (saveDirName, "saveTo");

	std::string str;
	env.getStringParamValue (str, "outputMarkUpToFile");
	if ((str == "true") || (str == "True") || (str == "TRUE"))
		outputToFile = true;

	return 0;
}


/****************************************************************************
*                       RecognizerExtend::setDataSet
*
*       Description:	Функция установки нового набора данных в текущий класс распознователя
*       Parameters:	newDataSet - новый набор данных
*       Returns:		0
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerExtend::setDataSet (const DataSet &newDataSet) {
	dataSet = newDataSet;
	return 0;
}


/****************************************************************************
*                       RecognizerExtend::setParamsOfDataSet
*
*       Description:	Функция установки нового значаения переменой класса params
*       Parameters:	newParams - новое значение вектора
*       Returns:		0
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerExtend::setParamsOfDataSet (std::vector<int> &newParams) {
	params = newParams;
	return 0;
}


/****************************************************************************
*                       RecognizerExtend::setPrecision
*
*       Description:	Функция установки нового значения параметра precision
*       Parameters:	newPrecision - новое значение параметра
*       Returns:		0 - param was set
*				-1 - param wasn't set
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerExtend::setPrecision (const double newPrecision) {
	if ( (newPrecision > 0.0) && (newPrecision < 1.0) ) {
		precision = newPrecision;
		return 0;
	}
	return -1;
}


/****************************************************************************
*                       RecognizerExtend::setCompareStatistic
*
*       Description:	Функция установки нового класса подсчета статистики
*       Parameters:		newCompareStatistic - новое значение, копия которого 
*						устанавливается как новый класс статистики
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int RecognizerExtend::setCompareStatistic (CompareStatistic *newCompareStatistic) {
	if (compareStatistic != NULL) {
		delete compareStatistic;
		compareStatistic = NULL;
	}
	if (newCompareStatistic != NULL) {
		compareStatistic = newCompareStatistic->copy();
	}
	return 0;
}


/****************************************************************************
*                       RecognizerExtend::copy
*
*       Description:	Функция возвращает копию данного распознавателя
*       Parameters:		-
*       Returns:		Recognizer* - ссылка на копию данного распознавателя
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
Recognizer* RecognizerExtend::copy (void) {
	RecognizerExtend* toReturn;
	toReturn = new RecognizerExtend;
	toReturn->setDataSet (dataSet);
	toReturn->setParamsOfDataSet (params);
	toReturn->setPrecision (precision);
	toReturn->setCompareStatistic (compareStatistic);
	toReturn->setOutputToFile(outputToFile, whereToSave, saveDirName);
	return (Recognizer*) toReturn;
}


/****************************************************************************
*						RecognizerExtend::learn
*
*       Description:	Функция обучения данного алгоритма распознавания
*       Parameters:		templateRecognizer - шаблон распознавателя, на котором
*						проверять качество обучения данного алгоритма распознавания
*       Returns:		double - значение целевой функции для лучшего набора параметров
*       Throws:			AxiomLibException - если параметры поиска заданы неверно
*       Author:			dk
*       History:
*
****************************************************************************/
double RecognizerExtend::learn (TemplateRecognizer &templateRecognizer) {
	// В данной функции будут производиться настройка параметра точности задания эталонной разметки prcision. 
	// так как в данном классе - это единственный параметр.
	// В будущем можно вызывать функцию learn у класса compareStatistic.
	// В данной функции ведется поиск оптимума ведется для параметра precision

	// 1. Проверка начальных значений
	if (goalStrategy == NULL)
		throw AxiomLibException("RecognizerExtend::learn: goalClass is not set in RecognizerExtend, but needed for learn function.");

	// 2. Подготовка переменных для поиска
	// 2.1. Начальные значения параметра, по которым вести поиск оптимального значения
	std::vector <double> curPrecision;
	curPrecision.resize(5);
	// Задание начальных значений элементам вектора curPrecision
	if ((precision > precMinValue) && (precision < precMaxValue)) {
		// Значит за центральную точку берем текущее значение параметра precision
		curPrecision[2] = precision;
	} else {
		// Значит начальное значение параметра не задано - определяем его случайно в заданном интервале
		do {
			curPrecision[2] = precCentralValue + precDelta*(2.0*((double) rand ()) / ((double) RAND_MAX) - 1.0);
		} while ((curPrecision[2] <= precMinValue) || (curPrecision[2] >= precMaxValue));
	}
	// Определяем минимальное положение параметра
	if (curPrecision[2] > (precMinValue + precDelta))
		curPrecision[0] = curPrecision[2] - precDelta;
	else 
		curPrecision[0] = precMinValue;
	// Определяем максимальное положение параметра
	if (curPrecision[2] < (precMaxValue - precDelta))
		curPrecision[4] = curPrecision[2] + precDelta;
	else 
		curPrecision[4] = precMaxValue;
	// Определяем положение промежуточных точек
	curPrecision[1] = 0.5*(curPrecision[0] + curPrecision[2]);		
	curPrecision[3] = 0.5*(curPrecision[2] + curPrecision[4]);

	// 2.2. Вектор для значений целевой функции, соответствующей значениям из curPrecision
	std::vector <double> curRes; 
	curRes.resize(5);
	for (unsigned int i=0; i < 5; i++) {
		curRes[i] = noValue;
	}
	
	// 2.3 Матрица относительных результатов для вектора curRes
	std::vector < std::vector <signed short> > comparisonTable; 
	comparisonTable.resize(5);
	for (unsigned int i=0; i < 5; i++) {
		comparisonTable[i].resize(5);
		comparisonTable[i][i] = 0;
	}

	// 3. Запуск рекурсивного алгоритма поиска 
	templateRecognizer.runPreprocessor();
	return sublearn (templateRecognizer, curRes, curPrecision, comparisonTable);
}


/****************************************************************************
*						RecognizerExtend::sublearn
*
*       Description:	Вспомогательная функция для алгоритма обучения. 
*						Рекурсивно производит поиск наилучшего значения параметра
*						precision. При этом интервал поиска с каждым циклом 
*						уменьшаяется ровно в 2 раза. Критерий выхода - размер
*						интеравала меньше параметра precMinDelta.
*       Parameters:		templateRecognizer - шаблон, на которо производить вычисление целевой функции
*						curRes - вектор значений целевой функции для значений precision из вектора curPrecision
*						curPrecision - вектор значений precision по которым ведется поиск лучшего значения
*						comparisonTable - матрица взаимного расположения элеметов вектора curRes
*       Returns:		double - 
*       Throws:			AxiomLibExceprion - если функция вызвана с противоречивыми параметрами
*       Author:			dk
*       History:
*
*****************************************************************************/
double RecognizerExtend::sublearn (TemplateRecognizer &templateRecognizer, std::vector <double> &curRes, std::vector <double> &curPrecision, std::vector < std::vector <signed short> > &comparisonTable) {
	// Определение значений целевой функции для значений параметра precision из вектора curPrecision
	int resFirst, resSecond;
	for (unsigned int i = 0; i < 5; i++) {
		if (curRes[i] == noValue) {
			precision = curPrecision[i];
			templateRecognizer.runRecognizer(resFirst, resSecond);
			curRes[i] = goalStrategy->compute (resFirst, resSecond);
		}
	}

	// Определение взаимного расположения результатов для различных значений параметра precision
	for (unsigned int i=0; i < 4; i++) {
		for (unsigned int j = i+1; j < 5; j++) {
			if (curRes[i] > (curRes[j] + epsilon))
				comparisonTable[i][j] = 1;
			else 
				if (curRes[i] < (curRes[j] - epsilon))
					comparisonTable[i][j] = -1;
				else
					comparisonTable[i][j] = 0;
			comparisonTable[j][i] = - comparisonTable[i][j];
		}
	}

	// Определение по таблице взаимных результатов - куда двигать дальше
	unsigned int direction;
	direction = chooseDirection (comparisonTable);
	
	// Условие выхода из рекурсивного цикла
	if ( ( (direction < 2) && ((curPrecision[2] - curPrecision[1]) < precMinDelta) ) || 
		 ( (direction >= 2) && ((curPrecision[3] - curPrecision[2]) < precMinDelta) ) ) {
		precision = curPrecision[direction];
		return curRes[direction];
	}
	
	// Изменение векторов в соответствии с выбранным направлением движения
	manageSets (direction, curRes, curPrecision);
	
	// Продолжение процедуры обучения
	return sublearn (templateRecognizer, curRes, curPrecision, comparisonTable);
}


/****************************************************************************
*						RecognizerExtend::chooseDirection
*
*       Description:	Вспомогательная функция для обучения класса по парамету precision.
*						В данной функции выбирается лучшее значение параметра precision по
*						уже составленной таблице сравенения результатов compTable.
*       Parameters:		compTable - сравнительная таблица результатов для разных значений 
*						параметра precision.
*       Returns:		unsigned int - целое число из [0,4] - лучшее направление изменения
*       Throws:			AxiomLibException - в случае неправильно составленной таблицы
*       Author:			dk
*       History:
*
*****************************************************************************/
inline unsigned int RecognizerExtend::chooseDirection (std::vector < std::vector <signed short> > &compTable) const {
	// Подготавливаем необходимые переменные
	std::vector <bool> vec; // Информация о том, какие из направлений являются наименьшими
	vec.resize(5);
	// Вычисление кандидатов для направлений изменения параметра precision
	int numOfCandidates = 5;
	for (unsigned int i = 0; i < 5; i++) {
		vec[i] = true;
		for (unsigned int j = 0; j < 5; j++) {
			if (compTable[i][j] > 0) {
				vec[i] = false;
				numOfCandidates--;
				break;
			}
		}
	}
	// Выбор лучшего направления изменения по вектору кандидатов
	switch (numOfCandidates) {
		case 1:
			for (int i = 0; i < 5; i++)
				if (vec[i]) return i;
			break;
		case 2:
			if (vec[2]) return 2;
			if (vec[0]) 
				if (!vec[4]) return 0;
				else
					if (compTable[1][3] >= 0)  return 0;
					else return 4;
			if (vec[4]) return 4;
			if (compTable[0][4] >= 0) return 1;
			else return 3;
			break;
		case 3:
			if (vec[2]) return 2;
			if (vec[0] && vec[3]) return 0;
			if (vec[1] && vec[4]) return 4;
			break;
		case 4:
			if (vec[2]) return 2;
			else return 0;
			break;
		case 5:
			return 2;
			break;
		default:
			throw AxiomLibException("RecognizerExtend::chooseDirection: internal error - irrelevant number of candidates.");
			break;
	}
	throw AxiomLibException("RecognizerExtend::chooseDirection: internal error - should be returned before this point.");
	return 2;
}


/****************************************************************************
*						RecognizerExtend::manageSets
*
*       Description:	Вспомогательная функция для обучения класса по парамету precision
*						В данной функции происходит изменение вектор значений precision 
*						согласно управляющему значению параметра pos.
*       Parameters:		pos - управляющий параметр может принимать целые значения [-2,2]
*						curRes - вектор со значениями целевой функции соответствующих 
*							значениям precision из вектора curPrecision
*						curPrecision - вектор значений precision, используемых в обучении
*       Returns:		-
*       Throws:			AxiomLibException - если неверно задан управляюший параметр
*       Author:			dk
*       History:
*
*****************************************************************************/
inline void RecognizerExtend::manageSets (unsigned int direction, std::vector <double> &curRes, std::vector <double> &curPrecision) const {
	switch (direction) {
		case 0:
			curPrecision[2] = curPrecision[0];
			curPrecision[3] = 0.5*(curPrecision[0] + curPrecision[1]);
			curPrecision[4] = curPrecision[1];
			if ( (2*curPrecision[2]) > (curPrecision[4] + precMinValue) )
				curPrecision[0] = 2*curPrecision[2] - curPrecision[4];	
			else
				curPrecision[0] = precMinValue;
			curPrecision[1] = 0.5*(curPrecision[0] + curPrecision[2]);
			curRes[2] = curRes[0];
			curRes[4] = curRes[1];
			curRes[0] = noValue;
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 1:
			curPrecision[3] = 0.5*(curPrecision[1] + curPrecision[2]);
			curPrecision[4] = curPrecision[2];
			curPrecision[2] = curPrecision[1];
			curPrecision[1] = 0.5*(curPrecision[0] + curPrecision[1]);
			curRes[4] = curRes[2];
			curRes[2] = curRes[1];
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 2:
			curPrecision[0] = curPrecision[1];
			curPrecision[4] = curPrecision[3];
			curPrecision[1] = 0.5*(curPrecision[1] + curPrecision[2]);
			curPrecision[3] = 0.5*(curPrecision[2] + curPrecision[3]);
			curRes[0] = curRes[1];
			curRes[4] = curRes[3];
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 3:
			curPrecision[0] = curPrecision[2];
			curPrecision[1] = 0.5*(curPrecision[2] + curPrecision[3]);
			curPrecision[2] = curPrecision[3];
			curPrecision[3] = 0.5*(curPrecision[3] + curPrecision[4]);
			curRes[0] = curRes[2];
			curRes[2] = curRes[3];
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 4:
			curPrecision[0] = curPrecision[3];
			curPrecision[1] = 0.5*(curPrecision[3] + curPrecision[4]);
			curPrecision[2] = curPrecision[4];
			if ( (2*curPrecision[2]) < (curPrecision[0] + precMaxValue) )
				curPrecision[4] = 2*curPrecision[2] - curPrecision[0];
			else
				curPrecision[4] = precMaxValue;
			curPrecision[3] = 0.5*(curPrecision[2] + curPrecision[4]);
			curRes[0] = curRes[3];
			curRes[2] = curRes[4];
			curRes[1] = noValue;
			curRes[3] = noValue;
			curRes[4] = noValue;
			break;
		default:
			throw AxiomLibException("RecognizerExtend::manageSets: unrecgnized value of managment input param 'direction'.");
			break;
	}
}


/****************************************************************************
*				RecognizerExtend::joinResults
*
*	Description:	Функция голосования. Реализаций может быть множество. 
*				В данном случае: проверяем результаты по каждой из размерностей ряда,
*				если хотя бы на одной размерности была поймана нештатная ситуация - 
*				то происходит голосование - в результат входит нештатная ситуация с номером - 
*				который встретился на большинстве размерностей. То есть результат определяется
*				по обычному большинству голосов. 
*	Parameters:	result - заполняемый вектор - результат голосования
*				prelimResult - входная структура с результатами поиска нештатный ситуакций по каждой рассматриваемой размерности мультиряда описания состояния системы
*				numOfClasses - число классов неисправностей, которые могут быть найдены при анализе временных рядов
*	Returns:		0
*	Throws:		AxiomLibException - если входные данные не согласованы или число классов неисправностей < 1
*	Author:		dk
*	History:
*
****************************************************************************/
inline signed int RecognizerExtend::joinResults (std::vector <int> &result, std::vector < std::vector <int> > &prelimResult, const int numOfClasses) const {
	// если объединять то и нечего - то просто копируем результат и выходим
	if (prelimResult.size() == 1) {
		result = prelimResult[0];
		return 0;
	}
	// проверка корректности входных данных
	bool flag = 0;
	for (unsigned int i = 1; i < prelimResult.size(); i++) {
		if (prelimResult[i].size() != prelimResult[i-1].size()) {
			flag = 1;
			break;
		}
	}
	if ( (prelimResult.size() < 2) || (flag != 0) || (numOfClasses < 1) ){
		throw AxiomLibException("RecognizerExtend::joinResults: incorrect results in recognition process.");
	}
	// подготовка переменных функции
	std::vector <int> bulletin;
	result.resize(prelimResult[0].size());
	bulletin.resize(numOfClasses);
	int curBest;
	int curScore;
	// в цикле - пробегаем по все размероностям предварительного результата и объединяем в единый результат по принципу - большинства 
	for (long int j = 0; j < (long int) prelimResult[0].size(); j++) {
		// обнуление результатов голосования
		for (int t = 0; t < (int) bulletin.size(); t++) {
			bulletin[t] = 0;
		}
		// голосование
		for (int k = 0; k < (int) prelimResult.size(); k++) {
			if (prelimResult[k][j] > 0) {
				(bulletin[ prelimResult[k][j] - 1 ])++;
			}
		}
		// подсчте результатов - выявление победителя
		curScore = 0;
		curBest = -1;
		for (int t = 0; t < (int) bulletin.size(); t++) {
			if (bulletin[t] > curScore) {
				curScore = bulletin[t];
				curBest = t;
			}
		}
		// объявление результатов голования
		if (curBest < 0) {
			result[j] = 0;
		}
		else {
			result[j] = curBest + 1; // восстанавливаем 1-цу, которая была отнята при заполнении бюллетеня
		}
	}
	return 0;
}



/****************************************************************************
*				RecognizerExtend::recognition
*
*	Description:	Функция распознования нештатных ситуаций по набору рядов из мультиряда и
*				вектору эталонных разметок всех доступных видов нештатных ситуаций в разметке
*				некоторой определенной системы аксиом.
*	Parameters:	first - заполняемый параметр - число ошибок распознования первого рода
*				second - заполняемый параметр - число ошибок распознования второго рода
*				markUps - разметка тестовых рядов данной системой аксом - состоит из векторов - разметок по каждому из используемых измерений мультиряда
*				corrMarkUp - корректная разметка тестовых рядов, необходима для подсчета статистики по данному тесту
*				refs - разметки всех эталонных траекторий всех классов неисправностей
*	Returns:		0
*	Throws:		AxiomLibException - если после подсчтета статистики оказалось, что число всех встреченных нештатный ситуаций - отричательное
*	Author:		dk
*	History:
*
****************************************************************************/
inline signed int RecognizerExtend::recognition (int &first, int &second,  
												 std::vector <std::vector <int> > &markUps,
												 std::vector <int> &corrMarkUp, 
												 std::vector < std::vector <RefMarkupSet> > &refs,
												 int numOfTest) const
{
	// Вариантов разметки и подсчета статистики может быть множество
	// В данной функции будет использован седующий алгоритм:
	//	1. В цикле по всем параметрам (т.е. размеронстям временного ряда) пробегаем и ищем независимо 
	//	какие нештатные ситуации были найдены по каждой из размерностей.
	//	2. Далее объединяем результата с каждой из размерностей функцией голосования, т.о. получая единый ряд разметки 
	//	3. Далее полученный единый ряд разметки сопоставляем с рядом корректной разметки и подсчитываем статистику
	//	4. Полученную статистику забиваем в соответствующие поля относящиеся к данному тесту и возвращаем
	// подготовка внутренних переменных функции
	long int cur = 0;
	int flag = 0;
	double tmpFirst, tmpSecond; // пременный для подсчета числа ошибок на каждом из тестов
	int wholeNum; // число образов на каждом из тестов
	std::vector <int> newMarkup;
	std::vector <int> newMarkupLen;
	std::vector < std::vector <int> > prelimResult;
	prelimResult.resize(markUps.size());
	std::vector <int> result;
	first = 0;
	second = 0;
	// 1. цикл по всем рассматриваемым размерностям временных рядов
	//#pragma omp for
	for (int j = 0; j < (int) markUps.size(); j++) {
		newMarkup.resize((markUps[j]).size());
		newMarkupLen.resize((markUps[j]).size());
		(prelimResult[j]).resize((markUps[j]).size());
		newMarkup[0] = markUps[j][0];
		newMarkupLen[0] = 1;
		prelimResult[j][0] = 0;
		// 1. поиск нештатных ситуаций непосредственно по markUps[j]
		for (long int i = 1; i < (long int) markUps[j].size(); i++) {
			prelimResult[j][i] = 0;
			// собираем ряд расширенной разметки
			if (markUps[j][i] != markUps[j][i-1]) {
				cur++;
				newMarkup[cur] = markUps[j][i];
				newMarkupLen[cur] = 1;
			}
			else {
				(newMarkupLen[cur])++;
			}
			flag = 0;
			// Определяем есть ли совпадения с эталонными рядами - т.е. ищем хотя бы 1 класс неисправности
			for (int k = 0; k < (int) refs[j].size(); k++) {
				flag = (refs[j][k]).check (newMarkup, newMarkupLen, cur);
				if (flag != 0) {
					prelimResult[j][i] = k+1;
					break;
				}
			}
		}
	}
	
	// 2. Объединение результатов для рассматриваемой системы аксиом на данном тесте по каждой из размерностей
	joinResults (result, prelimResult, (int) refs[0].size());
	// вывод результатов в файл, если это необходимо
	if (outputToFile) updateRows (corrMarkUp, result);
	
	// 3. Cоспоставление полученных результатов с корректными - подсчет статистики
	wholeNum = compareStatistic->getStatistic(result, corrMarkUp, tmpFirst, tmpSecond, comments);
	if (wholeNum < 0) 
		throw AxiomLibException("Error in RecognizerExtend::recognition: incorrect results in process of getting statistic.");
	
	// 4. Заполнение полей по статистике и возврат из функции
	first = (int) tmpFirst;
	second = (int) tmpSecond;
	
	return 0;
}


/****************************************************************************
*				RecognizerExtend::initRefs
*
*	Description:	Функция добавления расширенных эталонных разметок 
*				к вектору разметок. Добавление происходит всех доступных 
*				разметок по заданным параметрам.
*	Parameters:	pop - популчция систем аксиом 
*				asNum - номер системы аксиом в популяции
*				param - имя размерности анализируемого ряда - эталонные разметки по которой добалвляем
*				refer - хранилище разметок по данной размерноси
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
inline signed int RecognizerExtend::initRefs (AxiomSetPop& pop, const int asNum, const int param, std::vector <RefMarkupSet> &refer) const {
	// подготовка внутренних переменных функции
	int numOfClasses;
	std::vector<int> numOfMultiTS;
	std::vector<std::vector<int> > numOfTS;
	bool b = dataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	std::vector<int> resRef;
	// Подготовка данных для анализа ряда
	refer.resize (numOfClasses);
	std::vector<double> vec; // Вектор используется в цикле - как временный
	vec.clear();
	bool bres = false;
	// Заполнение массива класса неиспрваностей
	for (int i = 0; i < numOfClasses; i++) {
		RefMarkupSet rms;
		rms.setPrecision(precision);
		for (int j = 0; j < numOfMultiTS[i]; j++) {
			bres = dataSet.getTSByIndexFromClass (vec, i, j, param);
			if (bres) {
				pop.enter (resRef, vec, 0, (unsigned long) vec.size(), asNum);
				rms.addRefMarkup(resRef, comments);
			}
		}
		refer[i] = rms;
	}
	return 0;
}


/****************************************************************************
*				RecognizerExtend::initRefs
*
*	Description:	Функция добавления расширенных эталонных разметок 
*				к вектору разметок. Добавление происходит всех доступных 
*				разметок по заданным параметрам.
*	Parameters:	axiomSet - система аксиом
*				param - имя размерности анализируемого ряда - эталонные разметки по которой добалвляем
*				refer - хранилище разметок по данной размерноси
*				refStat - статистика по использованию аксиом для разметки эталонных траекторий
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
inline signed int RecognizerExtend::initRefs (AxiomSet& axiomSet, const int param, std::vector <RefMarkupSet> &refer, std::vector <bool> &refStat) const {
	// подготовка внутренних переменных функции
	int numOfClasses;
	std::vector<int> numOfMultiTS;
	std::vector<std::vector<int> > numOfTS;
	bool b = dataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	std::vector<int> resRef;
	std::vector <bool> curStat;
	// Подготовка данных для анализа ряда
	refer.resize (numOfClasses);
	std::vector<double> vec; // Вектор используется в цикле - как временный
	vec.clear();
	bool bres = false;
	// Заполнение массива класса неиспрваностей
	for (int i = 0; i < numOfClasses; i++) {
		RefMarkupSet rms;
		rms.setPrecision(precision);
		for (int j = 0; j < numOfMultiTS[i]; j++) {
			bres = dataSet.getTSByIndexFromClass (vec, i, j, param);
			if (bres) {
				axiomSet.enter (resRef, vec, 0, (unsigned long) vec.size(), curStat);
				updateVec (refStat, curStat);
				rms.addRefMarkup(resRef, comments);
			}
		}
		refer[i] = rms;
	}
	return 0;
}


/****************************************************************************
*				RecognizerExtend::analizeTest
*
*	Description:	Анализ одного теста - возвращает общее число траекторий нештатного поведения в данном тесте
*	Parameters:		corrMarkUp - корректная разметка теста
*					res - число траекторий в данном тесте, которые необходимо распознать
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline signed int RecognizerExtend::analizeTest (std::vector <int> &corrMarkUp, int &res) {
	unsigned int i = 0;
	unsigned int j = 0;
	res = 0;
	while (i < corrMarkUp.size()) {
		if (corrMarkUp[i] != 0) {
			res++;
			j = i+1;
			while (j < corrMarkUp.size()) {
				if (corrMarkUp[j] != corrMarkUp[i])
					break;
				j++;
			}
			i = j;
		} else {
			i++;
		}
	}
	return 0;
}


/****************************************************************************
*					RecognizerExtend::updateVec
*
*	Description:	Вспомогательная процедура обновление вектора
*	Parameters:		vec - вектор которые подлежит обновлению
*					newVec - вектор на который происходит обновление
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline void RecognizerExtend::updateVec (std::vector <bool> &vec, std::vector <bool> &newVec) const {
	if (vec.size() != newVec.size()) {
		vec = newVec;
	} else {
	for (unsigned int i = 0; i < newVec.size(); i++)
		vec[i] = vec[i] || newVec[i];
	}
}


/****************************************************************************
*					RecognizerExtend::countStat
*
*	Description:	Вспомогательная процедура для подсчета статистики по результатам распознавания
*	Parameters:		stat - заполняемый вектор статистики
*					resFisrt - число ошибок первого рода
*					resSecond - число ошибок второго рода
*					resShouldBe - общее число нештатных ситуаций, которые есть в тестовых данных
*					refStat - статистика по использованию аксиом для разметки эталонных траекторий
*					rowStat - статистика по использованию аксиом для разметки тестовых рядов
*	Returns:		-
*	Throws:			AxiomLibException - если не входные параметры не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/
inline void RecognizerExtend::countStat (std::vector <double> &stat, int resFirst, int resSecond, int resShouldBe, std::vector <bool> &refStat, std::vector <bool> &rowStat) const {
	if (refStat.size() != rowStat.size())
		throw AxiomLibException("RecognizerExtend::countStat : internal error - statistial data is uncoordinated.");
	stat.resize (refStat.size());
	int temp;
	double multiplier;
	multiplier = 0.5 * (double) (resShouldBe - resSecond) / (double) (resShouldBe + resFirst); // resFisrt и resSecond изначально перепутаны во всей программе, поэтому и здесь формула с такими подстановками
	for (unsigned int i = 0; i < refStat.size(); i++) {
		temp = 0;
		if (refStat[i]) temp++;
		if (rowStat[i]) temp++;
		stat[i] = (double) temp * multiplier;
	}
}


/****************************************************************************
*				RecognizerExtend::run
*
*	Description:	Запуск процесса распознавания
*	Parameters:	AxiomSetPop& pop - популяция, с которой работаем
*	Returns:		0
*	Throws:		AxiomLibExeption - если популяция нулевого размера, или не инициализирована, или залочена
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int RecognizerExtend::run (AxiomSetPop& pop) {
	const bool debug = false;
	if (debug) std::cout << "\n	RecognizerExtend:";
	// проверка; в случае лока или не инициализированной популяции, AxiomSetPop кинет exception
	if (pop.size() == 0)
		throw AxiomLibException("RecognizerExtend::run - population of zero size.");
	// подготовка внутренних переменных
	bool bres;
	int first, second, tmpFirst, tmpSecond;
	double tmpGoal;
	std::vector <double> row;
	std::vector <std::vector <int> > markUps;
	std::vector <int> corrMarkUp;
	// std::vector <double> corrMarkUpDouble;
	std::vector < std::vector <RefMarkupSet> > refs; // вектор разметок - по каждому из параметров предоставляет доступ к набору эталонных разметок
	int numOfTests = 0;
	std::vector<int> numOfTS;
	// запрос числа тестовых рядов
	bool c = dataSet.getTestSize (numOfTests, numOfTS);
	// соответствующее изменение размерностей используемых векторов для хранения разметок по определенной системе каксиом
	markUps.resize(params.size());
	refs.resize(params.size());
	// ходим по всем системам аксиом в популяции

	//#pragma omp parallel for schedule(dynamic, 1)
	for (int asNum = 0; asNum < pop.size(); asNum++) {
		if (debug) std::cout << "\n		As open " << asNum+1 << "\n";
		// Теперь для каждой системы аксиом, мы:
		// 1. размечаем ряды из dataset-а этой системой аксиом:
		//      - получаем reference markups по всем параметрам
		//      - получаем test markups по всем параметрам
		// 2. запускаем поиск ошибок первого и второго рода на разметках test markups с помощью reference markup
		first = 0;
		second = 0;
		if (outputToFile) clearPath (pop.name(asNum), asNum);

		// Получаем разметки всех траекторий аномального поведения
		if (debug) std::cout << "	InitRefs";
		for (int prm = 0; prm < (int) params.size(); prm++) {
			initRefs (pop, asNum, params[prm], refs[prm]);
			if (outputToFile) saveRefs (refs[prm], dataSet.paramName(params[prm]));
		}

		// Выбираем очередной тест и начинаем его обработку
		for (int t = 0; t < numOfTests; t++) {
			if (debug) std::cout << "			Test	" << t+1;
			if (outputToFile) genFileName (t);
			corrMarkUp.clear();
			dataSet.getTSByIndexFromTests (corrMarkUp, t, -1);
			for (int prm = 0; prm < (int) params.size(); prm++) {
				(markUps[prm]).clear();
				bres = dataSet.getTSByIndexFromTests (row, t, params[prm]);
				if (bres) {
					if (debug) std::cout << "	Entering";
					// разметка системой аксиом asNum одного из рядов относящихся к данному параметру
					pop.enter (markUps[prm], row, 0, (unsigned long) row.size(), asNum);
					// сохранение разметки в файл, если это необходимо
					if (outputToFile) saveRows (row, markUps[prm], dataSet.paramName(params[prm]));
				}
				else {
					throw AxiomLibException("Error in RecognizerExtend::run: some params are missing in Tests in DataSets.");
				}
			}

			// запуск распознователя и подсчета статисткики для данной системы аксиом по данному тесту
			if (debug) std::cout << "	Recognition";
			recognition (tmpFirst, tmpSecond, markUps, corrMarkUp, refs, t);
			if (debug) std::cout << "	Done.\n";
			first += tmpFirst;
			second += tmpSecond;
		}

		// статистика по данной системе аксиом подсчитана - забиваем ее в соотвествующие поля класса данной системы аксиом
		pop.getStats (asNum, tmpFirst, tmpSecond, tmpGoal);
		pop.setStats (asNum, first, second, tmpGoal);
		if (debug) std::cout << "\n		As closed";
	}
	return 0;
}


/****************************************************************************
*				RecognizerExtend::run
*
*	Description:	Запуск процесса распознавания
*	Parameters:	AxiomSetPop& pop - популяция, с которой работаем
*	Returns:		0
*	Throws:		AxiomLibExeption - если популяция нулевого размера, или не инициализирована, или залочена
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int RecognizerExtend::run (AxiomSetPop& pop, std::vector <double> &precs) {
	const bool debug = false;
	if (debug) std::cout << "\n	RecognizerExtend:";
	// проверка; в случае лока или не инициализированной популяции, AxiomSetPop кинет exception
	if (pop.size() == 0)
		throw AxiomLibException("RecognizerExtend::run - population of zero size.");
	// подготовка внутренних переменных
	bool bres;
	int first, second, tmpFirst, tmpSecond;
	double tmpGoal;
	std::vector <double> row;
	std::vector <std::vector <int> > markUps;
	std::vector <int> corrMarkUp;
	std::vector < std::vector <RefMarkupSet> > refs; // вектор разметок - по каждому из параметров предоставляет доступ к набору эталонных разметок
	int numOfTests = 0;
	std::vector<int> numOfTS;
	// запрос числа тестовых рядов
	bool c = dataSet.getTestSize (numOfTests, numOfTS);
	// соответствующее изменение размерностей используемых векторов для хранения разметок по определенной системе каксиом
	markUps.resize(params.size());
	refs.resize(params.size());
	// ходим по всем системам аксиом в популяции
	//#pragma omp parallel for schedule(dynamic, 1)
	for (int asNum = 0; asNum < pop.size(); asNum++) {
		//if (debug) cout << "\n	process  " << omp_get_num_threads() << endl;
		if (debug) std::cout << "\n		As open " << asNum+1 << "\n";
		// Теперь для каждой системы аксиом, мы:
		// 1. размечаем ряды из dataset-а этой системой аксиом:
		//      - получаем reference markups по всем параметрам
		//      - получаем test markups по всем параметрам
		// 2. запускаем поиск ошибок первого и второго рода на разметках test markups с помощью reference markup
		precision = precs[asNum];
		first = 0;
		second = 0;
		if (outputToFile) clearPath (pop.name(asNum), asNum);

		// Получаем разметки всех траекторий аномального поведения
		if (debug) std::cout << "	InitRefs";
		for (int prm = 0; prm < (int) params.size(); prm++) {
			initRefs (pop, asNum, params[prm], refs[prm]);
			if (outputToFile) saveRefs (refs[prm], dataSet.paramName(params[prm]));
		}

		for (int t = 0; t < numOfTests; t++) {
			if (debug) std::cout << "			Test	" << t+1;
			corrMarkUp.clear();
			dataSet.getTSByIndexFromTests (corrMarkUp, t, -1);
			if (outputToFile) genFileName (t);
			for (int prm = 0; prm < (int) params.size(); prm++) {
				(markUps[prm]).clear();
				bres = dataSet.getTSByIndexFromTests (row, t, params[prm]);
				if (bres) {
					if (debug) std::cout << "	Entering";
					// разметка системой аксиом asNum одного из рядов относящихся к данному параметру
					pop.enter (markUps[prm], row, 0, (unsigned long) row.size(), asNum);
					// сохранение разметки в файл, если это необходимо
					if (outputToFile) saveRows (row, markUps[prm], dataSet.paramName(params[prm]));					
				}
				else {
					throw AxiomLibException("Error in RecognizerExtend::run: some params are missing in Tests in DataSets.");
				}
			}
			// запуск распознователя и подсчета статисткики для данной системы аксиом по данному тесту
			if (debug) std::cout << "	Recognition";
			recognition (tmpFirst, tmpSecond, markUps, corrMarkUp, refs, t);
			if (debug) std::cout << "	Done.\n";
			first += tmpFirst;
			second += tmpSecond;
		}
		// статистика по данной системе аксиом подсчитана - забиваем ее в соотвествующие поля класса данной системы аксиом
		pop.getStats (asNum, tmpFirst, tmpSecond, tmpGoal);
		pop.setStats (asNum, first, second, tmpGoal);
		if (debug) std::cout << "\n		As closed";
	}
	return 0;
}


/****************************************************************************
*					RecognizerExtend::run
*
*	Description:	Прогон распознавания по заданному набору данных и системе аксиом
*	Parameters:		axiomSet - система аксиом, которой осуществляется разметка
*							временных рядов, для распознавания
*					dataSet - набор данных, на котором производится прогон системы аксиом
*					params - параметры набора данных, по которым ведется анализ
*					resFirst - число ошибок первого рода
*					resSecond - число ошибок второго рода
*					asIndex - индекс, под которым сохранять систему аксиом, если это необходимо
*	Returns:		0
*	Throws:			AxiomLibExeption - если популяция нулевого размера, или не инициализирована, или залочена
*	Author:			dk
*	History:
*
****************************************************************************/
signed int RecognizerExtend::run (AxiomSet& axiomSet, DataSet& extDataSet, std::vector<int>& extParams, int& resFirst, int& resSecond, const int asIndex) {
	std::vector<double> stat;
	return run (axiomSet, extDataSet, extParams, resFirst, resSecond, stat, asIndex);
}


/****************************************************************************
*					RecognizerExtend::run
*
*	Description:	Прогон распознавания по заданному набору данных и системе аксиом
*	Parameters:		axiomSet - система аксиом, которой осуществляется разметка
*							временных рядов, для распознавания
*					dataSet - набор данных, на котором производится прогон системы аксиом
*					params - параметры набора данных, по которым ведется анализ
*					resFirst - число ошибок первого рода
*					resSecond - число ошибок второго рода
*					stat - статистика по каждой из аксиом системы, которая показывает используется ли
*					аксиома а разметке эталонных траекторий и разметке основных рядов
*					asIndex - индекс, под которым сохранять систему аксиом, если это необходимо
*	Returns:		0
*	Throws:			AxiomLibExeption - если популяция нулевого размера, или не инициализирована, или залочена
*	Author:			dk
*	History:
*
****************************************************************************/
signed int RecognizerExtend::run (AxiomSet& axiomSet, DataSet& extDataSet, std::vector<int>& extParams, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex) {
	const bool debug = false;
	if (debug) std::cout << "\n	RecognizerExtend: run started\n";

	// подготовка внутренних переменных
	bool bres; // Локальная переменная, используемая в цикле
	int tmpFirst, tmpSecond; // Переменные для суммирования статистики
	std::vector <double> row; // Сюда будут вытягиваться ряды из extDataSet'a
	std::vector <std::vector <int> > markUps; // Здесь будут лежать разметки всех размерностей
	std::vector <int> corrMarkUp; // Здесь будет лежать корректная раметка текущего ряда
	std::vector < std::vector <RefMarkupSet> > refs; // вектор разметок - по каждому из параметров предоставляет доступ к набору эталонных разметок
	std::vector <bool> curStat; // вектор для получения резульатов об используемых аксимах в разметке
	std::vector <bool> refStat; // вектор для накопления результатов об используемых аксиомах в разметках эталонных траекторий
	std::vector <bool> rowStat; // вектор для накопления результатов об используемых аксиомах в разметках тестовых рядов
	// Запрос числа тестовых рядов в extDataSet'e
	int numOfTests = 0;
	std::vector<int> numOfTS;
	bool c = extDataSet.getTestSize (numOfTests, numOfTS);
	// Соответствующее изменение размерностей используемых векторов для хранения разметок по определенной системе каксиом
	markUps.resize(extParams.size());
	refs.resize(extParams.size());

	// Обнуляем показатели статистики по данной системе аксиом на данных extDataSet
	resFirst = 0;
	resSecond = 0;
	// Обнуляем показатели статистики: сколько должно быть найдено траекторий
	int resShouldBe = 0;
	int tmpShouldBe = 0;

	if (outputToFile) clearPath (axiomSet.name(), asIndex);
	// Получаем разметки всех траекторий аномального поведения
	if (debug) std::cout << "	InitRefs";
	for (int prm = 0; prm < (int) extParams.size(); prm++) {
		initRefs (axiomSet, extParams[prm], refs[prm], refStat);
		if (outputToFile) saveRefs (refs[prm], extDataSet.paramName(extParams[prm]));
	}
	
	// Теперь для каждой системы аксиом, мы:
	// 1. размечаем ряды из extDataSet-а этой системой аксиом:
	//      - получаем test markups по всем параметрам
	// 2. запускаем поиск ошибок первого и второго рода на разметках test markups с помощью reference markup
	
	// указание распараллеливать цикл вычислений
	// omp_set_dynamic(1);
	// #pragma omp parallel for schedule(dynamic, 1)
	// for ...
	//cout << "\n	process  " << omp_get_num_threads() << endl;

	// Цикл по всем тестам из extDataSet'a
	for (int t = 0; t < numOfTests; t++) {
		if (debug) std::cout << "			Test	" << t+1;
		if (outputToFile) genFileName (t);
		corrMarkUp.clear();
		// Вытаскиваем корректную разметку данного теста
		extDataSet.getTSByIndexFromTests (corrMarkUp, t, -1);
		// Цикл по всем размерностям текущего выбранного теста из extDataSet'a
		for (int prm = 0; prm < (int) extParams.size(); prm++) {
			(markUps[prm]).clear();
			// Вытягиваем одномерный ряд как часть составного ряда
			// Результат указывает нашелся ли такой ряд.
			bres = extDataSet.getTSByIndexFromTests (row, t, extParams[prm]);
			if (bres) {
				if (debug) std::cout << "	Entering";
				// Разметка системой аксиом asNum одного из рядов относящихся к данному параметру
				axiomSet.enter (markUps[prm], row, 0, (unsigned long) row.size(), curStat);
				updateVec (rowStat, curStat);
				// Сохранение полученных разметок в файл
				if (outputToFile) saveRows (row, markUps[prm], dataSet.paramName(params[prm]));
			}
			else {
				throw AxiomLibException("Error in RecognizerExtend::run: some params are missing in Tests in DataSets.");
			}
		}
		// запуск распознователя и подсчета статисткики для данной системы аксиом по данному тесту
		if (debug) std::cout << "	Recognition";
		recognition (tmpFirst, tmpSecond, markUps, corrMarkUp, refs, t);
		if (debug) std::cout << "	Done.\n";
		resFirst += tmpFirst;
		resSecond += tmpSecond;
		// получение статистики по данному тесту
		analizeTest (corrMarkUp, tmpShouldBe);
		resShouldBe += tmpShouldBe;
	}

	// Подсчет статистики
	countStat (stat, resFirst, resSecond, resShouldBe, refStat, rowStat);

	if (debug) std::cout << "	Overall number of trajectories is " << resShouldBe << "\n";
		
	return 0;
}


/****************************************************************************
*					RecognizerExtend::saveRows
*
*	Description:	Функция сохранения ряда и его разметки в файл
*	Parameters:		row - сохраняемый вектор временного ряда
*					markUp - разметка вектора row системой аксиом asName
*					paramName - название размерности вектора row в оригинале
*					testNum - номер теста в используемом наборе данных
*					asName - имя системы аксиом, разметившей ряд row в разметку markUp
*					asIndex - индекс системы аксиом в популяции (если используется популяция систем аксиом)
*	Returns:		0 - если сохранение произошло
*					< 0  - иначе
*	Throws:			AxiomLibException - если входные параметры разной длины
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerExtend::saveRows (std::vector <double> &row, std::vector<int> &markUp, std::string paramName) const {
	if (markUp.size() != row.size())
		throw AxiomLibException("Error in RecognizerExtend::clearPath: given current dir - is unreacheable.");
	boost::filesystem::path tmpPath( curFile );
	std::string fileName;
	fileName = curFile;
	fileName.append("^");
	boost::filesystem::path secPath( fileName );
	bool secondHand = false;
	if ( boost::filesystem::exists(tmpPath) ) {
		if ( boost::filesystem::exists(secPath) )
			boost::filesystem::remove (secPath);
		boost::filesystem::rename (tmpPath, secPath);
		secondHand = true;
	}
	
	// вывод информации в файл
	if (secondHand) {
		std::string str;
		boost::filesystem::ifstream inFile( secPath );
		ofstream file (curFile.c_str());
		if (!inFile.eof()) {
			std::getline(inFile, str);
			file << str;
		}
		file << paramName << ";";
		file << paramName.append("_markUp") << ";\n";
		for (unsigned int i = 0; i < row.size(); i++) {
			if (!inFile.eof()) {
				std::getline(inFile, str);
				file << str;
			}
			file << "\t" << row[i] << ";\t" << markUp[i] << ";\n";
		}
		file.close();
		inFile.close();
	} else {
		ofstream file (curFile.c_str());
		file << paramName << ";";
		file << paramName.append("_markUp") << ";\n";
		for (unsigned int i = 0; i < row.size(); i++) {
			file << "\t" << row[i] << ";\t" << markUp[i] << ";\n";
		}
		file.close();
	}

	if (secondHand)
		boost::filesystem::remove (secPath);
	return 0;
}


/****************************************************************************
*					RecognizerExtend::genFileName
*
*	Description:	Функция создает имя файла для текущей записи и сохраняет его во внутренней переменной класса
*	Parameters:		testNum - номер теста в используемом наборе данных
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerExtend::genFileName (int testNum) {
	curFile = curDir;
	char tmpStr[10];
	sprintf(tmpStr,"%d",testNum + 1);
	curFile.append("/");
	curFile.append(tmpStr);
	curFile.append(".csv");
	return 0;
}


/****************************************************************************
*					RecognizerExtend::clearPath
*
*	Description:	Функция создает каталог по заданным имени и индексу и очищает его содержимое
*	Parameters:		asName - имя системы аксиом, разметившей ряд row в разметку markUp
*					asIndex - индекс системы аксиом в популяции (если используется популяция систем аксиом)
*	Returns:		0 - если сохранение произошло
*					< 0  - иначе
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerExtend::clearPath (std::string asName, int asIndex) {
	std::string pathToDir;
	// Генерация пути к директории
	pathToDir = whereToSave;
	if (whereToSave.find_last_of("/\\") != (whereToSave.size() - 1))
		pathToDir.append ("/");
	pathToDir.append (saveDirName);
	if (asIndex < 0)
		asIndex = searchFreeIndex (pathToDir);
	char tmpNum[10];
	sprintf(tmpNum,"%d",asIndex + 1);
	pathToDir.append (tmpNum);
	//pathToDir.append (asName);
	//pathToDir.append ("_");	
	//pathToDir.append (tmpNum);
	// Создание директории если это необходимо и удаление ее содержимого
	boost::filesystem::path tmpPath( whereToSave );
	if ( !boost::filesystem::exists(tmpPath) )
		throw AxiomLibException("Error in RecognizerExtend::clearPath: given current dir - is unreacheable.");
	boost::filesystem::path dirPath( pathToDir );
	if ( boost::filesystem::exists(dirPath) )
		boost::filesystem::remove_all (dirPath);
	boost::filesystem::create_directory (dirPath);
	curDir = pathToDir;
	return 0;
}


/****************************************************************************
*					RecognizerExtend::searchFreeIndex
*
*	Description:	Функция находит индекс, следующий за которым такой, что будучи дополненным в виде 
*					суффикса к curPath даст название директории, которой еще не существует.
*	Parameters:		curPath - префикс директории
*	Returns:		int - значение суффикса, предшествующего суффиксу для curPath, вместе с которым 
*						образуется имя директории, еще не существующей в каталоге.
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerExtend::searchFreeIndex (const std::string &curPath) const {
	std::string path; 
	char tmss[20];
	int i = 1;
	for (i = 1; i < INT_MAX; i++) {
		path = curPath;
		tmss[0]='\0';
		sprintf(tmss,"%d",i);
		path.append (tmss);
		boost::filesystem::path tmpPath (path);
		if (!boost::filesystem::exists(tmpPath))
			break;
		if (!boost::filesystem::is_directory(tmpPath))
			break;
	}
	return (i - 1);
}


/****************************************************************************
*					RecognizerExtend::updateRows
*
*	Description:	Функция сохранения ряда и его разметки в файл
*	Parameters:		corrMarkUp - верная исходная разметка ряда нештатными ситуациями
*					result - полученная разметка нештатными ситуациями
*	Returns:		0 - если сохранение произошло
*					< 0  - иначе
*	Throws:			AxiomLibException - если входные параметры разной длины
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerExtend::updateRows (std::vector <int> &corrMarkUp, std::vector<int> &result) const {
	if (corrMarkUp.size() != result.size())
		throw AxiomLibException("Error in RecognizerExtend::clearPath: given current dir - is unreacheable.");
	boost::filesystem::path tmpPath( curFile );
	std::string fileName;
	fileName = curFile;
	fileName.append("^");
	boost::filesystem::path secPath( fileName );
	bool secondHand = false;
	if ( boost::filesystem::exists(tmpPath) ) {
		if ( boost::filesystem::exists(secPath) )
			boost::filesystem::remove (secPath);
		boost::filesystem::rename (tmpPath, secPath);
		secondHand = true;
	}
	
	// вывод информации в файл
	if (secondHand) {
		std::string str;
		boost::filesystem::ifstream inFile( secPath );
		ofstream file (curFile.c_str());
		if (!inFile.eof()) {
			std::getline(inFile, str);
			file << str;
		}
		file << "correctAbnormBehavior" << ";" << "axiomSetResults" << ";\n";
		for (unsigned int i = 0; i < corrMarkUp.size(); i++) {
			if (!inFile.eof()) {
				std::getline(inFile, str);
				file << str;
			}
			file << "\t" << corrMarkUp[i] << ";\t" << result[i] << ";\n";
		}
		file.close();
		inFile.close();
	} else {
		ofstream file (curFile.c_str());
		file << "correctAbnormBehavior" << ";" << "axiomSetResults" << ";\n";
		for (unsigned int i = 0; i < corrMarkUp.size(); i++) {
			file << "\t" << corrMarkUp[i] << ";\t" << result[i] << ";\n";
		}
		file.close();
	}

	if (secondHand)
		boost::filesystem::remove (secPath);
	return 0;
}


/****************************************************************************
*					RecognizerExtend::saveRefs
*
*	Description:	Функция сохранения разметок траекторий аномального поведения в файлы
*	Parameters:		ref - вектор разметок различных типов аномального поведения
*	Returns:		0 - если сохранение произошло
*					< 0  - иначе
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerExtend::saveRefs (std::vector <RefMarkupSet> &ref, std::string paramName) const {
	// Проверяем -существует ли директория, в которой сохранять файлы
	boost::filesystem::path dirPath( curDir );
	if ( !boost::filesystem::exists(dirPath) )
		return -1;
	// Создаем необходимые переменные
	std::string fileName, secName;
	bool secondHand;
	// Сохраняем содержимое ref в файле с адресом fileName
	std::vector < std::vector <int> > axioms, axiomMinLens, axiomMaxLens;
	for (unsigned int i = 0; i < ref.size(); i++) { // цикл по различным типам аномального поведения
		// Собираем информацию - что выводить в файл
		axioms.resize (ref[i].refMarkups.size());
		axiomMinLens.resize (ref[i].refMarkups.size());
		axiomMaxLens.resize (ref[i].refMarkups.size());
		for (unsigned int j = 0; j < ref[i].refMarkups.size(); j++) {
			ref[i].refMarkups[j].getCopy(axioms[j], axiomMinLens[j], axiomMaxLens[j]);
		}
		// Создаем путь к файлу
		fileName = curDir;
		fileName.append("/mu_");
		fileName.append(dataSet.className (i));
		fileName.append(".csv");
		secName = fileName;
		secName.append("^");
		boost::filesystem::path secPath( secName );
		secondHand = false;
		// Если файл с таким именем уже существует, то удаляем его
		boost::filesystem::path filePath( fileName );		
		if ( boost::filesystem::exists(filePath) ) {
			if ( boost::filesystem::exists(secPath) )
				boost::filesystem::remove (secPath);
			boost::filesystem::rename (filePath, secPath);
			secondHand = true;
		}
		// вывод информации в файл
		if (secondHand) {
			std::string str;
			boost::filesystem::ifstream inFile( secPath );
			ofstream file (fileName.c_str());
			if (!inFile.eof()) {
				std::getline(inFile, str);
				file << str;
			}
			// Заголовок
			unsigned int mLen = 0;
			for (unsigned int l = 0; l < axioms.size(); l++) {
				// пишем заголовок
				file << "axioms[MinLen,MaxLen]";
				if (l > 0) {
					file << "_" << l;
				}
				file <<";";
				// вычисляем максимальную длину вектора
				if (axioms[l].size() > mLen)
					mLen = axioms[l].size();
				// проводим проверку корректоности полученных данных
				if ( (axioms[l].size() != axiomMinLens[l].size()) || (axioms[l].size() != axiomMaxLens[l].size()) )
					throw AxiomLibException("Error in RecognizerExtend::saveRefs: incorrect function response in RefMarkUp.");
			}
			file << "\n";
			// Содержимое разметок
			for (unsigned int k = 0; k < mLen; k++) {
				if (!inFile.eof()) {
					std::getline(inFile, str);
					file << str;
				}
				for (unsigned int l = 0; l < axioms.size(); l++) {
					if (k < axioms[l].size()) {
						file << "\t" << axioms[l][k] << "[" << axiomMinLens[l][k] << "," << axiomMaxLens[l][k] << "];";
					} else {
						file << "\t\t;";
					}
				}
				file << "\n";
			}
			file.close();
			inFile.close();
		} else {
			ofstream file (fileName.c_str());
			// Заголовок
			unsigned int mLen = 0;
			for (unsigned int l = 0; l < axioms.size(); l++) {
				// пишем заголовок
				file << "axioms[MinLen,MaxLen]";
				if (l > 0) {
					file << "_" << l;
				}
				file <<";";
				// вычисляем максимальную длину вектора
				if (axioms[l].size() > mLen)
					mLen = axioms[l].size();
				// проводим проверку корректоности полученных данных
				if ( (axioms[l].size() != axiomMinLens[l].size()) || (axioms[l].size() != axiomMaxLens[l].size()) )
					throw AxiomLibException("Error in RecognizerExtend::saveRefs: incorrect function response in RefMarkUp.");
			}
			file << "\n";
			// Содержимое разметок
			for (unsigned int k = 0; k < mLen; k++) {
				for (unsigned int l = 0; l < axioms.size(); l++) {
					if (k < axioms[l].size()) {
						file << "\t" << axioms[l][k] << "[" << axiomMinLens[l][k] << "," << axiomMaxLens[l][k] << "];";
					} else {
						file << "\t\t;";
					}
				}
				file << "\n";
			}
			file.close();
		}
		if (secondHand)
			boost::filesystem::remove (secPath);
	}
	return 0;
}
