/****************************************************************************
*
*				Функции класса StartStrategy
*
****************************************************************************/

#include "StartStrategy.h"

using namespace AxiomLib;


// Коструктор класса с заданием начальных параметров по умолчанию
StartStrategy::StartStrategy (void) {
	gaVector.clear();
	popVector.clear();
	treatVector.clear();
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
StartStrategy::~StartStrategy (void) {}


/****************************************************************************
*				StartStrategy::initFromEnvStrart
*
*	Description:	Функция инициализации переменных класса путем чтения конфигурационных файлов envStart
*	Parameters:	-
*	Returns:		-
*	Throws:		AxiomLibException - 
*	Author:		dk
*	History:
*
****************************************************************************/
void StartStrategy::init (void) {
	// Считывание параметров данного класса
	Environment envStart;
	envStart.readConfigFile("./axiomlib_start.conf"); // если таких классов как этот будет несколько - то пусть каждый отвечает за себя - знает какой файл с параметрами к нему относится 
	
	// считывание числа стартов с каждым из классов предобработки с различными параметрами 
	// (то есть если numOfStarts равно 5, а число используемых классов предобработки составляет 10, 
	// то всего будет сделано 50 стартов ГА подбора системы аксиом - по каждому классу предобработки с различными пареметрами )
	if (envStart.getIntParamValue(numOfStarts, "numOfStarts") < 0)
		throw AxiomLibException("StartStrategy::init : data set directory is undefined.");	
	
	// Инициализация классов предобработки:
	// 	считываем все названия используемых классов предобработки 
	// 	по названию класса предобработки вероятно в конструкторе классов создаем соответствующий объект
	TreatmentFactory tf;
	std::set<std::string> names;
	if (envStart.getStringSetParamValue (names, "Treatment") < 0) // ВАЖНО _ В КОНФИГЕ СТРАТЕГИИ ПОДБОРА ДОЛЖНЫ ИДТИ С ШАПКОЙ TREATMENT - ЭТО МОЖНО ПОМЕНЯТЬ ЗДЕСЬ
		throw AxiomLibException("Error in StartStrategy::init : treatment strategies are undefined.");
	if (names.size() < 1) 
		throw AxiomLibException("Error in StartStrategy::init : incorrect number of treatment strategies.");
	treatVector.resize(names.size() * numOfStarts);
	std::set<std::string>::iterator iter;
	iter = names.begin();
	for (int g = 0; g < (int) names.size(); g++) {
		if (iter == names.end()) 
			throw AxiomLibException("Error in StartStrategy::init : error in creating treat Strategies.");
		for (int i = 0; i < numOfStarts; i++) {
			treatVector[g * numOfStarts + i] = tf.create (*iter);
		}
		iter++;
	}
	
	// Инициализация исходного набора данных - по данным из конфигурационного файла узнаем расположение исслудемого набора данных и считываем его
	std::string datasetDir, datasetName;
	if (envStart.getStringParamValue(datasetDir, "BaseDataSetDir") < 0)
		throw AxiomLibException("StartStrategy::init : data set directory is undefined.");
	if (envStart.getStringParamValue(datasetName, "DataSet") < 0)
		throw AxiomLibException("StartStrategy::init : data set is undefined.");
	// считываем необходимые для данного класса параметры о наборе данных
	EnvDataSet envDataSet;
	envDataSet.readConfigFile (datasetDir, datasetName);
	// установка корректного обозначения NullStr - обозначение остутсвия в данной точке ряда какого либо нештатного поведения
	dataSet.setNullStr (envDataSet);
	// собственно считываем набор данных - заводим его во внутреннее представление
	dataSet.readDataSet(datasetDir, datasetName);
	// восстанавливаем в данном классе вектор индексов параметров в каноническом представленнии по которым вести поиск нештатых ситуаций
	dataSet.getParamNums(params, envStart, envDataSet);
	
	// определение имени файла откуда будут читаться основные настройки для классов генетического алгоритма подбора системы аксиом
	if (envStart.getStringParamValue(bgConfigFile, "bgConfigFile") < 0)
		throw AxiomLibException("StartStrategy::init : bgConfigFile is undefined.");
	
	// Считывание информации по числу запусков и степени изменения параметров предобработчиков на каждому этапе
	//...
}


/****************************************************************************
*				StartStrategy::sort
*
*	Description:	Функция сортировки вектора индексов mask по вектороу значений  resVector
*	Parameters:	mask - сотрируемый вектор
*				resVector - вектора значений - по которому ведется сортировка
*	Returns:		0
*	Throws:		AxiomLibException - если вектора разной длины
*	Author:		dk
*	History:
*
****************************************************************************/
inline int StartStrategy::sort (std::vector<int> &mask, std::vector<double> &resVector) const {
	// проверка входных парметров
	if ( (mask.size() != resVector.size()) || (mask.size() < 1) )
		throw AxiomLibException ("Error in StartStrategy::sort: input parameters are of wrong length.");
	// собственно сортировка по принципу - размещения в порядке неубывания по goals индексов в mask
	int j = 0;
	for (int i = 0; i < (int) resVector.size(); i++){
		int j = 0;
		// поиск размешения
		while ( j < i ) {
			if (resVector[i] < resVector[mask[j]]) {
				break;
			}
			j++;
		}
		// смещение всех, у кого goal больше
		for (int k = i; k > j; k--) {
			mask[k] = mask[k-1];
		}
		// собственно размещение индекса
		mask[j] = i;
	}
	return 0;
}


/****************************************************************************
*				StartStrategy::deleteProcs
*
*	Description:	Функция удаляет предобработчики с параметрами из класса 
*				по значению в векторе alive - если оно false - то соответствующая 
*				позиция удаляется из вектора предобработчиков, га, популяций.
*	Parameters:	alive - вектор указаний - какие предобработчики оставить в рассмотрении, а какие нет
*	Returns:		0
*	Throws:		AxiomLibException - если длина входного вектора разнится с длиной внетренних векторов класса
*	Author:		dk
*	History:
*
****************************************************************************/
inline int StartStrategy::deleteProcs (std::vector<bool> &alive) {
	// прокерка на соответствие длин векторов
	if ( (alive.size() != treatVector.size()) || (alive.size() != gaVector.size()) || (alive.size() != popVector.size()) )
		throw AxiomLibException("Error in StartStrategy::deleteProcs: input vector's length is incorrect.");
	// подготовка итераторов для удаления выбранных в alive элементов векторов
	std::vector<Treatment *>::iterator iter1;
	std::vector <BaseGenetics>::iterator iter2;
	std::vector <AxiomSetPop>::iterator iter3;
	vector <bool>::iterator witer;
	// введение флага для отображения, что больше удалять никого не надо -  выход из цикла
	int flag = 0;
	while (flag == 0) {
		flag = 1;
		// настройка итераторов на начала векторов
		witer = alive.begin();
		iter1 = treatVector.begin();
		iter2 = gaVector.begin();
		iter3 = popVector.begin();
		// бкжим по вектору - как встретили кого нужно удалить - удаляем и начинаем бежать по векторам заново
		while ( (iter1 != treatVector.end()) && (iter2 != gaVector.end()) && (iter3 !=popVector.end()) ) {
			if (*witer == false) {
				alive.erase(witer);
				delete *iter1;
				treatVector.erase(iter1);
				gaVector.erase(iter2);
				popVector.erase(iter3);
				flag = 0;
				break;
			}
			witer++;
			iter1++;
			iter2++;
			iter3++;
		}
	}
	return 0;
}


// ЗАМЕНИТЬ НА БОЛЕЕ ПРОГРЕССИВНУЮ ФУНКЦИЮ СОКРАЩЕНИЯ (ЖЕЛАТЕЛЬНО С ИСПОЛЬЗОВНИЕМ НОМЕРА ИТЕРАЦИИ И ИСТОРИИ ПО КАЖДОМУ ПРЕДОБРАБОТЧИКУ)
/****************************************************************************
*				StartStrategy::reduce
*
*	Description:	Функция определяет сокращение используемых предобработчиков с параметрами 
*				в зависимости от номера шага i и результатов (индексирование котороых в mask)
*	Parameters:	mask - вектор индексов предобрабортчиков в порядке ухудшения целевой функции
*				i - номер шага на котором происходит данное сокращение
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
inline int StartStrategy::reduce(const int i, const std::vector<int> &mask) {
	// создаем вектор значений - какие предобработчики с какими пармаетрами оставить 
	std::vector <bool> alive;
	alive.resize(mask.size());
	for (unsigned int a = 0; a < alive.size(); a++)
		alive[a] = true; // сначала оаствляем все предобработчики
	// теперь вычеркиваем некоторые 
	alive[mask[mask.size() - 1]] = false; // вычеркиваем последний по значению предобработчик с параметрами
	// непосредственно вносим изменения в переменные класса 
	deleteProcs (alive);
	return 0;
}


//  ЗАМЕНИТЬ НА БОЛЕЕ ПРОГРЕССИВНУЮ ФУНКЦИЮ, НЕЖЕЛИ КОНСТАНТА
/****************************************************************************
*				StartStrategy::countNumOfIters
*
*	Description:	Функция определяет число итераций ГА на данной итерации
*	Parameters:	i - номер шага основного цикла
*	Returns:		число итераций ГА
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
inline int StartStrategy::countNumOfIters(const int i) {
	return 2;
}


/****************************************************************************
*				StartStrategy::saveToFile
*
*	Description:	Функция сохранения предобработчика с параметрами и соотвествующей системой аксиом в файл
*	Parameters:	i - индекс в векторе предобработчиков
*	Returns:		0 - если сохранение прошло успешно
*				-1 - если сохранения не было
*	Throws:		AxiomLibException - если указанный индекс выходит за возможные границы или внутренние пременные класса не согласованы
*	Author:		dk
*	History:
*
****************************************************************************/
int StartStrategy::saveToFile (const int i) const {
	// проверка входного параметра
	if ( (i < 0) || (i >= (int) treatVector.size()) || (treatVector.size() != gaVector.size()) || (treatVector.size() != popVector.size()) )
		throw AxiomLibException("Error in StartStrategy::saveToFile: base vectors are of different size or input index is out of range.");
	// сохранение i-го предобработчика с параметрами и лучшей системой аксиом из соответствующей популяции 
	// параметры файла - куда сохранять - берутся из конфигурационного файла при инициализации и хранятся до сего момента во внутренних переменных класса
	
	// ... saving... - пока не сделано - надо бы придумать формат (чтобы потом и считать можно было)
	
	return -1;
}


/****************************************************************************
*				StartStrategy::start
*
*	Description:	Основная функция класса - запуск алгоритма выбора лучшего
*				предобработчика с параметрами. Все необходимые данные
*				читаются из специального конфигурационного файла.
*	Parameters:	-
*	Returns:		-
*	Throws:		AxiomLibException - если внутренние переменные класса были некорректно определены или изменены
*	Author:		dk
*	History:
*
****************************************************************************/
void StartStrategy::start (void) {
	// Предварительный вид (алгоритм) данной функции (класса в обшем):
	// 1. Определение параметров данного класса
	//	(таких как параметры запсков ГА, параметры предобработчиков и др.)
	// 2. Содание начальных переменных класса по считанным параметрам 
	//	(сюда входит генерация классов ГА, считывание параметров для них, возможно создание начальных популяций, либо создание таковых в рамках классов ГА, считывание набора данных)
	// 3. Основной цикл функции : 
	// 	3.0, Проверка - не сошлись ли мы к 1-му предобработчику и 1-му набору параметров 
	//		(в этом случае следует выход из цикла)
	//	3.1. Для каждого созданного класса предобработки временных рядов запуск его предобработки с разными параметрами
	//	3.2. По чсилу разновидностей набора данных - запуск поиска оптимальной системы аксиом  с ограниченным числом систем аксиом
	//	3.3. Обработка резуьтатов работы классов генетических алгоритмов подбора систем аксиом 
	//		(результаты сортируются, выбиратся наилучшие направления и параметры предобработки, вектора используемых классов ГА и популчций к ним соответственно сужаются)
	//	3.4. Изменение параметров запуска классов ГА в соответствии с шагом данного цикла 
	//		(изменение параметров оставленных предобработчиков - какая либо мутация, возможно мутация в нескольких направлениях и свзяанное с этим возрастание вектора treatVector)
	// 4. Сошлись к одному предобработчику и одному набору параметров - сохраняем данные о том - к чему сошлись, выводи результаты - значения целевой функции и др.
	
	// Реализация концепции
	// 1. Определение параметров
	init ();
	
	// 2. Создание необходимых начальных переменных до входа в основной цикл
	if (/*numOfStarts **/ treatVector.size() <= 0) 
		throw AxiomLibException("StartStrategy::start: wrong number of starts.");
	popVector.resize(/*numOfStarts **/ treatVector.size());
	gaVector.resize(/*numOfStarts * */treatVector.size());
	Environment env; // чтение настроик для классов ГА
	env.readConfigFile(bgConfigFile.c_str());
	// Установка соответствующих настроек
	for (unsigned int i = 0; i < gaVector.size(); i++) {
		(gaVector[i]).shortInitFromEnv(env);
		(gaVector[i]).setParamsOfDataSet(params);
	}
	
	// 3. Основной цикл функции 
	bool exitCycleCond = false; // условие завершение основного цикла (не него в частности будет влиять сведение к 1-це числа используемых предобработчиков и его параметров)
	int curNumOfIter; // текущие максимальное число итераций ГА по кажому из предобработчиков с набором параметров
	std::vector <int> mask;
	std::vector <double> resVector;
	resVector.resize(treatVector.size());
	int num = 0; // номер итерации 
	while (exitCycleCond == false) {
		num++;
		std::cout << "\n	num is " << num;
		// 3.0. установка переменной exitCycleCond
		if (gaVector.size() <= 1) {
			exitCycleCond = true;
		}
		if ( ( treatVector.size() != gaVector.size() ) || (treatVector.size() != popVector.size()) ) {
			std::cout << "\n 	treatVector.size() is " << treatVector.size() << "	gaVector.size() is " << gaVector.size() << "	popVector.size() is " << popVector.size() << "\n";
			throw AxiomLibException("Error in StartStrategy::start : base vectors are of different size.");
		}
		curNumOfIter = countNumOfIters(num); // установка числа итераций ГА
		for (unsigned int i = 0; i < treatVector.size(); i++) { // данный цикл следует запускать паралельно по i
			// 3.1. Запуск различной предобработки набора данных
			DataSet newDataSet; // временный вектор для получений набора данных путем предобработки текущим предобработчиком
			(treatVector[i])->process(newDataSet, dataSet);
			// 3.2. Запуск подбора системы аксиом
			(gaVector[i]).setDataSet(newDataSet); // установка необходимых параметров для запуска ГА
			(gaVector[i]).setNumOfIter (curNumOfIter);
			//std::cout << "\n		i is " << i+1 << "	in";
			resVector[i] = (gaVector[i]).run (popVector[i]); // собственно запуск ГА подбора оптимальной системы аксиом
			//std::cout << "\n		i is " << i+1 << "	out";
			(treatVector[i])->addRes(resVector[i]); // добавление к истории текущего предобработчика результатов на данном этапе цикла - необходимо при определении как изменять параметры
		}
		if (exitCycleCond == true) break;
		//std::cout << "\n gaVector.size() is " << gaVector.size();
		//std::cout << "\n Going to reduce. Press enter.";
		//std::cin.get();
		//throw AxiomLibException (" TEST ");
		// 3.3. Сортировка по значению наилучшей целевой функции
		mask.resize(resVector.size());
		for (int j = 0; j < (int) mask.size(); j++)
			mask[j] = j;
		sort (mask, resVector);
		reduce(num, mask); // функция определяет сокращение используемых предобработчиков с параметрами в зависимости от номера шага i и результатов (индексирование котороых в mask)
		// 3.4. Изменение treatVector в соответствии с его историей и, возможно, введение нескольких новых преодбработчиков с параметрами в зависимости от стратегии
		for (unsigned int g = 0; g < treatVector.size(); g++) {
			(treatVector[g])->change (); // изменение параметров
			// введение новых проедобработчиков с параметрами (на основе данного g-го)
			// add(g); // пока не реализовано - да и стоит ли - это определяется стратегией
		}
	}
	 std::cout << "\n	Finish \n";
	// 4. Сохранение данных об итоговом предобработчике, его параметрах, системе аксиом, на которой были достигнуты наилучние результаты
	if ( (treatVector.size() != 1) || (gaVector.size() != 1) || (popVector.size() != 1) )
		throw AxiomLibException("Error in StartStrategy::start : The result is multiple or absent.");
	saveToFile (0); // сохраняем единственный результирующий - он сейчас в 0-м элементе вектора
}
