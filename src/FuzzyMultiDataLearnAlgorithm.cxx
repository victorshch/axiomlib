/****************************************************************************
*
*				Функции класса FuzzyMultiDataLearnAlgorithm
*
****************************************************************************/

#include "FuzzyMultiDataLearnAlgorithm.h"
#include "Defines.h"

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

using namespace AxiomLib;

// Набор определений, специфичных для данного класса
// Шаблоны для имен файлов с описаниями лучших элементарных условий и аксиом
#define str_defaultECName		"stageOneFuzzyData_"  // имя по умолчанию для элементарных условий, полученных на первом этапе
#define str_defaultAxiomName	"stageTwoFuzzyData_"  // имя по умолчанию для аксиом, полученных на втором этапе
// Имена файлов по умолчанию для сохранения списков имен файлов с описаниями аксиом и элементарных условий
#define str_default_ECList_fileName		"bestECList"
#define str_default_AxiomList_fileName	"bestAxiomList"

// Коструктор класса
FuzzyMultiDataLearnAlgorithm::FuzzyMultiDataLearnAlgorithm (void) {
	// Определяем по умолчанию не выводить никаких комментариев
	comments = false;
	recognizer = NULL;
	//compareStatistic = NULL;
	goalStrategy = NULL;
	ccNumPoints = 10;
	//bestECFileNames.clear();
	predefinedECFileName.clear();
	bestECForAllAbnormalTypes.clear();
 
	axiomNameTemplate.assign (str_defaultAxiomName);
	ecNameTemplate.assign (str_defaultECName);
	fileNameECList.assign (str_default_ECList_fileName);
	fileNameAxiomList.assign (str_default_AxiomList_fileName);
	axiomBaseDir.clear();
	dataSetParams.clear();
	axiomSetBaseDir.clear();
	axiomSetName.clear();
	bestAxiomsFileNames.clear();
	predefinedAxiomsFileName.clear();

	stage = 0;
	
	shiftErrIn = 10.0;
	extendMarkUpLensLevel = 0.0;

	numBestECs = 20;
	numBestECsMax = 40;
	maxAxiomPopSize = 30;
	maxIterNum = 100;
	maxAxiomRes = 1.0;
	percentBestAxioms = 1.0;

	maxAxiomSetPopSize = 100;
	maxNumberOfSteps = 10;
	bestAxiomSetGoal = 0.0;
	numberOfBestAxiomSets = 10;
	percentBestAxiomSets = 1.0;

	numberOfUsedClassTraj = 0;

	//Заполнение вектора типов ЭУ - по умолчанию используются все типы ЭУ
	ElemConditionsFactory ecf;
	initialECTypes.resize (2*N_EL_AXIOMS);
	for (unsigned int i = 0; i < N_EL_AXIOMS; i++) {
		initialECTypes[2*i].elemCondition = ecf.createAxiomByNum(i+1);
		initialECTypes[2*i].sign = true;
		initialECTypes[2*i].dimension = -1;
		initialECTypes[2*i+1].elemCondition = ecf.createAxiomByNum(i+1);
		initialECTypes[2*i+1].sign = false;
		initialECTypes[2*i+1].dimension = -1;
	}
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
FuzzyMultiDataLearnAlgorithm::~FuzzyMultiDataLearnAlgorithm (void) {
	if (recognizer != NULL) delete recognizer;
	//if (compareStatistic != NULL) delete compareStatistic;
	if (goalStrategy != NULL) delete goalStrategy;
	/*
	for (unsigned int i = 0; i < bestAxioms.size(); i++) {
		bestAxioms[i].clear();
	}*/
	for (unsigned int i = 0; i < bestAxiomSets.size(); i++) {
		bestAxiomSets[i].clear();
	}
}


/****************************************************************************
*				FuzzyMultiDataLearnAlgorithm::initFromEnv
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
int FuzzyMultiDataLearnAlgorithm::initFromEnv (const Environment& env) {
	// Инициализируем dataSet
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : data set is undefined.");
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

	// Определяем базовую директорию с наборами аксиом
	if (env.getStringParamValue(axiomBaseDir, "AxiomBaseDir") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : popPreset > 0 but axiom base dir is not set.");
	std::string curAxiomName;
	if (env.getStringParamValue(curAxiomName, "AxiomNameTemplate") >= 0) 
		axiomNameTemplate.assign (curAxiomName);
	if (env.getStringParamValue(curAxiomName, "ECNameTemplate") >= 0) 
		ecNameTemplate.assign (curAxiomName);
	if (env.getStringParamValue(curAxiomName, "ECListFileName") >= 0) 
		fileNameECList.assign (curAxiomName);
	if (env.getStringParamValue(curAxiomName, "AxiomListFileName") >= 0) 
		fileNameAxiomList.assign (curAxiomName);

	// Сохранение файлов с описанием элементарных условий во внутрнеей переменной для последующего своевременного включения этого набора в рассмотрение
	env.getStringSetParamValue(predefinedECFileName, "ECFileName");

	// Сохранение файлов с описанием аксиом во внутрнеей переменной для последующего своевременного включения этого набора в рассмотрение
	env.getStringSetParamValue(predefinedAxiomsFileName, "AxiomFileName");

	// Параметры, по которым сохранять лучщие решения
	if (env.getStringParamValue(axiomSetBaseDir, "AxiomSetBaseDir") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : axiomSetBaseDir directory is undefined.");
	if (env.getStringParamValue(axiomSetName, "AxiomSetNameTemplate") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : axiomName to save is undefined.");

	// создание класса распознавания участков разметки в ряду разметки
	std::string recogClassName;
	ReducedRecognizerFactory rrf;
	if (env.getStringParamValue(recogClassName, "ReducedRecognizer") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::initFromEnv: ReducedRecognizer class is undefined.");
	recognizer = rrf.create(recogClassName);
	recognizer->setParamsFromEnv (env);

	// создание класса подсчета статистики
	/*std::string compClassName;
	CompareClassFactory ccf;
	if (env.getStringParamValue(compClassName, "compareClass") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::initFromEnv: compare-class is undefined.");
	compareStatistic = ccf.create(compClassName);
	compareStatistic->setParamsFromEnv(env);*/
	if (env.getIntParamValue (ccNumPoints, "ccNumPoints") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : ccNumPoints is undefined.");

	// Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") < 0) 
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::initFromEnv: goal-class is undefined.");
	goalStrategy = gsf.create(goalStrategyName);
	goalStrategy->setParamsFromEnv(env);

	// Параметры алгоритма настройки элементарных условий
	if (env.getIntParamValue (leftLimit, "leftLimit") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : leftLimit is undefined.");
	if (env.getIntParamValue (rightLimit, "rightLimit") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : rightLimit is undefined.");
	if (env.getDoubleParamValue (reserve, "reserve") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : reserve is undefined.");
	if (env.getIntParamValue (numOfLevels, "numOfLevels") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numOfLevels is undefined.");
	if (env.getIntParamValue (numBestECs, "numBestECs") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numBestECs is undefined.");
	if (env.getIntParamValue (numBestECsMax, "numBestECsMax") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numBestECsMax is undefined.");

	// Параметры поиска аксиом
	if (env.getIntParamValue (maxAxiomPopSize, "maxAxiomPopSize") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : maxAxiomPopSize is undefined.");
	if (env.getIntParamValue (maxIterNum, "axiomAlgIterNum") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : axiomAlgIterNum is undefined.");
	if (env.getDoubleParamValue (maxAxiomRes, "maxCostForAxiom") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : maxCostForAxiom is undefined.");
	if (env.getDoubleParamValue (percentBestAxioms, "percentBestAxioms") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : percentBestAxioms is undefined.");

	// Стоимость ошибки, если при сдвиге разметок осталась область разметки без соответствия в другой разметке
	if (env.getDoubleParamValue (shiftErrIn, "shiftErrIn") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : shiftErrIn is undefined.");
	// Степень расширения длины вхождения разметки (при поиске разметок во время их подбора)
	if (env.getDoubleParamValue (extendMarkUpLensLevel, "extendMarkUpLensLevel") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : extendMarkUpLensLevel is undefined.");
	if (env.getIntParamValue (maxAxiomSetPopSize, "maxAxiomSetPopSize") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : maxAxiomSetPopSize is undefined.");
	if (env.getIntParamValue (maxNumberOfSteps, "maxNumberOfSteps") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : maxNumberOfSteps is undefined.");
	if (env.getDoubleParamValue (bestAxiomSetGoal, "bestAxiomSetGoal") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : bestAxiomSetGoal is undefined.");
	if (env.getIntParamValue (numberOfBestAxiomSets, "numberOfBestAxiomSets") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numberOfBestAxiomSets is undefined.");
	if (env.getDoubleParamValue (percentBestAxiomSets, "percentBestAxiomSets") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : percentBestAxiomSets is undefined.");
	if ((percentBestAxiomSets <= 0.0) || (percentBestAxiomSets > 1.0))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : percentBestAxiomSets is out of range.");
	
	if (env.getIntParamValue (numberOfUsedClassTraj, "numberOfUsedClassTraj") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numberOfUsedClassTraj is undefined.");

	return 0;
}


/****************************************************************************
*				FuzzyMultiDataLearnAlgorithm::initFromEnvRecognitionOnly
*
*	Description:	Функция устанавливает параметры объекта по данным из 
*					окружения - необходимые только для расопзнавания и 
*					подсчета целевой функции для системы аксиом
*	Parameters:		env - объект класса окружения, откуда берутся параметры данного класса
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::initFromEnvRecognitionOnly (const Environment& env) {
	// Инициализируем dataSet
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : data set is undefined.");
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

	// создание класса распознавания участков разметки в ряду разметки
	std::string recogClassName;
	ReducedRecognizerFactory rrf;
	if (env.getStringParamValue(recogClassName, "ReducedRecognizer") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::initFromEnv: ReducedRecognizer class is undefined.");
	recognizer = rrf.create(recogClassName);
	recognizer->setParamsFromEnv (env);

	if (env.getIntParamValue (ccNumPoints, "ccNumPoints") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : ccNumPoints is undefined.");

	// Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") < 0) 
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::initFromEnv: goal-class is undefined.");
	goalStrategy = gsf.create(goalStrategyName);
	goalStrategy->setParamsFromEnv(env);

	// Число траекторий, используемых для построения разметок эталонных траекторий
	if (env.getIntParamValue (numberOfUsedClassTraj, "numberOfUsedClassTraj") < 0)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : numberOfUsedClassTraj is undefined.");

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::setComments
*
*	Description:	Устанавливает новое значение переменной класса comments
*	Parameters:		newVal - новое значение переменной comments
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::setComments (const bool newVal) {
	this->comments = newVal;
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::run
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
int FuzzyMultiDataLearnAlgorithm::run (void) {
	// Получение информации о ранге текущего процесса
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	// Начальная стадия процесса построения системы аксиом и поиска разметок
	stage = 0;

	// ЭТАП 1. подбор параметров элементарных условий
	selectElemCond();
	// Сохранение списка файлов в отдельный файл - отчет
	std::string fileName;
	if (this->comments) {
		std::cout << "\n\tProcess " << rank+1 << " from " << size << ": saving results of 1st stage'\n";
	}
	fileName = saveBestECFileNames (rank);
	if (this->comments)
		std::cout << "\n\tProcess " << rank+1 << " from " << size << ": results saved to: '" << fileName << "'\n";
	// Этап завершен.
	stage = 1;
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tForming of Elementary Conditions is done. First stage is over.\n";
		std::cout.flush(); }

	// ЭТАП 2. Формирование аксиом из подобранных ранее элементарных условий
	// Вывод на экран стадии процесса
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tCreating of axioms from elementary conditions has began. Second stage has began.\n";
		std::cout.flush(); }
	// Поиск аксиом
	formAxioms ();
	// Сохранение списка файлов в отдельный файл - отчет
	if (rank == 0) {
		fileName = saveBestAxiomFileNames ();
		if (this->comments)
			std::cout << "\n\tResults saved to: '" << fileName << "'\n";
	}
	// Завершен поиск аксиом
	stage = 2;
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tForming of axioms is done. Second stage is over.\n";
		std::cout.flush(); }

	// ЭТАП 3. формирование системы аксиом из построенных аксиом
	// Вывод на экран стадии процесса
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tCreating of axiomSets from axioms has began. Third stage has began.\n";
		std::cout.flush(); }
	// Построение систем аксиом
	createAxiomSet ();
	// Завершено построение системы аксиом
	stage = 3;
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tCreating of AxiomSets is done. Third stage is over.\n";
		std::cout.flush(); }
	// Вывод на экран лучших значений целевой функции среди всех полученных результатов
	if (rank == 0)
		displayBestResult();

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::runFirstLevel
*
*	Description:	Функция реализует подбор параметров элементарных условий
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string FuzzyMultiDataLearnAlgorithm::runFirstLevel (void) {
	// Получение информации о ранге текущего процесса
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// Начальная стадия процесса построения системы аксиом и поиска разметок
	stage = 0;
	if ((!rank) && (this->comments)) {
		std::cout << "\n	Forming of Elementary Conditions has began. First stage has began.\n";
		std::cout.flush();
	}
	// ЭТАП 1. подбор параметров элементарных условий
	selectElemCond();
	// Сохранение списка файлов в отдельный файл - отчет
	std::string toRet;
	toRet = saveBestECFileNames (rank);
	// Этап завершен.
	stage = 1;
	if ((!rank) && (this->comments)) {
		std::cout << "\n	Forming of Elementary Conditions is done. First stage is over.\n";
		std::cout.flush();
	}
	return toRet;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::runSecondLevel
*
*	Description:	Функция реализует формирование аксиом из подобранных ранее элементарных условий
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string FuzzyMultiDataLearnAlgorithm::runSecondLevel (std::vector <std::string> &ecListFileVecs) {
	// Получение информации о ранге текущего процесса
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if ((rank < 0) || (rank >= (int) ecListFileVecs.size())) {
		//throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::runSecondLevel : incorrect input vector of strings - or incorrect number of started processes.");
		std::string toRet;
		toRet.clear();
		return toRet;
	}
	return runSecondLevel(ecListFileVecs[rank]);
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::runSecondLevel
*
*	Description:	Функция реализует формирование аксиом из подобранных ранее элементарных условий
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string FuzzyMultiDataLearnAlgorithm::runSecondLevel (const std::string ecListFilePath) {
	// Считываем имена файлов во внтуреннюю перменную
	restoreBestECFileNames (ecListFilePath);
	// Получение информации о ранге текущего процесса
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// ЭТАП 2. Формирование аксиом из подобранных ранее элементарных условий
	// Вывод на экран стадии процесса
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tCreating of axioms from elementary conditions has began. Second stage has began.\n";
		std::cout.flush();
	}
	// Поиск аксиом
	formAxioms ();	
	// Завершен поиск аксиом - сохраняем именя файлов с описаниями аксиом в файлыs
	std::string toRet;
	toRet.clear();
	if (rank == 0)
		toRet = saveBestAxiomFileNames ();
	// Вывод информации о стадии процесса
	stage = 2;
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n	Forming of axioms is done. Second stage is over.\n";
		std::cout.flush();
	}
	return toRet;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::runThirdLevel
*
*	Description:	Функция реализует подбор параметров элементарных условий
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string FuzzyMultiDataLearnAlgorithm::runThirdLevel (const std::string axiomListFilePath) {
	// Считываем имена файлов во внтуреннюю перменную
	restoreBestAxiomFileNames (axiomListFilePath);
	// Получение информации о ранге текущего процесса
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// ЭТАП 3. формирование системы аксиом из построенных аксиом
	// Вывод на экран стадии процесса
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n\tCreating of axiomSets from axioms has began. Third stage has began.\n";
		std::cout.flush();
	}
	// Построение систем аксиом
	createAxiomSet ();
	// Завершено построение системы аксиом
	stage = 3;
	if ((rank == 0) && (this->comments)) {
		std::cout << "\n	Creating of AxiomSets is done. Third stage is over.\n";
		std::cout.flush();
	}
	// Вывод на экран лучших значений целевой функции среди всех полученных результатов
	if (rank == 0)
		displayBestResult();

	// На будущее - здесь можно будет сохранять имена лучших систем аксиом с из характеристиками в файл - и выдавать в качестве отчета
	std::string toRet;
	toRet.clear();

	return toRet;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::saveBestECFileNames
*
*	Description:	Функция реализует сохранение всех имен файлов с описаниями
*					элементарных условий в файл.
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string FuzzyMultiDataLearnAlgorithm::saveBestECFileNames (const int rank) const {
	std::string curFileName;
	curFileName.assign (fileNameECList);
	char tmss[10]; 
	tmss[0]='\0';
	sprintf(tmss, "%d", rank + 1);
	curFileName.append ("_rank_");
	curFileName.append (tmss);
	AxiomBase axiomBase;
	axiomBase.saveToList (axiomBaseDir, curFileName, bestECForAllAbnormalTypes);
	std::string path;
	path = axiomBaseDir; 
	path.append ("/");
	path.append (curFileName);
	return path;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::restoreBestECFileNames
*
*	Description:	Функция реализует считывание имен файлов с описаниями
*					элементарных условий из файла.
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::restoreBestECFileNames (void) {
	std::string filePath;
	filePath.assign (axiomBaseDir);
	filePath.append ("/");
	filePath.append (fileNameECList);
	return restoreBestECFileNames (filePath);
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::restoreBestECFileNames
*
*	Description:	Функция реализует считывание имен файлов с описаниями
*					элементарных условий из файла.
*	Parameters:		fileName - имя файла, из которого читать названия файлов
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::restoreBestECFileNames (const std::string filePath) {
	bestECForAllAbnormalTypes.clear();
	AxiomBase axiomBase;
	axiomBase.readFromList (filePath, bestECForAllAbnormalTypes);
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::saveBestAxiomFileNames
*
*	Description:	Функция реализует сохранение всех имен файлов с описаниями
*					лучших аксиом в файл
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string FuzzyMultiDataLearnAlgorithm::saveBestAxiomFileNames (void) const {
	AxiomBase axiomBase;
	axiomBase.saveToList (axiomBaseDir, fileNameAxiomList, bestAxiomsFileNames);
	std::string path;
	path = axiomBaseDir; 
	path.append ("/");
	path.append (fileNameAxiomList);
	return path;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::restoreBestAxiomFileNames
*
*	Description:	Функция реализует считывание имен файлов с описаниями
*					аксиом из файла.
*	Parameters:		void
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::restoreBestAxiomFileNames (void) {
	std::string filePath;
	filePath.assign (axiomBaseDir);
	filePath.append ("/");
	filePath.append (fileNameAxiomList);
	return restoreBestAxiomFileNames (filePath);
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::restoreBestAxiomFileNames
*
*	Description:	Функция реализует считывание имен файлов с описаниями
*					аксиом из файла.
*	Parameters:		fileName - имя файла, из которого читать названия файлов
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::restoreBestAxiomFileNames (const std::string filePath) {
	bestAxiomsFileNames.clear();
	AxiomBase axiomBase;
	axiomBase.readFromList (filePath, bestAxiomsFileNames);
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::splitRange
*
*	Description:	Функция определяет границы разбиения области поиска для данного процесса
*	Parameters:		rank - ранг текущего процесса
*					size - общее число процессов, учавствующих в работе алгоритма
*					rangeMin - ранг текущего процесса
*					rangeMax - общее число процессов, учавствующих в работе алгоритма
*					from - заполняемое значение, левая граница назначенного интервала для данного процесса
*					upTo - заполняемое значение, правая граница назначенного интервала для данного процесса
*					rankFrom - заполняемое значение, левая граница назначенного интервала для данного процесса
*					rankUpTo - заполняемое значение, правая граница назначенного интервала для данного процесса
*	Returns:		0
*	Throws:			AxiomLibException - если входные параметры не согласованны
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::splitRange (const int rank, const int size, const int rangeMin, const int rangeMax, int &from, int &upTo, int &rankFrom, int &rankUpTo) const {
	int groupNum;
	double classesPerGroup;
	int numOfClasses;
	// Число элементов в разбиваемой области
	numOfClasses = rangeMax - rangeMin;
	if ((numOfClasses < 1) || (size < 1) || (rank < 0) || (rank >= size))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::splitRange : incorrect input parameters.");
	// Число групп, на которое разбивается область
	groupNum = min (size, numOfClasses);
	std::vector <int> groupDiv, rankDiv;
	rankDiv.resize ((int) groupNum + 1, 0);
	rankDiv[groupNum] = size;
	groupDiv.resize(size);
	int initGroup = 0; 
	for (int h = 0; h < size; h++) {
		if (initGroup != (int) ((double) (h) * (double) groupNum / (double) size)) {
			rankDiv[initGroup + 1] = h;
			initGroup++;
		}
		groupDiv[h] = initGroup;
	}
	classesPerGroup = (double) numOfClasses / (double) groupNum;
	from = rangeMin + (int) (groupDiv[rank] * classesPerGroup);
	upTo = rangeMin + (int) ((groupDiv[rank] + 1) * classesPerGroup);
	rankFrom = rankDiv[groupDiv[rank]];
	rankUpTo = rankDiv[groupDiv[rank]+1];
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::displayBestResult
*
*	Description:	Функция выводит на экран значения целевой функции для 
*					лучших систем аксиом, хранящихся в локальной переменной класса.
*	Parameters:		-
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::displayBestResult(void) const {
	// Сортируем вектор маски - для вывода на экран отсортированных значений
	std::vector <unsigned int> mask;
	mask.resize(bestAxiomSetsNamesPlus.size()); 
	for (unsigned int i = 0; i < mask.size(); i++) 
		mask[i] = i;
	unsigned int temp;
	for (unsigned int i = mask.size() - 1; i > 0; i-- ) {
		for (unsigned int j = 0; j < i; j++) {
			if (bestAxiomSetsNamesPlus[mask[j]].doubleVal > bestAxiomSetsNamesPlus[mask[j+1]].doubleVal) {
				temp = mask[j+1];
				mask[j+1] = mask[j];
				mask[j] = temp;
			}
		}
	}
	// При выводе на экран - назавния систем аксиом будем выводить такие же, какие они были при сохранении в файлы
	std::cout << "\n\tBest results are:";
	for (unsigned int asNum = 0; asNum < mask.size(); asNum++) {
		AxiomExprSetPlus axiomSet;
		axiomSet.initAxiomSetFromFile(axiomSetBaseDir, bestAxiomSetsNamesPlus[mask[asNum]].str, fuzzyDataSet.getParamNames());
		printf ("\n\t%d.\t%s\t%.4f\t(%d,%d)", asNum + 1, bestAxiomSetsNamesPlus[mask[asNum]].str.c_str(), bestAxiomSetsNamesPlus[mask[asNum]].doubleVal, axiomSet.errFirst, axiomSet.errSecond);
	}
	std::cout << "\n";

	return 0;
}
