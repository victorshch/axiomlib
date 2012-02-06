/****************************************************************************
*
*			Функции класса IncreaseRightElemCondition
*
****************************************************************************/

#include <string>
#include "IncreaseRightElemCondition.h"
#include "DecreaseRightElemCondition.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			IncreaseRightElemCondition::IncreaseRightElemCondition
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
IncreaseRightElemCondition::IncreaseRightElemCondition (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	increaseMinLevel = double_default_Min_Level_Limit;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	increaseMaxLevel = 10;
	
	// Число точек требуемых для элементарного условия - слева
	increaseLeft = 0;
	
	// Число точек требуемых для элементарного условия - справа
	increaseRight = 1;

	// Минимальная граница изменения для параметра increaseMinLevel, которая учитывается при подборе параметров перебором
	increaseMinLevelLimit = increaseMinLevel;
	
	// Максиомальная граница изменения для параметра increaseMaxLevel, которая учитывается при подборе параметров перебором
	increaseMaxLevelLimit = increaseMaxLevel;

	// Шаг изменения параметров increaseMinLevel и increaseMaxLevel, при подборе параметров перебором
	levelDelta = increaseMaxLevel - increaseMinLevel;
	
	// Максимальное значение параметра increaseLeft, которое учитывается при подборе параметров перебором
	increaseLeftLimit = increaseLeft;
	
	// Максимальное значение параметра increaseRight, которое учитывается при подборе параметров перебором
	increaseRightLimit = increaseRight;
}

