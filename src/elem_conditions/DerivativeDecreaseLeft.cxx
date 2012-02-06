/****************************************************************************
*
*			Функции класса DerivativeDecreaseLeft
*
****************************************************************************/

#include <string>
#include "DerivativeDecreaseLeft.h"
#include "DerivativeIncreaseLeft.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			DerivativeDecreaseLeft::DerivativeDecreaseLeft
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
DerivativeDecreaseLeft::DerivativeDecreaseLeft (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	derivDecreaseMinLevel = double_default_Min_Level_Limit;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	derivDecreaseMaxLevel = 10;
	
	// Число точек требуемых для элементарного условия - слева
	derivDecreaseLeft = 2;
	
	// Число точек требуемых для элементарного условия - справа
	derivDecreaseRight = 0;

	// Минимальная граница изменения для параметра derivDecreaseMinLevel, которая учитывается при подборе параметров перебором
	derivDecreaseMinLevelLimit = derivDecreaseMinLevel;
	
	// Максиомальная граница изменения для параметра derivDecreaseMaxLevel, которая учитывается при подборе параметров перебором
	derivDecreaseMaxLevelLimit = derivDecreaseMaxLevel;

	// Шаг изменения параметров derivDecreaseMinLevel и derivDecreaseMaxLevel, при подборе параметров перебором
	levelDelta = derivDecreaseMaxLevel - derivDecreaseMinLevel;
	
	// Максимальное значение параметра derivDecreaseLeft, которое учитывается при подборе параметров перебором
	derivDecreaseLeftLimit = derivDecreaseLeft;
	
	// Максимальное значение параметра derivDecreaseRight, которое учитывается при подборе параметров перебором
	derivDecreaseRightLimit = derivDecreaseRight;
}

