/****************************************************************************
*				CompareStatisticSimple.cxx
*
*	Description:	Функции класса CompareStatisticSimple - Реализация
*	Author:		dk,gevor
*	History:		
*
****************************************************************************/
#include "CompareStatisticSimple.h"

#include <boost/lexical_cast.hpp>

using namespace AxiomLib;


/****************************************************************************
*					CompareStatisticSimple::CompareStatisticSimple
*
*	Description:	Стандартный конструкор с заданием значений параметров класса по умолчанию
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
CompareStatisticSimple::CompareStatisticSimple (void) {
	this->left = 0;
	this->right = 0;
	this->ImH = 10;
	this->minPoints = 0;
}


/****************************************************************************
*					CompareStatisticSimple::name
*
*	Description:	Возвращает имя данного класса кодсчета статистики размекти
*	Parameters:	-
*	Returns:		std::string - имя данного класса
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string CompareStatisticSimple::name (void) {
	std::string toReturn("CompareStatisticSimple");
	return toReturn;
}

/****************************************************************************
*                       CompareStatisticSimple::setParamsFromEnv
*
*       Description:    Инициализировать параметры данными из env
*       Parameters:     const Environment& env - среда
*       Returns:        0
*       Throws:         AxiomLibException - если нет какого-топараметра
*       Author:         gevor
*       History:
*
****************************************************************************/
int CompareStatisticSimple::setParamsFromEnv (const Environment& env) {
  // У нас есть два параметра: "gcFirst" и "gcSecond"
  if ( (env.getIntParamValue(left, "ccLeft") < 0) ||
       (env.getIntParamValue(right, "ccRight") < 0 ) ||
       (env.getIntParamValue(ImH, "ccNumPoints") < 0 ) ||
	   (env.getIntParamValue(minPoints, "ccMinNumPoints") < 0)) {
    throw AxiomLibException ("Error in CompareStatisticSimple::setParamsFromEnv - can not find all needed parameners.");
  }
  
  if (left < 0 || right < 0 || ImH < 0 || minPoints < 0)
    throw AxiomLibException ("Error in CompareStatisticSimple::setParamsFromEnv - invalid parameter values.");

  return 0;
}


