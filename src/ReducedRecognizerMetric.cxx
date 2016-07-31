/****************************************************************************
*					ReducedRecognizerMetric.cxx
*
*	Description:	Функции класса ReducedRecognizerMetric - Реализация
*	Author:			dk
*	History:		
*
****************************************************************************/
#include "ReducedRecognizerMetric.h"
#include "Defines.h"

using namespace AxiomLib;


// Стандартный конструктор - задание значений внутренних переменных по умолчанию
ReducedRecognizerMetric::ReducedRecognizerMetric (void) { 
	
}


// Стандартный деструктор - удаление всех динамических объектов класса
ReducedRecognizerMetric::~ReducedRecognizerMetric (void) { 
	
}


/****************************************************************************
*                       ReducedRecognizerMetric::setParamsFromEnv
*
*       Description:    Инициализировать параметры данными из env
*       Parameters:     const Environment& env - окружение
*       Returns:        0
*       Throws:         AxiomLibException - если нет какого-топараметра
*       Author:         gevor
*       History:
*
****************************************************************************/
int ReducedRecognizerMetric::setParamsFromEnv (const Environment& env) {
	// установка параметра точности распознавателя из конфига
	ReducedRecognizerDistance::setParamsFromEnv(env);
	
	return 0;
}


/****************************************************************************
*                       ReducedRecognizerMetric::run
*
*       Description:    Функция вызывает поиска разметки ethalon в markUp и запись результатов поиска в result
*       Parameters:     markUp - данный для анализа ряд разметки
*						etalon - разметка траектории нештатного поведения, т.е. искомый участок ряда
*						result - вектор с результатами распознавания
*       Returns:        0
*       Throws:         -
*       Author:         dk
*       History:
*
****************************************************************************/
int ReducedRecognizerMetric::run (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) {
	return countMetricDist(markUp, etalon, result);
}

