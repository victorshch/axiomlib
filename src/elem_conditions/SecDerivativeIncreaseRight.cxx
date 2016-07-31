/****************************************************************************
*
*			Функции класса SecDerivativeIncreaseRight
*
****************************************************************************/

#include <string>
#include "SecDerivativeIncreaseRight.h"
#include "SecDerivativeDecreaseRight.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			SecDerivativeIncreaseRight::SecDerivativeIncreaseRight
*
*	Description:	Дефолтный конструктор, заполняет переменные
*				минимальным и максимальным значениями
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
SecDerivativeIncreaseRight::SecDerivativeIncreaseRight (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	secDerivIncreaseMinLevel = double_default_Min_Level_Limit;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	secDerivIncreaseMaxLevel = 10;
	
	// Число точек требуемых для элементарного условия - слева
	secDerivIncreaseLeft = 0;
	
	// Число точек требуемых для элементарного условия - справа
	secDerivIncreaseRight = 3;

	// Минимальная граница изменения для параметра secDerivIncreaseMinLevel, которая учитывается при подборе параметров перебором
	secDerivIncreaseMinLevelLimit = secDerivIncreaseMinLevel;
	
	// Максиомальная граница изменения для параметра secDerivIncreaseMaxLevel, которая учитывается при подборе параметров перебором
	secDerivIncreaseMaxLevelLimit = secDerivIncreaseMaxLevel;

	// Шаг изменения параметров secDerivIncreaseMinLevel и secDerivIncreaseMaxLevel, при подборе параметров перебором
	levelDelta = secDerivIncreaseMaxLevel - secDerivIncreaseMinLevel;
	
	// Максимальное значение параметра secDerivIncreaseLeft, которое учитывается при подборе параметров перебором
	secDerivIncreaseLeftLimit = secDerivIncreaseLeft;
	
	// Максимальное значение параметра secDerivIncreaseRight, которое учитывается при подборе параметров перебором
	secDerivIncreaseRightLimit = secDerivIncreaseRight;
}

