/****************************************************************************
*				DecreaseLeftElemCondition
*
*	Description:	Элементарное условие убывания слева в заданном угле
*	Author:		dk
*	History:
*
****************************************************************************/

#ifndef __AXIOMS_LIB_EL_DecreaseLeftElemCondition_HXX
#define __AXIOMS_LIB_EL_DecreaseLeftElemCondition_HXX

#include "ElemCondition.h"

namespace AxiomLib { 

class DecreaseLeftElemCondition : public ElemCondition {

  private:

	//объявления, необходимые для сериализации
	friend class boost::serialization::access;

	//функция сериализации - в ней должны быть указаны члены-данные,
	//характеризующие состояние объекта
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ElemCondition);
		archive & BOOST_SERIALIZATION_NVP(decreaseLeft);
		archive & BOOST_SERIALIZATION_NVP(decreaseLeftLimit);
		archive & BOOST_SERIALIZATION_NVP(decreaseMaxLevel);
		archive & BOOST_SERIALIZATION_NVP(decreaseMaxLevelLimit);
		archive & BOOST_SERIALIZATION_NVP(decreaseMinLevel);
		archive & BOOST_SERIALIZATION_NVP(decreaseMinLevelLimit);
		archive & BOOST_SERIALIZATION_NVP(decreaseRight);
		archive & BOOST_SERIALIZATION_NVP(decreaseRightLimit);
		archive & BOOST_SERIALIZATION_NVP(levelDelta);
	}

	// Минимальная граница изменения для параметра decreaseMinLevel, которая учитывается при подборе параметров перебором
	double decreaseMinLevelLimit;
	
	// Максиомальная граница изменения для параметра decreaseMaxLevel, которая учитывается при подборе параметров перебором
	double decreaseMaxLevelLimit;

	// Шаг изменения параметров decreaseMinLevel и decreaseMaxLevel, при подборе параметров перебором
	double levelDelta;
	
	// Максимальное значение параметра decreaseLeft, которое учитывается при подборе параметров перебором
	int decreaseLeftLimit;
	
	// Максимальное значение параметра decreaseRight, которое учитывается при подборе параметров перебором
	int decreaseRightLimit;

  protected:
	  
	// Минимальное возрастание подходящее данному элементарному условию
	double decreaseMinLevel;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	double decreaseMaxLevel;
	
	// Число точек требуемых для элементарного условия - слева
	int decreaseLeft;
	
	// Число точек требуемых для элементарного условия - справа
	int decreaseRight;
	
  public:
	
	// Пустой конструктор
	DecreaseLeftElemCondition(void);
	
	// Деструктор - удаляает все созданные динамические объекты
	~DecreaseLeftElemCondition(void) {};
	
	// Функция возвращает имя данного элементарного условия
	std::string name(void) const;
	
	// Функция проверки - выполняется ли данное элементарное условие в точке ряда
	inline signed int check (const unsigned long k, const std::vector<double>& x) const;
	
	// Оператор сравнения для данного элементарного условия
	double operator< (const ElemCondition& second) const;
	
	// Оператор мутации элементарного условия
	signed int transmute (const double p);
	
	// Оператор задания параметров данного элементарного условия
	signed int setParams (const std::map<std::string,std::string>& newParams);
	
	// Функция возвращает текущие выставленные параметры
	signed int getParams (std::map<std::string,std::string> &params) const;
	
	// Функция устанавливает значение одного параметра элементарного условия, если такой параметр присутствует в данном элементарном условии
	signed int setParamValue (const double param, const std::string &paramName);
	
	// Функция записывает значение одного параметра элементарного условия, если такой параметр присутствует в данном элементарном условии
	signed int getParamValue (double &param, const std::string &paramName) const;
	
	// Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
	signed int getECParamNames (std::vector<std::string> &ecParamNames) const;
	
	// Оператор проверки совместимости элемнтарного условия с задаваемым в параметрах
	signed int checkAndCompatibility (const ElemCondition& toCheck) const;

	// Функция устанавливает максимальные значения параметров decreaseLeftLimit и decreaseRightLimit
	signed int setLimits (const int newLeftLimit, const int newRightLimit);

	// Функция устанавливает значения параметров decreaseMinLevelLimit, decreaseMaxLevelLimit и levelDelta
	signed int setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels);

	// Функция изменяет параметры элементарного условия на число шагов, указанное в параметрах
	signed int changeParams (const int numOfSteps);

	// Функция клонирования - реализация шаблона "виртуальный конструктор копирования"
	DecreaseLeftElemCondition * clone() const { return new DecreaseLeftElemCondition(*this); }

	// Функция присваивает данному ЭУ ЭУ other и возвращает true, если у них совпадает тип
	// Иначе возвращает false
	bool assign(const ElemCondition *other) { return assign_impl(this, other); }
	
}; // of class

}; // of namespace



#endif /* __AXIOMS_LIB_EL_DecreaseLeftElemCondition_HXX */
