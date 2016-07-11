/****************************************************************************
*					ReducedRecognizerDTW.cxx
*
*	Description:	Функции класса ReducedRecognizerDTW - Реализация
*	Author:			dk
*	History:		
*
****************************************************************************/
#include <boost/lexical_cast.hpp>

#include "ReducedRecognizerDTW.h"
#include "Logger.h"
#include "Defines.h"

using namespace AxiomLib;

#define QUESTION_MARK_LOCAL_INDEX -1

// Стандартный конструктор - задание значений внутренних переменных по умолчанию
ReducedRecognizerDTW::ReducedRecognizerDTW (void) { 
    //precision = 0.0;
	stretch = 1.0;
}

// Стандартный деструктор - удаление всех динамических объектов класса
ReducedRecognizerDTW::~ReducedRecognizerDTW (void) { 
	
}


/****************************************************************************
*                       ReducedRecognizerDTW::setParamsFromEnv
*
*       Description:    Инициализировать параметры данными из env
*       Parameters:     const Environment& env - среда
*       Returns:        0
*       Throws:         AxiomLibException - если нет какого-топараметра
*       Author:         wictor
*       History:
*
****************************************************************************/
int ReducedRecognizerDTW::setParamsFromEnv (const Environment& env) {
	// precision устанавливается в родителе
	ReducedRecognizerDistance::setParamsFromEnv(env);
	
	if (env.getDoubleParamValue(stretch, "Stretch") < 0) {
		throw AxiomLibException("ReducedRecognizerDTW::setParamsFromEnv : stretch is undefined.");
	}
	if (stretch < 1.0)
		throw AxiomLibException("ReducedRecognizerDTW::setParamsFromEnv : stretch has invalid value .");

	return 0;
}


// Редирект на функцию типа const
int ReducedRecognizerDTW::run (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) {
	return countDTWDist (markUp, etalon, result);
}


