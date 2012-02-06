/****************************************************************************
*			Функции класса RefMarkupSimple.h
*
*	Description:	Эталонная разметка - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "RefMarkupSimple.h"
#include "Defines.h"
using namespace AxiomLib;

// Конструктор класса с установой начальных значений
RefMarkupSimple::RefMarkupSimple () {
	axiomTypes.clear();
	maxDistance = 0.0;
	dtwUse = false; // установка по умолчанию использовать обычную метрику или алгоритм на основе DTW
	// какой из алгоритмов DTW использовать - достаточно чтобы хотя бы один из алгоритмов был задействован при dtwUse = True
	// если оба будут true - то автоматически выберется один из них, а второй так же будет работать - но его рузультаты не будут учтены
	useDTWwideSearch = true; 
	useDTWdeepSearch = false;
	delUse = true;
	// Задание весовых коэффициентов
	delPrice.resize(2);
	delPrice [0] = 1.0; // цена удаления нулевого символа из разметки ряда
	delPrice [1] = 1.0; // цена удаления ненулевого символа из разметки ряда
	insUse = true;
	insPrice.resize(2);
	insPrice [0] = 1.0; // цена вставки нулевого символа в разметку ряда
	insPrice [1] = 1.0; // цена вставки ненулевого символа в разметку ряда
	chgUse = true;
	chgPrice.resize(3);
	chgPrice [0] = 1.0; // 2.0; // цена замены нулевого символа в разметке ряда на ненулевой
	chgPrice [1] = 1.0; // 2.0; // цена замены ненулевого символа в разметке ряда на ненулевой
	chgPrice [2] = 1.0; // 2.0; // цена замены ненулевого символа в разметке ряда на нулевой
	// Подсчет средних величин - средняя цена за удаление символа
	averDel = 0.0;
	for (unsigned int i = 0; i < delPrice.size(); i++) averDel += (float) delPrice[i];
	averDel /= delPrice.size();
	// Средняя велечина за вставку символа
	averIns = 0.0;
	for (unsigned int i = 0; i < insPrice.size(); i++) averIns += (float) insPrice[i];
	averIns /= insPrice.size();
	// Средняя величина за изменение символа
	averChg = 0.0;
	for (unsigned int i = 0; i < chgPrice.size(); i++) averChg += (float) chgPrice[i];
	averChg /= chgPrice.size();
	// Установка на начальне значения переменных, которые не несут определенного значения и при использовании будут переопределены
	a1 = 0.0; 
	a2 = 0.0;
	b1 = 0.0;
	b2 = 0.0;
	// это коэффициенты по усечению области рассмотрения путей в матрице расстояний в алгоритме DTW
	// Каждый из коэффициентов означает насколько сильно урезать область путей для алгоритма DTW 
	//  Например, если truncationAxioms = 0.0  - то никакаких ограничений наложено не будет
	//							= 1.0 - не останется ни одного пути в матрице расстояний
	// ОЧЕНЬ важно - в связи с описанным выше - задавать ограничения из области [0.1, 0.9] ! 
	// 			   иначе возможно полное отсутсвие путей в матрцие расстояний или возмодное деление на число близкое к 0.
	truncationAxioms = 0.5;
	truncationRow = 0.5;
	// Кофицинты сжатия и растяжения в пределах которых искать соответствие эталонной разметке
	float coef1 = (float) 0.9; 
	float coef2 = (float) 1.1;
}


/****************************************************************************
*                       RefMarkupSimple::getCopy
*
*       Description:	Выдает текущее состояние разметки
*       Parameters:		axTypes - заполняемый вектор элеметнов разметки
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int RefMarkupSimple::getCopy (std::vector<int> &axTypes) const {
	axTypes = axiomTypes;
	return 0;
}


/****************************************************************************
*                       RefMarkupSimple::setParamsFromEnv
*
*       Description:	Инициализировать параметры данными из env
*       Parameters:	const Environment& env - среда
*       Returns:		0
*       Throws:		AxiomLibException - если нет какого-то параметра
*       Author:		dk
*       History:
*
****************************************************************************/
int RefMarkupSimple::setParamsFromEnv (const Environment& env) {
	std::string tempStr;
	double tempDB1, tempDB2, tempDB3;
	
	if (env.getDoubleParamValue(tempDB1, "MaxDistance") >= 0) {
		if (tempDB1 < 0) {
			throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : wrong value of MaxDistance in config file.");
		} else {
			maxDistance = tempDB1;
		}
	}
	
	if (env.getStringParamValue(tempStr, "Recognizer") < 0)
		throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : recognizer not set.");
	if ( (tempStr == "DTW") || (tempStr == "dtw") ) {
		dtwUse = true;
		useDTWwideSearch = true;  // Значение устанавливается по умолчанию - так как поиск в глубину для данного алгоритма много медленней алгоритма посика в глубину
		useDTWdeepSearch = false;
	} else {
		if ( (tempStr == "Metric") || (tempStr == "metric") || (tempStr == "METRIC") ) { 
			dtwUse = false; // установка использовать обычную метрику
		} else {
			throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : recognizer not set.");
		}
	}
	// Задание весовых коэффициентов
	if (env.getStringParamValue(tempStr, "DelUse") >= 0) {
		if ( (tempStr == "true") || (tempStr == "True") || (tempStr == "TRUE") ) {
			if ( (env.getDoubleParamValue(tempDB1, "DelPriceNull") < 0) || (env.getDoubleParamValue(tempDB2, "DelPriceNoNull") < 0) ) 
				throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : delete costs are not set.");
			setDelPrice (true, tempDB1, tempDB2);
		}
		if ( (tempStr == "false") || (tempStr == "False") || (tempStr == "FALSE") ) {
			delUse = false;
		}
	}
	
	if (env.getStringParamValue(tempStr, "InsUse") >= 0) {
		if ( (tempStr == "true") || (tempStr == "True") || (tempStr == "TRUE") ) {
			if ( (env.getDoubleParamValue(tempDB1, "InsPriceNull") < 0) || (env.getDoubleParamValue(tempDB2, "InsPriceNoNull") < 0) ) 
				throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : insert costs are not set.");
			setInsPrice (true, tempDB1, tempDB2);
		}
		if ( (tempStr == "false") || (tempStr == "False") || (tempStr == "FALSE") ) {
			insUse = false;
		}
	}
	
	if (env.getStringParamValue(tempStr, "ChgUse") >= 0) {
		if ( (tempStr == "true") || (tempStr == "True") || (tempStr == "TRUE") ) {
			if ( (env.getDoubleParamValue(tempDB1, "ChgNullToNoNull") < 0) || (env.getDoubleParamValue(tempDB2, "ChgNoNullToNoNull") < 0) || (env.getDoubleParamValue(tempDB3, "ChgNoNullToNull") < 0) ) 
				throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : change costs are not set.");
			setChgPrice (true, tempDB1, tempDB2, tempDB3);
		}
		if ( (tempStr == "false") || (tempStr == "False") || (tempStr == "FALSE") ) {
			chgUse = false;
		}
	}
	
	if (env.getDoubleParamValue(tempDB1, "TruncationRef") >= 0) {
		if ( (tempDB1 >= 0.1) && (tempDB1 <= 0.9) ) {
			truncationAxioms = (float) tempDB1; 
		} else {
			throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : wrong value of truncationAxioms in config file.");
		}
	}
	
	if (env.getDoubleParamValue(tempDB1, "TruncationRow") >= 0) {
		if ( (tempDB1 >= 0.1) && (tempDB1 <= 0.9) ) {
			truncationRow = (float) tempDB1; 
		} else {
			throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : wrong value of truncationRow in config file.");
		}
	}
	
	if (env.getDoubleParamValue(tempDB1, "CompressionDegree") >= 0) {
		if ( (tempDB1 > 0.0) && (tempDB1 <= 1.0) ) {
			coef1 = (float) tempDB1; 
		} else {
			throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : wrong value of CompressionDegree in config file.");
		}
	}
	
	if (env.getDoubleParamValue(tempDB1, "StretchingDegree") >= 0) {
		if (tempDB1 >= 1.0) {
			coef2 = (float) tempDB1; 
		} else {
			throw AxiomLibException("RefMarkupSimple::setParamsFromEnv : wrong value of StretchingDegree in config file.");
		}
	}
	
	return 0;
}

