/****************************************************************************
*				RecognizerMetric.cxx
*
*	Description:	Функции класса RecognizerMetric - Реализация
*	Author:		gevor
*	History:		
*
****************************************************************************/
#include <limits.h>

#include "RecognizerMetric.h"

using namespace AxiomLib;

// Стандартный конструктор - задание значений внутренних переменных по умолчанию
RecognizerMetric::RecognizerMetric (void) { 
	comments = false; 
	outputToFile = false;
	whereToSave = "./";
	saveDirName = "axiomSet_";
	curDir.clear();
	curFile.clear();
	precision = 0.0; 
	compareStatistic = NULL;
}

// Стандартный деструктор - удаление всех динамических объектов класса
RecognizerMetric::~RecognizerMetric () { 
	if (compareStatistic != NULL) delete compareStatistic; 
}

/****************************************************************************
*                       RecognizerMetric::setParamsFromEnv
*
*       Description:    Инициализировать параметры данными из env
*       Parameters:     const Environment& env - среда
*       Returns:        0
*       Throws:         AxiomLibException - если нет какого-топараметра
*       Author:         gevor
*       History:
*
****************************************************************************/
int RecognizerMetric::setParamsFromEnv (const Environment& env) {
	// инициализируем dataset
	std::string datasetDir, datasetName;
	if (env.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("RecognizerMetric::setParamsFromEnv : data set directory is undefined.");
	if (env.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("RecognizerMetric::setParamsFromEnv : data set is undefined.");

	// считываем необходимые для данного класса параметры о наборе данных
	EnvDataSet envDataSet;
	envDataSet.readConfigFile (datasetDir, datasetName);

	// установка корректного обозначения NullStr - обозначение остутсвия в данной точке ряда какого либо нештатного поведения
	dataSet.setNullStr (envDataSet);
	// собственно считываем набор данных - заводим его во внутреннее представление
	dataSet.readDataSet(datasetDir, datasetName);

	// восстанавливаем в данном классе вектор индексов параметров в каноническом представленнии по которым вести поиск нештатых ситуаций
	dataSet.getParamNums(params, env, envDataSet);

	// Установка параметров для класса набора разметок эталонных траекторий
	rms.setParamsFromEnv(env);

	// установка параметров относящикся к MarkUp
	if (env.getDoubleParamValue(precision, "MaxDistance") < 0)
		throw AxiomLibException("RecognizerMetric::setParamsFromEnv : precision is undefined.");
 	//std::cout << "\n Precision set to " << precision << " !\n";
	// создание класса подсчета статистики

	std::string compClassName;
	CompareClassFactory ccf;
	if (env.getStringParamValue(compClassName, "compareClass") < 0)
		throw AxiomLibException("RecognizerMetric::setParamsFromEnv : compare-class is undefined.");
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
*                       RecognizerMetric::setNominalParamsFromEnv
*
*       Description:	Инициализировать только основные параметры данными из env
*       Parameters:	const Environment& env - среда
*       Returns:		0
*       Throws:		AxiomLibException - если нет какого-то параметра
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerMetric::setNominalParamsFromEnv (const Environment& env) {
	// установка параметров относящикся к разметке
	if (env.getDoubleParamValue(precision, "precision") < 0)
		throw AxiomLibException("RecognizerMetric::setParamsFromEnv : precision is undefined.");
	
	// Установка параметров для класса набора разметок эталонных траекторий
	rms.setParamsFromEnv(env);
	
	// создание класса подсчета статистики
	std::string compClassName;
	CompareClassFactory ccf;
	if (env.getStringParamValue(compClassName, "compareClass") < 0)
		throw AxiomLibException("RecognizerMetric::setParamsFromEnv : compare-class is undefined.");
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
*                       RecognizerMetric::setDataSet
*
*       Description:	Функция установки нового набора данных в текущий класс распознователя
*       Parameters:	newDataSet - новый набор данных
*       Returns:		0
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerMetric::setDataSet (const DataSet &newDataSet) {
	dataSet = newDataSet;
	return 0;
}


/****************************************************************************
*                       RecognizerMetric::setParamsOfDataSet
*
*       Description:	Функция установки нового значаения переменой класса params
*       Parameters:	newParams - новое значение вектора
*       Returns:		0
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int RecognizerMetric::setParamsOfDataSet (std::vector<int> &newParams) {
	params = newParams;
	return 0;
}


/****************************************************************************
*                       RecognizerMetric::setPrecision
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
int RecognizerMetric::setPrecision (const double newPrecision) {
	if ( (newPrecision > 0.0) && (newPrecision < 1.0) ) {
		precision = newPrecision;
		return 0;
	}
	return -1;
}


/****************************************************************************
*                       RecognizerMetric::setCompareStatistic
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
int RecognizerMetric::setCompareStatistic (CompareStatistic *newCompareStatistic) {
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
*                       RecognizerMetric::setRMS
*
*       Description:	Функция установки нового класса разметок
*       Parameters:		newRMS - новое значение, копия которого 
*						устанавливается как новый класс разметок
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int RecognizerMetric::setRMS (RefMarkupSetSimple &newRMS) {
	rms = newRMS;
	return 0;
}


/****************************************************************************
*                       RecognizerMetric::copy
*
*       Description:	Функция возвращает копию данного распознавателя
*       Parameters:		-
*       Returns:		Recognizer* - ссылка на копию данного распознавателя
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
Recognizer* RecognizerMetric::copy (void) {
	RecognizerMetric* toReturn;
	toReturn = new RecognizerMetric;
	toReturn->setDataSet (dataSet);
	toReturn->setParamsOfDataSet (params);
	toReturn->setPrecision (precision);
	toReturn->setCompareStatistic (compareStatistic);
	toReturn->setRMS(rms);
	toReturn->setOutputToFile(outputToFile, whereToSave, saveDirName);
	return (Recognizer*) toReturn;
}


/****************************************************************************
*						RecognizerMetric::learn
*
*       Description:	Функция обучения данного алгоритма распознавания
*       Parameters:		templateRecognizer - шаблон распознавателя, на котором
*						проверять качество обучения данного алгоритма распознавания
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
double RecognizerMetric::learn (TemplateRecognizer &templateRecognizer) {

	return -1.0;
}


/****************************************************************************
*				RecognizerMetric::joinResults
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
inline signed int RecognizerMetric::joinResults (std::vector <int> &result, std::vector < std::vector <int> > &prelimResult, const int numOfClasses) const {
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
		throw AxiomLibException("RecognizerMetric::joinResults: incorrect results in recognition process.");
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
		for (unsigned int t = 0; t < bulletin.size(); t++) {
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
*				RecognizerMetric::recognition
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
inline signed int RecognizerMetric::recognition (int &first, int &second,  std::vector <std::vector <int> > &markUps, 
													std::vector <int> &corrMarkUp, 
													std::vector < std::vector <RefMarkupSetSimple> > &refs,
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
	int flag = 0;
	double tmpFirst, tmpSecond; // пременный для подсчета числа ошибок на каждом из тестов
	int wholeNum; // число образов на каждом из тестов
	std::vector < std::vector <int> > prelimResult;
	prelimResult.resize(markUps.size());
	std::vector <int> result;
	first = 0;
	second = 0;
	
	// 1. цикл по всем рассматриваемым размерностям временных рядов
	for (int j = 0; j < (int) markUps.size(); j++) {
		(prelimResult[j]).resize((markUps[j]).size());
		prelimResult[j][0] = 0;
		// 1. поиск нештатных ситуаций непосредственно по markUps[j]
		for (long int i = 1; i < (long int) markUps[j].size(); i++) {
			prelimResult[j][i] = 0;
			flag = 0;
			// Определяем есть ли совпадения с эталонными рядами - т.е. ищем хотя бы 1 класс неисправности
			for (int k = 0; k < (int) refs[j].size(); k++) {
				flag = (refs[j][k]).check (precision, markUps[j], i);
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
		throw AxiomLibException("Error in RecognizerMetric::recognition: incorrect results in process of getting statistic.");
	
	// 4. Заполнение полей по статистике и возврат из функции
	first = (int) tmpFirst;
	second = (int) tmpSecond;
	return 0;
}


/****************************************************************************
*				RecognizerMetric::initRefs
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
inline signed int RecognizerMetric::initRefs (AxiomSetPop& pop, const int asNum, const int param, std::vector <RefMarkupSetSimple> &refer) {
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
*				RecognizerMetric::run
*
*	Description:	Запуск процесса распознавания
*	Parameters:	AxiomSetPop& pop - популяция, с которой работаем
*	Returns:		0
*	Throws:		AxiomLibExeption - если популяция нулевого размера, или не инициализирована, или залочена
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int RecognizerMetric::run (AxiomSetPop& pop) {
	const bool debug = false;
	if (debug) std::cout << "\n	Recognizer:";
	// проверка; в случае лока или не инициализированной популяции, AxiomSetPop кинет exception
	if (pop.size() == 0)
		throw AxiomLibException("RecognizerMetric::run - population of zero size.");
	// подготовка внутренних переменных
	bool bres;
	int first, second, tmpFirst, tmpSecond;
	double tmpGoal;
	std::vector <double> row;
	std::vector <std::vector <int> > markUps;
	std::vector <int> corrMarkUp;
	std::vector <double> corrMarkUpDouble;
	std::vector < std::vector <RefMarkupSetSimple> > refs; // вектор разметок - по каждому из параметров предоставляет доступ к набору эталонных разметок
	int numOfTests = 0;
	std::vector<int> numOfTS;
	// запрос числа тестовых рядов
	bool c = dataSet.getTestSize (numOfTests, numOfTS);
	// соответствующее изменение размерностей используемых векторов для хранения разметок по определенной системе каксиом
	markUps.resize(params.size());
	refs.resize(params.size());
	// ходим по всем системам аксиом в популяции
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
			dataSet.getTSByIndexFromTests (corrMarkUpDouble, t, -1);
			// Перевод вектора корректных разметок в целые числа
			if (debug) std::cout << "        Resizing";
			corrMarkUp.resize(corrMarkUpDouble.size());
			for (unsigned int r = 0; r < corrMarkUpDouble.size(); r++) corrMarkUp[r] = (int) corrMarkUpDouble[r];
			for (unsigned int prm = 0; prm < params.size(); prm++) {
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
					throw AxiomLibException("Error in RecognizerMetric::run: some params are missing in Tests in DataSets.");
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
		if (debug) std::cout << "		As closed";
	}
	return 0;
}


// Провести разметку рядов в системе аксиом,
  // прогнать распознаватель, посчитать количество
  // ошибок первого и второго рода
signed int RecognizerMetric::run (AxiomSetPop& pop, std::vector <double> &precs) {
	run (pop);
	return 0;
}


/****************************************************************************
*					RecognizerMetric::saveRows
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
int RecognizerMetric::saveRows (std::vector <double> &row, std::vector<int> &markUp, std::string paramName) const {
	if (markUp.size() != row.size())
		throw AxiomLibException("Error in RecognizerMetric::clearPath: given current dir - is unreacheable.");
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
*					RecognizerMetric::genFileName
*
*	Description:	Функция создает имя файла для текущей записи и сохраняет его во внутренней переменной класса
*	Parameters:		testNum - номер теста в используемом наборе данных
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int RecognizerMetric::genFileName (int testNum) {
	curFile = curDir;
	char tmpStr[10];
	sprintf(tmpStr,"%d",testNum + 1);
	curFile.append("/");
	curFile.append(tmpStr);
	curFile.append(".csv");
	return 0;
}


/****************************************************************************
*					RecognizerMetric::clearPath
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
int RecognizerMetric::clearPath (std::string asName, int asIndex) {
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
		throw AxiomLibException("Error in RecognizerMetric::clearPath: given current dir - is unreacheable.");
	boost::filesystem::path dirPath( pathToDir );
	if ( boost::filesystem::exists(dirPath) )
		boost::filesystem::remove_all (dirPath);
	boost::filesystem::create_directory (dirPath);
	curDir = pathToDir;
	return 0;
}


/****************************************************************************
*					RecognizerMetric::searchFreeIndex
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
int RecognizerMetric::searchFreeIndex (const std::string &curPath) const {
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
*					RecognizerMetric::updateRows
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
int RecognizerMetric::updateRows (std::vector <int> &corrMarkUp, std::vector<int> &result) const {
	if (corrMarkUp.size() != result.size())
		throw AxiomLibException("Error in RecognizerMetric::clearPath: given current dir - is unreacheable.");
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
*					RecognizerMetric::saveRefs
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
int RecognizerMetric::saveRefs (std::vector <RefMarkupSetSimple> &ref, std::string paramName) const {
	// Проверяем -существует ли директория, в которой сохранять файлы
	boost::filesystem::path dirPath( curDir );
	if ( !boost::filesystem::exists(dirPath) )
		return -1;
	// Создаем необходимые переменные
	std::string fileName, secName;
	bool secondHand;
	// Сохраняем содержимое ref в файле с адресом fileName
	std::vector < std::vector <int> > axioms;
	for (unsigned int i = 0; i < ref.size(); i++) { // цикл по различным типам аномального поведения
		// Собираем информацию - что выводить в файл
		axioms.resize (ref[i].refMarkups.size());
		for (unsigned int j = 0; j < ref[i].refMarkups.size(); j++) {
			ref[i].refMarkups[j].getCopy(axioms[j]);
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
				file << "axioms";
				if (l > 0) {
					file << "_" << l;
				}
				file <<";";
				// вычисляем максимальную длину вектора
				if (axioms[l].size() > mLen)
					mLen = axioms[l].size();
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
						file << "\t" << axioms[l][k] << ";";
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
				file << "axioms";
				if (l > 0) {
					file << "_" << l;
				}
				file <<";";
				// вычисляем максимальную длину вектора
				if (axioms[l].size() > mLen)
					mLen = axioms[l].size();
			}
			file << "\n";
			// Содержимое разметок
			for (unsigned int k = 0; k < mLen; k++) {
				for (unsigned int l = 0; l < axioms.size(); l++) {
					if (k < axioms[l].size()) {
						file << "\t" << axioms[l][k] << ";";
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