/****************************************************************************
*                       ReducedRecognizerMetric::countMetricDist
*
*       Description:    Функция реализует поиск разметки ethalon в markUp и запись результатов поиска в result
*       Parameters:     markUp - данный для анализа ряд разметки
*						etalon - разметка траектории нештатного поведения, т.е. искомый участок ряда
*						result - вектор с результатами распознавания
*       Returns:        0
*       Throws:         -
*       Author:         dk
*       History:
*
****************************************************************************/
int ReducedRecognizerMetric::countMetricDist (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) const {
	// Сохраняем длину ряда, чтобы каждый раз к функции не обращаться
	int len = (int) markUp.size();
	result.resize(len, 0);
	int etLen = etalon.size();
	if (etLen < 1)
		return 0;
	int etLenMinusOne = etLen - 1;
	double curPrecisionF = precision() * (double) etLen;
	//int curPrecision = round (curPrecisionF);
	// Вычисление границ для поиска соответствия для ряда etalon
	int minLen, maxLen;
	maxLen = round ((1.0 + precision()) * ((double) etLen));
	minLen = round ((1.0 - precision()) * ((double) etLen));
	if (minLen < 1)
		minLen = 1;
	// Подготавливаем матрицу для подсчета минимального расстояния
	std::vector <std::vector <int> > matrix;
	matrix.resize (etLen);
	// Сначала отдельно обрабатываем область от 0 до maxLen в ряду markUp
	int upTo, j, l, curVal;
	int rightDist, diagDist, downDist, minDist;
	upTo = min (len, maxLen);
	for (int i = minLen-1; i < upTo; i++) {
		// изменяем размер матрицы в соответствии с текущим обрабатываемым отсчетом i
		for (j = 0; j < etLen; j++) {
			matrix[j].resize(i+1);
		}
		// Вычисляем значение в крайнем элементе матрицы
		if (markUp[i] == etalon[etLenMinusOne]) {
			matrix[etLenMinusOne][i] = 0;
			curVal = 0;
		} else {
			matrix[etLenMinusOne][i] = 1;
			curVal = 1;
		}
		// Вычисляем значение в крайних элементах матрицы
		for (int k = etLen - 2; k >= 0; k--) {
			curVal++;
			matrix[k][i] = curVal;
		}
		curVal = matrix[etLenMinusOne][i];
		for (int k = i-1; k >= 0; k--) {
			curVal++;
			matrix[etLenMinusOne][k] = curVal;
		}
		// Вычисляем значение в элементах матрицы
		for (int k = 1; k <= min(etLenMinusOne, i); k++) {
			// идем по горизонтали
			j = etLenMinusOne - k;
			for (l = i - k; l >= 0; l--) {
				// вычисляем значение для всех возможных путей
				rightDist = matrix[j][l+1] + 1;
				diagDist =  matrix[j+1][l+1];
				if (markUp[l] != etalon[j])
					diagDist++;
				downDist =  matrix[j+1][l] + 1;
				if ((diagDist <= rightDist) && (diagDist <= downDist)) {
					matrix[j][l] = diagDist;
				} else {
					if (rightDist <= downDist) {
						matrix[j][l] = rightDist;
					} else {
						matrix[j][l] = downDist;
					}
				}
			}
			// идем по вертикали
			l = i - k;
			for (j = etLenMinusOne - k - 1; j >= 0; j--) {
				// вычисляем значение для всех возможных путей
				rightDist = matrix[j][l+1] + 1;
				diagDist =  matrix[j+1][l+1];
				if (markUp[l] != etalon[j])
					diagDist++;
				downDist =  matrix[j+1][l] + 1;
				if ((diagDist <= rightDist) && (diagDist <= downDist)) {
					matrix[j][l] = diagDist;
				} else {
					if (rightDist <= downDist) {
						matrix[j][l] = rightDist;
					} else {
						matrix[j][l] = downDist;
					}
				}
			}
		}
		// Выбираем наименьшее значение среди возможных соответствий
		minDist = matrix[0][0];
		for (int k = 1; k <= (i - minLen + 1); k++) {
			if (matrix[0][k] < minDist)
				minDist = matrix[0][k];
		}
		result[i] = (double)minDist / (double) etLen;
//		// Определяем было ли совпадение
//		if (minDist <= curPrecision) {
//			result[i] = 1;
//		} else {
//			result[i] = 0;
//		}
	}
	// Продолжаем идти по ряду - только теперь его длина заведомо больше maxLen
	int maxLenMinusOne = maxLen - 1;
	upTo = min(etLenMinusOne, maxLenMinusOne);
	for (int i = maxLen; i < len; i++) {
		// Вычисляем значение в крайнем элементе матрицы
		if (markUp[i] == etalon[etLenMinusOne]) {
			matrix[etLenMinusOne][maxLenMinusOne] = 0;
			curVal = 0;
		} else {
			matrix[etLenMinusOne][maxLenMinusOne] = 1;
			curVal = 1;
		}
		// Вычисляем значение в крайних элементах матрицы
		for (int k = etLen - 2; k >= 0; k--) {
			curVal++;
			matrix[k][maxLenMinusOne] = curVal;
		}
		curVal = matrix[etLenMinusOne][maxLenMinusOne];
		for (int k = maxLen - 2; k >= 0; k--) {
			curVal++;
			matrix[etLenMinusOne][k] = curVal;
		}
		// Вычисляем значение в элементах матрицы
		for (int k = 1; k <= upTo; k++) {
			// идем по горизонтали
			j = etLenMinusOne - k;
			for (l = maxLenMinusOne - k; l >= 0; l--) {
				// вычисляем значение для всех возможных путей
				rightDist = matrix[j][l+1] + 1;
				diagDist =  matrix[j+1][l+1];
				if (markUp[i - maxLenMinusOne + l] != etalon[j])
					diagDist++;
				downDist =  matrix[j+1][l] + 1;
				if ((diagDist <= rightDist) && (diagDist <= downDist)) {
					matrix[j][l] = diagDist;
				} else {
					if (rightDist <= downDist) {
						matrix[j][l] = rightDist;
					} else {
						matrix[j][l] = downDist;
					}
				}
			}
			// идем по вертикали
			l = maxLenMinusOne - k;
			for (j = etLenMinusOne - k - 1; j >= 0; j--) {
				// вычисляем значение для всех возможных путей
				rightDist = matrix[j][l+1] + 1;
				diagDist =  matrix[j+1][l+1];
				if (markUp[i - k] != etalon[j])
					diagDist++;
				downDist =  matrix[j+1][l] + 1;
				if ((diagDist <= rightDist) && (diagDist <= downDist)) {
					matrix[j][l] = diagDist;
				} else {
					if (rightDist <= downDist) {
						matrix[j][l] = rightDist;
					} else {
						matrix[j][l] = downDist;
					}
				}
			}
		}
		// Выбираем наименьшее значение среди возможных соответствий
		minDist = matrix[0][0];
		for (int k = 1; k <= (maxLen - minLen); k++) {
			if (matrix[0][k] < minDist)
				minDist = matrix[0][k];
		}
		result[i] = (double)minDist / (double)etLen;
		// Определяем было ли совпадение
//		if (minDist <= curPrecision) {
//			result[i] = 1;
//		} else {
//			result[i] = 0;
//		}
	}

	return 0;
}
