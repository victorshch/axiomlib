/****************************************************************************
*
*				Функции класса FuzzyDataLearnAlgorithm
*
****************************************************************************/
#include "FuzzyDataLearnAlgorithm.h"
#include "Defines.h"

using namespace AxiomLib;

#define numBestECs 10

#define eps 0.001

// Коструктор класса
FuzzyDataLearnAlgorithm::FuzzyDataLearnAlgorithm (void) {
	// Определяем по умолчанию не выводить никаких комментариев
	comments = false;
	compareStatistic = NULL;
	goalStrategy = NULL;

	dataSetParams.clear();
	axiomSetBaseDir.clear();
	axiomName.clear();
	stage = 0;
	costEthIn = 1.0;
	costEthOut = 1.0;
	costNormalIn = 1.0;
	costNormalOut = 1.0;

	costStrongPoint = 1.0;
	costMaxMinLenRatio = 1.0;
	costLenRatio = 1.0;
	costHiAreasNum = 1.0;
	hiAreasNumEffect = 1.0;
	shiftErrIn = 10.0;
	extendMarkUpLensLevel = 0.0;

	maxAxiomPopSize = 30;
	maxIterNum = 100;
	maxAxiomRes = 1.0;
	percentBestAxioms = 1.0;
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
FuzzyDataLearnAlgorithm::~FuzzyDataLearnAlgorithm (void) {
	if (compareStatistic != NULL) delete compareStatistic;
	if (goalStrategy != NULL) delete goalStrategy;
}


/****************************************************************************
*				FuzzyDataLearnAlgorithm::initFromEnv
*
*	Description:	Функция устанавливает параметры объекта по данным из 
*					окружения, которое подается как параметр функции
*	Parameters:	env - объект класса окружения, откуда берутся параметры данного класса
*	Returns:	0
*	Throws:		AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:		dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::initFromEnv (const Environment& env) {
	// Инициализируем dataSet
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : data set is undefined.");
	// считываем необходимые для данного класса параметры о наборе данных
	EnvDataSet envDataSet;
	envDataSet.readConfigFile (datasetDir, datasetName);
	// установка корректного обозначения NullStr - обозначение остутсвия в данной точке ряда какого либо нештатного поведения
	fuzzyDataSet.setNullStr (envDataSet);
	fuzzyDataSet.setNormalStr (envDataSet);
	// собственно считываем набор данных - заводим его во внутреннее представление
	fuzzyDataSet.readDataSet(datasetDir, datasetName);
	// восстанавливаем в данном классе вектор индексов параметров в каноническом представленнии по которым вести поиск нештатых ситуаций
	fuzzyDataSet.getParamNums(dataSetParams, env, envDataSet);

	// Параметры, по которым сохранять лучщие решения
	if (env.getStringParamValue(axiomSetBaseDir, "AxiomSetBaseDir") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : axiomSetBaseDir directory is undefined.");
	if (env.getStringParamValue(axiomName, "saveTo") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : axiomName to save is undefined.");

	// создание класса подсчета статистики
	std::string compClassName;
	CompareClassFactory ccf;
	if (env.getStringParamValue(compClassName, "compareClass") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::initFromEnv: compare-class is undefined.");
	compareStatistic = ccf.create(compClassName);
	compareStatistic->setParamsFromEnv(env);

	// Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") < 0) 
		throw AxiomLibException("FuzzyDataLearnAlgorithm::initFromEnv: goal-class is undefined.");
	goalStrategy = gsf.create(goalStrategyName);
	goalStrategy->setParamsFromEnv(env);

	// Параметры алгоритма настройки элементарных условий
	if (env.getIntParamValue (leftLimit, "leftLimit") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : leftLimit is undefined.");
	if (env.getIntParamValue (rightLimit, "rightLimit") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : rightLimit is undefined.");
	if (env.getDoubleParamValue (reserve, "reserve") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : reserve is undefined.");
	if (env.getIntParamValue (numOfLevels, "numOfLevels") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : numOfLevels is undefined.");

	// Стадия процесса построения системы аксиом и поиска разметок изначально 0 (поиск не начинался)
	stage = 0;

	// Стоимость выполнения элементарного условия на участке, содержащем нештатное поведение
	if (env.getDoubleParamValue (costEthIn, "costEthIn") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costEthIn is undefined.");
	// Стоимость невыполнения элементарного условия на участке, содержащем нештатное поведение
	if (env.getDoubleParamValue (costEthOut, "costEthOut") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costEthOut is undefined.");
	// Стоимость выполнения элементарного условия на участке нормального поведения
	if (env.getDoubleParamValue (costNormalIn, "costNormalIn") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costNormalIn is undefined.");
	// Стоимость невыполнения элементарного условия на участке нормального поведения
	if (env.getDoubleParamValue (costNormalOut, "costNormalOut") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costNormalOut is undefined.");

	// Параметры поиска аксиом
	if (env.getIntParamValue (maxAxiomPopSize, "maxAxiomPopSize") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : maxAxiomPopSize is undefined.");
	if (env.getIntParamValue (maxIterNum, "axiomAlgIterNum") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : axiomAlgIterNum is undefined.");
	if (env.getDoubleParamValue (maxAxiomRes, "maxCostForAxiom") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : maxCostForAxiom is undefined.");
	if (env.getDoubleParamValue (percentBestAxioms, "percentBestAxioms") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : percentBestAxioms is undefined.");

	// Стоимость для оценивания вероятности существования опорных точек
	if (env.getDoubleParamValue (costStrongPoint, "costStrongPoint") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costStrongPoint is undefined.");
	// Стоимость близости отношения минимальной длине к максимальной для участков где аксиома выполняется на тестовой выборке
	if (env.getDoubleParamValue (costMaxMinLenRatio, "costMaxMinLenRatio") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costMaxMinLenRatio is undefined.");
	// Стоимость близости к 0 отношения числа точек, где аксиома срабатывает к общему число на тестовой выборке.
	if (env.getDoubleParamValue (costLenRatio, "costLenRatio") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costLenRatio is undefined.");
	// Стоимость отсутствия областей срабатывания аксиомы на траекториях нормального поведения
	if (env.getDoubleParamValue (costHiAreasNum, "costNormalInWhole") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : costNormalInWhole is undefined.");
	// Гибкость, с которой учитывать costHiAreasNum
	if (env.getDoubleParamValue (hiAreasNumEffect, "normalInWholeEffect") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : normalInWholeEffect is undefined.");

	// Стоимость ошибки, если при сдвиге разметок осталась область разметки без соответствия в другой разметке
	if (env.getDoubleParamValue (shiftErrIn, "shiftErrIn") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : shiftErrIn is undefined.");

	// Степень расширения длины вхождения разметки (при поиске разметок во время их подбора) 
	if (env.getDoubleParamValue (extendMarkUpLensLevel, "extendMarkUpLensLevel") < 0)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setParamsFromEnv : extendMarkUpLensLevel is undefined.");

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::setComments
*
*	Description:	Устанавливает новое значение переменной класса comments
*	Parameters:		newVal - новое значение переменной comments
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::setComments (const bool newVal) {
	this->comments = newVal;
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::run
*
*	Description:	Основная функция в классе - реализует алгоритм поиска
*					системы аксиом и разметки траекторий аномального поведения.
*	Parameters:		void
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::run (void) {
	// ВАЖНО!!! : проверяем, что в конфиге указан один анализируемый параметр, так как алгоритм построен для одномерных временных рядов
	if (dataSetParams.size() != 1)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::run : algorithm is not designed to work with multiple number of params.");
	int param = dataSetParams[0];

	// Начальная стадия процесса построения системы аксиом и поиска разметок
	stage = 0;

	// 1. подбор параметров элементарных условий
	selectElemCond (param);
	// Завершен поиск элементарных условий
	stage = 1;

	// 2. создание набора аксиом из элементрных условий
	formAxioms (param);
	// Завершен поиск аксиом
	stage = 2;

	// 3. формирование системы аксиом из построенных аксиом
	createAxiomSet (param);
	// Завершено построение системы аксиом
	stage = 3;

	// 4. сохранение результатов в файл
	//axiomSet->saveAxiomSetToFile(axiomSetBaseDir, axiomName);
	// Найденная система аксиом сохранена в файл
	stage = 4;
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::storeBestECs
*
*	Description:	Функция сохранения лучших элементарных условий и соответствующих 
*					значений целевой функции
*	Parameters:		ec - сохраняемое элементарное условие
*					curFun - значение целевой функции
*					bestECs - вектор лучших условий того же типа
*					bestECres - вектор лучших значений целевой функции
*	Returns:		1 - если условие сохранено
*					0 - если условие не входит в список лучших 
*	Throws:			AxiomLibException - если входные вектора не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::storeBestECs (ElemCondPlus &ec, double &curFun, std::vector <ElemCondPlus> &bestECs, std::vector <double> &bestECres) {
	// Проверка входных параметров
	if (bestECs.size() != bestECres.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::storeBestECs : incorrect input parameters.");
	// Проверка - входит ли данное ЭУ в список лучших за историю
	int whereToSave = -1;
	for (int i = 0; i < (int) bestECs.size(); i++) {
		if (bestECs[i].elemCondition == NULL) {
			bestECs[i] = ec;
			bestECres[i] = curFun;
			return 1;
		} else {
			if (bestECres[i] < curFun) {
				whereToSave = i;
				break;
			}
		}
	}
	// Если условие не среди лучших - выходим с 0
	if (whereToSave < 0)
		return 0;
	// Сдвигаем старые ЭУ, чтобы они были выстроены по порядку значения целевой функции
	for (unsigned int i = bestECs.size() - 1; i > (unsigned int) whereToSave; i--) {
		bestECs[i] = bestECs[i-1];
		bestECres[i] = bestECres[i-1];
	}
	// Сохраняем ЭУ
	bestECs[whereToSave] = ec;
	bestECres[whereToSave] = curFun;
	return 1;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::numOfCarriedOutItems
*
*	Description:	Подсчет числа точек в ряду, на которых элементарное условие выполняется
*	Parameters:		ec - проверяемое элементарное условие
*					row - ряд для порверки элементарного условия
*	Returns:		int - число точек, на которых элементарное условие выполнено
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::numOfCarriedOutItems (ElemCondPlus &ec, std::vector <double> &row) const {
	int count = 0;
	for (unsigned long i = 0; i < row.size(); i++) {
		if (ec.check (i, row) > 0)
			count++;
	}
	return count;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::matterECFunc
*
*	Description:	Подсчет целевой функции для элементарного условия
*	Parameters:		ec - Элементарное условие
*					param - параметр набора данных, на котором проверяется элементароное условие
*	Returns:		double - значение целевой функции
*	Throws:			AxiomLibException - если функция получает несогласованные параметры от набора данных
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyDataLearnAlgorithm::matterECFunc (ElemCondPlus &ec, const int param) {
	// Объявленеи необходимых переменных
	double toReturn = 0.0;
	int numOfClasses, numOfNormalMultiTS;
	std::vector <int> numOfMultiTS, numOfNormalTS;
	std::vector < std::vector <int> > numOfTS;
	std::vector <double> curTS;
	bool bres = false;
	double curItem, classCount;
	int curRes, ethNum, ethInc;
	// Получение информации о наборе данных
	fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	// Цикл по траекториям из обучающей выборки
	for (int i = 0; i < (int) numOfMultiTS.size(); i++) {
		if (numOfMultiTS[i] != numOfTS[i].size())
			throw AxiomLibException("FuzzyDataLearnAlgorithm::matterECFunc : incorrect response from internal function.");
		classCount = 0.0;
		ethNum = 0;
		ethInc = 0;
		for (int j = 0; j < (int) numOfTS[i].size(); j++) {
			curTS.clear();
			bres = fuzzyDataSet.getTSByIndexFromClass (curTS, i, j, param);
			if (bres) {
				curRes = numOfCarriedOutItems (ec, curTS);
				// classCount += (double) curRes;
				ethNum++;
				if (curRes > 0)
					ethInc++;
			}
		}
		if (ethInc > 0)
			toReturn += costEthIn * (double) ethInc / (double) ethNum;
		else
			toReturn -= costEthOut;
	}
	// Цикл по траекториям нормального поведения
	fuzzyDataSet.getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	if (numOfNormalMultiTS != numOfNormalTS.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::matterECFunc : incorrect response from internal function getNormalClassSize");
	classCount = 0.0;
	int sumLen = 0;
	for (int j = 0; j < numOfNormalMultiTS; j++) {
		curTS.clear();
		bres = fuzzyDataSet.getNormalTS (curTS, j, param);
		if (bres) {
			curItem = (double) numOfCarriedOutItems (ec, curTS);
			classCount += curItem;
			sumLen += curTS.size();
		}
	}
	if (classCount < eps)
		toReturn += costNormalOut;
	else
		toReturn -= costNormalIn * classCount / (double) sumLen;

	return toReturn;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::selectElemCond
*
*	Description:	Функция подбора и настройки элементарных условий.
*	Parameters:		void
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::selectElemCond (const int param) {
	ElemConditionsFactory ecf;
	// сначала забиваем вектор elemConds различными по типу элементарными условиями
	std::vector <ElemCondPlus> elemConds;
	elemConds.resize (2*N_EL_AXIOMS);
	for (unsigned int i = 0; i < N_EL_AXIOMS; i++) {
		elemConds[2*i].elemCondition = ecf.createAxiomByNum(i+1);
		elemConds[2*i].sign = true;
		elemConds[2*i+1].elemCondition = ecf.createAxiomByNum(i+1);
		elemConds[2*i+1].sign = false;
	}
	// Достаем необходимый для обучения ряд
	std::vector<double> teachRow;
	int numOfMultiTS;
	std::vector <int> numOfTS;
	fuzzyDataSet.getNormalClassSize (numOfMultiTS, numOfTS);
	if ((numOfTS.size() != numOfMultiTS) || (numOfMultiTS < 1))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::selectElemCond : not enough data for the algorithm.");
	for (int i = 0; i < numOfMultiTS; i++) {
		if (numOfTS[i] > param) {
			fuzzyDataSet.getNormalTS (teachRow, i, param);
			if (teachRow.size() > 0)
				break;
		}
	}
	if (teachRow.size() < 1)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::selectElemCond : incorrect normal data set.");
	// Данный участок ряда необходим чтобы собрать начальную статистику по максимальным и минимальным значениям для
	// некоторого набора переменных в элементарных условиях. Поэтому слишком большие объемы не нужны, можно ограничить 1000.
	if (teachRow.size() > 1000) {
		teachRow.resize (1000);
	}
	
	//double curFun;
	std::vector <double> curFun;
	curFun.resize (elemConds.size());
	bestECs.resize (elemConds.size());
	bestECres.resize (elemConds.size());
	// теперь в цикле обучаем каждое элементарное условие
	if (this->comments)
		std::cout << "\n\tTeaching elementary conditions: \n\n";
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < (int) elemConds.size(); i++) {
		if (this->comments) {
			printf ("\r\t Elementary condition: %d out of %d.", (i+1), elemConds.size());
			std::cout.flush();
		}
		bestECs[i].resize(numBestECs);
		bestECres[i].resize(numBestECs);
		elemConds[i].elemCondition->setLimits (leftLimit, rightLimit);
		elemConds[i].elemCondition->setLimits (teachRow, reserve, numOfLevels);
		elemConds[i].elemCondition->changeParams (0);
		curFun[i] = matterECFunc (elemConds[i], param);
		storeBestECs (elemConds[i], curFun[i], bestECs[i], bestECres[i]);
		while (elemConds[i].elemCondition->changeParams (1) >= 0) {
			curFun[i] = matterECFunc (elemConds[i], param);
			storeBestECs (elemConds[i], curFun[i], bestECs[i], bestECres[i]);
		}
	}
	if (this->comments) {
		std::cout << "\n\n\tChoosing parameters for elem conds done. First stage is over.\n";
		std::cout.flush();
	}

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::matterAxiomFunc
*
*	Description:	Подсчет целевой функции для аксиомы
*	Parameters:		ax - аксиома, для которой вычисляется значение целевой функции
*					param - параметр набора данных, на котором проверяется аксиома
*	Returns:		double - значение целевой функции
*	Throws:			AxiomLibException - если функция получает несогласованные параметры от набора данных
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyDataLearnAlgorithm::matterAxiomFunc (AxiomExpr &ax, const int param) {
	// Объявленеи необходимых переменных
	double toReturn = 0.0;
	int numOfClasses, numOfNormalMultiTS;
	std::vector <int> numOfMultiTS, numOfNormalTS;
	std::vector < std::vector <int> > numOfTS;
	std::vector <double> curTS;
	bool bres = false;
	double maxMinRelation, lenRelation;
	int ethNum, ethInc, len, num, minLen, maxLen, wholeNum, hiLen, sumLen;
	// Получение информации о наборе данных
	fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	if ((numOfClasses != numOfMultiTS.size()) || (numOfClasses != numOfTS.size()))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function.");
	// Цикл по классам траекторий нештатного поведения обучающей выборки
	for (int i = 0; i < numOfClasses; i++) {
		if (numOfMultiTS[i] != numOfTS[i].size())
			throw AxiomLibException("FuzzyDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function*.");
		ethNum = 0;
		ethInc = 0;
		wholeNum = 0;
		maxMinRelation = 1.0;
		lenRelation = 0.0;
		for (int j = 0; j < numOfMultiTS[i]; j++) {
			curTS.clear();
			bres = fuzzyDataSet.getTSByIndexFromClass (curTS, i, j, param);
			if (bres) {
				if (curTS.size() < 1)
					throw AxiomLibException("FuzzyDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal request to dataSet.");
				len = axiomMarkUpStat (ax, curTS, num, minLen, maxLen);
				wholeNum += num;
				if (num > 0) {
					maxMinRelation = min (maxMinRelation, (double) minLen / (double) maxLen);
				}
				lenRelation = max (lenRelation, (double) len / (double) curTS.size());
				ethNum++;
				if (len > 0)
					ethInc++;
			}
		}
		// Вклад от ethInc
		if (ethInc > 0)
			toReturn += costEthIn * (double) ethInc / (double) ethNum;
		else
			toReturn -= costEthOut;
		// Вклад от wholeNum
		toReturn += costStrongPoint * ((double) (wholeNum + ethNum - abs(wholeNum - ethNum)) / (double) (wholeNum + ethNum));
		// Вклад от maxMinRelation и lenRelation
		if (wholeNum > 0) {
			// Вклад от maxMinRelation
			toReturn += costMaxMinLenRatio * maxMinRelation;
			// Вклад от lenRelation
			toReturn += costLenRatio * pow4(1.0 - lenRelation);
		}
	}
	// Цикл по траекториям нормального поведения
	fuzzyDataSet.getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	if (numOfNormalMultiTS != numOfNormalTS.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function getNormalClassSize");
	sumLen = 0;
	hiLen = 0;
	wholeNum = 0;
	for (int j = 0; j < numOfNormalMultiTS; j++) {
		curTS.clear();
		bres = fuzzyDataSet.getNormalTS (curTS, j, param);
		if (bres) {
			if (curTS.size() < 1)
				throw AxiomLibException("FuzzyDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal request to dataSet*.");
			len = axiomMarkUpStat (ax, curTS, num, minLen, maxLen);
			wholeNum += num;
			hiLen += len;
			sumLen += curTS.size();
		}
	}
	// Сравнение числа вхождений с 0 - учет hiLen
	if (hiLen < 1)
		toReturn += costNormalOut;
	else
		toReturn -= costNormalIn * (double) hiLen / (double) sumLen;
	// Вклад wholeNum
	toReturn += costHiAreasNum * hiAreasNumEffect / (hiAreasNumEffect + (double) wholeNum);

	return toReturn;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::axiomMarkUpStat
*
*	Description:	Анализа разметки аксиомой axiomExpr ряда curTS и сбор статистики
*	Parameters:		axiomExpr - аксиома, для которой производится сбор статистики
*					row - временной ряд, на котором проверяется аксиома axiomExpr
*					hiAreasNum - число областей связности (таких где подряд выполняется аксиома axiomExpr) 
*					hiAreasMinLen - минимальная длина таких областей
*					hiAreasMaxLen - максимальная длина таких областей
*	Returns:		int - число точек ряда row, в которых аксиома axiomExpr выполняется
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::axiomMarkUpStat (AxiomExpr &axiomExpr, std::vector <double> &row, int &hiAreasNum, int &hiAreasMinLen, int &hiAreasMaxLen) const {
	bool pred = false;
	std::vector <long> hiLens;
	unsigned long predStart = 0;
	unsigned long wholeLen = 0;
	hiLens.clear();
	hiAreasNum = 0;
	for (unsigned long i = 0; i < row.size(); i++) {
		if (axiomExpr.check (i, row) > 0) {
			if (!pred) {
				pred = true;
				predStart = i;
			}
		} else {
			if (pred) {
				pred = false;
				hiLens.push_back ((long) (i - predStart));
				wholeLen += i - predStart;
			}
		}
	}
	if (pred) {
		hiLens.push_back ((long) (row.size() - predStart));
		wholeLen += row.size() - predStart;
	}
	hiAreasNum = hiLens.size();
	long curMin, curMax;
	if (hiAreasNum > 0) {
		curMin = hiLens[0];
		curMax = hiLens[0];
		for (unsigned int i = 1; i < hiLens.size(); i++) {
			if (hiLens[i] > curMax)
				curMax = hiLens[i];
			if (hiLens[i] < curMin)
				curMin = hiLens[i];
		}
		hiAreasMinLen = (int) curMin;
		hiAreasMaxLen = (int) curMax;
	} else {
		hiAreasMinLen = 0;
		hiAreasMaxLen = 0;
	}
	return (int) wholeLen;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::getPriority
*
*	Description:	Вспомогательная функция сортировки массива по убыванию 
*					с занесением результатов в отдельный вектор индексов.
*					Значения исходного сортируемого вектора не изменяются и не 
*					перемещаются.
*	Parameters:		vec - сортируемый вектор
*					pos - вектор индексов для указания результата
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::getPriority (std::vector <double> &vec, std::vector <unsigned int> &pos) const {
	pos.resize(vec.size()); 
	for (unsigned int i = 0; i < vec.size(); i++) 
		pos[i] = i;
	unsigned int temp;
	for (unsigned int i = vec.size() - 1; i > 0; i--) {
		for (unsigned int j = 0; j < i; j++) {
			if (vec[pos[j]] < vec[pos[j+1]]) {
				temp = pos[j+1];
				pos[j+1] = pos[j];
				pos[j] = temp;
			}
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::cutDownBestAxioms
*
*	Description:	Функция сокращает популяцию существующих аксиом до определенного в параметрах значения
*	Parameters:		void
*	Returns:		0 - если урезания популяции не было
*					1 - в противном случае
*	Throws:			AxiomLibException - если внутренние переменные класса не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::cutDownBestAxioms (void) {
	// Проверка входных параметров
	if ((maxAxiomPopSize < 1) || (bestAxioms.size() <= (unsigned int) maxAxiomPopSize))
		return 0;
	// Проверка согласованности внутренних переменных
	if (bestAxioms.size() != bestAxiomsRes.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::cutDownBestAxioms : incorrect internal variables.");
	
	// Сортировка аксиом по значению целевой функции
	std::vector <unsigned int> pos;
	getPriority (bestAxiomsRes, pos);
	
	// Определение числа лучших аксиом, сохраняемых в популяции
	unsigned int numBestToSave, numToChoose, numFrom;
	numBestToSave = (unsigned int) round (percentBestAxioms*maxAxiomPopSize);
	numToChoose = (unsigned int) maxAxiomPopSize - numBestToSave;
	
	numFrom = bestAxioms.size() - numBestToSave;
	std::vector <bool> chosen;
	chosen.resize (numFrom, false);
	int curChoice;
	std::vector <unsigned int> toSave;
	toSave.resize (maxAxiomPopSize);
	// Заносим в вектор сохраняемых элементов - заданное число лучших
	for (unsigned int i = 0; i < numBestToSave; i++) {
		toSave[i] = pos[i];
	}
	// Случайный выбор оставшихся
	for (unsigned int i = 0; i < numToChoose; i++) {
		curChoice = round (((double) rand() / (double) RAND_MAX) * (double) (numFrom - i - 1));
		for (unsigned int t = (unsigned int) curChoice; t < numFrom; t++)
			if (!chosen[t]) {
				chosen[t] = true;
				toSave[numBestToSave + i] = pos[numBestToSave + t];
				break;
			}
	}

	// Сортировка массива выбранных для сохранения элементов по убыванию
	unsigned int tmpInt;
	for (unsigned int i = maxAxiomPopSize - 1; i > 0; i--) {
		for (unsigned int j = 0; j < i; j++) {
			if (toSave[j] < toSave[j+1]) {
				tmpInt = toSave[j+1];
				toSave[j+1] = toSave[j];
				toSave[j] = tmpInt;
			}

		}
	}

	// Удаление невыбранных аксиом
	// Очищаем содержимое элементов вектора аксиом
	if ((toSave[0] + 1) < bestAxioms.size()) {
		for (unsigned int j = toSave[0] + 1; j < bestAxioms.size(); j++)
			bestAxioms[j].clear();
		bestAxioms.erase (bestAxioms.begin() + toSave[0] + 1, bestAxioms.end());
		bestAxiomsRes.erase (bestAxiomsRes.begin() + toSave[0] + 1, bestAxiomsRes.end());
	}
	for (int i = 1; i < maxAxiomPopSize; i++) {
		if ((toSave[i] + 1) < toSave[i-1]) {
			// Очищаем содержимое элементов вектора аксиом
			for (unsigned int j = toSave[i] + 1; j < toSave[i-1]; j++)
				bestAxioms[j].clear();
			bestAxioms.erase (bestAxioms.begin() + toSave[i] + 1, bestAxioms.begin() + toSave[i-1]);
			bestAxiomsRes.erase (bestAxiomsRes.begin() + toSave[i] + 1, bestAxiomsRes.begin() + toSave[i-1]);
		}
	}
	// Очищаем содержимое элементов вектора аксиом
	if (toSave[maxAxiomPopSize-1] > 0) {
		for (unsigned int j = 0; j < toSave[maxAxiomPopSize-1]; j++)
			bestAxioms[j].clear();
		bestAxioms.erase (bestAxioms.begin(), bestAxioms.begin() + toSave[maxAxiomPopSize-1]);
		bestAxiomsRes.erase (bestAxiomsRes.begin(), bestAxiomsRes.begin() + toSave[maxAxiomPopSize-1]);
	}
	
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::combineAxioms
*
*	Description:	Объединение двух аксиом и вычисление результата для полученных новых аксиом.
*	Parameters:		i - номер одной из аксиом, на основе которой будут построены новые варианты аксиом
*					j - номер одной из аксиом, на основе которой будут построены новые варианты аксиом
*					axiomExpr - заполняемая переменная аксиомы, в случае если удалось построить лучшую аксиому
*					axiomExprRes - результат функции значимости для лучшей созданной аксиомы
*	Returns:		0 - если новые аксиомы, созданные из двух указанных оказались не лучше
*					>0 - если созданная аксиома оказалась лучше предшественников
*					-1 - если не удалось провести комбинирование указанных аксиом
*	Throws:			AxiomLibException - если указанные входные параметры не согласованы с внутренними пременными класса
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::combineAxioms (const unsigned int i, const unsigned int j, AxiomExpr &axiomExpr, double &axiomExprRes, const int param) {
	// Проверка входных параметров
	if ((i >= bestAxioms.size()) || (j >= bestAxioms.size()) || (bestAxioms.size() != bestAxiomsRes.size()))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::combineAxioms : input parameters out of range.");
	
	// Создание новых аксиом
	AxiomExpr axiomExprAnd, axiomExprOr;
	double axiomExprAndRes, axiomExprOrRes;
	axiomExprAnd.andExpr(bestAxioms[i], bestAxioms[j]);
	axiomExprOr.orExpr(bestAxioms[i], bestAxioms[j]);
	
	// Вычисление целевых функций
	axiomExprAndRes = matterAxiomFunc (axiomExprAnd, param);
	axiomExprOrRes = matterAxiomFunc (axiomExprOr, param);

	// Определение лучше ли получились значения целевых функций у новых аксиом
	if ((axiomExprAndRes > max (bestAxiomsRes[i], bestAxiomsRes[j])) && (axiomExprAndRes > axiomExprOrRes)) {
		axiomExpr = axiomExprAnd;
		axiomExprRes = axiomExprAndRes;
		return 1;
	}
	if (axiomExprOrRes > max (bestAxiomsRes[i], bestAxiomsRes[j])) {
		axiomExpr = axiomExprOr;
		axiomExprRes = axiomExprOrRes;
		return 2;
	}

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::formAxioms
*
*	Description:	Функция формирования аксиом из элементарных условий
*	Parameters:		void
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::formAxioms (const int param) {
	// Проверка того, что на входе в функцию все необходимые данные согласованы
	if ((bestAxioms.size () != 0) || (bestAxiomsRes.size() != 0))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::formAxioms : set of axioms is not empty.");
	if (bestECs.size() < 1)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::formAxioms : set of prepared elementary conditions is empty.");
	unsigned int totSize = 0;
	for (unsigned int i = 0; i < bestECs.size(); i++)
		totSize += bestECs[i].size();
	if (totSize < 1)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::formAxioms : set of prepared elementary conditions is indeed empty.");

	// Создание начальной популяции аксиом, из которых буду строиться аксиомы дальше в итеративном алгоритме
	bestAxioms.resize(totSize);
	unsigned int k = 0;
	for (unsigned int i = 0; i < bestECs.size(); i++) {
		for (unsigned int j = 0; j < bestECs[i].size(); j++, k++) {
			bestAxioms[k].expression.resize(1);
			bestAxioms[k].expression[0].resize(1);
			bestAxioms[k].expression[0][0] = bestECs[i][j];
		}
	}
	// Заполняем начальную статистику по популяции
	bestAxiomsRes.resize(totSize);
	for (unsigned int i = 0; i < totSize; i++)
		bestAxiomsRes[i] = matterAxiomFunc (bestAxioms[i], param);

	// Подготовка переменных для поиска аксиом
	AxiomExpr axiomExpr;
	double axiomExprRes;
	int iterNum = 0;
	bool criteriaToContinue = true;
	int curSize;
	// Итеративное изменение популяции 
	while (criteriaToContinue) {
		iterNum++;
		// Урезаем, если необходимо, вектор аксиом до максимально допустимых размеров
		cutDownBestAxioms ();
		// Создаем новые элементы и вычисляем их значение целевой функции
		curSize = bestAxioms.size();
		std::vector <AxiomExpr> bestAxiomsNew;
		std::vector <double> bestAxiomsResNew;
		bestAxiomsNew.resize(round ((double)((curSize - 1) * curSize) / 2.0));
		bestAxiomsResNew.resize(round ((double)((curSize - 1) * curSize) / 2.0));
		int curBest = 0;
		for (unsigned int i = 0; i < (unsigned int) (curSize - 1); i++) {
			for (unsigned int j = i+1; j < (unsigned int) curSize; j++) {
				if (combineAxioms (i, j, axiomExpr, axiomExprRes, param) > 0) {
					/*bestAxioms.resize(bestAxioms.size() + 1);
					bestAxioms[bestAxioms.size() - 1] = axiomExpr;
					bestAxiomsRes.resize(bestAxiomsRes.size() + 1);
					bestAxiomsRes[bestAxiomsRes.size() - 1] = axiomExprRes;
					*/
					bestAxiomsNew[curBest] = axiomExpr;
					bestAxiomsResNew[curBest] = axiomExprRes;
					curBest++;
				}
			}
		}
		// Заносим новые элементы в конец bestAxioms
		if (curBest > 0) {
			bestAxioms.resize(bestAxioms.size() + curBest); 
			bestAxiomsRes.resize(bestAxiomsRes.size() + curBest); 
			for (int i = 0; i < curBest; i++) {
				bestAxioms[bestAxioms.size() - i - 1] = bestAxiomsNew[i];
				bestAxiomsRes[bestAxiomsRes.size() - i - 1] = bestAxiomsResNew[i];
			}
		}

		// Проверяем критерии останова
		if (curSize == bestAxioms.size())
			criteriaToContinue = false;
		if (iterNum > maxIterNum)
			criteriaToContinue = false;
		for (unsigned int i = 0; i < bestAxiomsRes.size(); i++)
			if (bestAxiomsRes[i] > maxAxiomRes)
				criteriaToContinue = false;
	}

	cutDownBestAxioms ();

	if (this->comments) {
		std::cout << "\n	Forming of axioms is done. Second stage is over.\n";
		std::cout.flush();
	}
	
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::createMarkUp
*
*	Description:	Разметка ряда curTS аксиомой axiom с записью результата в resMarkUp.
*	Parameters:		resMarkUp - разметка траектории curTS аксиомой axiom
*					curTS - размечаемая траектория
*					axiom - аксиома, которой происходит разметка
*					axNum - номер аксиомы в векторе bestAxioms данного класса
*	Returns:		int - размер траектории
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::createMarkUp (std::vector <int> &resMarkUp, std::vector <double> &curTS, AxiomExpr &axiom, const int axNum) const {
	resMarkUp.resize (curTS.size());
	for (unsigned long i = 0; i < curTS.size(); i++)
		if (axiom.check (i, curTS) > 0)
			resMarkUp[i] = axNum;
		else
			resMarkUp[i] = 0;
	return resMarkUp.size();
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::shiftErrors
*
*	Description:	Вспомогательная функция подсчета статистики - число
*					областей с разметкой. 
*	Parameters:		markUp - разметка траектории аномального поведения
*					from - номер отсчета, с которого начинать 
*					upTo - номер отсчета, до которого считать
*	Returns:		double - число областей с разметкой, умноженное на 
*							 соответствующий вес
*	Throws:			AxiomLibException - если входные данные заданы неверно
*	Author:			dk
*	History:
*
****************************************************************************/
inline double FuzzyDataLearnAlgorithm::shiftErrors (std::vector <int> &markUp, const int from, const int upTo) const {
	double toRet = 0;
	if ((from < 0) || (from > upTo) || (upTo > (int) markUp.size()))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::shiftErrors : incorrect input parameters.");
	int t = from;
	while (t < upTo) {
		t++;
		if (markUp[t-1] > 0) {
			toRet += 1.0;
			while (t < upTo) {
				if (markUp[t] != markUp[t-1])
					break;
				t++;
			}
		}
	}
	return toRet*shiftErrIn;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::shiftErrors
*
*	Description:	Вспомогательная функция подсчета статистики - число
*					несовпадений областей разметки на двух траекторях. 
*	Parameters:		row1 - разметка одной траектории аномального поведения
*					from1 - номер отсчета, с которого начинать на row1
*					row2 - разметка другой траектории аномального поведения
*					from2 - номер отсчета, с которого начинать на row2
*					len - сколько отсчетов траектории сравнивать
*	Returns:		double - число несовпадений областей с разметкой, 
*							 умноженное на соответствующий вес
*	Throws:			AxiomLibException - если входные данные заданы неверно
*	Author:			dk
*	History:
*
****************************************************************************/
inline double FuzzyDataLearnAlgorithm::shiftErrors (std::vector <int> &row1, const int from1, std::vector <int> &row2, const int from2, const int len) const {
	if ((from1 < 0) || (from2 < 0) || ((from1 + len) > (int) row1.size()) || ((from2 + len) > (int) row2.size()))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::shiftErrors for 2 rows : incorrect input parameters.");
	
	bool flag;
	double toRet = 0;
	int t = 0;
	
	// Бежим по первому ряду и считаем те области, которые не нашли отражение во втором ряду
	while (t < len) {
		if (row1[from1 + t] > 0) {
			flag = false;
			while (t < len) {
				if (row1[from1 + t] == row2[from2 + t]) {
					flag = true;
					t++;
					while (t < len) {
						if (row1[from1 + t - 1] != row1[from1 + t])
							break;
						t++;
					}
					break;
				}
				t++;
				if (row1[from1+t-1] != row1[from1+t])
					break;
			}
			if (!flag)
				toRet += 1.0;
		}
		t++;
	}

	// Бежим по второму ряду и считаем те области, которые не нашли отражение в первом ряду
	t = 0;
	while (t < len) {
		if (row2[from2 + t] > 0) {
			flag = false;
			while (t < len) {
				if (row2[from2 + t] == row1[from1 + t]) {
					flag = true;
					t++;
					while (t < len) {
						if (row2[from2 + t - 1] != row2[from2 + t])
							break;
						t++;
					}
					break;
				}
				t++;
				if (row2[from2+t-1] != row2[from2+t])
					break;
			}
			if (!flag)
				toRet += 1.0;
		}
		t++;
	}

	return toRet*shiftErrIn;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::moveVal
*
*	Description:	Вспомогательная функция обновдения векторов newMarkUp и newMarkUpSens 
*					из вектора markUp с указанием границ областей обновления.
*	Parameters:		newMarkUp - новая траектория обобщенной разметки
*					newMarkUpSens - вектор частот встречаемости разметок в newMarkUpSens
*					newFrom - номер осчета, с которго начинать на newMarkUp
*					markUp - разметка траектории аномального поведения
*					from - номер отсчета, с которого начинать 
*					upTo - номер отсчета, до которого считать
*	Returns:		0
*	Throws:			AxiomLibException - если входные данные заданы неверно
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::moveVal (std::vector <int> &newMarkUp, std::vector <int> &newMarkUpSens, const int newFrom, std::vector <int> &markUp, const int from, const int upTo) const {
	if ((from < 0) || (from > upTo) || (upTo > (int) markUp.size())
		|| (newFrom < 0) || (newMarkUp.size() != newMarkUpSens.size()) || ((upTo - from + newFrom) > (int) newMarkUp.size()) )
		throw AxiomLibException("FuzzyDataLearnAlgorithm::moveVal I : incorrect input parameters.");
	for (int i = from, j = newFrom; i < upTo; i++, j++) {
		newMarkUp[j] = markUp[i];
		if (markUp[i] > 0) 
			newMarkUpSens[j] = 1;
		else
			newMarkUpSens[j] = 0;
	}
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::moveVal
*
*	Description:	Вспомогательная функция обновдения векторов newMarkUp и newMarkUpSens 
*					из вектов base, sens с указанием границ областей обновления.
*	Parameters:		newMarkUp - новая траектория обобщенной разметки
*					newMarkUpSens - вектор частот встречаемости разметок в newMarkUpSens
*					newFrom - номер осчета, с которго начинать на newMarkUp
*					base - старая траектория обобщенной разметки
*					sens - старый вектор частот
*					from - номер отсчета, с которого начинать 
*					upTo - номер отсчета, до которого считать
*	Returns:		0
*	Throws:			AxiomLibException - если входные данные заданы неверно
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::moveVal (std::vector <int> &newMarkUp, std::vector <int> &newMarkUpSens, const int newFrom, std::vector <int> &base, std::vector <int> &sens, const int from, const int upTo) const {
	if ((from < 0) || (from > upTo) || (base.size() != sens.size()) || (upTo > (int) base.size())
		|| (newFrom < 0) || (newMarkUp.size() != newMarkUpSens.size()) || ((upTo - from + newFrom) > (int) newMarkUp.size()) )
		throw AxiomLibException("FuzzyDataLearnAlgorithm::moveVal II : incorrect input parameters.");
	for (int i = from, j = newFrom; i < upTo; i++, j++) {
		newMarkUp[j] = base[i];
		newMarkUpSens[j] = sens[i];	
	}
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::moveVal
*
*	Description:	Вспомогательная функция обновдения векторов newMarkUp и newMarkUpSens 
*					из вектора markUp и векторов base, sens с указанием границ областей обновления.
*	Parameters:		newMarkUp - новая траектория обобщенной разметки
*					newMarkUpSens - вектор частот встречаемости разметок в newMarkUpSens
*					newFrom - номер осчета, с которго начинать на newMarkUp
*					base - старая траектория обобщенной разметки
*					sens - старый вектор частот
*					fromBase - номер отсчета, с которого начинать на base
*					markUp - разметка траектории аномального поведения
*					fromMarkUp - номер отсчета, с которого начинать 
*					len - длина обрабатываемого участка
*	Returns:		0
*	Throws:			AxiomLibException - если входные данные заданы неверно
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::moveVal (std::vector <int> &newMarkUp, std::vector <int> &newMarkUpSens, const int newFrom, std::vector <int> &base, std::vector <int> &sens, const int fromBase, std::vector <int> &markUp, const int fromMarkUp, const int len) const {
	if ((fromBase < 0) || (base.size() != sens.size()) || ((fromBase + len) > (int) base.size())
		|| (fromMarkUp < 0) || ((fromMarkUp + len) > (int) markUp.size())
		|| (newFrom < 0) || (newMarkUp.size() != newMarkUpSens.size()) || ((newFrom + len) > (int) newMarkUp.size()) )
		throw AxiomLibException("FuzzyDataLearnAlgorithm::moveVal III : incorrect input parameters.");
	for (int i = fromBase, j = newFrom, k = fromMarkUp; i < (fromBase + len); i++, j++, k++) {
		if (markUp[k] > 0) {
			newMarkUp[j] = markUp[k];
			if (base[i] == markUp[k]) {
				newMarkUpSens[j] = sens[i] + 1;
			} else {
				newMarkUpSens[j] = 1;
			}
		} else { 
			newMarkUp[j] = base[i];
			newMarkUpSens[j] = sens[i];
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::createBaseMarkUp
*
*	Description:	Cдвиг основных разметок и формирование обобщенной разметки
*	Parameters:		sens - формируемая обобщенная разметка (число повторений разметки в данной позиции)
*					markUps - разметки траекторий аномального поведения одного 
*								типа и созданные одной аксиомой
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::createBaseMarkUp (std::vector <int> &sens, std::vector < std::vector <int> > &markUps) const {
	int  lastStart, newStart;
	double lastScore, newScore;
	bool flag = true;
	std::vector <int> base, newBase, newSens;
	base.clear();
	sens.clear();
	newBase.clear();
	newSens.clear();
	for (int i = 0; i < (int) markUps.size(); i++) {
		if (markUps[i].size() > 0) {
			if (flag) {
				base = markUps[i];
				sens.resize(base.size());
				for (unsigned int h = 0; h < base.size(); h++) {
					if (base[h] > 0) sens[h] = 1;
					else sens[h] = 0;
				}
				flag = false;
			} else {
				lastStart = 0;
				lastScore = -1.0;
				// бежим по пикам в base
				for (int y = 0; y < (int) base.size(); y++) {
					if (base[y] > 0) {
						// для каждого пика в base бежим по всем пикам markUps[i]
						for (int z = 0; z < (int) markUps[i].size(); z++) {
							// для каждого пика в base и в markUps[i] - считаем число совпадений
							if (markUps[i][z] > 0) {
								newStart = y - z;
								if (newStart < 0) {
									// от 0 до z - y бежим по markUps[i]
									newScore = shiftErrors (markUps[i], 0, z-y);
									if ((markUps[i].size() + newStart) > base.size()) {
										// от 0 до base.size() бежим по base и по markUps[i]
										newScore += shiftErrors (base, 0, markUps[i], z-y, base.size());
										// от base.size() - newStart до markUps[i].size() по markUps[i]
										newScore += shiftErrors (markUps[i], base.size() - newStart, markUps[i].size());
									} else {
										// от 0 до markUps[i].size() + newStart по base и по markUps[i]
										newScore += shiftErrors (base, 0, markUps[i], z-y, markUps[i].size() + newStart);
										// от markUps[i].size() + newStart до base.size() по base
										newScore += shiftErrors (base, markUps[i].size() + newStart, base.size());
									}
								} else {
									// от 0 до y - z бежим по base
									newScore = shiftErrors (base, 0, newStart);
									if ((markUps[i].size() + newStart) > base.size()) {
										// от y - z до base.size() бежим по base и по markUps[i] от 0 до base.size() - y + z
										newScore += shiftErrors (base, newStart, markUps[i], 0, base.size() - newStart);
										// от base.size() - newStart до markUps[i].size() по markUps[i]
										newScore += shiftErrors (markUps[i], base.size() - newStart, markUps[i].size());
									} else {
										// от y-z до markUps[i].size() + newStart по base и по markUps[i]
										newScore += shiftErrors (base, newStart, markUps[i], 0, markUps[i].size());
										// от markUps[i].size() + newStart до base.size() по base
										newScore += shiftErrors (base, markUps[i].size() + newStart, base.size());
									}
								}
								// подсчитали число совпадений для данного сдвига (для данных двух пиков)
								// Тогда сохрянаем лучшее значение относительного сдвига
								if ((lastScore < 0) || (newScore < lastScore)) {
									lastScore = newScore;
									lastStart = newStart;
								}
							}
						}
					}
				}
				// Пробежали по всем пикам и возможным вариантам сдвига - теперь производим дополнение base в соответствии с лучшим значением сдвига
				if (lastStart < 0) {
					if ((markUps[i].size() + lastStart) > base.size()) {
						newBase.resize(markUps[i].size());
						newSens.resize(markUps[i].size());
						// от 0 до -lastStart бежим по markUps[i]
						moveVal (newBase, newSens, 0, markUps[i], 0, -lastStart);
						// от 0 до base.size() бежим по base и по markUps[i]
						moveVal (newBase, newSens, -lastStart, base, sens, 0, markUps[i], -lastStart, base.size());
						// от base.size() - lastStart до markUps[i].size() по markUps[i]
						moveVal (newBase, newSens, base.size() - lastStart, markUps[i], base.size() - lastStart, markUps[i].size());
					} else {
						newBase.resize(base.size() - lastStart);
						newSens.resize(base.size() - lastStart);
						// от 0 до -lastStart бежим по markUps[i]
						moveVal (newBase, newSens, 0, markUps[i], 0, -lastStart);
						// от 0 до markUps[i].size() + lastStart по base и по markUps[i]
						moveVal (newBase, newSens, -lastStart, base, sens, 0, markUps[i], -lastStart, markUps[i].size() + lastStart);
						// от markUps[i].size() + lastStart до base.size() по base
						moveVal (newBase, newSens, markUps[i].size(), base, sens, markUps[i].size() + lastStart, base.size());
					}
				} else {
					if ((markUps[i].size() + lastStart) > base.size()) {
						newBase.resize(markUps[i].size() + lastStart);
						newSens.resize(markUps[i].size() + lastStart);
						// от 0 до lastStart бежим по base
						moveVal (newBase, newSens, 0, base, sens, 0, lastStart);
						// от y - z до base.size() бежим по base и по markUps[i] от 0 до base.size() - y + z
						moveVal (newBase, newSens, lastStart, base, sens, lastStart, markUps[i], 0, base.size() - lastStart);
						// от base.size() - lastStart до markUps[i].size() по markUps[i]
						moveVal (newBase, newSens, base.size(), markUps[i], base.size() - lastStart, markUps[i].size());
					} else {
						newBase.resize(base.size());
						newSens.resize(base.size());
						// от 0 до lastStart бежим по base
						moveVal (newBase, newSens, 0, base, sens, 0, lastStart);
						// от y-z до markUps[i].size() + lastStart по base и по markUps[i]
						moveVal (newBase, newSens, lastStart, base, sens, lastStart, markUps[i], 0, markUps[i].size());
						// от markUps[i].size() + lastStart до base.size() по base
						moveVal (newBase, newSens, markUps[i].size() + lastStart, base, sens, markUps[i].size() + lastStart, base.size());
					}
				}
				base = newBase;
				sens = newSens;
			}
		}
	}

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::formGeneralizedMarkUps
*
*	Description:	Формирование обобщенных разметок для заданного типа 
*					аномального поведения и заданной аксиомы:
*					Получаем разметки всех обучающих траекторий для заданного 
*					типа аномального поведения и для всех аксиом. Сдвигаем эти 
*					разметки до наилучшего соответствия и формируем обобщенные 
*					разметки.
*	Parameters:		baseMarkUps - обобщенные разметки по различным аксиомам
*					resMarkUps - массив всех разметок для данной аксиомы
*					i - номер класса аномального поведения в наборе данных, 
*						который использовать для анализа
*					axNum - номер аксиомы в векторе bestAxioms, которую использовать
*							в данном алгоритме
*					param - размерность набора данных по которой вести анализ
*	Returns:		0
*	Throws:			AxiomLibException - при некорректном ответе от набора данных или некорректных входных параметрах
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::formGeneralizedMarkUps (std::vector <int> &baseMarkUp, std::vector < std::vector <int> > &resMarkUps, int axNum, int i, int numOfMultiTS, const int param) {
	// Проверка входных параметров
	if (axNum >= (int) bestAxioms.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::formGeneralizedMarkUps : incorrect input parameters.");
	// Получаем разметки всех траекторий аномального поведения типа i для аксиомы axNum
	bool bres;
	std::vector <double> curTS;
	resMarkUps.resize (numOfMultiTS);
	for (int j = 0; j < numOfMultiTS; j++) {
		bres = fuzzyDataSet.getTSByIndexFromClass (curTS, i, j, param);
		if (bres) {
			if (curTS.size() < 1)
				throw AxiomLibException("FuzzyDataLearnAlgorithm::formGeneralizedMarkUps : incorrect response from internal request to dataSet.");
			// Разметка траектории curTS
			createMarkUp (resMarkUps[j], curTS, bestAxioms[axNum], axNum+1);
		}
	}
	// На основании полученных разметок траекторий аномального поведения - производим их сдвиг и формируем обобщенную разметку
	createBaseMarkUp (baseMarkUp, resMarkUps);
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::simplifyMarkUp
*
*	Description:	Упрощение разметки и приведение ее в сокращенное описание
*	Parameters:		simp - сокращенный и упрощенный вариант разметки
*					len - вектор длин вхождений элементов разметки из simp в 
*						  исходную разметку row
*					row - преобразуемая разметка
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::simplifyMarkUp (std::vector <int> &simp, std::vector <int> &len, std::vector <int> &row, const int axNum) const {
	simp.clear();
	len.clear();
	// Убираем все 0 в начале вектора - их не учитываем
	unsigned int i = 0;
	while (i < row.size()) {
		if (row[i] > 0)
			break;
		i++;
	}
	// не учитываем все 0 в конеце вектора
	unsigned int upTo = row.size() - 1;
	while (upTo > i) {
		if (row[upTo] > 0)
			break;
		upTo--;
	}
	upTo++;
	// Если были только 0 - то выходим из функции
	if (((i+1) == upTo) && (row[i] == 0))
		return -1;
	// Пробегаем по всем содержательным элементам и запоминаем максимальное значение вектора
	int maxVal = 0;
	for (unsigned int k = i; k < upTo; k++) {
		if (row[k] > maxVal)
			maxVal = row[k];
	}
	// Цикл по всем содержательным элементам ветора row
	unsigned int count = i;
	unsigned int j = 0;
	while ( i < upTo ) {
		if (row[i] == 0) {
			simp.push_back (axNum);
			len.push_back (i - count);
			count = i;
			while (i < upTo) {
				if (row[i] > 0)
					break;
				i++;
			}
			simp.push_back (0);
			len.push_back(i - count);
			count = i;
		}
		i++;
	}
	simp.push_back (axNum);
	len.push_back (i - count);
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::subcheck 
*
*	Description:	Функция проверки содержания одной из разметок в другой.
*	Parameters:		rowAxiomTypes - входной ряд аксиом (разметка ряда)
*					rowAxiomLen - параметры разметки
*					upTo - конец ряда откуда в сторону начала искать разметку
*					curEnd - текущее положение в эталонной разметке для сравнения
*	Returns:		0 - false
*					1 - true
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::subcheck (std::vector<int> &rowAxiomTypes, std::vector<int> &rowAxiomLen, int upTo, std::vector<int> &axiomTypes, std::vector<int> &axiomMinLen, std::vector<int> &axiomMaxLen, int curEnd) const {
	if (curEnd < 0) {
		return 1;
	}
	long int j = upTo;
	int flag = 0;
	for (int i = curEnd; i >= 0; i--) {
		// Не учитываем разметку эталонного ряда - если там -1
		if (axiomTypes[i] == -1) {
			continue;
		}
		// Не учитываем разметку ряда - если там -1 - что означает пустую аксиому
		if ((rowAxiomTypes[j] == -1) && (j != upTo))  {
			j--;
			if (j < 0) {
				// Ряд слишком короткий чтобы что-то определить
				return 0;
			}
		}
		// Если аксиома может и не входить в размету - то проверяем - совпадают ли ряды ее при отсутствии 
		if (axiomMinLen[i] == 0) {
			flag = this->subcheck (rowAxiomTypes, rowAxiomLen, j, axiomTypes, axiomMinLen, axiomMaxLen, curEnd - 1);
			if (flag != 0) {
				return flag;
			}
		}
		// Просто проверка вхождения эталонной последовательности в разметку
		if (axiomTypes[i] != rowAxiomTypes[j]) {
			// Несовпадают типы аксиом
			return 0;
		}
		// Проверка - совпадают ли при этом длины - то есть ссответствие расширенной разметки
		if ((axiomMinLen[i] > rowAxiomLen[j]) || ((axiomMaxLen[i] < rowAxiomLen[j]) && (i != 0) ) ) {
			// Неподходят длины аксиомы
			return 0;
		}
		// Сдвигаемся по ряду - для сравнения следующей аксиомы
		j--;
		if ((j < 0) && (i != 0)) {
			// Ряд слишком короткий чтобы что-то определить
			return 0;
		}
	}
	// раз не выпрыгнули из функции раньше - значит разметка подходит - выходим с true	
	return 1;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::splitMarkUps
*
*	Description:	Объединение расширенных разметок
*	Parameters:		markUp - новая образованная разметка
*					markUpMinLen - минимальное число вхождений аксиом в новую разметку
*					markUpMaxLen - максимальное число вхождений аксиом в новую разметку
*					set - индексы разметок, которые объединять
*					markUps - вектор разметок, из которых выбирать разметки для объединения
*					markUpsMinLen - вектор минимальных вхождений аксиом разметки
*					markUpsMaxLen - вектор максимальных вхождений аксиом разметки
*	Returns:		0
*	Throws:			AxiomLibException - 
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::splitMarkUps (std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <int> &set, std::vector < std::vector <int> > &markUps, std::vector < std::vector <int> > &markUpsMinLen, std::vector < std::vector <int> > &markUpsMaxLen) const {
	// ПОКА ТОЛЬКО ЗАГЛУШКА
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::countMarkUpEfficiency
*
*	Description:	Подсчет эффективности разметки на траекториях обучающей выборки
*	Parameters:		markUp - тестируемая разметка
*					markUpMinLen - минимальное число вхождений аксиом в разметку
*					markUpMaxLen - максимальное число вхождений аксиом в разметку
*					localMarkUps - разметки траекторий обучающей выборки
*					localMarkUpsEngaged - принимаемые во внимание разметки 
*								          траекторий обучающей выборки
*	Returns:		int - число вхождений разметки в разметку обучающей выборки
*	Throws:			AxiomLibException - если входные параметры localMarkUpsEngaged и 
*					localMarkUps разной длины
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::countMarkUpEfficiency (std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <std::vector <int> > &localMarkUps, std::vector <bool> &localMarkUpsEngaged) const {
	// Проверка входных данных
	if (localMarkUpsEngaged.size() != localMarkUps.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::countMarkUpEfficiency : incorrect input parameters.");
	// Бежим по разметкам траектрий обучающей выборки и отмечаем те, где нашли требуемую разметку
	std::vector <int> row, rowLen;
	int rowSize;
	int toRet = 0;
	for (unsigned int tsNum = 0; tsNum < localMarkUps.size(); tsNum++) {
		if (!localMarkUpsEngaged[tsNum])
			continue;
		if (localMarkUps[tsNum].size() < 1) {
			localMarkUpsEngaged[tsNum] = false;
			continue;
		}
		row.resize(localMarkUps[tsNum].size());
		rowLen.resize(localMarkUps[tsNum].size());
		row[0] = localMarkUps[tsNum][0];
		rowLen[0] = 1;
		rowSize = 0; // указывает индекс последней позиции в векторах row и rowLen
		for (unsigned int i = 1; i < localMarkUps[tsNum].size(); i++) {
			// собираем ряд расширенной разметки
			if (localMarkUps[tsNum][i] != localMarkUps[tsNum][i-1]) {
				rowSize++;
				row[rowSize] = localMarkUps[tsNum][i];
				rowLen[rowSize] = 1;
			}
			else {
				rowLen[rowSize]++;
			}
			// Определяем есть ли совпадение с заданной разметкой
			if (subcheck (row, rowLen, rowSize, markUp, markUpMinLen, markUpMaxLen, (int) markUp.size() - 1) > 0) {
				toRet++;
				break;
			}
		}
	}
	return toRet;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::setEngagedMarkUps
*
*	Description:	Обновление принимаемых во внимание разметок траекторий 
*					обучающей выборки в соответствии с разметками, содержащими 
*					разметку markUp.
*	Parameters:		markUp - тестируемая разметка
*					markUpMinLen - минимальное число вхождений аксиом в разметку
*					markUpMaxLen - максимальное число вхождений аксиом в разметку
*					localMarkUps - разметки траекторий обучающей выборки
*					localMarkUpsEngaged - принимаемые во внимание разметки 
*								          траекторий обучающей выборки
*	Returns:		bool - остались ли еще разметки траекторий обучающей выборки 
*						не покрытые текущей разметкой траектории нештатного поведения
*	Throws:			AxiomLibException - если входные параметры localMarkUpsEngaged и 
*					localMarkUps разной длины
*	Author:			dk
*	History:
*
****************************************************************************/
bool FuzzyDataLearnAlgorithm::setEngagedMarkUps (std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <std::vector <int> > &localMarkUps, std::vector <bool> &localMarkUpsEngaged) const {
	// Проверка входных данных
	if (localMarkUpsEngaged.size() != localMarkUps.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::setEngagedMarkUps : incorrect input parameters.");
	// Бежим по разметкам траектрий обучающей выборки и отмечаем те, где нашли требуемую разметку
	std::vector <int> row, rowLen;
	int rowSize;
	bool toRet = false;
	for (unsigned int tsNum = 0; tsNum < localMarkUps.size(); tsNum++) {
		if (!localMarkUpsEngaged[tsNum])
			continue;
		if (localMarkUps[tsNum].size() < 1) {
			localMarkUpsEngaged[tsNum] = false;
			continue;
		}
		row.resize(localMarkUps[tsNum].size());
		rowLen.resize(localMarkUps[tsNum].size());
		row[0] = localMarkUps[tsNum][0];
		rowLen[0] = 1;
		rowSize = 0; // указывает индекс последней позиции в векторах row и rowLen
		for (unsigned int i = 1; i < localMarkUps[tsNum].size(); i++) {
			// собираем ряд расширенной разметки
			if (localMarkUps[tsNum][i] != localMarkUps[tsNum][i-1]) {
				rowSize++;
				row[rowSize] = localMarkUps[tsNum][i];
				rowLen[rowSize] = 1;
			}
			else {
				rowLen[rowSize]++;
			}
			// Определяем есть ли совпадение с заданной разметкой
			if (subcheck (row, rowLen, rowSize, markUp, markUpMinLen, markUpMaxLen, (int) markUp.size() - 1) > 0) {
				localMarkUpsEngaged[tsNum] = false;
				break;
			}
		}
		if (localMarkUpsEngaged[tsNum]) {
			toRet = true;
		}
	}
	return toRet;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::expandMarkUp
*
*	Description:	Преобразование расширенной разметки к простой разметке
*	Parameters:		simpleMarkUp - результат преобразования, простая разметка
*					markUp - расширенная разметка
*					markUpMinLen - минимальное число вхождений аксиом в расширенную разметку
*					markUpMaxLen - максимальное число вхождений аксиом в расширенную разметку
*	Returns:		0
*	Throws:			AxiomLibException - 
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::expandMarkUp (std::vector <int> &simpleMarkUp, std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen) const {
	// ТОЛЬКО ЗАГЛУШКА НА ВРЕМЯ ТЕСТИРОВАНИЯ
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::extendMarkUpLen
*
*	Description:	Преобразует элементы вектора в интервал из минимального и 
*					максимального значения с заданным коэффиентом
*	Parameters:		stdLen - преобразуемые элементы
*					minLen - заполняемый вектор минимальных значений
*					maxLen - заполняемый вектор максимальных значений
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::extendMarkUpLen (std::vector <int> &stdLen, std::vector <int> &minLen, std::vector <int> &maxLen) const {
	minLen.resize (stdLen.size());
	maxLen.resize (stdLen.size());
	for (unsigned int i = 0; i < stdLen.size(); i++) {
		minLen[i] = max (0, round (((double) stdLen[i]) * (1.0 - extendMarkUpLensLevel)));
		maxLen[i] = round (((double) stdLen[i]) * (1.0 + extendMarkUpLensLevel));
	}
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::addResults
*
*	Description:	Функция формирует ряд результатов сопоставления эталонной 
*					разметки и разметки анализируемого ряда
*	Parameters:		result - обновляемый ряд результатов распознавания
*					markUp - тестируемая разметка
*					markUpMinLen - минимальное число вхождений аксиом в разметку
*					markUpMaxLen - максимальное число вхождений аксиом в разметку
*					wholeRow - разметка анализируемого ряда 
*					indicator - индекс класса неисправности, по которому ведется анализ
*	Returns:		0
*	Throws:			AxiomLibException - если входные параметры localMarkUpsEngaged и 
*					localMarkUps разной длины
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::addResults (std::vector <int> &result, std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <int> &wholeRow, int indicator) const {
	// Проверка входных данных
	if (result.size() != wholeRow.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::addResults : incorrect input parameters.");
	if (wholeRow.size() < 1)
		return -1;
	std::vector <int> row, rowLen;
	int rowPos;
	int markUpPos = markUp.size() - 1;
	row.resize(wholeRow.size());
	rowLen.resize(wholeRow.size());
	row[0] = wholeRow[0];
	rowLen[0] = 1;
	rowPos = 0; // указывает индекс последней позиции в векторах row и rowLen
	for (unsigned int i = 1; i < wholeRow.size(); i++) {
		// собираем ряд расширенной разметки
		if (wholeRow[i] != wholeRow[i-1]) {
			rowPos++;
			row[rowPos] = wholeRow[i];
			rowLen[rowPos] = 1;
		} else {
			rowLen[rowPos]++;
		}
		// Определяем есть ли совпадение с заданной разметкой
		if (subcheck (row, rowLen, rowPos, markUp, markUpMinLen, markUpMaxLen, markUpPos) > 0) {
			result[i] = indicator;
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::countExtraStat
*
*	Description:	Функция подсчета статистики для вектора разметок на контрольной 
*					выборке и на разметках траекторий аномального поведения.
*	Parameters:		curSet - индексы разметок, которые учавствуют в распознавании
*					simpMarkUps - вектор всех разметок для траекторий аномального поведения
*					simpMarkUpsMinLens - вектор минимально-необходимого числа вхождений элемента эталонной разметки в исходную
*					simpMarkUpsMinLens - вектор максимального числа вхождений элемента эталонной разметки в исходную
*					extraMarkUps - разметки всех траекторий контрольной выборки и 
*								   траекторий нормального поведения всеми аксиомами.
*					correctExtraMarkUps - вхождение траекторий аномального поведения 
*								   для extraMarkUps.
*					indicator - индекс класса неисправности, по которому ведется анализ
*	Returns:		int - линейная клмбинация от числа ошибок первого и второго рода, полученных при распознавании
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::countExtraStat (std::vector <int> &curSet, std::vector < std::vector <int> > &simpMarkUps, std::vector < std::vector <int> > &simpMarkUpsMinLens, std::vector < std::vector <int> > &simpMarkUpsMaxLens, std::vector < std::vector <std::vector <int> > > &extraMarkUps, std::vector <std::vector <int> > &correctExtraMarkUps, const int indicator) const {
	int toRet = 0;
	int wholeNum, inpSize, first, second;
	double tmpFirst, tmpSecond;
	std::vector <int> result;

	// Проверка входных параметров
	if (extraMarkUps.size() < 1)
		throw AxiomLibException("FuzzyDataLearnAlgorithm::countExtraStat : input params are clear");
	
	inpSize = extraMarkUps[0].size();
	
	for (unsigned int setItr = 0; setItr < curSet.size(); setItr++)
		if (extraMarkUps[curSet[setItr]].size() != inpSize)
			throw AxiomLibException("FuzzyDataLearnAlgorithm::countExtraStat : input params are corrupted");

	first = 0;
	second = 0;
	// Цикл по всем рядам контрольный выборки
	for (int rowItr = 0; rowItr < inpSize; rowItr++) {
		result.resize (extraMarkUps[0][rowItr].size(), 0);
		for (unsigned int setItr = 0; setItr < curSet.size(); setItr++) {
			addResults (result, simpMarkUps[curSet[setItr]], simpMarkUpsMinLens[curSet[setItr]], simpMarkUpsMaxLens[curSet[setItr]], extraMarkUps[curSet[setItr]][rowItr], indicator);
		}
		wholeNum = compareStatistic->getStatistic(result, correctExtraMarkUps[rowItr], tmpFirst, tmpSecond, comments);
		first += (int) tmpFirst;
		second += (int) tmpSecond;
	}

	// Вычисление значения целевой функции
	toRet = round (goalStrategy->compute (first, second));

	return toRet;
}

/****************************************************************************
*					FuzzyDataLearnAlgorithm::createCandidates
*
*	Description:	Формирование кандидатов в разметки для заданного типа 
*					аномального поведения  
*	Parameters:		candMarkUps - формируемые кандидаты в разметки
*					candMarkUpsMinLen - минимальное число вхождений аксиом в разметки
*					candMarkUpsMaxLen - максимальное число вхождений аксиом в разметки
*					baseMarkUps - обобщенные разметки по различным аксиомам
*					localMarkUps - разметки всех траекторий обучающей выборки
*								   всеми аксиомами.
*					extraMarkUps - разметки всех траекторий контрольной выборки и 
*								   траекторий нормального поведения всеми аксиомами.
*					correctExtraMarkUps - вхождение траекторий аномального поведения 
*								   для extraMarkUps.
*					indicator - индекс класса неисправности, по которому ведется анализ
*	Returns:		0
*	Throws:			AxiomLibException - если во входных параметрах пустые переменные
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyDataLearnAlgorithm::createCandidates (std::vector < std::vector <int> > &candMarkUps, std::vector < std::vector <int> > &candMarkUpsMinLens, std::vector < std::vector <int> > &candMarkUpsMaxLens, std::vector < std::vector <int> > &baseMarkUps, std::vector < std::vector <std::vector <int> > > &localMarkUps, std::vector < std::vector <std::vector <int> > > &extraMarkUps, std::vector <std::vector <int> > &correctExtraMarkUps, const int indicator) const {
	std::vector < std::vector <int> > simpMarkUps;
	std::vector < std::vector <int> > simpMarkUpsLens, simpMarkUpsMinLens, simpMarkUpsMaxLens;
	// Проверка длины массивва
	if (baseMarkUps.size() < 1)
		return 0;		
	simpMarkUps.resize (baseMarkUps.size());
	simpMarkUpsLens.resize(baseMarkUps.size());
	simpMarkUpsMinLens.resize(baseMarkUps.size());
	simpMarkUpsMaxLens.resize(baseMarkUps.size());
	// Упрощение всех обобщенных разметок и приведение их к расширенной форме
	for (int i = 0; i < (int) baseMarkUps.size(); i++) {
		simplifyMarkUp (simpMarkUps[i], simpMarkUpsLens[i], baseMarkUps[i], i+1);
		extendMarkUpLen (simpMarkUpsLens[i], simpMarkUpsMinLens[i], simpMarkUpsMaxLens[i]);
	}
	// На основе расширенных упрощенных обобщенных разметок - формируем кандидаты в разметки для данного типа аномального поведения
	// Проверяем входные переменны
	for (unsigned int axNum = 1; axNum < localMarkUps.size(); axNum++) {
		if (localMarkUps[axNum].size() != localMarkUps[axNum-1].size()) 
			throw AxiomLibException("FuzzyDataLearnAlgorithm::createCandidates : different number of TimeSeries per different axioms - incompatible condition");
	}
	if (localMarkUps.size() < 1) 
		throw AxiomLibException("FuzzyDataLearnAlgorithm::createCandidates : localMarkUps variable is undefined");
	// Создаем необходимые переменные
	std::vector < bool > localMarkUpsEngaged;
	localMarkUpsEngaged.resize (localMarkUps[0].size(), true);
	std::vector <int> curMarkUp, newMarkUp, toSave; 
	std::vector <int> curMarkUpMinLen, curMarkUpMaxLen, newMarkUpMinLen, newMarkUpMaxLen, toSaveMinLen, toSaveMaxLen;
	curMarkUp.clear();
	curMarkUpMinLen.clear();
	curMarkUpMaxLen.clear();
	int curRes, newRes, bestNum, curStart, lastRes;
	bool flag;
	std::vector < std::vector <int> *> curMarkUps, curMarkUpsMinLens, curMarkUpsMaxLens;
	curMarkUps.clear();
	curMarkUpsMinLens.clear();
	curMarkUpsMaxLens.clear();
	std::vector <int> curSet, lastSet;
	lastRes = -1;
	lastSet.clear();
	// Цикл по всем разметкам - с какой начинать чтобы собирать достаточное покрытие возможных разметок
	for (int base = 0; base < (int) simpMarkUps.size(); base++) {
		// Включаем в состав покрытия разметку с индексом base
		localMarkUpsEngaged.resize (localMarkUps[0].size(), true);
		curSet.resize (1, base);
		flag = setEngagedMarkUps (simpMarkUps[base], simpMarkUpsMinLens[base], simpMarkUpsMaxLens[base], localMarkUps[base], localMarkUpsEngaged);
		// Итеративно наращиваем покрытие до максимального
		curStart = base;
		while (flag) {
			bestNum = -1;
			curRes = 0;
			for (int i = curStart + 1; i < (int) simpMarkUps.size(); i++) {
				newRes = countMarkUpEfficiency (simpMarkUps[i], simpMarkUpsMinLens[i], simpMarkUpsMaxLens[i], localMarkUps[i], localMarkUpsEngaged);
				if (newRes > curRes) {
					curRes = newRes;
					bestNum = i;
				}
			}
			// Проверка - была ли найдена разметка лучше текущей
			if (bestNum < 0) {
				// Если разметки лучше не было найдено - то выходим из цикла
				flag = false;
				break;
			}
			// Если нашли разметку лучше - то сохраняем ее
			curSet.push_back (bestNum);
			// ставим false на тех исходных разметках - где текущий вариант работает
			flag = setEngagedMarkUps (simpMarkUps[bestNum], simpMarkUpsMinLens[bestNum], simpMarkUpsMaxLens[bestNum], localMarkUps[bestNum], localMarkUpsEngaged);
		}
		// Вычисляем значение целевой функции для полученного набора разметок на контрольной выборке
		curRes = countExtraStat (curSet, simpMarkUps, simpMarkUpsMinLens, simpMarkUpsMaxLens, extraMarkUps, correctExtraMarkUps, indicator);
		if ((lastRes < 0) || (curRes < lastRes)) {
			lastRes = curRes;
			lastSet = curSet;
			if (curRes == 0)
				break;
		}
	}
	// Полученный набор разметок сливаем в одну
	//splitMarkUps (curMarkUp, curMarkUpMinLen, curMarkUpMaxLen, lastSet, simpMarkUps, simpMarkUpsMinLens, simpMarkUpsMaxLens);
	// Слитую разметку переводим в обычный вид
	candMarkUps.resize (lastSet.size());
	candMarkUpsMinLens.resize (lastSet.size());
	candMarkUpsMaxLens.resize (lastSet.size());
	for (unsigned int mu = 0; mu < lastSet.size(); mu++) {
		candMarkUps[mu] = simpMarkUps[lastSet[mu]];
		candMarkUpsMinLens[mu] = simpMarkUpsMinLens [lastSet[mu]];
		candMarkUpsMaxLens[mu] = simpMarkUpsMaxLens [lastSet[mu]];
	}

	// Output
	std::cout << "\n		Best Result is " << lastRes << "\n";
	for (unsigned int mu = 0; mu < lastSet.size(); mu++) {
		std::cout << "\n		";
		for (unsigned int s = 0; s < simpMarkUps[lastSet[mu]].size(); s++)
			std::cout << simpMarkUps[lastSet[mu]][s] << "[" << simpMarkUpsMinLens [lastSet[mu]][s] << "," << simpMarkUpsMaxLens [lastSet[mu]][s] << "] ";
	}
	std::cout << "\n";
	std::cout.flush();

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::sortCandidateMarkUps
*
*	Description:	Отбор разметок для заданного типа аномального поведения из 
*					сформированных кандидатов
*	Parameters:		markUps - отобранные разметки траекторий аномального поведения
*					candMarkUps - кандидаты в разметки
*					param - размерной траекторий набора данных с которой работаем 
*					в данном алгоритме
*	Returns:		0
*	Throws:			AxiomLibException - 
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::sortCandidateMarkUps (std::vector < std::vector <int> > &markUps, std::vector < std::vector <int> > &candMarkUps, const int  param) const {
	// Пока только заглушка

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::formAxiomSets
*
*	Description:	Создание систем аксиом на основе разметок и всего набора аксиом
*	Parameters:		markUps - наборы разметок траекторий аномального поведения
*	Returns:		0
*	Throws:			AxiomLibException - 
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::formAxiomSets (std::vector < std::vector <std::vector <int> > > &markUps, std::vector < std::vector <std::vector <int> > > &markUpsMinLens, std::vector < std::vector <std::vector <int> > > &markUpsMaxLens) {
	// Пока только заглушка

	return 0;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::saveResults
*
*	Description:	Сохранение сформированных систем аксиом и разметок траекторий 
*					аномального поведения
*	Parameters:		void
*	Returns:		int
*	Throws:			AxiomLibException - 
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::saveResults (void) {
	// Пока только заглушка

	return -1;
}


/****************************************************************************
*					FuzzyDataLearnAlgorithm::getExtraMarkUps
*
*	Description:	Функция создает разметки всех траекторий контрольной выборки и траекторий норомального
*					поведения заданной аксиомой.
*	Parameters:		markUps - разметки траекторий контрольной выборки и траекторий нормального поведения
*					correctMarkUps - корректные вхождения траекторий обучающей выборки в markUps
*					i - номер класса аномального поведения в наборе данных, 
*						который использовать для анализа
*					axNum - номер аксиомы в векторе bestAxioms, которую использовать
*							в данном алгоритме
*					param - размерность набора данных по которой вести анализ
*	Returns:		0
*	Throws:			AxiomLibException - при некорректном ответе от набора данных или некорректных входных параметрах
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::getExtraMarkUps (std::vector < std::vector <int> > &markUps, std::vector < std::vector <int> > &correctMarkUps, int axNum, int i, const int param) const {
	// Проверка входных параметров
	if (axNum >= (int) bestAxioms.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::getExtraMarkUps : incorrect input parameters.");
	// Получаем разметки всех траекторий контрольноый выборки для аксиомы axNum
	std::vector <double> row;
	bool bres;
	int curRes;
	int numOfTests = 0;
	int numOfNormalMultiTS;
	std::vector <int> numOfTS, numOfNormalTS;

	// запрос числа рядов
	fuzzyDataSet.getTestSize (numOfTests, numOfTS);
	// запрос числа рядов нормального поведения
	fuzzyDataSet.getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	if (numOfNormalMultiTS != numOfNormalTS.size())
		throw AxiomLibException("FuzzyDataLearnAlgorithm::getExtraMarkUps : incorrect response from internal function getNormalClassSize");

	// Проверка того - собрана ли уже корректная разметка
	bool correctEmpty = true;
	if (correctMarkUps.size () > 0)
		correctEmpty = false;
	else {
		correctMarkUps.resize (numOfTests + numOfNormalMultiTS);
	}
	// Изменение числа разметок траекторий
	markUps.resize(numOfTests + numOfNormalMultiTS);
	
	// Цикл по всем тестам - получение разметок их траекторий
	for (int testNum = 0; testNum < numOfTests; testNum++) {
		// Обработка корректной разметки
		if (correctEmpty) {
			fuzzyDataSet.getTSByIndexFromTests (correctMarkUps[testNum], testNum, -1);
			// Удаляем из разметки траекторий контрольной выборки всю информацию о вхождении эталонных траекторий отличных от i-го класса аномального поведения.
			for (unsigned int j = 0; j < correctMarkUps[testNum].size(); j++)
				if ((correctMarkUps[testNum][j] > 0) && (correctMarkUps[testNum][j] != i))
					correctMarkUps[testNum][j] = 0;
		}
		// Получение собственно разметки 
		bres = fuzzyDataSet.getTSByIndexFromTests (row, testNum, param);
		if (bres) {
			markUps[testNum].resize(row.size(), 0);
			for (unsigned int k = 0; k < row.size(); k++) {
				curRes = bestAxioms[axNum].check(k, row);
				if (curRes > 0) {
					markUps[testNum][k] = i;
				}
			}
		} else {
			throw AxiomLibException("Error in RecognizerExtend::getExtraMarkUps: some params are missing in Tests in DataSets.");
		}
	}

	// Разметка траекторий нормального поведения
	for (int j = 0; j < numOfNormalMultiTS; j++) {
		bres = fuzzyDataSet.getNormalTS (row, j, param);
		if (bres) {
			markUps[numOfTests + j].resize(row.size(), 0);
			for (unsigned int k = 0; k < row.size(); k++) {
				curRes = bestAxioms[axNum].check(k, row);
				if (curRes > 0) {
					markUps[numOfTests + j][k] = i;
				}
			}
			if (correctEmpty)
				correctMarkUps[numOfTests + j].resize(row.size(), 0);
		} else {
			throw AxiomLibException("Error in RecognizerExtend::getExtraMarkUps: some params are missing in NormalRows in DataSets.");
		}
	}

	return 0;
}



/****************************************************************************
*					FuzzyDataLearnAlgorithm::createAxiomSet
*
*	Description:	функция формирования системы аксиом из аксиом
*	Parameters:		param - размерность траекторий набора данных с которой работаем 
*					в данном алгоритме
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyDataLearnAlgorithm::createAxiomSet (const int param) {
	// Объявление необходимых переменных
	int numOfClasses;
	std::vector <int> numOfMultiTS;
	std::vector < std::vector <int> > numOfTS;
	std::vector < std::vector <int> > baseMarkUps;
	std::vector < std::vector <int> > candMarkUps;
	std::vector < std::vector <std::vector <int> > > markUps, markUpsMinLen, markUpsMaxLen; // Первая размерость - по типам аномального поведения, остальные - набор разметок
	std::vector < std::vector <std::vector <int> > > localMarkUps; // Для каждого типа аномального поведения, для каждой аксиомы - разметки всех траекторий обучающей выборки, содержащих траектории аномального поведения
	std::vector < std::vector <std::vector <int> > > extraMarkUps; // Для каждого типа аномального поведения, для каждой аксиомы - разметки всех траекторий контрольной выборки, содержащих траектории аномального поведения
	std::vector < std::vector <int> > correctExtraMarkUps; // Правильные ответы для траекторий контрольноый выборки

	// Получение инфомарции об обучающей выборке	
	fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	if ((numOfClasses != numOfMultiTS.size()) || (numOfClasses != numOfTS.size()))
		throw AxiomLibException("FuzzyDataLearnAlgorithm::createAxiomSet : incorrect response from internal function.");

	// Цикл по всем типам аномального поведения
	markUps.resize (numOfClasses);
	markUpsMinLen.resize (numOfClasses);
	markUpsMaxLen.resize (numOfClasses);
	for (int i = 0; i < numOfClasses; i++) {
		// 0. Проверка набора данных
		if (numOfMultiTS[i] != numOfTS[i].size())
			throw AxiomLibException("FuzzyDataLearnAlgorithm::createAxiomSet : incorrect response from internal function*.");
		
		// 1. Получаем разметки всех обучающих траекторий для данного типа аномального поведения и для всех аксиом
		//    Сдвигаем эти разметки до наилучшего соответствия и формируем обобщенные разметки
		baseMarkUps.clear();
		baseMarkUps.resize (bestAxioms.size());
		//    Цикл по всем сформироанным ранее аксиомам
		localMarkUps.resize(bestAxioms.size());
		extraMarkUps.resize(bestAxioms.size());
		correctExtraMarkUps.clear();
		for (unsigned int axNum = 0; axNum < bestAxioms.size(); axNum++) {	
			formGeneralizedMarkUps (baseMarkUps[axNum], localMarkUps[axNum], axNum, i, numOfMultiTS[i], param);
			getExtraMarkUps (extraMarkUps[axNum], correctExtraMarkUps, axNum, i+1, param);
		}

		// 2. По обобщенным разметкам для разных аксиом формируем кандидаты на разметки для данного типа аномального поведения
		createCandidates (markUps[i], markUpsMinLen[i], markUpsMaxLen[i], baseMarkUps, localMarkUps, extraMarkUps, correctExtraMarkUps, i+1);
		
		// 3. Отбор разметок для данного типа аномального поведения среди сформированных кандидатов
		//sortCandidateMarkUps (markUps[i], candMarkUps, param);
	}

	// Создание систем аксиом на основе анализа сформированных разметок
	formAxiomSets (markUps, markUpsMinLen, markUpsMaxLen);

	// Сохранение сформированных систем аксиом и разметок траекторий аномального поведения
	saveResults ();

	if (this->comments) {
		std::cout << "\n	Creating of AxiomSets is done. Third stage is over.\n";
		std::cout.flush();
	}

	return 0;
}