/****************************************************************************
*			SecDerivativeIncreaseRight::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string SecDerivativeIncreaseRight::name (void) const {
	std::string toReturn("second_derivative_increase_right");
	return toReturn;
}


/****************************************************************************
*			SecDerivativeIncreaseRight::check
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
signed int SecDerivativeIncreaseRight::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (((int) k < secDerivIncreaseLeft) || ((int) (x.size() - k) <= secDerivIncreaseRight) || (secDerivIncreaseRight < 2)) {
		toReturn = -1;
		//throw AxiomLibException("Error in SecDerivativeIncreaseRight::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	double prevRes = x[k+1] - x[k];
	double currRes;
	for (int i = 1; i < secDerivIncreaseRight; i++) {
		currRes = x[k+i+1] - x[k+i];
		if (((currRes - prevRes) < secDerivIncreaseMinLevel) || ((currRes - prevRes) > secDerivIncreaseMaxLevel)) {
			return 0;
		}
		prevRes = currRes;
	}
	
	return toReturn;
}

/****************************************************************************
*			SecDerivativeIncreaseRight::operator<
*
*	Description:	Сравнение двух SecDerivativeIncreaseRight на включение (x < y).
*	Parameters:	SecDerivativeIncreaseRight& second - правая часть выражения
*	Returns:		1 если < и second - это SecDerivativeIncreaseRight
*				-1 если > и second - это SecDerivativeIncreaseRight
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не SecDerivativeIncreaseRight)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double SecDerivativeIncreaseRight::operator< (const ElemCondition& second) const{
	// пытаемся получить y типа SecDerivativeIncreaseRight* из second типа el_axiom
	const SecDerivativeIncreaseRight* y = dynamic_cast<const SecDerivativeIncreaseRight*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип SecDerivativeIncreaseRight&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((secDerivIncreaseMaxLevel < y->secDerivIncreaseMaxLevel + 0.01) && (secDerivIncreaseMinLevel > y->secDerivIncreaseMinLevel - 0.01))
			return 1;
		else if ((y->secDerivIncreaseMaxLevel < secDerivIncreaseMaxLevel + 0.01) && (y->secDerivIncreaseMinLevel > secDerivIncreaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			SecDerivativeIncreaseRight::transmute
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
signed int SecDerivativeIncreaseRight::transmute (const double p) {
	// Проверка границ p
	if (p < 0 || p > 1) {
		return -1;
	}
	
	// Получаем границы изменений
	double change = (((double)rand())/ ((double) RAND_MAX)) * p ;
	
	if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
		// увеличиваем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// увеличиваем верхнюю границу
			secDerivIncreaseMaxLevel = secDerivIncreaseMaxLevel * (1 + change);
		}
		else {
			// уменьшаем нижнюю границу
			secDerivIncreaseMinLevel = secDerivIncreaseMinLevel * (1 - change);
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			secDerivIncreaseMaxLevel = secDerivIncreaseMaxLevel - (secDerivIncreaseMaxLevel  - secDerivIncreaseMinLevel) * change;
		}
		else {
			// увеличиваем нижнюю границу
			secDerivIncreaseMinLevel = secDerivIncreaseMinLevel + (secDerivIncreaseMaxLevel  - secDerivIncreaseMinLevel) * change;
		}
	}
	
	return 1;
}


/****************************************************************************
*			SecDerivativeIncreaseRight::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int SecDerivativeIncreaseRight::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in SecDerivativeIncreaseRight::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("secDerivIncreaseMinLevel")) {
			// ok, нашли, находим value
			secDerivIncreaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("secDerivIncreaseMaxLevel")) {
			// ok, нашли, находим value
			secDerivIncreaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("secDerivIncreaseLeft")) {
			// ok, нашли, находим value
			secDerivIncreaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("secDerivIncreaseRight")) {
			// ok, нашли, находим value
			secDerivIncreaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in SecDerivativeIncreaseRight::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			SecDerivativeIncreaseRight::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int SecDerivativeIncreaseRight::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", secDerivIncreaseMinLevel);
	std::string str1 (temp);
	params["secDerivIncreaseMinLevel"] = str1;
	sprintf( temp, "%f", secDerivIncreaseMaxLevel);
	std::string str2 (temp);
	params["secDerivIncreaseMaxLevel"] = str2;
	sprintf( temp, "%d", secDerivIncreaseLeft);
	std::string str3 (temp);
	params["secDerivIncreaseLeft"] = str3;
	sprintf( temp, "%d", secDerivIncreaseRight);
	std::string str4 (temp);
	params["secDerivIncreaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			SecDerivativeIncreaseRight::setParamValue
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
signed int SecDerivativeIncreaseRight::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "secDerivIncreaseMinLevel") {
		secDerivIncreaseMinLevel = param;
		return 0;
	}
	if (paramName == "secDerivIncreaseMaxLevel") {
		secDerivIncreaseMaxLevel = param;
		return 0;
	}
	if (paramName == "secDerivIncreaseLeft") {
		secDerivIncreaseLeft = round (param);
		return 0;
	}
	if (paramName == "secDerivIncreaseRight") {
		secDerivIncreaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			SecDerivativeIncreaseRight::getParamValue
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
signed int SecDerivativeIncreaseRight::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "secDerivIncreaseMinLevel") {
		param = secDerivIncreaseMinLevel;
		return 0;
	}
	if (paramName == "secDerivIncreaseLevel") {
		param = secDerivIncreaseMaxLevel;
		return 0;
	}
	if (paramName == "secDerivIncreaseLeft") {
		param = (double) secDerivIncreaseLeft;
		return 0;
	}
	if (paramName == "secDerivIncreaseRight") {
		param = (double) secDerivIncreaseRight;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			SecDerivativeIncreaseRight::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int SecDerivativeIncreaseRight::getECParamNames (std::vector<std::string> &ecParamNames) const{
	ecParamNames.resize(4);
	ecParamNames[0] = "secDerivIncreaseMinLevel";
	ecParamNames[1] = "secDerivIncreaseMaxLevel";
	ecParamNames[2] = "secDerivIncreaseLeft";
	ecParamNames[3] = "secDerivIncreaseRight";
	return 0;
}


/****************************************************************************
*			SecDerivativeIncreaseRight::checkAndCompatibility
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
signed int SecDerivativeIncreaseRight::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа SecDerivativeIncreaseRight* из second типа el_axiom
	const SecDerivativeIncreaseRight* y = dynamic_cast<const SecDerivativeIncreaseRight*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип SecDerivativeIncreaseRight&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((secDerivIncreaseMaxLevel >= y->secDerivIncreaseMaxLevel) && (secDerivIncreaseMinLevel <= y->secDerivIncreaseMaxLevel)) ||
		     ((secDerivIncreaseMaxLevel >= y->secDerivIncreaseMinLevel) && (secDerivIncreaseMinLevel <= y->secDerivIncreaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const SecDerivativeDecreaseRight* x = dynamic_cast<const SecDerivativeDecreaseRight*> (&second);
		if (x != NULL) 
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					SecDerivativeIncreaseRight::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров secDerivIncreaseLeftLimit и secDerivIncreaseRightLimit
*	Parameters:		newLeftLimit - новое значение для параметра secDerivIncreaseLeftLimit
*					newRightLimit - новое значение для параметра secDerivIncreaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int SecDerivativeIncreaseRight::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in SecDerivativeIncreaseRight::setLimits : parameters have a wrong values.");
	secDerivIncreaseLeftLimit = newLeftLimit;
	secDerivIncreaseRightLimit = newRightLimit;
	// Левый предел для данного условия по определению устанавливаем равным 0.
	secDerivIncreaseLeftLimit = 0;
	return 0; 
}


/****************************************************************************
*					SecDerivativeIncreaseRight::setLimits
*
*	Description:	Функция устанавливает значения параметров secDerivIncreaseMinLevelLimit, secDerivIncreaseMaxLevelLimit и levelDelta
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
signed int SecDerivativeIncreaseRight::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	// Подсчет значений элементарного условия
	if (row.size() < 3) 
		return -1;
	secDerivIncreaseMinLevelLimit = max (row[0] - 2*row[1] + row[2], double_default_Min_Level_Limit);
	secDerivIncreaseMaxLevelLimit = secDerivIncreaseMinLevelLimit;
	for (int k = 2; k < (int) (row.size() - 1); k++) {
		secDerivIncreaseMinLevelLimit = min (row[k-1] - 2*row[k] + row[k+1], secDerivIncreaseMinLevelLimit);
		secDerivIncreaseMinLevelLimit = max (double_default_Min_Level_Limit, secDerivIncreaseMinLevelLimit);
		secDerivIncreaseMaxLevelLimit = max (row[k-1] - 2*row[k] + row[k+1], secDerivIncreaseMaxLevelLimit);
	}
	// Определение минимальных и максимальных значений
	if (reserve < 0.001)
		return -1;
	if (secDerivIncreaseMinLevelLimit < 0)
		secDerivIncreaseMinLevelLimit *= reserve;
	else 
		secDerivIncreaseMinLevelLimit /= reserve;
	if (secDerivIncreaseMaxLevelLimit > 0)
		secDerivIncreaseMaxLevelLimit *= reserve;
	else 
		secDerivIncreaseMaxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in SecDerivativeIncreaseRight::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (secDerivIncreaseMaxLevelLimit - secDerivIncreaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					SecDerivativeIncreaseRight::changeParams
*
*	Description:	Функция изменяет параметры элементарного условия на число шагов, указанное в параметрах
*	Parameters:		numOfSteps - число шагов, на которое изменять параметры элементарного условия
*	Returns:		0 - если изменение произошло удачно
*					-1 - елси изменение на указанное число шаго невозможно
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int SecDerivativeIncreaseRight::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		secDerivIncreaseMinLevel = secDerivIncreaseMinLevelLimit;
		secDerivIncreaseMaxLevel = secDerivIncreaseMinLevelLimit;
		secDerivIncreaseLeft = 0;
		secDerivIncreaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движение на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((secDerivIncreaseMaxLevel+levelDelta - secDerivIncreaseMaxLevelLimit) > eps) {
				if ((secDerivIncreaseMinLevel+levelDelta - secDerivIncreaseMaxLevelLimit) > eps) {
					if (secDerivIncreaseLeft >= secDerivIncreaseLeftLimit) {
						if (secDerivIncreaseRight >= secDerivIncreaseRightLimit)
							return -1;
						else {
							secDerivIncreaseRight++;
							secDerivIncreaseLeft = 0;
							secDerivIncreaseMinLevel = secDerivIncreaseMinLevelLimit;
							secDerivIncreaseMaxLevel = secDerivIncreaseMinLevelLimit;
						}
					} else {
						secDerivIncreaseLeft++;
						secDerivIncreaseMinLevel = secDerivIncreaseMinLevelLimit;
						secDerivIncreaseMaxLevel = secDerivIncreaseMinLevelLimit;
					}
				} else {
					secDerivIncreaseMinLevel += levelDelta;
					secDerivIncreaseMaxLevel = secDerivIncreaseMinLevel;
				}
			} else
				secDerivIncreaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((secDerivIncreaseMaxLevel-levelDelta - secDerivIncreaseMinLevel) < -eps) {
				if ((secDerivIncreaseMinLevel-levelDelta - secDerivIncreaseMinLevelLimit) < -eps) {
					if (secDerivIncreaseLeft == 0) {
						if (secDerivIncreaseRight == 0)
							return -1;
						else {
							secDerivIncreaseRight--;
							secDerivIncreaseLeft = secDerivIncreaseLeftLimit;
							secDerivIncreaseMinLevel = secDerivIncreaseMaxLevelLimit;
							secDerivIncreaseMaxLevel = secDerivIncreaseMaxLevelLimit;
						}
					} else {
						secDerivIncreaseLeft--;
						secDerivIncreaseMinLevel = secDerivIncreaseMaxLevelLimit;
						secDerivIncreaseMaxLevel = secDerivIncreaseMaxLevelLimit;
					}
				} else {
					secDerivIncreaseMinLevel -= levelDelta;
					secDerivIncreaseMaxLevel = secDerivIncreaseMaxLevelLimit;
				}
			} else
				secDerivIncreaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