/****************************************************************************
*			DerivativeDecreaseLeft::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string DerivativeDecreaseLeft::name (void) const {
	std::string toReturn("derivative_decrease_left");
	return toReturn;
}


/****************************************************************************
*			DerivativeDecreaseLeft::check
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
signed int DerivativeDecreaseLeft::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (((int) k < derivDecreaseLeft) || ((int) (x.size() - k) <= derivDecreaseRight)) {
		toReturn = -1;
		//throw AxiomLibException("Error in DerivativeDecreaseLeft::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	for (int i = 0; i < derivDecreaseLeft; i++) {
		if (((x[k-i-1] - x[k-i]) < derivDecreaseMinLevel) || ((x[k-i-1] - x[k-i]) > derivDecreaseMaxLevel)) {
			return 0;
		}
	}
	
	return toReturn;
}

/****************************************************************************
*			DerivativeDecreaseLeft::operator<
*
*	Description:	Сравнение двух DerivativeDecreaseLeft на включение (x < y).
*	Parameters:	DerivativeDecreaseLeft& second - правая часть выражения
*	Returns:		1 если < и second - это DerivativeDecreaseLeft
*				-1 если > и second - это DerivativeDecreaseLeft
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не DerivativeDecreaseLeft)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double DerivativeDecreaseLeft::operator< (const ElemCondition& second) const{
	// пытаемся получить y типа DerivativeDecreaseLeft* из second типа el_axiom
	const DerivativeDecreaseLeft* y = dynamic_cast<const DerivativeDecreaseLeft*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип DerivativeDecreaseLeft&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((derivDecreaseMaxLevel < y->derivDecreaseMaxLevel + 0.01) && (derivDecreaseMinLevel > y->derivDecreaseMinLevel - 0.01))
			return 1;
		else if ((y->derivDecreaseMaxLevel < derivDecreaseMaxLevel + 0.01) && (y->derivDecreaseMinLevel > derivDecreaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			DerivativeDecreaseLeft::transmute
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
signed int DerivativeDecreaseLeft::transmute (const double p) {
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
			derivDecreaseMaxLevel = derivDecreaseMaxLevel * (1 + change);
			toReturn = 1;
		}
		else {
			// уменьшаем нижнюю границу
			derivDecreaseMinLevel = derivDecreaseMinLevel * (1 - change);
			toReturn = 1;
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			derivDecreaseMaxLevel = derivDecreaseMaxLevel - (derivDecreaseMaxLevel  - derivDecreaseMinLevel) * change;
			toReturn = 1;
		}
		else {
			// увеличиваем нижнюю границу
			derivDecreaseMinLevel = derivDecreaseMinLevel + (derivDecreaseMaxLevel  - derivDecreaseMinLevel) * change;
			toReturn = 1;
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DerivativeDecreaseLeft::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeDecreaseLeft::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in DerivativeDecreaseLeft::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("derivDecreaseMinLevel")) {
			// ok, нашли, находим value
			derivDecreaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("derivDecreaseMaxLevel")) {
			// ok, нашли, находим value
			derivDecreaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("derivDecreaseLeft")) {
			// ok, нашли, находим value
			derivDecreaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("derivDecreaseRight")) {
			// ok, нашли, находим value
			derivDecreaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in DerivativeDecreaseLeft::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DerivativeDecreaseLeft::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeDecreaseLeft::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", derivDecreaseMinLevel);
	std::string str1 (temp);
	params["derivDecreaseMinLevel"] = str1;
	sprintf( temp, "%f", derivDecreaseMaxLevel);
	std::string str2 (temp);
	params["derivDecreaseMaxLevel"] = str2;
	sprintf( temp, "%d", derivDecreaseLeft);
	std::string str3 (temp);
	params["derivDecreaseLeft"] = str3;
	sprintf( temp, "%d", derivDecreaseRight);
	std::string str4 (temp);
	params["derivDecreaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			DerivativeDecreaseLeft::setParamValue
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
signed int DerivativeDecreaseLeft::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "derivDecreaseMinLevel") {
		derivDecreaseMinLevel = param;
		return 0;
	}
	if (paramName == "derivDecreaseMaxLevel") {
		derivDecreaseMaxLevel = param;
		return 0;
	}
	if (paramName == "derivDecreaseLeft") {
		derivDecreaseLeft = round (param);
		return 0;
	}
	if (paramName == "derivDecreaseRight") {
		derivDecreaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			DerivativeDecreaseLeft::getParamValue
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
signed int DerivativeDecreaseLeft::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "derivDecreaseMinLevel") {
		param = derivDecreaseMinLevel;
		return 0;
	}
	if (paramName == "derivDecreaseMaxLevel") {
		param = derivDecreaseMaxLevel;
		return 0;
	}
	if (paramName == "derivDecreaseLeft") {
		param = (double) derivDecreaseLeft;
		return 0;
	}
	if (paramName == "derivDecreaseRight") {
		param = (double) derivDecreaseLeft;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			DerivativeDecreaseLeft::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeDecreaseLeft::getECParamNames (std::vector<std::string> &ecParamNames) const{
	ecParamNames.resize(4);
	ecParamNames[0] = "derivDecreaseMinLevel";
	ecParamNames[1] = "derivDecreaseMaxLevel";
	ecParamNames[2] = "derivDecreaseLeft";
	ecParamNames[3] = "derivDecreaseRight";
	return 0;
}


/****************************************************************************
*			DerivativeDecreaseLeft::checkAndCompatibility
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
signed int DerivativeDecreaseLeft::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа DerivativeDecreaseLeft* из second типа el_axiom
	const DerivativeDecreaseLeft* y = dynamic_cast<const DerivativeDecreaseLeft*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип DerivativeDecreaseLeft&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((derivDecreaseMaxLevel >= y->derivDecreaseMaxLevel) && (derivDecreaseMinLevel <= y->derivDecreaseMaxLevel)) ||
		     ((derivDecreaseMaxLevel >= y->derivDecreaseMinLevel) && (derivDecreaseMinLevel <= y->derivDecreaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const DerivativeIncreaseLeft* x = dynamic_cast<const DerivativeIncreaseLeft*> (&second);
		if (x != NULL)
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					DerivativeDecreaseLeft::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров derivDecreaseLeftLimit и derivDecreaseRightLimit
*	Parameters:		newLeftLimit - новое значение для параметра derivDecreaseLeftLimit
*					newRightLimit - новое значение для параметра derivDecreaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int DerivativeDecreaseLeft::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in DerivativeDecreaseLeft::setLimits : parameters have a wrong values.");
	derivDecreaseLeftLimit = newLeftLimit;
	derivDecreaseRightLimit = newRightLimit;
	// Правый предел по определению условия должен быть равен 0
	derivDecreaseRightLimit = 0;
	return 0; 
}


/****************************************************************************
*					DerivativeDecreaseLeft::setLimits
*
*	Description:	Функция устанавливает значения параметров derivDecreaseMinLevelLimit, derivDecreaseMaxLevelLimit и levelDelta
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
signed int DerivativeDecreaseLeft::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	// Подсчет значений элементарного условия
	if (row.size() < 2) 
		return -1;
	derivDecreaseMinLevelLimit = max (row[0] - row[1], double_default_Min_Level_Limit);
	derivDecreaseMaxLevelLimit = derivDecreaseMinLevelLimit;
	for (int k = 2; k < (int) row.size(); k++) {
		derivDecreaseMinLevelLimit = min (row[k-1] - row[k], derivDecreaseMinLevelLimit);
		derivDecreaseMinLevelLimit = max (double_default_Min_Level_Limit, derivDecreaseMinLevelLimit);
		derivDecreaseMaxLevelLimit = max (row[k-1] - row[k], derivDecreaseMaxLevelLimit);
	}
	// Определение минимальных и максимальных значений
	if (reserve < 0.001)
		return -1;
	if (derivDecreaseMinLevelLimit < 0)
		derivDecreaseMinLevelLimit *= reserve;
	else 
		derivDecreaseMinLevelLimit /= reserve;
	if (derivDecreaseMaxLevelLimit > 0)
		derivDecreaseMaxLevelLimit *= reserve;
	else 
		derivDecreaseMaxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in DerivativeDecreaseLeft::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (derivDecreaseMaxLevelLimit - derivDecreaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					DerivativeDecreaseLeft::changeParams
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
signed int DerivativeDecreaseLeft::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		derivDecreaseMinLevel = derivDecreaseMinLevelLimit;
		derivDecreaseMaxLevel = derivDecreaseMinLevelLimit;
		derivDecreaseLeft = 0;
		derivDecreaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движение на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((derivDecreaseMaxLevel+levelDelta - derivDecreaseMaxLevelLimit) > eps) {
				if ((derivDecreaseMinLevel+levelDelta - derivDecreaseMaxLevelLimit) > eps) {
					if (derivDecreaseLeft >= derivDecreaseLeftLimit) {
						if (derivDecreaseRight >= derivDecreaseRightLimit)
							return -1;
						else {
							derivDecreaseRight++;
							derivDecreaseLeft = 0;
							derivDecreaseMinLevel = derivDecreaseMinLevelLimit;
							derivDecreaseMaxLevel = derivDecreaseMinLevelLimit;
						}
					} else {
						derivDecreaseLeft++;
						derivDecreaseMinLevel = derivDecreaseMinLevelLimit;
						derivDecreaseMaxLevel = derivDecreaseMinLevelLimit;
					}
				} else {
					derivDecreaseMinLevel += levelDelta;
					derivDecreaseMaxLevel = derivDecreaseMinLevel;
				}
			} else
				derivDecreaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((derivDecreaseMaxLevel-levelDelta - derivDecreaseMinLevel) < -eps) {
				if ((derivDecreaseMinLevel-levelDelta - derivDecreaseMinLevelLimit) < -eps) {
					if (derivDecreaseLeft == 0) {
						if (derivDecreaseRight == 0)
							return -1;
						else {
							derivDecreaseRight--;
							derivDecreaseLeft = derivDecreaseLeftLimit;
							derivDecreaseMinLevel = derivDecreaseMaxLevelLimit;
							derivDecreaseMaxLevel = derivDecreaseMaxLevelLimit;
						}
					} else {
						derivDecreaseLeft--;
						derivDecreaseMinLevel = derivDecreaseMaxLevelLimit;
						derivDecreaseMaxLevel = derivDecreaseMaxLevelLimit;
					}
				} else {
					derivDecreaseMinLevel -= levelDelta;
					derivDecreaseMaxLevel = derivDecreaseMaxLevelLimit;
				}
			} else
				derivDecreaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