// Пустой (пока) деструктор
RefMarkupSimple::~RefMarkupSimple () {
}


// Функция возвращает длину вектора с эталонной разметкой
int RefMarkupSimple::size (void) const {
	return (int) axiomTypes.size();
}


// Удаление всех добавленных элементов из класса
int RefMarkupSimple::clear () {
	axiomTypes.clear();
	return 0;
}


/****************************************************************************
*					RefMarkupSimple::setDTWUse
*
*	Description:	Функция задания значения переменной dtwUse
*	Parameters:	newDTWUse - использовать ли алгоритм DTW для определения расстояния между сравниваемыми строками
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSimple::setDTWUse (bool newDTWUse) {
	dtwUse = newDTWUse;
}


/****************************************************************************
*					RefMarkupSimple::setDelPrice
*
*	Description:	Функция задания цен на операции удаления
*	Parameters:	newUse - использовать ли операцию
*				delNull - цена удаления нулевого символа
*				delNoNull - цена удаления ненулевого символа
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSimple::setDelPrice (bool newUse, double delNull, double delNoNull) {
	delUse = newUse;
	delPrice[0] = delNull;
	delPrice[1] = delNoNull;
}


/****************************************************************************
*					RefMarkupSimple::setInsPrice
*
*	Description:	Функция задания цен на операцию вставки
*	Parameters:	newUse - использовать ли операцию
*				insNull - цена вставки нулевого символа
*				insNoNull - цена вставки ненулевого символа
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSimple::setInsPrice (bool newUse, double insNull, double insNoNull) {
	insUse = newUse;
	insPrice[0] = insNull;
	insPrice[1] = insNoNull;
}


/****************************************************************************
*					RefMarkupSimple::setChgPrice
*
*	Description:	Функция задания цен на операции замены
*	Parameters:	newUse - использовать ли операцию
*				chgNullToNoNull - цена изменения нулевого символа на ненулевой
*				chgNoNullToNoNull - цена изменения ненулевого символа на ненулевой
*				chgNoNullToNull - цена изменения ненулевого символа на нулевой
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSimple::setChgPrice (bool newUse, double chgNullToNoNull, double chgNoNullToNoNull, double chgNoNullToNull) {
	chgUse = newUse;
	chgPrice[0] = chgNullToNoNull;
	chgPrice[1] = chgNoNullToNoNull;
	chgPrice[2] = chgNoNullToNull;
}


/****************************************************************************
*					RefMarkupSimple::create
*
*	Description:	Функция заполнения переменных класса
*	Parameters:	newAxiomTypes - вектор разметки ряда аксиомами
*	Returns:		void
*	Throws:		AxiomLibException - если входные параметры некорректной длины
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSimple::create (std::vector<signed int> &newAxiomTypes) {
	// проверка корректности параметров
	if (newAxiomTypes.size() <= 0) {
		throw AxiomLibException ("Error in RefMarkupSimple::create: wrong parameters to create RefMarkupSimple. ");
	}
	// заолнение переменных данного класса
	this->axiomTypes.resize(0);
	this->axiomTypes = newAxiomTypes;
}


/****************************************************************************
*					RefMarkupSimple::operator ==
*
*	Description:	Функция четкого сравнения двух разметок
*	Parameters:	second - вторая сравниваемая аксиома
*	Returns:		bool - результат четкого сравнения
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool RefMarkupSimple::operator == (const RefMarkupSimple& second) const {
	if ( this->axiomTypes.size() != second.size() ) return false;
	for (unsigned int i = 0; i < this->axiomTypes.size(); i++) {
		if (this->axiomTypes[i] != second.axiomTypes[i]) return false;
	}
	return true;
}


/****************************************************************************
*					RefMarkupSimple::operator ==
*
*	Description:	Функция четкого сравнения двух разметок
*	Parameters:	second - вторая сравниваемая строка меток аксиом
*	Returns:		bool - результат четкого сравнения
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool RefMarkupSimple::operator == (std::vector <int>& second) const {
	if ( axiomTypes.size() != second.size() ) return false;
	for (unsigned int i = 0; i < axiomTypes.size(); i++) {
		if (axiomTypes[i] != second[i]) return false;
	}
	return true;
}


/****************************************************************************
*					RefMarkupSimple::check 
*
*	Description:	Функция проверки - подходит ли разметка
*				класса к вектору разметки во вхоном параметре.
*				То есть проверяется - найдется ли в конце входного ряда аксиом 
*				раметка данного класса. (нахождение в конце - подразумевает,
*				что последняя аксиома разметки 
*				входного ряда должна совпадать с последней аксиомой 
*				эталонной разметки)
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*				newMaxDistance - максимальние расстояние между участками рядов 
*	Returns:		0 - если разметки не совпадают
*				1 - если разметки совпадают с заданной точностью
*	Throws:		AxiomLibException - если задан некорректно параметр upTo
*	Author:		dk
*	History:
*
****************************************************************************/
int RefMarkupSimple::check (const double newMaxDistance, std::vector<int> &rowAxiomTypes, long int upTo) {
	// Проверка на корректность входных данных
	if (upTo >= (long int) rowAxiomTypes.size()) {
		throw AxiomLibException ("Error in RefMarkupSimple::check: wrong parameters of check functuion");
	}
	// Поиск разметки в ряду
	double toReturn;
	maxDistance = newMaxDistance;
	if (dtwUse) {
		toReturn = subcheckDTW (rowAxiomTypes, upTo);
	}
	else {
		toReturn = subcheck (0.0, rowAxiomTypes, upTo, (int) axiomTypes.size() - 1);
	}
	if (toReturn < maxDistance) return 1;
	return 0;
}


