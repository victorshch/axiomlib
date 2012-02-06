/****************************************************************************
*
*			Функции класса SecDerivativeDecreaseRight
*
****************************************************************************/

#include <string>
#include "SecDerivativeDecreaseRight.h"
#include "SecDerivativeIncreaseRight.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			SecDerivativeDecreaseRight::SecDerivativeDecreaseRight
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
SecDerivativeDecreaseRight::SecDerivativeDecreaseRight (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	secDerivDecreaseMinLevel = double_default_Min_Level_Limit;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	secDerivDecreaseMaxLevel = 10;
	
	// Число точек требуемых для элементарного условия - слева
	secDerivDecreaseLeft = 0;
	
	// Число точек требуемых для элементарного условия - справа
	secDerivDecreaseRight = 3;

	// Минимальная граница изменения для параметра secDerivDecreaseMinLevel, которая учитывается при подборе параметров перебором
	secDerivDecreaseMinLevelLimit = secDerivDecreaseMinLevel;
	
	// Максиомальная граница изменения для параметра secDerivDecreaseMaxLevel, которая учитывается при подборе параметров перебором
	secDerivDecreaseMaxLevelLimit = secDerivDecreaseMaxLevel;

	// Шаг изменения параметров secDerivDecreaseMinLevel и secDerivDecreaseMaxLevel, при подборе параметров перебором
	levelDelta = secDerivDecreaseMaxLevel - secDerivDecreaseMinLevel;
	
	// Максимальное значение параметра secDerivDecreaseLeft, которое учитывается при подборе параметров перебором
	secDerivDecreaseLeftLimit = secDerivDecreaseLeft;
	
	// Максимальное значение параметра secDerivDecreaseRight, которое учитывается при подборе параметров перебором
	secDerivDecreaseRightLimit = secDerivDecreaseRight;
}