/****************************************************************************
*					CompareStatisticSimple::setParams
*
*	Description:	Функция задания значений параметров класса
*	Parameters:	Возможно в качестве значений параметров этой функции давать нулевые 
*				значения - и тогда соответсвующий параметр не будет обновляться.
*				newLeft - число точек учитываемых при распозновании слева
*				newRight - число точек учитываемых при распозновании справа
*				newImH - число со скольки опознований одного образа учитывать это в first
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int CompareStatisticSimple::setParams (const int newLeft, const int newRight, const int newImH) {
	if ( (newLeft < 0) || (newRight < 0) || (newImH < 0) ) {
		throw AxiomLibException ("Error in CompareStatisticSimple::setParams: wrong input parameters.");
	}
	if (newLeft > 0) { this->left = newLeft; }
	if (newRight > 0) { this->right = newRight; }
	if (newImH > 0) { this->ImH = newImH; }
	return 0;
}


/****************************************************************************
*					CompareStatisticSimple::setParams
*
*	Description:	Функция задания значений параметров класса
*	Parameters:	params - отображение из строки с названием параметра класса в устанавливаемое значение этого параметра в виде строки
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int CompareStatisticSimple::setParams (std::map<std::string, std::string> &params) {
	std::map<std::string, std::string>::iterator iter;
	std::string strTemp;
	int temp;
	for (iter = params.begin(); iter != params.end(); ++iter) {
		strTemp = iter->first;
		if (strTemp == "ccLeft") {
			strTemp = iter->second;
			temp = atoi (strTemp.c_str());
			if (temp < 0) {
				throw AxiomLibException ("Error in CompareStatisticSimple::setParams: set in config file value of ccLeft is wrong");
			}
			left = temp;
			//std::cout << "\n	left is " << left << "\n";
			continue;
		}
		if (strTemp == "ccRight") {
			strTemp = iter->second;
			temp = atoi (strTemp.c_str());
			if (temp < 0) {
				throw AxiomLibException ("Error in CompareStatisticSimple::setParams: set in config file value of ccRight is wrong");
			}
			right = temp;
			//std::cout << "\n	right is " << right << "\n";
			continue;
		}
		if (strTemp == "ccNumPoints") {
			strTemp = iter->second;
			temp = atoi (strTemp.c_str());
			if (temp < 0) {
				throw AxiomLibException ("Error in CompareStatisticSimple::setParams: set in config file value of ccNumPoints is wrong");
			}
			ImH = temp;
			//std::cout << "\n	ImH is " << ImH << "\n";
			continue;
		}
		// значит в отобразении указан какой - то еще параметр - выдаем исключение так как это может быть важно
		throw AxiomLibException ("Error in CompareStatisticSimple::setParams: in config - too many params for CompareStatisticSimple Class ");
	}
	return 0;
}


/****************************************************************************
*					CompareStatisticSimple::copy
*
*	Description:	Возвращает копию данного класса
*	Parameters:		-
*	Returns:		CompareStatistic* - указатель на копию данного класса
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
CompareStatistic* CompareStatisticSimple::copy (void) {
//	CompareStatisticSimple* toReturn;
//	toReturn = new CompareStatisticSimple;
//	toReturn->setParams (left, right, ImH);
//	return (CompareStatistic*) toReturn;
	return new CompareStatisticSimple(*this);
}


/****************************************************************************
*					CompareStatisticSimple::getStatistic
*
*	Description:	Получить число ошибок первого и второго рода по ряду и корректной разметке
*	Parameters:	row - размеченный систсемой аксиом ряд
*				correct - корректная разметка данного ряда
*				first - заполняемое число ошибок - число ложных распознований
*				second - заполняемое число ошибок - число неверных распознований
*	Returns:		положительно целое  - если программа удачно завершается - число образов
*				-1 - если возникает противоречие и функция не выполняется
*	Throws:		AxiomLibException - если число элементов в веторе разметки и число элементов 
*								в векторе корректной разметки разное
*	Author:		dk
*	History:
*
****************************************************************************/
int CompareStatisticSimple::getStatistic (const std::vector<int> &result, const std::vector<int> &correct, double &first, double &second, const bool comments) {
	int toReturn = 0;
	std::vector<int> row = result;
	if(minPoints > 1) {
		for(int i = 0; i < (int)row.size(); ++i) {
			if(row[i] == 0) {
				continue;
			}
			int from = i - left;
			if(from < 0) {
				from = 0;
			}
			int upTo = i + right;
			if(upTo >= (int)row.size()) {
				upTo = (int)row.size() - 1;
			}
			int count = 0;
			for(int j = from; j <= upTo; ++j) {
				if(result[j] == row[i]) {
					++count;
				}
			}
			
			if(count < minPoints) {
				row[i] = 0;
			}
		}
	}
	if (row.size() == correct.size()) {
		// переменные для накопления числа ошибок
		int f = 0;
		int s = 0;
		// параметры алгорима - в какой окрестности искать соответствие - задаются в setParams
		//this->left = 2;
		//this->right = 2;
		// вспомогательные переменные алгоритма
		int from;
		int upTo;
		int j;
		// флаги - обозначающие нахождение требуемого номера аксиомы или нахождение какого-то номера аксиомы
		int flag = 0;
		int smth = 0;
		int h;
		// собственно запуск алгоритма сопоставления
		// начинаем с 1 - потому как всего по 1 остчету нереално что-то понять
		// Counting First
		for (int i = 1; i < (int) row.size(); i++) {
				if (row[i] != 0) {
					flag = 0;
					smth = 0;
					h = i;
					while ((i < (int) row.size()) && (row[h] == row[i])) {
						if (flag == 0) {
							// Выставляем номер ряда откуда искать соответствие
							from = i - left;
							if (from < 0) {
								from = 0;
							}
							// Выставляем  номер ряда докуда искать соответствие
							upTo = i + right;
							if (upTo >= (int) row.size()) {
								upTo = (int) row.size() - 1;
							}
							// поиск собственно соответствия
							for (j = from; j <= upTo; j++) { 
								if (correct[j] != 0) {
									smth = 1;
								}
								if (correct[j] == row[i]) {
									flag = 1;
									break;
								}
							}
						}
						i++;
					}
					// анализ поиска
					if (flag == 0) {
							// Значит произошло ложное распознование - там где не должно было быть
							f++;
							//if (comments) {std::cout << "\n first is on " << h << "\n";}
					}
					// Коррекция числа ошибок первого рода
					if ( (i - h) > ImH ) {
						int numOfTr = 0;
						for (int n = h; n < i; n++) {
							if (correct[n] == row[h]) {
								numOfTr++;
							}
						}
						int fPlus = i - h - ImH - numOfTr;
						if (fPlus > 0) {
							f += fPlus;
							// f += (int) sqrt((double) fPlus); // увеличиваем число ошибок первого рода на разницу между максимальным ожидаемым
									// числом подяд идуших траекторий одного типаи реально стретившимся числом траекторий этого типа
						}
						continue;
					}
				}
		}
		// Counting second
		for (int i = 1; i < (int) row.size(); i++) {
				if (correct[i] != 0) {
					flag = 0;
					smth = 0;
					h = i;
					toReturn++;
					while ((i < (int) correct.size()) && (correct[h] == correct[i])) {
						if (flag == 0) {
							// Выставляем номер ряда откуда искать соответствие
							from = i - left;
							if (from < 0) {
								from = 0;
							}
							// Выставляем  номер ряда докуда искать соответствие
							upTo = i + right;
							if (upTo >= (int) row.size()) {
								upTo = (int) row.size() - 1;
							}
							// поиск собственно соответствия
							for (j = from; j <= upTo; j++) { 
								if (row[j] != 0) {
									smth = 1;
								}
								if (row[j] == correct[i]) {
									flag = 1;
									break;
								}
							}
						}
						i++;
					}
					// анализ поиска
					if ((flag == 0) && (smth == 0)) {
						// Значит распознование не произошло - даже ложного
						s++;
					}
				}
		}
		first = (double) f;
		second = (double) s;
		return toReturn;
	}
	else {
		std::string msg = "Error in CompareStatisticSimple::getStatistic: wrong input rows - different number of elements: ";
		msg += "row.size() = " + boost::lexical_cast<std::string>(row.size()) + ", ";
		msg += "correct.size() = " + boost::lexical_cast<std::string>(correct.size());
		throw AxiomLibException (msg);
	}
	
	return -1;
}
