/****************************************************************************
*
*			Функции класса DerivativeIncreaseRight
*
****************************************************************************/

#include <string>
#include "DerivativeIncreaseRight.h"
#include "DerivativeDecreaseRight.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			DerivativeIncreaseRight::DerivativeIncreaseRight
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
DerivativeIncreaseRight::DerivativeIncreaseRight (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	derivIncreaseMinLevel = double_default_Min_Level_Limit;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	derivIncreaseMaxLevel = 10;
	
	// Число точек требуемых для элементарного условия - слева
	derivIncreaseLeft = 0;
	
	// Число точек требуемых для элементарного условия - справа
	derivIncreaseRight = 2;

	// Минимальная граница изменения для параметра derivIncreaseMinLevel, которая учитывается при подборе параметров перебором
	derivIncreaseMinLevelLimit = derivIncreaseMinLevel;
	
	// Максиомальная граница изменения для параметра derivIncreaseMaxLevel, которая учитывается при подборе параметров перебором
	derivIncreaseMaxLevelLimit = derivIncreaseMaxLevel;

	// Шаг изменения параметров derivIncreaseMinLevel и derivIncreaseMaxLevel, при подборе параметров перебором
	levelDelta = derivIncreaseMaxLevel - derivIncreaseMinLevel;
	
	// Максимальное значение параметра derivIncreaseLeft, которое учитывается при подборе параметров перебором
	derivIncreaseLeftLimit = derivIncreaseLeft;
	
	// Максимальное значение параметра derivIncreaseRight, которое учитывается при подборе параметров перебором
	derivIncreaseRightLimit = derivIncreaseRight;
}