/****************************************************************************
*					RefMarkupSimple::subcheck 
*
*	Description:	Вспомогательная функция для check - рекурсивный вызов 
*				функции проверки совпадания разметок - для учета 
*				того, что число вхождений аксиомы может быть равным 0.
*				Алгоритм на основе метрики для измерения расстояний между разметакми
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				axiomLen - параметры разметки
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*				curEnd - текущее положение в эталонной разметке для сравнения
*	Returns:		0 - false
*				1 - true
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double RefMarkupSimple::subcheck (const double curDist, std::vector<int> &rowAxiomTypes, long int upTo, int curEnd) {
	if (curEnd < 0) {
		return 0;
	}
	long int j = upTo;
	// Если текущее расстоние между строками больше максимально допустимого - то не продолжаем сравнение - и так понятно - нет совпадения с заданной точностью
	if (curDist > maxDistance) return curDist;
	for (int i = curEnd; i >= 0; i--) {
		// Проверка вхождения эталонной последовательности в разметку
		if (axiomTypes[i] != rowAxiomTypes[j]) {
			// Несовпадают типы аксиом - собственно запускаем механизм поиска расстояния - как минимального расстояния между сравниваемыми строками
			std::vector <double> dist; // временное хранение расстояния между двумя участками разсеток
			dist.resize(3);
			for (unsigned int k = 0; k < dist.size(); k++) dist[k] = -1.0;
			// В зависимости от того, какие операции использовать  - запускаем поиск кратчайщего расстояния
			if (delUse) {
				if (rowAxiomTypes[j] == -1) // это значит что текущая аксиома -  нулевая
					dist[0] = subcheck (delPrice[0] + curDist, rowAxiomTypes, j-1, i);
				else 
					dist[0] = subcheck (delPrice[1] + curDist, rowAxiomTypes, j-1, i);
			}
			if (insUse) {
				if (axiomTypes[i] == -1)
					dist[1] = subcheck (insPrice[0] + curDist, rowAxiomTypes, j, i-1);
				else 
					dist[1] = subcheck (insPrice[1] + curDist, rowAxiomTypes, j, i-1);
			}
			if (chgUse) {
				if (axiomTypes[i] == -1)
					dist[2] = subcheck (chgPrice[0] + curDist, rowAxiomTypes, j-1, i-1);
				else {
					if (axiomTypes[i] == -1)
						dist[2] = subcheck (chgPrice[2] + curDist, rowAxiomTypes, j-1, i-1);
					else 
						dist[2] = subcheck (chgPrice[1] + curDist, rowAxiomTypes, j-1, i-1);
				}
			}
			// Выбор наименьшего расстояния из полученных
			int wayToDo = 0;
			for (int f = 1; f  < (int) dist.size(); f++) {
				if ( (dist[f] < dist[wayToDo]) || (dist[wayToDo] == -1) ){
					wayToDo = f;
				}
			}
			// проверка - что хотя бы одно расстояние высчитано
			if (dist[wayToDo] < 0)
				throw AxiomLibException ("Error in RefMarkupSimple::subcheck: no operation to use. ");
			// возвращаем результат
			return dist[wayToDo];
		}
		// Сдвигаемся по ряду - для сравнения следующей аксиомы
		j--;
		if ((j < 0) && (i != 0)) {
			// Ряд слишком короткий чтобы что-то определить
			return curDist + i*delPrice[0]; // возвращаем цену удаления всех оставщихся без соответствия символов в эталонной разметке
		}
	}
	// раз не выпрыгнули из функции раньше - значит закончен подсчет расстояния - выходим с текущим значением расстояния
	return curDist;
}


/****************************************************************************
*					RefMarkupSimple::subcheckDTW
*
*	Description:	Вспомогательная функция для check - подсчет расстояния между 
*				строкой эталонной разметки и подстракой анализируемого ряда, 
*				которая заканчивается в upTo.
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*	Returns:		double - расстояние между объектами
*	Throws:		AxiomLibException - если до вызова этой функции не был 
*							       заполнен вектор разметки эталонной траектории
*	Author:		dk
*	History:
*
****************************************************************************/
double RefMarkupSimple::subcheckDTW (std::vector<int> &rowAxiomTypes, long int upTo) {
	// Схема работы функции
	//  - Определение множества допустимых подстрок анализиремого ряда для сравнения с эталонной разметкой, которые заканчиваются в upTo
	//  - Запуск поиска наименьшего расстояния для каждой их подстрок 
	//  - Выбор наименьшего расстояния
	const bool debug = false;
	// проверка на задание самой разметки эталонной траектории
	if (axiomTypes.size() < 1) throw AxiomLibException ("Error in RefMarkupSimple::subcheckDTW: RefMarkupSimple not initialised.");
	// определение границ в которых искать соответствие
	long int leftMin, leftMax;
	leftMin = upTo - (long int) (axiomTypes.size() * coef1);
	if (leftMin < 0) leftMin = 0;
	leftMax = upTo - (long int) (axiomTypes.size() * coef2);
	if (leftMax < 0) leftMax = 0;
	// При необходимости - меняем значения местами так, чтобы leftMax был меньше leftMin
	if (leftMin < leftMax) {
		long int temp = leftMax;
		leftMax = leftMin;
		leftMin = temp;
	}
	// матрицы расстояний
	std::vector < std::vector <float> > distMatrix;
	distMatrix.resize(axiomTypes.size());
	// заполнение матрицы расстояний
	for (int i = 0; i < (int) distMatrix.size(); i++) {
		(distMatrix[i]).resize(upTo - leftMax + 1);
		for (int j= 0; j < (int) (distMatrix[i]).size(); j++) {
			distMatrix[i][j] = distance (axiomTypes[i], rowAxiomTypes[upTo-j]);
		}
	}
	// Создание вектора предварительных результатов
	std::vector <double> preResult;
	preResult.resize(leftMin - leftMax + 1);
	// запуск непосредственно расчета расстояния в distMatrix методом DTW и запись результатов в preResult
	// какой DTW-алгоритм использовать - определяется так же в этой функцие
	runDistDTW (preResult, distMatrix);
	// Выбор наименьшего расстояния и его возврат в качестве результата
	double curDist;
	curDist = preResult[0];
	for (unsigned int g = 1; g < preResult.size(); g++) {
		if (curDist > preResult[g]) curDist = preResult[g];
	}
	if (debug) {
		std::cout << "\r	curDist is " << curDist /*<< "		preResult.size() is " << preResult.size() */<< "\r";
	}
	return curDist;
}


