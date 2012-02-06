/****************************************************************************
*
*			Функции класса BetweenTwoLevels
*
****************************************************************************/

#include <string>
#include "BetweenTwoLevels.h"
#include "DecreaseLeftElemCondition.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;
using namespace std;



#define eps		0.0000000001


/****************************************************************************
*			BetweenTwoLevels::BetweenTwoLevels
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
BetweenTwoLevels::BetweenTwoLevels (void) {
	// Задаваемые по умолчанию занчения параметров
	
	// Минимальное возрастание подходящее данному элементарному условию
	Min = -100;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	Max = 100;
	
	// Число точек требуемых для элементарного условия - слева
	Left = 0;
	
	// Число точек требуемых для элементарного условия - справа
	Right = 0;

	// Минимальная граница изменения для параметра Min, которая учитывается при подборе параметров перебором
	minLimit = Min;
	
	// Максиомальная граница изменения для параметра Max, которая учитывается при подборе параметров перебором
	maxLimit = Max;

	// Шаг изменения параметров Min и Max, при подборе параметров перебором
	levelDelta = Max - Min;
	
	// Максимальное значение параметра Left, которое учитывается при подборе параметров перебором
	leftLimit = Left;
	
	// Максимальное значение параметра Right, которое учитывается при подборе параметров перебором
	rightLimit = Right;
}

/****************************************************************************
*			BetweenTwoLevels::name
*
*	Description:	Функция возвращает имя аксиомы
*	Parameters:	-
*	Returns:		std::string - имя аксиомы
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string BetweenTwoLevels::name (void) const {
	std::string toReturn("between_two_levels");
	return toReturn;
}


/****************************************************************************
*			BetweenTwoLevels::check
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
signed int BetweenTwoLevels::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// проверка границ
	if (( (int) k < Left) || ( (int) (x.size() - k) <= Right)) {
		toReturn = -1;
		//throw AxiomLibException("Error in BetweenTwoLevels::check : out of range.");
		return toReturn;
	}
	// проверка элементарного условия
	toReturn = 1;
	for (int i = (int) k - Left; i <= (int) k + Right; i++) {
		if ( (x[i] > Max) || (x[i] < Min) ) {
			return 0;
		}
	}
	
	return toReturn;
}

/****************************************************************************
*			BetweenTwoLevels::operator<
*
*	Description:	Сравнение двух BetweenTwoLevels на включение (x < y).
*	Parameters:	BetweenTwoLevels& second - правая часть выражения
*	Returns:		1 если < и second - это BetweenTwoLevels
*				-1 если > и second - это BetweenTwoLevels
*				0 иначе (по сути, если параметры отличаются не более, чем на 0.01 или если second - это не BetweenTwoLevels)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double BetweenTwoLevels::operator< (const ElemCondition& second) const {
	// пытаемся получить y типа BetweenTwoLevels* из second типа el_axiom
	const BetweenTwoLevels* y = dynamic_cast<const BetweenTwoLevels*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип BetweenTwoLevels&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ((Max < y->Max + 0.01) && (Min > y->Min - 0.01))
			return 1;
		else if ((y->Max < Max + 0.01) && (y->Min > Min - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			BetweenTwoLevels::transmute
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
signed int BetweenTwoLevels::transmute (const double p) {
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
			Max = Max + (Max - Min) * change;
			toReturn = 1;
		}
		else {
			// уменьшаем нижнюю границу
			Min = Min -  (Max - Min) * change;
			toReturn = 1;
		}
	}
	else {
		// уменьшаем размеры области
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// уменьшаем верхнюю границу
			Max = Max - (Max  - Min) * change;
			toReturn = 1;
		}
		else {
			// увеличиваем нижнюю границу
			Min = Min + (Max  - Min) * change;
			toReturn = 1;
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			BetweenTwoLevels::setParams
*
*	Description:	Задает значение параметров элементарного условия 
*	Parameters:	map<string,string> - карта название параметра <-> строчка значения
*	Returns:		0 если все ok (пусть даже не все параметры есть в карте)
*	Throws:		AxiomLibException если карта пустая или в карте есть лишние параметры
*	Author:		dk
*	History:
*
****************************************************************************/
signed int BetweenTwoLevels::setParams (const std::map<std::string, std::string>& newParams) {
	signed int toReturn=0;
	
	// проверяем, пустая ли карта?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in BetweenTwoLevels::setParams : zero parameter map.");
	}
	int flag = 0;
	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// Ищем, какому параметру соответствует i
		if (paramName == (const std::string)("Min")) {
			// ok, нашли, находим value
			Min = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("Max")) {
			// ok, нашли, находим value
			Max = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("Left")) {
			// ok, нашли, находим value
			Left = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("Right")) {
			// ok, нашли, находим value
			Right = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// неизвестный параметр
			throw AxiomLibException("Error in BetweenTwoLevels::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			BetweenTwoLevels::getParams
*
*	Description:	Возвращает значения параметров
*	Parameters:	map<string,string> - заполняемая карта название параметра <-> строчка значения
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int BetweenTwoLevels::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", Min);
	std::string str1 (temp);
	params["Min"] = str1;
	sprintf( temp, "%f", Max);
	std::string str2 (temp);
	params["Max"] = str2;
	sprintf( temp, "%d", Left);
	std::string str3 (temp);
	params["Left"] = str3;
	sprintf( temp, "%d", Right);
	std::string str4 (temp);
	params["Right"] = str4;
	return 0;
}


/****************************************************************************
*			BetweenTwoLevels::setParamValue
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
signed int BetweenTwoLevels::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "Min") {
		Min = param;
		return 0;
	}
	if (paramName == "Max") {
		Max = param;
		return 0;
	}
	if (paramName == "Left") {
		Left = round (param);
		return 0;
	}
	if (paramName == "Right") {
		Right = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			BetweenTwoLevels::getParamValue
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
signed int BetweenTwoLevels::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "Min") {
		param = Min;
		return 0;
	}
	if (paramName == "Max") {
		param = Max;
		return 0;
	}
	if (paramName == "Left") {
		param = (double) Left;
		return 0;
	}
	if (paramName == "Right") {
		param = (double) Right;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			BetweenTwoLevels::getECParamNames
*
*	Description:	Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор с названиями параметров
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int BetweenTwoLevels::getECParamNames (std::vector<std::string> &ecParamNames) const {
	ecParamNames.resize(4);
	ecParamNames[0] = "Min";
	ecParamNames[1] = "Max";
	ecParamNames[2] = "Left";
	ecParamNames[3] = "Right";
	return 0;
}


/****************************************************************************
*			BetweenTwoLevels::checkAndCompatibility
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
signed int BetweenTwoLevels::checkAndCompatibility (const ElemCondition& second) const{
	// пытаемся получить y типа BetweenTwoLevels* из second типа el_axiom
	const BetweenTwoLevels* y = dynamic_cast<const BetweenTwoLevels*> (&second);
  
	// если y != NULL, т.н. second действительно имело тип BetweenTwoLevels&, то тогда что-то проверяем
	// если y == NULL, т.е. на сравнение нам подали аксиому другого типа, сразу возвращаем 0
	if (y != NULL) {
		if ( ((Max >= y->Max) && (Min <= y->Max)) || ((Max >= y->Min) && (Min <= y->Min)) )
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
*					BetweenTwoLevels::setLimits
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
signed int BetweenTwoLevels::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in BetweenTwoLevels::setLimits : parameters have a wrong values.");
	leftLimit = newLeftLimit;
	rightLimit = newRightLimit;
	return 0; 
}


/****************************************************************************
*					BetweenTwoLevels::setLimits
*
*	Description:	Функция устанавливает значения параметров minLimit, maxLimit и levelDelta
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
signed int BetweenTwoLevels::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	if (row.size() < 1)
		return -1;
	// Определение минимальных и максимальных значений
	minLimit = row [0];
	maxLimit = row [0];
	for (unsigned int j = 1; j < row.size(); j++) {
		minLimit = min (row [j], minLimit);
		maxLimit = max (row [j], maxLimit);
	}
	if (reserve < 0.001)
		return -1;
	if (minLimit < 0)
		minLimit *= reserve;
	else 
		minLimit /= reserve;
	if (maxLimit > 0)
		maxLimit *= reserve;
	else 
		maxLimit /= reserve;
	// Определение шага изменения параметров
	if (numOfLevels < 1)
		throw AxiomLibException("Error in BetweenTwoLevels::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (maxLimit - minLimit) / (double) numOfLevels;
	if(levelDelta < 1e-8) {
		return -1;
	}	
	return 0;
}


/****************************************************************************
*					BetweenTwoLevels::changeParams
*
*	Description:	Функция изменяет параметры элементарного условия на число шагов numOfSteps
*	Parameters:		numOfSteps - число шагов, на которое изменять параметры элементарного условия
*	Returns:		0 - если изменение произошло удачно
*					-1 - елси изменение на указанное число шаго невозможно
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int BetweenTwoLevels::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // Если входной параметр = 0 - то устанавливаем начальные значения для параметров класса
		Min = minLimit;
		Max = minLimit;
		Left = 0;
		Right = 0;
		return 0;
	}
	if (numOfSteps > 0) { // движенеи на определенное число шагов вперед (наращивание параметров)
		for (int num = 0; num < numOfSteps; num++) {
			if ((Max+levelDelta - maxLimit) > eps) {
				if ((Min+levelDelta - maxLimit) > eps) {
					if (Left >= leftLimit) {
						if (Right >= rightLimit)
							return -1;
						else {
							Right++;
							Left = 0;
							Min = minLimit;
							Max = minLimit;
						}
					} else {
						Left++;
						Min = minLimit;
						Max = minLimit;
					}
				} else {
					Min += levelDelta;
					Max = Min;
				}
			} else
				Max += levelDelta;
		}
		return 0;
	} else { // движение на определенное количество шагов назад
		for (int num = 0; num > numOfSteps; num--) {
			if ((Max-levelDelta - Min) < -eps) {
				if ((Min-levelDelta - minLimit) < -eps) {
					if (Left == 0) {
						if (Right == 0)
							return -1;
						else {
							Right--;
							Left = leftLimit;
							Min = maxLimit;
							Max = maxLimit;
						}
					} else {
						Left--;
						Min = maxLimit;
						Max = maxLimit;
					}
				} else {
					Min -= levelDelta;
					Max = maxLimit;
				}
			} else
				Max -= levelDelta;
		}
		return 0;
	}

	return -1;
}