/****************************************************************************
*			DerivativeIncreaseRight::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string DerivativeIncreaseRight::name (void) const {
	std::string toReturn("derivative_increase_right");
	return toReturn;
}


/****************************************************************************
*			DerivativeIncreaseRight::check
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
signed int DerivativeIncreaseRight::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (((int) k < derivIncreaseLeft) || ((int)(x.size() - k) <= derivIncreaseRight)) {
		toReturn = -1;
		//throw AxiomLibException("Error in DerivativeIncreaseRight::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	for (int i = 0; i < derivIncreaseRight; i++) {
		if (((x[k+i+1] - x[k+i]) < derivIncreaseMinLevel) || ((x[k+i+1] - x[k+i]) > derivIncreaseMaxLevel)) {
			return 0;
		}
	}
	
	return toReturn;
}

/****************************************************************************
*			DerivativeIncreaseRight::operator<
*
*	Description:	Сравнение двух DerivativeIncreaseRight на включение (x < y).
*	Parameters:	DerivativeIncreaseRight& second - правая часть выражения
*	Returns:		1 если < и second - это DerivativeIncreaseRight
*				-1 если > и second - это DerivativeIncreaseRight
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не DerivativeIncreaseRight)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double DerivativeIncreaseRight::operator< (const ElemCondition& second) const{
	// пытаемся получить y типа DerivativeIncreaseRight* из second типа el_axiom
	const DerivativeIncreaseRight* y = dynamic_cast<const DerivativeIncreaseRight*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип DerivativeIncreaseRight&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((derivIncreaseMaxLevel < y->derivIncreaseMaxLevel + 0.01) && (derivIncreaseMinLevel > y->derivIncreaseMinLevel - 0.01))
			return 1;
		else if ((y->derivIncreaseMaxLevel < derivIncreaseMaxLevel + 0.01) && (y->derivIncreaseMinLevel > derivIncreaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			DerivativeIncreaseRight::transmute
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
signed int DerivativeIncreaseRight::transmute (const double p) {
	signed int toReturn = 0;

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
			derivIncreaseMaxLevel = derivIncreaseMaxLevel * (1 + change);
			toReturn = 1;
		}
		else {
			// уменьшаем нижнюю границу
			derivIncreaseMinLevel = derivIncreaseMinLevel * (1 - change);
			toReturn = 1;
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			derivIncreaseMaxLevel = derivIncreaseMaxLevel - (derivIncreaseMaxLevel  - derivIncreaseMinLevel) * change;
			toReturn = 1;
		}
		else {
			// увеличиваем нижнюю границу
			derivIncreaseMinLevel = derivIncreaseMinLevel + (derivIncreaseMaxLevel  - derivIncreaseMinLevel) * change;
			toReturn = 1;
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DerivativeIncreaseRight::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseRight::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in DerivativeIncreaseRight::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("derivIncreaseMinLevel")) {
			// ok, нашли, находим value
			derivIncreaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("derivIncreaseMaxLevel")) {
			// ok, нашли, находим value
			derivIncreaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("derivIncreaseLeft")) {
			// ok, нашли, находим value
			derivIncreaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("derivIncreaseRight")) {
			// ok, нашли, находим value
			derivIncreaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in DerivativeIncreaseRight::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DerivativeIncreaseRight::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseRight::getParams (std::map<std::string,std::string> &params) const {
	params.clear();
	char temp[64];
	sprintf( temp, "%f", derivIncreaseMinLevel);
	std::string str1 (temp);
	params["derivIncreaseMinLevel"] = str1;
	sprintf( temp, "%f", derivIncreaseMaxLevel);
	std::string str2 (temp);
	params["derivIncreaseMaxLevel"] = str2;
	sprintf( temp, "%d", derivIncreaseLeft);
	std::string str3 (temp);
	params["derivIncreaseLeft"] = str3;
	sprintf( temp, "%d", derivIncreaseRight);
	std::string str4 (temp);
	params["derivIncreaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			DerivativeIncreaseRight::setParamValue
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
signed int DerivativeIncreaseRight::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "derivIncreaseMinLevel") {
		derivIncreaseMinLevel = param;
		return 0;
	}
	if (paramName == "derivIncreaseMaxLevel") {
		derivIncreaseMaxLevel = param;
		return 0;
	}
	if (paramName == "derivIncreaseLeft") {
		derivIncreaseLeft = round (param);
		return 0;
	}
	if (paramName == "derivIncreaseRight") {
		derivIncreaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			DerivativeIncreaseRight::getParamValue
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
signed int DerivativeIncreaseRight::getParamValue (double &param, const std::string &paramName) const{
	if (paramName == "derivIncreaseMinLevel") {
		param = derivIncreaseMinLevel;
		return 0;
	}
	if (paramName == "derivIncreaseMaxLevel") {
		param = derivIncreaseMaxLevel;
		return 0;
	}
	if (paramName == "derivIncreaseLeft") {
		param = (double) derivIncreaseLeft;
		return 0;
	}
	if (paramName == "derivIncreaseRight") {
		param = (double) derivIncreaseRight;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			DerivativeIncreaseRight::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseRight::getECParamNames (std::vector<std::string> &ecParamNames) const{
	ecParamNames.resize(4);
	ecParamNames[0] = "derivIncreaseMinLevel";
	ecParamNames[1] = "derivIncreaseMaxLevel";
	ecParamNames[2] = "derivIncreaseLeft";
	ecParamNames[3] = "derivIncreaseRight";
	return 0;
}


/****************************************************************************
*			DerivativeIncreaseRight::checkAndCompatibility
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
signed int DerivativeIncreaseRight::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа DerivativeIncreaseRight* из second типа el_axiom
	const DerivativeIncreaseRight* y = dynamic_cast<const DerivativeIncreaseRight*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип DerivativeIncreaseRight&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((derivIncreaseMaxLevel <= y->derivIncreaseMaxLevel) && (derivIncreaseMinLevel <= y->derivIncreaseMaxLevel)) ||
		     ((derivIncreaseMaxLevel <= y->derivIncreaseMinLevel) && (derivIncreaseMinLevel >= y->derivIncreaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const DerivativeDecreaseRight* x = dynamic_cast<const DerivativeDecreaseRight*> (&second);
		if (x != NULL)
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					DerivativeIncreaseRight::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров derivIncreaseLeftLimit и derivIncreaseRightLimit
*	Parameters:		newLeftLimit - новое значение для параметра derivIncreaseLeftLimit
*					newRightLimit - новое значение для параметра derivIncreaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseRight::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in DerivativeIncreaseRight::setLimits : parameters have a wrong values.");
	derivIncreaseLeftLimit = newLeftLimit;
	derivIncreaseRightLimit = newRightLimit;
	// Левый предел по определению условия должен быть равен 0
	derivIncreaseLeftLimit = 0;
	return 0; 
}


/****************************************************************************
*					DerivativeIncreaseRight::setLimits
*
*	Description:	Функция устанавливает значения параметров derivIncreaseMinLevelLimit, derivIncreaseMaxLevelLimit и levelDelta
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
signed int DerivativeIncreaseRight::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	// Подсчет значений элементарного условия
	if (row.size() < 2) 
		return -1;
	derivIncreaseMinLevelLimit = max (row[1] - row[0], double_default_Min_Level_Limit);
	derivIncreaseMaxLevelLimit = derivIncreaseMinLevelLimit;
	for (int k = 2; k < (int) row.size(); k++) {
		derivIncreaseMinLevelLimit = min (row[k] - row[k-1], derivIncreaseMinLevelLimit);
		derivIncreaseMinLevelLimit = max (double_default_Min_Level_Limit, derivIncreaseMinLevelLimit);
		derivIncreaseMaxLevelLimit = max (row[k] - row[k-1], derivIncreaseMaxLevelLimit);
	}
	// Определение минимальных и максимальных значений
	if (reserve < 0.001)
		return -1;
	if (derivIncreaseMinLevelLimit < 0)
		derivIncreaseMinLevelLimit *= reserve;
	else 
		derivIncreaseMinLevelLimit /= reserve;
	if (derivIncreaseMaxLevelLimit > 0)
		derivIncreaseMaxLevelLimit *= reserve;
	else 
		derivIncreaseMaxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in DerivativeIncreaseRight::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (derivIncreaseMaxLevelLimit - derivIncreaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					DerivativeIncreaseRight::changeParams
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
signed int DerivativeIncreaseRight::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		derivIncreaseMinLevel = derivIncreaseMinLevelLimit;
		derivIncreaseMaxLevel = derivIncreaseMinLevelLimit;
		derivIncreaseLeft = 0;
		derivIncreaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движение на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((derivIncreaseMaxLevel+levelDelta - derivIncreaseMaxLevelLimit) > eps) {
				if ((derivIncreaseMinLevel+levelDelta - derivIncreaseMaxLevelLimit) > eps) {
					if (derivIncreaseLeft >= derivIncreaseLeftLimit) {
						if (derivIncreaseRight >= derivIncreaseRightLimit)
							return -1;
						else {
							derivIncreaseRight++;
							derivIncreaseLeft = 0;
							derivIncreaseMinLevel = derivIncreaseMinLevelLimit;
							derivIncreaseMaxLevel = derivIncreaseMinLevelLimit;
						}
					} else {
						derivIncreaseLeft++;
						derivIncreaseMinLevel = derivIncreaseMinLevelLimit;
						derivIncreaseMaxLevel = derivIncreaseMinLevelLimit;
					}
				} else {
					derivIncreaseMinLevel += levelDelta;
					derivIncreaseMaxLevel = derivIncreaseMinLevel;
				}
			} else
				derivIncreaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((derivIncreaseMaxLevel-levelDelta - derivIncreaseMinLevel) < -eps) {
				if ((derivIncreaseMinLevel-levelDelta - derivIncreaseMinLevelLimit) < -eps) {
					if (derivIncreaseLeft == 0) {
						if (derivIncreaseRight == 0)
							return -1;
						else {
							derivIncreaseRight--;
							derivIncreaseLeft = derivIncreaseLeftLimit;
							derivIncreaseMinLevel = derivIncreaseMaxLevelLimit;
							derivIncreaseMaxLevel = derivIncreaseMaxLevelLimit;
						}
					} else {
						derivIncreaseLeft--;
						derivIncreaseMinLevel = derivIncreaseMaxLevelLimit;
						derivIncreaseMaxLevel = derivIncreaseMaxLevelLimit;
					}
				} else {
					derivIncreaseMinLevel -= levelDelta;
					derivIncreaseMaxLevel = derivIncreaseMaxLevelLimit;
				}
			} else
				derivIncreaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