/****************************************************************************
*					RefMarkupSimple::distance
*
*	Description:	Вспомогательная функция для subcheckDTW - 
*				опрелеление расстояния между двумя метками 
*	Parameters:	symbA - первый сравниваемый символ
*				symbB - второй сравниваемый символ
*	Returns:		float - расстояние между символами
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
inline float RefMarkupSimple::distance (int symbA, int symbB) {
	if (symbA == symbB) return (float) 0.0;
	if (symbA == -1) return (float) chgPrice[2];
	if (symbB == -1) return (float) chgPrice[0];
	return (float) chgPrice[1];
}


/****************************************************************************
*					RefMarkupSimple::runDistDTW
*
*	Description:	Вспомогательная функция для check - подсчет расстояния между 
*				строкой эталонной разметки и подстракой анализируемого ряда, 
*				которая заканчивается в upTo.
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*	Returns:		int
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int RefMarkupSimple::runDistDTW (std::vector <double> &preResult, std::vector < std::vector <float> > &distMatrix) {
	const bool debug = false;
	if (useDTWwideSearch)  {
		// задание значений переменным, необходимым как ограничения при поиске наименьшего расстояния
		// ограничения задаются - как отсечения недопустимых углов в матрице расстояний, через которые идти нельзя при поиске пути
		// фактически это угловые коэффициенты двух прямых отсечения(a1 и b1) и отступы (a2 и b2)
		a1 = axiomTypes.size() * truncationAxioms / (truncationRow * (distMatrix[0]).size());
		a2 = axiomTypes.size() * (1- truncationAxioms);
		b1 = axiomTypes.size() * truncationAxioms / (truncationRow * ((distMatrix[0]).size() - preResult.size()));
		b2 = b1* ((truncationRow - 1) * (distMatrix[0]).size() - truncationRow * preResult.size());
		// запус поиска расстояний в матрице distMatrix
		distDTWwideSearch (preResult, distMatrix);
		return 1;
	}
	if (useDTWdeepSearch)  {
		int deep;
		for (int t = 0; t < (int) preResult.size(); t++) {
			// задание значений переменным, необходимым как ограничения при поиске наименьшего расстояния
			// ограничения задаются - как отсечения недопустимых углов в матрице расстояний, через которые идти нельзя при поиске пути
			// фактически это угловые коэффициенты двух прямых отсечения(a1 и b1) и отступы (a2 и b2)
			a1 = axiomTypes.size() * truncationAxioms / (truncationRow * ( (distMatrix[0]).size() - t ) );
			a2 = axiomTypes.size() * (1- truncationAxioms);
			b1 = a1;
			b2 = a1* (truncationRow - 1) * ((distMatrix[0]).size() - t);
			deep = 1; // значение по умолчанию для длины пути в матрице расстояний
			preResult[t] = distDTWdeepSearch (0.0, deep, distMatrix, 0, t);
			// нормирование пути - по его длине с вычислением расстония DTW
			preResult[t] = sqrt (preResult[t]) / (double) deep; // так, если получается одинаковое расстояние между строками, то предпочтение отдается более длинной.
		}
		return 2;
	}
	if (debug) {
		std::cout << "\n	useDTWdeepSearch is " << useDTWdeepSearch;
		std::cout << "\n	useDTWwideSearch is " << useDTWwideSearch;
	}
	throw AxiomLibException ("Error in RefMarkupSimple::runDistDTW: No DTW algorithm was chosen to recognize.");
	return 0;
}


/****************************************************************************
*					RefMarkupSimple::distDTWdeepSearch
*
*	Description:	Вспомогательная функция для subcheckDTW - 
*				опрелеление расстояния между двумя подстроками 
*				с использованием алгоритма DTW - метод поиска в длину
*	Parameters:	curDist - текущее расстояние между сравниваемыми строками - расстояние между их начальными подстроками
*				curDeep - текущее значение глубины найденного пути
*				distMatrix - матрица расстояний между символами строк
*				i - элемент эталонной разметки, с которого идет подсчет расстояния 
*				j - элемент в анализируемой строке, с которого идет подсчет расстояния
*	Returns:		double - расстояние между подстроками
*	Throws:		AxiomLibException -  если попали в момент поиска наименьшего расстояния между двумя элементами таблицы в систуацию, 
*								когда нет направленя для движения по таблице в целях достижения крайнего диагонального элемента
*	Author:		dk
*	History:
*
****************************************************************************/
double RefMarkupSimple::distDTWdeepSearch (const double curDist, int &curDeep, std::vector < std::vector <float> > &distMatrix, const int i, const int j) const {
	bool debug = false;
	if (debug) std::cout << "\n	( " << i << ", " << j << " )";
	// Если достигли углового значения - то возвращаем полученный результат - алгоритм останавливается
	if ( ( i == ( distMatrix.size() - 1 ) )  && ( j == ( (distMatrix[i]).size() - 1) ) ) {
		curDeep = 1; // задание глубины на нижнем уровне
		return curDist + distMatrix[i][j];
	}
	// проверяем можем ли мы идти вверх - если да - то считаем путь если пойдем туда
	double up = -1.0;
	int curDeepUp = curDeep;
	if ( ( (i + 1.5) - (j+0.5)*a1- a2 < 0.01) && ( (i + 1.5) - (j+0.5)*b1 - b2 > -0.01) && ( (i + 1) < (int) distMatrix.size()) ) {
		if (debug)std::cout << "	up";
		up = distDTWdeepSearch(curDist + distMatrix[i][j], curDeepUp, distMatrix, i+1, j);
	}
	// проверяем можем ли мы идти вправо - если да - то считаем путь если пойдем туда
	double right = -1.0;
	int curDeepRight = curDeep;
	if ( ((i+0.5) - (j + 1.5)*a1- a2 < 0.01) && ((i+0.5) - (j + 1.5)*b1 - b2 > -0.01) && ((j + 1) < (int) (distMatrix[i]).size()) ) {
		if (debug)std::cout << "	right";
		right = distDTWdeepSearch(curDist + distMatrix[i][j], curDeepRight, distMatrix, i, j+1);
	}
	// проверяем можем ли мы идти по диагонале - если да - то считаем путь если пойдем туда
	double diag = -1.0;
	int curDeepDiag = curDeep;
	if ( ( (i+1.5) - (j + 1.5)*a1- a2 < 0.01) && ( (i+1.5) - (j + 1.5)*b1 - b2 > -0.01) && ((j + 1) < (int) (distMatrix[i]).size()) && ( (i + 1) < (int) distMatrix.size()) ) {
		if (debug)std::cout << "	diag";
		diag = distDTWdeepSearch(curDist + distMatrix[i][j], curDeepDiag, distMatrix, i+1, j+1);
	}
	// собираем в up наименьшее значение
	curDeep = curDeepUp;
	if (right >= -0.01) {
		if ( (right < up) || (up < -0.01) ) { 
			up = right;
			curDeep = curDeepRight;
		}
	}
	if (diag >= -0.01) {
		if ( (diag < up) || (up < -0.01) ) {
			up = diag;
			curDeep = curDeepDiag;
		}
	}
	// проверяем - нашелся ли хоть один путть до крайнего диагонального элемента
	if (up < -0.01) {
		if (debug) {
			std::cout << "\n 	 up is " << up << "\n";
			std::cout << " 	 curDeep is " << curDeep << "\n";
			std::cout << " 	 i is " << i << "\n";
			std::cout << " 	 j is " << j << "\n";
			std::cout << " 	 distMatrix.size() is " << distMatrix.size() << "\n";
			std::cout << " 	 (distMatrix[i]).size() is " << (distMatrix[i]).size() << "\n";
			std::cout << " 	 (i + 1.5) - (j+0.5)*a1- a2 is " << (i + 1.5) - (j+0.5)*a1- a2 << "\n";
			std::cout << " 	 (i + 1.5) - (j+0.5)*b1 - b2 is " << (i + 1.5) - (j+0.5)*b1 - b2  << "\n";
			std::cout << " 	 (i+0.5) - (j+1.5)*a1- a2 is " << (i+0.5) - (j+1.5)*a1- a2 << "\n";
			std::cout << " 	 (i+0.5)- (j+1.5)*b1 - b2 is " << (i+0.5)- (j+1.5)*b1 - b2  << "\n";
			std::cout << " 	 (i + 1.5) - (j+1.5)*a1- a2 " << (i + 1.5) - (j+1.5)*a1- a2 << "\n";
			std::cout << " 	 (i + 1.5) - (j+1.5)*b1 - b2 is " << (i + 1.5) - (j+1.5)*b1 - b2  << "\n"; 
		}
		 throw AxiomLibException ("Error in RefMarkupSimple::distDTWdeepSearch: wrong result of subfunction distDTWdeepSearch. ");
	}
	// возвращаем наименьшее расстояние между элементов в (i,j) и (max_i, max_j) - крайний диагональный элемент
	if (debug) std::cout << "	down";
	curDeep++;
	return up;
}