/****************************************************************************
*                       ReducedRecognizerDTW::run
*
*       Description:    Функция поиска разметки ethalon в markUp и запись результатов поиска в result
*       Parameters:     markUp - данный для анализа ряд разметки
*						etalon - разметка траектории нештатного поведения, т.е. искомый участок ряда
*						result - вектор с результатами распознавания
*       Returns:        0
*       Throws:         -
*       Author:         dk, wictor (запись в result именно расстояний)
*       History:
*
****************************************************************************/
int ReducedRecognizerDTW::countDTWDist (const std::vector <int> &markup, const std::vector<int> &etalon, std::vector <double> &result) const {
	// Вычисление границ для поиска соответствия для ряда etalon
	int minLen, maxLen;
	maxLen = round (stretch * (double) etalon.size());
	minLen = round (((double) etalon.size()) / stretch);
	if (minLen < 1)
		minLen = 1;
	// Сохраняем длину ряда, чтобы каждый раз к функции не обращаться
	int len = (int) markup.size();
	result.assign(len, -1);
	// Подготавливаем матрицы для ошибок и длины пути
	std::vector <std::vector <int> > matrixLen, matrixErr;
	int etLen = etalon.size();
	if (etLen < 1)
		return 0;
	int etLenMinusOne = etLen - 1;
	matrixLen.resize (etLen);
	matrixErr.resize (etLen);
	// Сначала отдельно обрабатываем область от 0 до maxLen в ряду markUp
	int upTo, j, l, curVal;
	double minDist, curDist;
	int rightDist, diagDist, downDist;
	upTo = min (len, maxLen);
	for (int i = minLen-1; i < upTo; i++) {
		// составляем матрицы для длины и ошибки в пути
		for (j = 0; j < etLen; j++) {
			matrixErr[j].resize(i+1);
			matrixLen[j].resize(i+1);
		}
		// Вычисляем значение в крайнем элементе матрицы
        if (markup[i] == etalon[etLenMinusOne] || etalon[etLenMinusOne] == QUESTION_MARK_LOCAL_INDEX)
			curVal = 0;
		else
			curVal = 1;
		matrixErr[etLenMinusOne][i] = curVal;
		matrixLen[etLenMinusOne][i] = 1;
		// Вычисляем значение в крайних элементах матрицы
		for (int k = etLen - 2; k >= 0; k--) {
            if (markup[i] != etalon[k] && etalon[k] != QUESTION_MARK_LOCAL_INDEX)
				curVal++;
			matrixErr[k][i] = curVal;
			matrixLen[k][i] = etLen - k;
		}
		curVal = matrixErr[etLenMinusOne][i];
		for (int k = i-1; k >= 0; k--) {
            if (markup[k] != etalon[etLenMinusOne] && etalon[etLenMinusOne] != QUESTION_MARK_LOCAL_INDEX)
				curVal++;
			matrixErr[etLenMinusOne][k] = curVal;
			matrixLen[etLenMinusOne][k] = i - k + 1;
		}
		// Вычисляем значение в элементах матрицы
		for (int k = 1; k <= min(etLenMinusOne, i); k++) {
			// идем по горизонтали
			j = etLenMinusOne - k;
			for (l = i - k; l >= 0; l--) {
                if (markup[l] == etalon[j] || etalon[j] == QUESTION_MARK_LOCAL_INDEX)
					curVal = 0;
				else
					curVal = 1;
				// вычисляем значение для всех возможных путей
				rightDist = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l+1] + 1);
				diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j][l+1] + 1);
				if (diagDist <= rightDist) {
					// тогда диагональный элемент сравниваем с нижним
					diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j+1][l+1] + 1);
					if (diagDist <= downDist) {
						matrixErr[j][l] = matrixErr[j+1][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				} else {
					// тогда правый элемент сравниваем с нижним
					rightDist  = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j][l+1] + 1);
					if (rightDist <= downDist) {
						matrixErr[j][l] = matrixErr[j][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				}
				/*
				rightDist = ((double) (matrixErr[j][l+1] + curVal))   / ((double)(matrixLen[j][l+1] + 1));
				diagDist =  ((double) (matrixErr[j+1][l+1] + curVal)) / ((double)(matrixLen[j+1][l+1] + 1));
				downDist =  ((double) (matrixErr[j+1][l] + curVal))   / ((double)(matrixLen[j+1][l] + 1));
				if ((diagDist <= rightDist) && (diagDist <= downDist)) {
					matrixErr[j][l] = matrixErr[j+1][l+1] + curVal;
					matrixLen[j][l] = matrixLen[j+1][l+1] + 1;
				} else {
					if (rightDist <= downDist) {
						matrixErr[j][l] = matrixErr[j][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				}
				*/
			}
			// идем по вертикали
			l = i - k;
			for (j = etLenMinusOne - k - 1; j >= 0; j--) {
                if (markup[l] == etalon[j] || etalon[j] == QUESTION_MARK_LOCAL_INDEX)
					curVal = 0;
				else
					curVal = 1;
				// вычисляем значение для всех возможных путей
				rightDist = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l+1] + 1);
				diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j][l+1] + 1);
				if (diagDist <= rightDist) {
					// тогда диагональный элемент сравниваем с нижним
					diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j+1][l+1] + 1);
					if (diagDist <= downDist) {
						matrixErr[j][l] = matrixErr[j+1][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				} else {
					// тогда правый элемент сравниваем с нижним
					rightDist  = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j][l+1] + 1);
					if (rightDist <= downDist) {
						matrixErr[j][l] = matrixErr[j][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				}
			}
		}
		// Выбираем наименьшее значение среди возможных соответствий
		minDist = ((double) matrixErr[0][0]) / ((double) matrixLen[0][0]);
		for (int k = 1; k <= (i - minLen + 1); k++) {
			curDist = ((double) matrixErr[0][k]) / ((double) matrixLen[0][k]);
			if (curDist < minDist)
				minDist = curDist;
		}
		result[i] = minDist;
		// Определяем было ли совпадение