/****************************************************************************
*			IncreaseRightElemCondition::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string IncreaseRightElemCondition::name (void) const {
	std::string toReturn("increase_right_elementary_condition");  
	return toReturn;
}


/****************************************************************************
*			IncreaseRightElemCondition::check
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
signed int IncreaseRightElemCondition::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (((int) k < increaseLeft) || ((int) (x.size() - k) <= increaseRight)) {
		toReturn = -1;
		//throw AxiomLibException("Error in IncreaseRightElemCondition::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	for (int i = 0; i < increaseRight; i++) {
		if (((x[k+i+1] - x[k]) < increaseMinLevel*(i+1)) || ((x[k+i+1] - x[k]) > increaseMaxLevel*(i+1))) {
			return 0;
		}
	}
	
	return toReturn;
}

/****************************************************************************
*			IncreaseRightElemCondition::operator<
*
*	Description:	Сравнение двух IncreaseRightElemCondition на включение (x < y).
*	Parameters:	IncreaseRightElemCondition& second - правая часть выражения
*	Returns:		1 если < и second - это IncreaseRightElemCondition
*				-1 если > и second - это IncreaseRightElemCondition
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не IncreaseRightElemCondition)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double IncreaseRightElemCondition::operator< (const ElemCondition& second) const{
	// пытаемся получить y типа IncreaseRightElemCondition* из second типа el_axiom
	const IncreaseRightElemCondition* y = dynamic_cast<const IncreaseRightElemCondition*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип IncreaseRightElemCondition&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((increaseMaxLevel < y->increaseMaxLevel + 0.01) && (increaseMinLevel > y->increaseMinLevel - 0.01))
			return 1;
		else if ((y->increaseMaxLevel < increaseMaxLevel + 0.01) && (y->increaseMinLevel > increaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			IncreaseRightElemCondition::transmute
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
signed int IncreaseRightElemCondition::transmute (const double p) {
	signed int toReturn=0;
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
			increaseMaxLevel = increaseMaxLevel * (1 + change);
			toReturn = 1;
		}
		else {
			// уменьшаем нижнюю границу
			increaseMinLevel = increaseMinLevel * (1 - change);
			toReturn = 1;
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			increaseMaxLevel = increaseMaxLevel - (increaseMaxLevel  - increaseMinLevel) * change;
			toReturn = 1;
		}
		else {
			// увеличиваем нижнюю границу
			increaseMinLevel = increaseMinLevel + (increaseMaxLevel  - increaseMinLevel) * change;
			toReturn = 1;
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			IncreaseRightElemCondition::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int IncreaseRightElemCondition::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in IncreaseRightElemCondition::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("increaseMinLevel")) {
			// ok, нашли, находим value
			increaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("increaseMaxLevel")) {
			// ok, нашли, находим value
			increaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("increaseLeft")) {
			// ok, нашли, находим value
			increaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("increaseRight")) {
			// ok, нашли, находим value
			increaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in IncreaseRightElemCondition::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			IncreaseRightElemCondition::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int IncreaseRightElemCondition::getParams (std::map<std::string,std::string> &params) const {
	params.clear();
	char temp[64];
	sprintf( temp, "%f", increaseMinLevel);
	std::string str1 (temp);
	params["increaseMinLevel"] = str1;
	sprintf( temp, "%f", increaseMaxLevel);
	std::string str2 (temp);
	params["increaseMaxLevel"] = str2;
	sprintf( temp, "%d", increaseLeft);
	std::string str3 (temp);
	params["increaseLeft"] = str3;
	sprintf( temp, "%d", increaseRight);
	std::string str4 (temp);
	params["increaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			IncreaseRightElemCondition::setParamValue
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
signed int IncreaseRightElemCondition::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "increaseMinLevel") {
		increaseMinLevel = param;
		return 0;
	}
	if (paramName == "increaseMaxLevel") {
		increaseMaxLevel = param;
		return 0;
	}
	if (paramName == "increaseLeft") {
		increaseLeft = round (param);
		return 0;
	}
	if (paramName == "increaseRight") {
		increaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			IncreaseRightElemCondition::getParamValue
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
signed int IncreaseRightElemCondition::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "increaseMinLevel") {
		param = increaseMinLevel;
		return 0;
	}
	if (paramName == "increaseMaxLevel") {
		param = increaseMaxLevel;
		return 0;
	}
	if (paramName == "increaseLeft") {
		param = (double) increaseLeft;
		return 0;
	}
	if (paramName == "increaseRight") {
		param = (double) increaseRight;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			IncreaseRightElemCondition::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int IncreaseRightElemCondition::getECParamNames (std::vector<std::string> &ecParamNames) const {
	ecParamNames.resize(4);
	ecParamNames[0] = "increaseMinLevel";
	ecParamNames[1] = "increaseMaxLevel";
	ecParamNames[2] = "increaseLeft";
	ecParamNames[3] = "increaseRight";
	return 0;
}


/****************************************************************************
*			IncreaseRightElemCondition::checkAndCompatibility
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
signed int IncreaseRightElemCondition::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа IncreaseRightElemCondition* из second типа el_axiom
	const IncreaseRightElemCondition* y = dynamic_cast<const IncreaseRightElemCondition*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип IncreaseRightElemCondition&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((increaseMaxLevel >= y->increaseMaxLevel) && (increaseMinLevel <= y->increaseMaxLevel)) ||
		     ((increaseMaxLevel >= y->increaseMinLevel) && (increaseMinLevel <= y->increaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const DecreaseRightElemCondition* x = dynamic_cast<const DecreaseRightElemCondition*> (&second);
		if (x != NULL)
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					IncreaseRightElemCondition::setLimits
*
*	Description:	Функция устанавливает максимальные значения параметров increaseLeftLimit и increaseRightLimit
*	Parameters:		newLeftLimit - новое значение для параметра increaseLeftLimit
*					newRightLimit - новое значение для параметра increaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - если хотя бы один из входных параметров 
*					имеет отрицательное значение. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int IncreaseRightElemCondition::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in IncreaseRightElemCondition::setLimits : parameters have a wrong values.");
	increaseLeftLimit = newLeftLimit;
	increaseRightLimit = newRightLimit;
	// Левый предел по определению условия должен быть равен 0
	increaseLeftLimit = 0;
	return 0; 
}


/****************************************************************************
*					IncreaseRightElemCondition::setLimits
*
*	Description:	Функция устанавливает значения параметров increaseMinLevelLimit, increaseMaxLevelLimit и levelDelta
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
signed int IncreaseRightElemCondition::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	double tmp;
	// Подсчет значений элементарного условия
	if (row.size() < 2) 
		return -1;
	increaseMinLevelLimit = max (row[1] - row[0], double_default_Min_Level_Limit);
	increaseMaxLevelLimit = increaseMinLevelLimit;
	for (int k = increaseLeftLimit; k < (int) row.size() - increaseRightLimit; k++) {
		for (int i = 1; i <= increaseRightLimit; i++) {
			tmp = (row[k+i] - row[k]) / (double) i;
			increaseMinLevelLimit = min (tmp, increaseMinLevelLimit);
			increaseMinLevelLimit = max (double_default_Min_Level_Limit, increaseMinLevelLimit); // контроль - чтобы не уйти в отрицательную зону
			increaseMaxLevelLimit = max (tmp, increaseMaxLevelLimit);
		}
	}
	// Определение минимальных и максимальных значений
	if (reserve < 0.001)
		return -1;
	if (increaseMinLevelLimit < 0)
		increaseMinLevelLimit *= reserve;
	else 
		increaseMinLevelLimit /= reserve;
	if (increaseMaxLevelLimit > 0)
		increaseMaxLevelLimit *= reserve;
	else 
		increaseMaxLevelLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in IncreaseRightElemCondition::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (increaseMaxLevelLimit - increaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					IncreaseRightElemCondition::changeParams
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
signed int IncreaseRightElemCondition::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		increaseMinLevel = increaseMinLevelLimit;
		increaseMaxLevel = increaseMinLevelLimit;
		increaseLeft = 0;
		increaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движение на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((increaseMaxLevel+levelDelta - increaseMaxLevelLimit) > eps) {
				if ((increaseMinLevel+levelDelta - increaseMaxLevelLimit) > eps) {
					if (increaseLeft >= increaseLeftLimit) {
						if (increaseRight >= increaseRightLimit)
							return -1;
						else {
							increaseRight++;
							increaseLeft = 0;
							increaseMinLevel = increaseMinLevelLimit;
							increaseMaxLevel = increaseMinLevelLimit;
						}
					} else {
						increaseLeft++;
						increaseMinLevel = increaseMinLevelLimit;
						increaseMaxLevel = increaseMinLevelLimit;
					}
				} else {
					increaseMinLevel += levelDelta;
					increaseMaxLevel = increaseMinLevel;
				}
			} else
				increaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((increaseMaxLevel-levelDelta - increaseMinLevel) < -eps) {
				if ((increaseMinLevel-levelDelta - increaseMinLevelLimit) < -eps) {
					if (increaseLeft == 0) {
						if (increaseRight == 0)
							return -1;
						else {
							increaseRight--;
							increaseLeft = increaseLeftLimit;
							increaseMinLevel = increaseMaxLevelLimit;
							increaseMaxLevel = increaseMaxLevelLimit;
						}
					} else {
						increaseLeft--;
						increaseMinLevel = increaseMaxLevelLimit;
						increaseMaxLevel = increaseMaxLevelLimit;
					}
				} else {
					increaseMinLevel -= levelDelta;
					increaseMaxLevel = increaseMaxLevelLimit;
				}
			} else
				increaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