/****************************************************************************
*					RefMarkupSimple::distDTWwideSearch
*
*	Description:	Вспомогательная функция для subcheckDTW - 
*				опрелеление расстояния между двумя подстроками 
*				с использованием алгоритма DTW - метод поиска в ширину
*	Parameters:	preResult - вектор для записи предварительных результатов полученных по матрице distMatrix
*				distMatrix - матрица расстояний между символами строк
*	Returns:		double - введено для соответствия поиску в глубину и не несет информации
*	Throws:		AxiomLibException - если произошла ошибка алгоритма в поиске в ширину
*	Author:		dk
*	History:
*
****************************************************************************/
double RefMarkupSimple::distDTWwideSearch (std::vector <double> &preResult, std::vector < std::vector <float> > &distMatrix) const {
	const bool debug = false;
	// Инициализируем требуемые переменные
	const int dimY = (int) distMatrix.size();
	const int dimX = (int) distMatrix[distMatrix.size() - 1].size();
	// Создаем матрицы для поиска пути (матрица числителей и матрица знаменателей) и инициализирем их
	std::vector < std::vector <float> > nomin; // требуется две матрицы в данном алгоритме - для хранения числителей 
	std::vector <std::vector <float> > denom; // и для хранения знаменателей (фактически здесь длина пути с наименьшей DTW-стоимостью до данной ячейки) - соотношение из которых дает расстояние 
	nomin.resize(dimY);
	denom.resize(dimY);
	for (int i = 0; i < dimY; i++) {
		(nomin[i]).resize(dimX);
		(denom[i]).resize(dimX);
	}
	nomin[dimY - 1][dimX - 1] = distMatrix[dimY - 1][dimX - 1];
	denom[dimY - 1][dimX - 1] = 1.0;
	// Создание матрицы допустимости - она содержит Ture в тех ячейках, которые могут входить в допустимый путь
	std::vector <std::vector <bool> > permit; 
	permit.resize(dimY);
	for (int i = 0; i < dimY; i++) {
		(permit[i]).resize(dimX);
		for (int j = 0; j < dimX; j++) 
			permit[i][j] = checkInRange(i, j);
	}
	// Запуск алгоритма поиска наименьшего расстояния
	// заполнение крайнего правого столбца - наполнение ячейки в данном столбце зависит от вышестоящей ячейки
	for (int i = dimY - 2; i >= 0; i--) {
		if (permit[i][dimX-1]) {
			denom[i][dimX-1] = denom[i+1][dimX-1] + 1;
			if (distMatrix[i][dimX-1] < 0.01)
				nomin[i][dimX-1] = nomin[i+1][dimX-1];
			else
				nomin[i][dimX-1] = nomin[i+1][dimX-1] + averIns;
		}
		else
			break;
	}
	// заполнение верхней строки - наполнение ячейки а данной строке зависит от одной соседней
	for (int j = dimX - 2; j >=0; j--) {
		if (permit[dimY-1][j]) {
			denom[dimY-1][j] = denom[dimY-1][j+1] + 1;
			if (distMatrix[dimY-1][j] < 0.01)
				nomin[dimY-1][j] = nomin[dimY-1][j+1];
			else
				nomin[dimY-1][j] = nomin[dimY-1][j+1] + averDel;
		}
		else
			break;
		
	}
	// Заполнение оставшихся частей матриц nomin и denim
	int j = 0; 
	float tempUp, tempDiag, tempRight;
	for (int sum = dimX + dimY - 4; sum >=0; sum--) {
		if (debug) std::cout << "\n	sum is " << sum << "	dimX is " << dimX << "	dimY is " << dimY;
		for (int i = min (sum, dimY - 2); i >= 0; i--) {
			j = sum - i; 
			// если индекс выходит за границу - то естественно не ведем дальнейших расчетов
			if (j > (dimX - 2)) break;
			// если попали в облать - заперщенную наложенными ограничениями
			if (permit[i][j] == false) continue;
			if (debug) std::cout << "\n	( " << i << ", " << j << " )" << "	sum is " << sum << "	dimX is " << dimX << "	dimY is " << dimY;
			// если область разрешенная - то вычисляем стоимость путей через различные направления
			// Путь идущий сверху
			if (permit[i+1][j]) 
				if (distMatrix[i][j] < 0.01) tempUp = (float) ((nomin[i+1][j]) / (denom[i+1][j] + 1.0));
				else tempUp = (float) ((nomin[i+1][j] + averIns) / (denom[i+1][j] + 1.0));
			else
				tempUp = -1.0;
			// Путь идущий справа
			if  (permit[i][j+1]) 
				if (distMatrix[i][j] < 0.01) tempRight = (float) ((nomin[i][j+1]) / (denom[i][j+1] + 1.0));
				else tempRight = (float) ((nomin[i][j+1] + averDel) / (denom[i][j+1] + 1.0));
			else
				tempRight = -1.0;
			// Путь идущий по диагонали
			if  (permit[i+1][j+1]) 
				if (distMatrix[i][j] < 0.01) tempDiag = (float) ((nomin[i+1][j+1]) / (denom[i+1][j+1] + 1.0));
				else tempDiag = (float) ((nomin[i+1][j+1] + averChg) / (denom[i+1][j+1] + 1.0));
			else
				tempDiag = -1.0;
			// Выбираем путь с наименьшей стоимостью и его записываем в данную ячейку
			if ( (tempDiag > -0.01) && ((tempUp >= tempDiag) || (tempUp < -0.01)) && ((tempRight >= tempDiag) || (tempRight < -0.01)) ) {
				denom[i][j] = denom[i+1][j+1] + (float) 1.0;
				if (distMatrix[i][j] < 0.01) nomin[i][j] = nomin[i+1][j+1];
				else nomin[i][j] = nomin[i+1][j+1] + averChg;
				continue;
			}
			if ( (tempRight > -0.01) && ( (tempUp >= tempRight)|| (tempUp < -0.01) ) ) {
				denom[i][j] = denom[i][j+1] + (float) 1.0;
				if (distMatrix[i][j] < 0.01) nomin[i][j] = nomin[i][j+1];
				else nomin[i][j] = nomin[i][j+1] + averDel;
				continue;
			}
			if (tempUp > -0.01) {
				denom[i][j] = denom[i+1][j] + (float) 1.0;
				if (distMatrix[i][j] < 0.01) nomin[i][j] = nomin[i+1][j];
				else nomin[i][j] = nomin[i+1][j] + averIns;
				continue;
			}
			if (debug) {
				std::cout << "\n 	 i is " << i << "\n";
				std::cout << " 	 j is " << j << "\n";
				std::cout << " 	 tempUp is " << tempUp << "\n";
				std::cout << " 	 tempRight is " << tempRight << "\n";
				std::cout << " 	 tempDiag is " << tempDiag << "\n";
				/*std::cout << " 	 a1 is " << a1 << "\n";
				std::cout << " 	 a2 is " << a2 << "\n";
				std::cout << " 	 b1 is " << b1 << "\n";
				std::cout << " 	 b2 is " << b2 << "\n";
				std::cout << "\n	Permit is \n";
				for (int i = permit.size() - 1; i >=0 ; i--) {
					for (int j = 0; j < (permit[i]).size(); j++)
						std::cout << "  " << permit[i][j]; 
					std::cout << "\n";
				}*/
			}
			throw AxiomLibException ("Error in RefMarkupSimple::distDTWwideSearch: no way was selected. ");
		}
	}
	// Заполняем массив preResult полученными результатами
	for (unsigned int t = 0; t < preResult.size(); t++) {
		preResult[t] = sqrt (nomin[0][t]) / denom[0][t];
	}
	return 0.0;
}


