/****************************************************************************
*
*			Функции класса Integral
*
****************************************************************************/

#include <string>
#include "Integral.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;

#define eps		0.0000000001


/****************************************************************************
*			Integral::Integral
*
*	Description:	Дефолтный конструктор, заполняет переменные
*				минимальным и максимальным значениями (или близкими)
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
Integral::Integral (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	minLevel = -100;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	maxLevel = 100;
	
	// Число точек требуемых для элементарного условия - слева
	left = 5;
	
	// Число точек требуемых для элементарного условия - справа
	right = 5;

	// Минимальная граница изменения для параметра minLevel, которая учитывается при подборе параметров перебором
	minLevelLimit = minLevel;
	
	// Максиомальная граница изменения для параметра maxLevel, которая учитывается при подборе параметров перебором
	maxLevelLimit = maxLevel;

	// Шаг изменения параметров minLevel и maxLevel, при подборе параметров перебором
	levelDelta = maxLevel - minLevel;
	
	// Максимальное значение параметра left, которое учитывается при подборе параметров перебором
	leftLimit = left;
	
	// Максимальное значение параметра right, которое учитывается при подборе параметров перебором
	rightLimit = right;
}

/****************************************************************************
*			Integral::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string Integral::name (void) const {
	std::string toReturn("integral");
	return toReturn;
}


/****************************************************************************
*			Integral::check
*
*	Description:	Проверка выполнения данного элем. условия для точки с номером k (начиная с нуля) ряда x
*	Parameters:	unsigned long k		- номер точки, для которой проверяется выполнимость, начиная с 0
*				std::vector<double> x	- рассматриваемый временной ряд
*	Returns:		1 если аксиома выполняется
*				0 если аксиома не выполняется
*				XXX: -1 если аксиома не применима, например из-за того, что точка k слишком близко к границе ряда
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	double curRes = 0;
	
	// проверка границ
	if (( (int) k < left) || ( (int) (x.size() - k) <= right)) {
		toReturn = -1;
		//throw AxiomLibException("Error in Integral::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = -1;
	curRes = 0.5 * (x[k-left] + x[k+right]);
	for (int i = (int) k - left + 1; i < (int) k + right; i++) {
		curRes += x[i];
	}
	//if ((left == 0) && (right == 0)) curRes = 0; // интеграл в точке
	if ((curRes <= maxLevel) && (curRes >= minLevel)) {
		return 1;
	} 
	else {
		return 0;
	}
	return toReturn;
}

/****************************************************************************
*			Integral::operator<
*
*	Description:	Сравнение двух Integral на включение (x < y).
*	Parameters:	Integral& second - правая часть выражения
*	Returns:		1 если < и second - это Integral
*				-1 если > и second - это Integral
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не Integral)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double Integral::operator< (const ElemCondition& second) const {
	// пытаемся получить y типа Integral* из second типа el_axiom
	const Integral* y = dynamic_cast<const Integral*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип Integral&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if (( maxLevel < y->maxLevel + 0.01) && ( minLevel > y->minLevel - 0.01))
			return 1;
		else if ((y->maxLevel <  maxLevel + 0.01) && (y->minLevel >  minLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			Integral::transmute
*
*	Description:	Мутация, изменяет значение параметров в не более чем p раз
*	Parameters:	double p - степень изменения параметра
*	Returns:		0 если все ok
*				XXX: -1 если p<0 или p>1 или по каким-то причинам не получилось поменять
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::transmute (const double p) {
	signed int toReturn=0;

	// Проверка границ p
	if (p < 0 || p > 1) {
		return -1;
	}
	
	// Получаем границы изменений
	double change = (((double)rand())/ ((double) RAND_MAX)) * p;
	
	if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
		// увеличиваем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// увеличиваем верхнюю границу
			maxLevel =  maxLevel + ( maxLevel -  minLevel) * change;
			toReturn = 1;
		}
		else {
			// уменьшаем нижнюю границу
			minLevel =  minLevel -  ( maxLevel -  minLevel) * change;
			toReturn = 1;
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			maxLevel =  maxLevel - ( maxLevel  -  minLevel) * change;
			toReturn = 1;
		}
		else {
			// увеличиваем нижнюю границу
			minLevel =  minLevel + ( maxLevel  -  minLevel) * change;
			toReturn = 1;
		}
	}
	
	// Изменяем left и right
	// Получаем границы изменений
	double change1 = (((double)rand())/ ((double) RAND_MAX)) * p;
	if ( ( (double)rand() )/ ( (double) RAND_MAX ) > 0.5 ) { // так как изменяем left и right не всегда
		int newVal = 0; 
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// увеличиваем размеры области
			if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
				// увеличиваем right
				newVal = right + round ((right -  left) * change1);
				if ((right > 0) && (newVal > 0)) right = newVal;
				toReturn = 1;
			}
			else {
				// увеличиваем left
				newVal = left -  round ((right -  left) * change1);
				if ((left > 0)&&(newVal > 0)) left = newVal;
				toReturn = 1;
			}
		}
		else {
			// уменьшаем размеры области
			if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
				// уменьшаем right
				newVal =  right - round ((right -  left) * change1);
				if ((right > 0)&&(newVal > 0)) right = newVal;
				toReturn = 1;
			}
			else {
				// уменьшаем left
				newVal = left + round ((right -  left) * change1);
				if ((left > 0)&&(newVal > 0)) left = newVal;
				toReturn = 1;
			}
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			Integral::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in Integral::setParams : zero parameter map.");
	}
	int flag;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("minLevel")) {
			// ok, нашли, находим value
			minLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("maxLevel")) {
			// ok, нашли, находим value
			maxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("left")) {
			// ok, нашли, находим value
			left = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("right")) {
			// ok, нашли, находим value
			right = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in Integral::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			Integral::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", minLevel);
	std::string str1 (temp);
	params["minLevel"] = str1;
	sprintf( temp, "%f", maxLevel);
	std::string str2 (temp);
	params["maxLevel"] = str2;
	sprintf( temp, "%d", left);
	std::string str3 (temp);
	params["left"] = str3;
	sprintf( temp, "%d", right);
	std::string str4 (temp);
	params["right"] = str4;
	return 0;
}


/****************************************************************************
*			Integral::setParamValue
*
*	Description:	Функция устанавливает значение одного параметра элементарного условия, 
*				если такой параметр присутствует в данном элементарном условии
*	Parameters:	param - значение, на которое хотим изменить параметр
*				paramName - имя параметра, занчение которого хотим поменять
*	Returns:		0 - если удалось обновить значение параметра
*				-1 - если не удалось обновить значение парметра (возможно 
*				       парамета с таким именем нет в данном элементарном условии)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "minLevel") {
		minLevel = param;
		return 0;
	}
	if (paramName == "maxLevel") {
		maxLevel = param;
		return 0;
	}
	if (paramName == "left") {
		left = round (param);
		return 0;
	}
	if (paramName == "right") {
		right = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			Integral::getParamValue
*
*	Description:	Функция записывает значение одного параметра элементарного условия, 
*				если такой параметр присутствует в данном элементарном условии
*	Parameters:	param - заполняемое значения параметра
*				paramName - срока с именем параметра, значение которого требуется получить
*	Returns:		0 - если удалось прочитать параметр
*				-1 - если не удалось считать параметр (возможно его просто нет в данном элементарном условии)
*				1 - если значение перечислимого типа
*				2 - если значение не числового и не перечислимго типа
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "minLevel") {
		param = minLevel;
		return 0;
	}
	if (paramName == "maxLevel") {
		param = maxLevel;
		return 0;
	}
	if (paramName == "left") {
		param = (double) left;
		return 0;
	}
	if (paramName == "right") {
		param = (double) right;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			Integral::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::getECParamNames (std::vector<std::string> &ecParamNames) const {
	ecParamNames.resize(4);
	ecParamNames[0] = "minLevel";
	ecParamNames[1] = "maxLevel";
	ecParamNames[2] = "left";
	ecParamNames[3] = "right";
	return 0;
}


/****************************************************************************
*			Integral::checkAndCompatibility
*
*	Description:	Проверяет совместимость элементарной аксиомы с другими по логической связке "И"
*	Parameters:	const ElemCondition& second - аксиома, с которой проверяем на совместимость
*	Returns:		1 если совместима
*				0 если не совместима
*				-1 если undefined
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Integral::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа Integral* из second типа el_axiom
	const Integral* y = dynamic_cast<const Integral*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип Integral&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( (( maxLevel >= y->maxLevel) && ( minLevel <= y->maxLevel)) || (( maxLevel >= y->minLevel) && ( minLevel <= y->minLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		return 1;
	}
	return -1;
}


/****************************************************************************
*					Integral::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров leftLimit и rightLimit
*	Parameters:		newLeftLimit - новое значение для параметра leftLimit
*					newRightLimit - новое значение для параметра rightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int Integral::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in Integral::setLimits : parameters have a wrong values.");
	leftLimit = newLeftLimit;
	rightLimit = newRightLimit;
	return 0; 
}


/****************************************************************************
*					Integral::setLimits
*
*	Description:	Функция устанавливает значения параметров minLevelLimit, maxLevelLimit и levelDelta
*	Parameters:		row - участок траектории, по которому определяютя значения параметров
*					reserve - величина относительного запаса на которую следует ослаблять значения граничных параметров
*					numOfLevels - число шагов по увеличению параметров элементарного условия 
*						с минимального до максимального, по которому определяется levelDelta
*	Returns:		0 - если новые параметры установлены
*					-1 - если не удалось установить новые параметры
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int Integral::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	std::vector <double> curRes;
	int len;
	// Проверка достаточности длины ряда
	len = row.size() - rightLimit - leftLimit;
	if (len < 1)
		return -1;
	// Подсчет крайнего значения интеграла
	curRes.resize (len);
	for (int k = leftLimit; k < (int) row.size() - rightLimit; k++) {
		curRes[k-leftLimit] = 0.5 * (row[k-leftLimit] + row[k+rightLimit]);
		for (int i = k - leftLimit + 1; i < k + rightLimit; i++) {
			curRes[k-leftLimit] += row[i];
		}
	}
	// Определение минимальных и максимальных значений
	minLevelLimit = min (curRes[0], row [leftLimit]);
	maxLevelLimit = max (curRes[0], row [leftLimit]);
	for (int j = 1; j < len; j++) {
		minLevelLimit = min3 (curRes[j], row [j + leftLimit], minLevelLimit);
		maxLevelLimit = max3 (curRes[j], row [j + leftLimit], maxLevelLimit);
	}
	if (reserve < 0.001)
		return -1;
	if (minLevelLimit < 0)
		minLevelLimit *= reserve;
	else 
		minLevelLimit /= reserve;
	if (maxLevelLimit > 0)
		maxLevelLimit *= reserve;
	else 
		maxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in Integral::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (maxLevelLimit - minLevelLimit) / (double) numOfLevels;
	if(levelDelta < 1e-8) {
		return -1;
	}
	return 0;
}


/****************************************************************************
*					Integral::changeParams
*
*	Description:	Функция изменяет параметры элементарного условия на число шагов, указанное в параметрах
*	Parameters:		numOfSteps - число шагов, на которое изменять параметры элементарного условия
*	Returns:		0 - если изменение произошло удачно
*					-1 - елси изменение на указанное число шагов невозможно
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int Integral::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		minLevel = minLevelLimit;
		maxLevel = minLevelLimit; // maxLevelLimit;
		left = 0; //leftLimit;
		right = 0; //rightLimit;
		return 0;
	}
	if (numOfSteps > 0) { // движенеи на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((maxLevel+levelDelta - maxLevelLimit) > eps) {
				if ((minLevel+levelDelta - maxLevelLimit) > eps) {
					if (left >= leftLimit) {
						if (right >= rightLimit)
							return -1;
						else {
							right++;
							left = 0;
							minLevel = minLevelLimit;
							maxLevel = minLevelLimit;
						}
					} else {
						left++;
						minLevel = minLevelLimit;
						maxLevel = minLevelLimit;
					}
				} else {
					minLevel += levelDelta;
					maxLevel = minLevel;
				}
			} else
				maxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((maxLevel-levelDelta - minLevel) < -eps) {
				if ((minLevel-levelDelta - minLevelLimit) < -eps) {
					if (left == 0) {
						if (right == 0)
							return -1;
						else {
							right--;
							left = leftLimit;
							minLevel = maxLevelLimit;
							maxLevel = maxLevelLimit;
						}
					} else {
						left--;
						minLevel = maxLevelLimit;
						maxLevel = maxLevelLimit;
					}
				} else {
					minLevel -= levelDelta;
					maxLevel = maxLevelLimit;
				}
			} else
				maxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