/****************************************************************************
*			SecDerivativeDecreaseRight::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string SecDerivativeDecreaseRight::name (void) const {
	std::string toReturn("second_derivative_decrease_right");
	return toReturn;
}


/****************************************************************************
*			SecDerivativeDecreaseRight::check
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
signed int SecDerivativeDecreaseRight::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (((int) k < secDerivDecreaseLeft) || ((int) (x.size() - k) <= secDerivDecreaseRight) || (secDerivDecreaseRight < 2)) {
		toReturn = -1;
		//throw AxiomLibException("Error in SecDerivativeDecreaseRight::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	double prevRes = x[k+1] - x[k];
	double currRes;
	for (int i = 1; i < secDerivDecreaseRight; i++) {
		currRes = x[k+i+1] - x[k+i];
		if (((prevRes - currRes) < secDerivDecreaseMinLevel) || ((prevRes - currRes) > secDerivDecreaseMaxLevel)) {
			return 0;
		}
		prevRes = currRes;
	}
	
	return toReturn;
}

/****************************************************************************
*			SecDerivativeDecreaseRight::operator<
*
*	Description:	Сравнение двух SecDerivativeDecreaseRight на включение (x < y).
*	Parameters:	SecDerivativeDecreaseRight& second - правая часть выражения
*	Returns:		1 если < и second - это SecDerivativeDecreaseRight
*				-1 если > и second - это SecDerivativeDecreaseRight
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не SecDerivativeDecreaseRight)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double SecDerivativeDecreaseRight::operator< (const ElemCondition& second) const{
	// пытаемся получить y типа SecDerivativeDecreaseRight* из second типа el_axiom
	const SecDerivativeDecreaseRight* y = dynamic_cast<const SecDerivativeDecreaseRight*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип SecDerivativeDecreaseRight&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((secDerivDecreaseMaxLevel < y->secDerivDecreaseMaxLevel + 0.01) && (secDerivDecreaseMinLevel > y->secDerivDecreaseMinLevel - 0.01))
			return 1;
		else if ((y->secDerivDecreaseMaxLevel < secDerivDecreaseMaxLevel + 0.01) && (y->secDerivDecreaseMinLevel > secDerivDecreaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			SecDerivativeDecreaseRight::transmute
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
signed int SecDerivativeDecreaseRight::transmute (const double p) {
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
			secDerivDecreaseMaxLevel = secDerivDecreaseMaxLevel * (1 + change);
		}
		else {
			// уменьшаем нижнюю границу
			secDerivDecreaseMinLevel = secDerivDecreaseMinLevel * (1 - change);
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			secDerivDecreaseMaxLevel = secDerivDecreaseMaxLevel - (secDerivDecreaseMaxLevel  - secDerivDecreaseMinLevel) * change;
		}
		else {
			// увеличиваем нижнюю границу
			secDerivDecreaseMinLevel = secDerivDecreaseMinLevel + (secDerivDecreaseMaxLevel  - secDerivDecreaseMinLevel) * change;
		}
	}
	
	return 1;
}


/****************************************************************************
*			SecDerivativeDecreaseRight::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int SecDerivativeDecreaseRight::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in SecDerivativeDecreaseRight::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("secDerivDecreaseMinLevel")) {
			// ok, нашли, находим value
			secDerivDecreaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("secDerivDecreaseMaxLevel")) {
			// ok, нашли, находим value
			secDerivDecreaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("secDerivDecreaseLeft")) {
			// ok, нашли, находим value
			secDerivDecreaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("secDerivDecreaseRight")) {
			// ok, нашли, находим value
			secDerivDecreaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in SecDerivativeDecreaseRight::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			SecDerivativeDecreaseRight::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int SecDerivativeDecreaseRight::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", secDerivDecreaseMinLevel);
	std::string str1 (temp);
	params["secDerivDecreaseMinLevel"] = str1;
	sprintf( temp, "%f", secDerivDecreaseMaxLevel);
	std::string str2 (temp);
	params["secDerivDecreaseMaxLevel"] = str2;
	sprintf( temp, "%d", secDerivDecreaseLeft);
	std::string str3 (temp);
	params["secDerivDecreaseLeft"] = str3;
	sprintf( temp, "%d", secDerivDecreaseRight);
	std::string str4 (temp);
	params["secDerivDecreaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			SecDerivativeDecreaseRight::setParamValue
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
signed int SecDerivativeDecreaseRight::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "secDerivDecreaseMinLevel") {
		secDerivDecreaseMinLevel = param;
		return 0;
	}
	if (paramName == "secDerivDecreaseMaxLevel") {
		secDerivDecreaseMaxLevel = param;
		return 0;
	}
	if (paramName == "secDerivDecreaseLeft") {
		secDerivDecreaseLeft = round (param);
		return 0;
	}
	if (paramName == "secDerivDecreaseRight") {
		secDerivDecreaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			SecDerivativeDecreaseRight::getParamValue
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
signed int SecDerivativeDecreaseRight::getParamValue (double &param, const std::string &paramName) const{
	if (paramName == "secDerivDecreaseMinLevel") {
		param = secDerivDecreaseMinLevel;
		return 0;
	}
	if (paramName == "secDerivDecreaseLevel") {
		param = secDerivDecreaseMaxLevel;
		return 0;
	}
	if (paramName == "secDerivDecreaseLeft") {
		param = (double) secDerivDecreaseLeft;
		return 0;
	}
	if (paramName == "secDerivDecreaseRight") {
		param = (double) secDerivDecreaseRight;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			SecDerivativeDecreaseRight::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int SecDerivativeDecreaseRight::getECParamNames (std::vector<std::string> &ecParamNames) const{
	ecParamNames.resize(4);
	ecParamNames[0] = "secDerivDecreaseMinLevel";
	ecParamNames[1] = "secDerivDecreaseMaxLevel";
	ecParamNames[2] = "secDerivDecreaseLeft";
	ecParamNames[3] = "secDerivDecreaseRight";
	return 0;
}


/****************************************************************************
*			SecDerivativeDecreaseRight::checkAndCompatibility
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
signed int SecDerivativeDecreaseRight::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа SecDerivativeDecreaseRight* из second типа el_axiom
	const SecDerivativeDecreaseRight* y = dynamic_cast<const SecDerivativeDecreaseRight*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип SecDerivativeDecreaseRight&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((secDerivDecreaseMaxLevel >= y->secDerivDecreaseMaxLevel) && (secDerivDecreaseMinLevel <= y->secDerivDecreaseMaxLevel)) ||
		     ((secDerivDecreaseMaxLevel >= y->secDerivDecreaseMinLevel) && (secDerivDecreaseMinLevel <= y->secDerivDecreaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const SecDerivativeIncreaseRight* x = dynamic_cast<const SecDerivativeIncreaseRight*> (&second);
		if (x != NULL)
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					SecDerivativeDecreaseRight::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров secDerivDecreaseLeftLimit и secDerivDecreaseRightLimit
*	Parameters:		newLeftLimit - новое значение для параметра secDerivDecreaseLeftLimit
*					newRightLimit - новое значение для параметра secDerivDecreaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int SecDerivativeDecreaseRight::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in SecDerivativeDecreaseRight::setLimits : parameters have a wrong values.");
	secDerivDecreaseLeftLimit = newLeftLimit;
	secDerivDecreaseRightLimit = newRightLimit;
	// Левый предел по определению условия должен быть равен 0
	secDerivDecreaseLeftLimit = 0;
	return 0; 
}


/****************************************************************************
*					SecDerivativeDecreaseRight::setLimits
*
*	Description:	Функция устанавливает значения параметров secDerivDecreaseMinLevelLimit, secDerivDecreaseMaxLevelLimit и levelDelta
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
signed int SecDerivativeDecreaseRight::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	// Подсчет значений элементарного условия
	if (row.size() < 3) 
		return -1;
	secDerivDecreaseMinLevelLimit = max (2*row[1] - row[0] - row[2], double_default_Min_Level_Limit);
	secDerivDecreaseMaxLevelLimit = secDerivDecreaseMinLevelLimit;
	for (int k = 2; k < (int) (row.size() - 1); k++) {
		secDerivDecreaseMinLevelLimit = min (2*row[k] - row[k-1] - row[k+1], secDerivDecreaseMinLevelLimit);
		secDerivDecreaseMinLevelLimit = max (double_default_Min_Level_Limit, secDerivDecreaseMinLevelLimit);
		secDerivDecreaseMaxLevelLimit = max (2*row[k] - row[k-1] - row[k+1], secDerivDecreaseMaxLevelLimit);
	}
	// Определение минимальных и максимальных значений
	if (reserve < 0.001)
		return -1;
	if (secDerivDecreaseMinLevelLimit < 0)
		secDerivDecreaseMinLevelLimit *= reserve;
	else 
		secDerivDecreaseMinLevelLimit /= reserve;
	if (secDerivDecreaseMaxLevelLimit > 0)
		secDerivDecreaseMaxLevelLimit *= reserve;
	else 
		secDerivDecreaseMaxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in SecDerivativeDecreaseRight::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (secDerivDecreaseMaxLevelLimit - secDerivDecreaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					SecDerivativeDecreaseRight::changeParams
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
signed int SecDerivativeDecreaseRight::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		secDerivDecreaseMinLevel = secDerivDecreaseMinLevelLimit;
		secDerivDecreaseMaxLevel = secDerivDecreaseMinLevelLimit;
		secDerivDecreaseLeft = 0;
		secDerivDecreaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движение на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((secDerivDecreaseMaxLevel+levelDelta - secDerivDecreaseMaxLevelLimit) > eps) {
				if ((secDerivDecreaseMinLevel+levelDelta - secDerivDecreaseMaxLevelLimit) > eps) {
					if (secDerivDecreaseLeft >= secDerivDecreaseLeftLimit) {
						if (secDerivDecreaseRight >= secDerivDecreaseRightLimit)
							return -1;
						else {
							secDerivDecreaseRight++;
							secDerivDecreaseLeft = 0;
							secDerivDecreaseMinLevel = secDerivDecreaseMinLevelLimit;
							secDerivDecreaseMaxLevel = secDerivDecreaseMinLevelLimit;
						}
					} else {
						secDerivDecreaseLeft++;
						secDerivDecreaseMinLevel = secDerivDecreaseMinLevelLimit;
						secDerivDecreaseMaxLevel = secDerivDecreaseMinLevelLimit;
					}
				} else {
					secDerivDecreaseMinLevel += levelDelta;
					secDerivDecreaseMaxLevel = secDerivDecreaseMinLevel;
				}
			} else
				secDerivDecreaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((secDerivDecreaseMaxLevel-levelDelta - secDerivDecreaseMinLevel) < -eps) {
				if ((secDerivDecreaseMinLevel-levelDelta - secDerivDecreaseMinLevelLimit) < -eps) {
					if (secDerivDecreaseLeft == 0) {
						if (secDerivDecreaseRight == 0)
							return -1;
						else {
							secDerivDecreaseRight--;
							secDerivDecreaseLeft = secDerivDecreaseLeftLimit;
							secDerivDecreaseMinLevel = secDerivDecreaseMaxLevelLimit;
							secDerivDecreaseMaxLevel = secDerivDecreaseMaxLevelLimit;
						}
					} else {
						secDerivDecreaseLeft--;
						secDerivDecreaseMinLevel = secDerivDecreaseMaxLevelLimit;
						secDerivDecreaseMaxLevel = secDerivDecreaseMaxLevelLimit;
					}
				} else {
					secDerivDecreaseMinLevel -= levelDelta;
					secDerivDecreaseMaxLevel = secDerivDecreaseMaxLevelLimit;
				}
			} else
				secDerivDecreaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