/****************************************************************************
*					RefMarkupSimple::checkInRange
*
*	Description:	Вспомогательная функция для distDTWwideSearch
*				Определеляет - укладываются ли координаты ячейки в матрице расстояний 
*				в накладываемые ограницения на допустимый путь в данной матрице
*	Parameters:	i - координата в матрице по вертикале (номер строки) 
*				j - координата в матрице по горизонтале (номер столбца)
*	Returns:		bool - результат проверки ограничений
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
inline bool RefMarkupSimple::checkInRange (const int i, const int j) const{
	if ( ( (i+0.5) - (j + 0.5)*a1- a2 < 0.01) && ( (i+0.5) - (j + 0.5)*b1 - b2 > -0.01) ) {
		return true;
	}
	return false;
}


// Тестовая функция - вывод настроек
int RefMarkupSimple::setOut () {
	std::cout << "\n	RefMarkupSimple::setOut:\n";
	
    std::cout << "\n	dtwUse = " << dtwUse;
    std::cout << "	useDTWwideSearch = " << useDTWwideSearch;
    std::cout << "	useDTWdeepSearch = " << useDTWdeepSearch;
    std::cout << "\n	a1= " << a1 << "	a2 = " << a2 << "	 b1 = " << b1 << "	b2 = " << b2;

    // Одна разметка
    std::cout << "\n	Axioms:";
    for (unsigned int i = 0; i < axiomTypes.size(); i++ ) {
    	std::cout << "   " << axiomTypes[i];
    }
    //std::cout << "\n";
    
    // Хранение значения целевой функции сравнения, от которого считать сравниваемые методом использования метрики участки рядов различными
    std::cout << "\n	maxDistance = " << maxDistance;
    // Указания какие операции использовать
    std::cout << "\n	delUse = " << delUse;
    std::cout << "	insUse = " << insUse;
    std::cout << "	chgUse = " << chgUse;
    // Цены за удаление, вставку, замену символа в строке эталонной разметки и в строке разметки ряда.
    std::cout << "\n	delPrice:";
    for (unsigned int i = 0; i < delPrice.size(); i++) {
    	std::cout << " " << delPrice[i];
    }
    std::cout << "\n	insPrice:";
    for (unsigned int i = 0; i < insPrice.size(); i++) {
    	std::cout << " " << insPrice[i];
    }
    std::cout << "\n	chgPrice:";
    for (unsigned int i = 0; i < chgPrice.size(); i++) {
    	std::cout << " " << chgPrice[i];
    }
        
    // Усреднения цен удаления, вставки и замены символа- используется в алгоритме DTW с алгоритмом поиска в ширину 
    std::cout << "\n	averDel = " << averDel;
    std::cout << "	averIns = " << averIns;
    std::cout << "	averChg = " << averChg;
    
    // коэффициенты по усечению области рассмотрения путей в матрице расстояний в алгоритме DTW
    std::cout << "\n	truncationAxioms = " << truncationAxioms;
    std::cout << "	truncationRow = " << truncationRow;
    std::cout << "\n	coef1 = " << coef1;
    std::cout << "	coef2 = " << coef2;    
    std::cout << "\n";
	//std::cout << "To continue press Enter.";
	//std::cin.get();
	return 0;
}


/****************************************************************************
*					RefMarkupSimple::operator=
*
*	Description:	Оператор присваивания
*	Parameters:		Присваиваемый объект класса эталонной разметки
*	Returns:		*this
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
RefMarkupSimple& RefMarkupSimple::operator= (const RefMarkupSimple& second) {
	axiomTypes.clear();
	axiomTypes = second.axiomTypes;
	maxDistance = second.maxDistance;
	delPrice.clear();
	delPrice = second.delPrice;
	insPrice.clear();
	insPrice = second.insPrice;
	chgPrice.clear();
	chgPrice = second.chgPrice;
	averDel = second.averDel;
	averIns = second.averIns;
	averChg = second.averChg;
	delUse = second.delUse;
	insUse = second.insUse;
	chgUse = second.chgUse;
	truncationAxioms = second.truncationAxioms;
	truncationRow = second.truncationRow;
	coef1 = second.coef1;
	coef2 = second.coef2;
	dtwUse = second.dtwUse;
	useDTWwideSearch = second.useDTWwideSearch;
	useDTWdeepSearch = second.useDTWdeepSearch;
	return *this;
}
