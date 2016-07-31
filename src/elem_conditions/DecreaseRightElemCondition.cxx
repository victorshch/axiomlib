/****************************************************************************
*
*			Функции класса DecreaseRightElemCondition
*
****************************************************************************/

#include <string>
#include "DecreaseRightElemCondition.h"
#include "IncreaseRightElemCondition.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			DecreaseRightElemCondition::DecreaseRightElemCondition
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
DecreaseRightElemCondition::DecreaseRightElemCondition (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	decreaseMinLevel = double_default_Min_Level_Limit;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	decreaseMaxLevel = 10;
	
	// Число точек требуемых для элементарного условия - слева
	decreaseLeft = 0;
	
	// Число точек требуемых для элементарного условия - справа
	decreaseRight = 1;

	// Минимальная граница изменения для параметра decreaseMinLevel, которая учитывается при подборе параметров перебором
	decreaseMinLevelLimit = decreaseMinLevel;
	
	// Максиомальная граница изменения для параметра decreaseMaxLevel, которая учитывается при подборе параметров перебором
	decreaseMaxLevelLimit = decreaseMaxLevel;

	// Шаг изменения параметров decreaseMinLevel и decreaseMaxLevel, при подборе параметров перебором
	levelDelta = decreaseMaxLevel - decreaseMinLevel;
	
	// Максимальное значение параметра decreaseLeft, которое учитывается при подборе параметров перебором
	decreaseLeftLimit = decreaseLeft;
	
	// Максимальное значение параметра decreaseRight, которое учитывается при подборе параметров перебором
	decreaseRightLimit = decreaseRight;
}