//		if (minDist <= precision) {
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
        if (markup[i] == etalon[etLenMinusOne] || etalon[etLenMinusOne] == QUESTION_MARK_LOCAL_INDEX)
			curVal = 0;
		else
			curVal = 1;
		matrixErr[etLenMinusOne][maxLenMinusOne] = curVal;
		//matrixLen[etLenMinusOne][maxLenMinusOne] = 1; // Это присваивание уже и так было сделано
		// Вычисляем значение в крайних элементах матрицы
		for (int k = etLen - 2; k >= 0; k--) {
            if (markup[i] != etalon[k] && etalon[k] != QUESTION_MARK_LOCAL_INDEX)
				curVal++;
			matrixErr[k][maxLenMinusOne] = curVal;
			//matrixLen[k][maxLenMinusOne] = etLen - k; // Это присваивание уже и так было сделано
		}
		curVal = matrixErr[etLenMinusOne][maxLenMinusOne];
		for (int k = maxLen - 2; k >= 0; k--) {
            if (markup[i - maxLenMinusOne + k] != etalon[etLenMinusOne] && etalon[etLenMinusOne] != QUESTION_MARK_LOCAL_INDEX)
				curVal++;
			matrixErr[etLenMinusOne][k] = curVal;
			//matrixLen[etLenMinusOne][k] = maxLen - k; // Это присваивание уже и так было сделано
		}
		// Вычисляем значение в элементах матрицы
		for (int k = 1; k <= upTo; k++) {
			// идем по горизонтали
			j = etLenMinusOne - k;
			for (l = maxLenMinusOne - k; l >= 0; l--) {
                if (markup[i - maxLenMinusOne + l] == etalon[j] || etalon[j] == QUESTION_MARK_LOCAL_INDEX)
					curVal = 0;
				else
					curVal = 1;
				// вычисляем значение для всех возможных путей
				rightDist = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l+1] + 1);
				diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j][l+1] + 1);
				if (diagDist <= rightDist) {
					// тогда диагональный элемент сравниваем с нижним
					diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j+1][l+1] + 1);
					if (diagDist <= downDist) {
						matrixErr[j][l] = matrixErr[j+1][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				} else {
					// тогда правый элемент сравниваем с нижним
					rightDist  = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j][l+1] + 1);
					if (rightDist <= downDist) {
						matrixErr[j][l] = matrixErr[j][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				}
			}
			// идем по вертикали
			l = maxLenMinusOne - k;
			for (j = etLenMinusOne - k - 1; j >= 0; j--) {
                if (markup[i - k] == etalon[j] || etalon[j] == QUESTION_MARK_LOCAL_INDEX)
					curVal = 0;
				else
					curVal = 1;
				// вычисляем значение для всех возможных путей
				rightDist = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l+1] + 1);
				diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j][l+1] + 1);
				if (diagDist <= rightDist) {
					// тогда диагональный элемент сравниваем с нижним
					diagDist  = (matrixErr[j+1][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j+1][l+1] + 1);
					if (diagDist <= downDist) {
						matrixErr[j][l] = matrixErr[j+1][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				} else {
					// тогда правый элемент сравниваем с нижним
					rightDist  = (matrixErr[j][l+1] + curVal)*(matrixLen[j+1][l] + 1);
					downDist = (matrixErr[j+1][l] + curVal)*(matrixLen[j][l+1] + 1);
					if (rightDist <= downDist) {
						matrixErr[j][l] = matrixErr[j][l+1] + curVal;
						matrixLen[j][l] = matrixLen[j][l+1] + 1;
					} else {
						matrixErr[j][l] = matrixErr[j+1][l] + curVal;
						matrixLen[j][l] = matrixLen[j+1][l] + 1;
					}
				}
			}
		}
		// Выбираем наименьшее значение среди возможных соответствий
		minDist = ((double) matrixErr[0][0]) / ((double) matrixLen[0][0]);
		for (int k = 1; k <= (maxLen - minLen); k++) {
			curDist = ((double) matrixErr[0][k]) / ((double) matrixLen[0][k]);
			if (curDist < minDist)
				minDist = curDist;
		}
		result[i] = minDist;
		// Определяем было ли совпадение
//		if (minDist <= precision) {
//			result[i] = 1;
//		}  else {
//			result[i] = 0;
//		}
	}	

	return 0;
}