/****************************************************************************
*			DecreaseRightElemCondition::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string DecreaseRightElemCondition::name (void) const {
	std::string toReturn("decrease_right_elementary_condition");  
	return toReturn;
}


/****************************************************************************
*			DecreaseRightElemCondition::check
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
signed int DecreaseRightElemCondition::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (((int) k < decreaseLeft) || ((int)(x.size() - k) <= decreaseRight)) {
		toReturn = -1;
		//throw AxiomLibException("Error in DecreaseRightElemCondition::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	for (int i = 0; i < decreaseRight; i++) {
		if (((x[k] - x[k+i+1]) < decreaseMinLevel*(i+1)) || ((x[k] - x[k+i+1]) > decreaseMaxLevel*(i+1))) {
			return 0;
		}
	}
	
	return toReturn;
}

/****************************************************************************
*			DecreaseRightElemCondition::operator<
*
*	Description:	Сравнение двух DecreaseRightElemCondition на включение (x < y).
*	Parameters:	DecreaseRightElemCondition& second - правая часть выражения
*	Returns:		1 если  <  и second - это DecreaseRightElemCondition
*				-1 если  >  и second - это DecreaseRightElemCondition
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не DecreaseRightElemCondition)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double DecreaseRightElemCondition::operator< (const ElemCondition& second) const{
	// пытаемся получить y типа DecreaseRightElemCondition* из second типа el_axiom
	const DecreaseRightElemCondition* y = dynamic_cast<const DecreaseRightElemCondition*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип DecreaseRightElemCondition&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((decreaseMaxLevel < y->decreaseMaxLevel + 0.01) && (decreaseMinLevel > y->decreaseMinLevel - 0.01))
			return 1;
		else if ((y->decreaseMaxLevel < decreaseMaxLevel + 0.01) && (y->decreaseMinLevel > decreaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			DecreaseRightElemCondition::transmute
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
signed int DecreaseRightElemCondition::transmute (const double p) {
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
			decreaseMaxLevel = decreaseMaxLevel * (1 + change);
		}
		else {
			// уменьшаем нижнюю границу
			decreaseMinLevel = decreaseMinLevel * (1 - change);
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			decreaseMaxLevel = decreaseMaxLevel - (decreaseMaxLevel  - decreaseMinLevel) * change;
		}
		else {
			// увеличиваем нижнюю границу
			decreaseMinLevel = decreaseMinLevel + (decreaseMaxLevel  - decreaseMinLevel) * change;
		}
	}
	
	return 0;
}


/****************************************************************************
*			DecreaseRightElemCondition::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DecreaseRightElemCondition::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in DecreaseRightElemCondition::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("decreaseMinLevel")) {
			// ok, нашли, находим value
			decreaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("decreaseMaxLevel")) {
			// ok, нашли, находим value
			decreaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("decreaseLeft")) {
			// ok, нашли, находим value
			decreaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("decreaseRight")) {
			// ok, нашли, находим value
			decreaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in DecreaseRightElemCondition::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DecreaseRightElemCondition::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DecreaseRightElemCondition::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", decreaseMinLevel);
	std::string str1 (temp);
	params["decreaseMinLevel"] = str1;
	sprintf( temp, "%f", decreaseMaxLevel);
	std::string str2 (temp);
	params["decreaseMaxLevel"] = str2;
	sprintf( temp, "%d", decreaseLeft);
	std::string str3 (temp);
	params["decreaseLeft"] = str3;
	sprintf( temp, "%d", decreaseRight);
	std::string str4 (temp);
	params["decreaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			DecreaseRightElemCondition::setParamValue
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
signed int DecreaseRightElemCondition::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "decreaseMinLevel") {
		decreaseMinLevel = param;
		return 0;
	}
	if (paramName == "decreaseMaxLevel") {
		decreaseMaxLevel = param;
		return 0;
	}
	if (paramName == "decreaseLeft") {
		decreaseLeft = round (param);
		return 0;
	}
	if (paramName == "decreaseRight") {
		decreaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			DecreaseRightElemCondition::getParamValue
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
signed int DecreaseRightElemCondition::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "decreaseMinLevel") {
		param = decreaseMinLevel;
		return 0;
	}
	if (paramName == "decreaseMaxLevel") {
		param = decreaseMaxLevel;
		return 0;
	}
	if (paramName == "decreaseLeft") {
		param = (double) decreaseLeft;
		return 0;
	}
	if (paramName == "decreaseRight") {
		param = (double) decreaseRight;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			DecreaseRightElemCondition::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DecreaseRightElemCondition::getECParamNames (std::vector<std::string> &ecParamNames) const{
	ecParamNames.resize(4);
	ecParamNames[0] = "decreaseMinLevel";
	ecParamNames[1] = "decreaseMaxLevel";
	ecParamNames[2] = "decreaseLeft";
	ecParamNames[3] = "decreaseRight";
	return 0;
}


/****************************************************************************
*			DecreaseRightElemCondition::checkAndCompatibility
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
signed int DecreaseRightElemCondition::checkAndCompatibility (const ElemCondition& second) const {
	// пытаемся получить y типа DecreaseRightElemCondition* из second типа el_axiom
	const DecreaseRightElemCondition* y = dynamic_cast<const DecreaseRightElemCondition*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип DecreaseRightElemCondition&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((decreaseMaxLevel >= y->decreaseMaxLevel) && (decreaseMinLevel <= y->decreaseMaxLevel)) ||
		     ((decreaseMaxLevel >= y->decreaseMinLevel) && (decreaseMinLevel <= y->decreaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const IncreaseRightElemCondition* x = dynamic_cast<const IncreaseRightElemCondition*> (&second);
		if (x != NULL)
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					DecreaseRightElemCondition::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров decreaseLeftLimit и decreaseRightLimit
*	Parameters:		newLeftLimit - новое значение для параметра decreaseLeftLimit
*					newRightLimit - новое значение для параметра decreaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int DecreaseRightElemCondition::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in DecreaseRightElemCondition::setLimits : parameters have a wrong values.");
	decreaseLeftLimit = newLeftLimit;
	decreaseRightLimit = newRightLimit;
	// Левый предел по определению условия должен быть равен 0
	decreaseLeftLimit = 0;
	return 0; 
}


/****************************************************************************
*					DecreaseRightElemCondition::setLimits
*
*	Description:	Функция устанавливает значения параметров decreaseMinLevelLimit, decreaseMaxLevelLimit и levelDelta
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
signed int DecreaseRightElemCondition::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	double tmp;
	// Подсчет значений элементарного условия
	if (row.size() < 2) 
		return -1;
	decreaseMinLevelLimit = max (row[0] - row[1], double_default_Min_Level_Limit);
	decreaseMaxLevelLimit = decreaseMinLevelLimit;
	for (int k = decreaseLeftLimit; k < (int) row.size() - decreaseRightLimit; k++) {
		for (int i = 1; i <= decreaseRightLimit; i++) {
			tmp = (row[k] - row[k+i]) / (double) i;
			decreaseMinLevelLimit = min (tmp, decreaseMinLevelLimit);
			decreaseMinLevelLimit = max (double_default_Min_Level_Limit, decreaseMinLevelLimit); // контроль - чтобы не уйти в отрицательную зону
			decreaseMaxLevelLimit = max (tmp, decreaseMaxLevelLimit);
		}
	}
	// Определение минимальных и максимальных значений
	if (reserve < 0.001)
		return -1;
	if (decreaseMinLevelLimit < 0)
		decreaseMinLevelLimit *= reserve;
	else 
		decreaseMinLevelLimit /= reserve;
	if (decreaseMaxLevelLimit > 0)
		decreaseMaxLevelLimit *= reserve;
	else 
		decreaseMaxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in DecreaseRightElemCondition::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (decreaseMaxLevelLimit - decreaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					DecreaseRightElemCondition::changeParams
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
signed int DecreaseRightElemCondition::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		decreaseMinLevel = decreaseMinLevelLimit;
		decreaseMaxLevel = decreaseMinLevelLimit;
		decreaseLeft = 0;
		decreaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движение на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((decreaseMaxLevel+levelDelta - decreaseMaxLevelLimit) > eps) {
				if ((decreaseMinLevel+levelDelta - decreaseMaxLevelLimit) > eps) {
					if (decreaseLeft >= decreaseLeftLimit) {
						if (decreaseRight >= decreaseRightLimit)
							return -1;
						else {
							decreaseRight++;
							decreaseLeft = 0;
							decreaseMinLevel = decreaseMinLevelLimit;
							decreaseMaxLevel = decreaseMinLevelLimit;
						}
					} else {
						decreaseLeft++;
						decreaseMinLevel = decreaseMinLevelLimit;
						decreaseMaxLevel = decreaseMinLevelLimit;
					}
				} else {
					decreaseMinLevel += levelDelta;
					decreaseMaxLevel = decreaseMinLevel;
				}
			} else
				decreaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((decreaseMaxLevel-levelDelta - decreaseMinLevel) < -eps) {
				if ((decreaseMinLevel-levelDelta - decreaseMinLevelLimit) < -eps) {
					if (decreaseLeft == 0) {
						if (decreaseRight == 0)
							return -1;
						else {
							decreaseRight--;
							decreaseLeft = decreaseLeftLimit;
							decreaseMinLevel = decreaseMaxLevelLimit;
							decreaseMaxLevel = decreaseMaxLevelLimit;
						}
					} else {
						decreaseLeft--;
						decreaseMinLevel = decreaseMaxLevelLimit;
						decreaseMaxLevel = decreaseMaxLevelLimit;
					}
				} else {
					decreaseMinLevel -= levelDelta;
					decreaseMaxLevel = decreaseMaxLevelLimit;
				}
			} else
				decreaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
