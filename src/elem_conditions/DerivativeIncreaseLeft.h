/****************************************************************************
*				DerivativeIncreaseLeft
*
*	Description:	Элементарное условие возрастания слева
*	Author:		dk
*	History:
*
****************************************************************************/

#ifndef __AXIOMS_LIB_EL_DerivativeIncreaseLeft_HXX
#define __AXIOMS_LIB_EL_DerivativeIncreaseLeft_HXX

#include "ElemCondition.h"

namespace AxiomLib { 

class DerivativeIncreaseLeft : public ElemCondition {

  private:
	//объявления, необходимые для сериализации
	friend class boost::serialization::access;

	//функция сериализации - в ней должны быть указаны члены-данные,
	//характеризующие состояние объекта
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ElemCondition);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseLeft);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseLeftLimit);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseMaxLevel);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseMaxLevelLimit);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseMinLevel);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseMinLevelLimit);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseRight);
		archive & BOOST_SERIALIZATION_NVP(derivIncreaseRightLimit);
		archive & BOOST_SERIALIZATION_NVP(levelDelta);
	}

	// Минимальная граница изменения для параметра derivIncreaseMinLevel, которая учитывается при подборе параметров перебором
	double derivIncreaseMinLevelLimit;
	
	// Максиомальная граница изменения для параметра derivIncreaseMaxLevel, которая учитывается при подборе параметров перебором
	double derivIncreaseMaxLevelLimit;

	// Шаг изменения параметров derivIncreaseMinLevel и derivIncreaseMaxLevel, при подборе параметров перебором
	double levelDelta;
	
	// Максимальное значение параметра derivIncreaseLeft, которое учитывается при подборе параметров перебором
	int derivIncreaseLeftLimit;
	
	// Максимальное значение параметра derivIncreaseRight, которое учитывается при подборе параметров перебором
	int derivIncreaseRightLimit;

  protected:
	  
	// Минимальное возрастание подходящее данному элементарному условию
	double derivIncreaseMinLevel;
	
	// Максиомальное возрастание пододящее данному элементарному условию
	double derivIncreaseMaxLevel;
	
	// Число точек требуемых для элементарного условия - слева
	int derivIncreaseLeft;
	
	// Число точек требуемых для элементарного условия - справа
	int derivIncreaseRight;
	
  public:
	
	// Пустой конструктор
	DerivativeIncreaseLeft(void);
	
	// Деструктор - удаляает все созданные динамические объекты
	~DerivativeIncreaseLeft(void) {};
	
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

	// Функция устанавливает максимальные значения параметров derivIncreaseLeftLimit и derivIncreaseRightLimit
	signed int setLimits (const int newLeftLimit, const int newRightLimit);

	// Функция устанавливает значения параметров derivIncreaseMinLevelLimit, derivIncreaseMaxLevelLimit и levelDelta
	signed int setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels);

	// Функция изменяет параметры элементарного условия на число шагов, указанное в параметрах
	signed int changeParams (const int numOfSteps);
	
	// Функция клонирования - реализация шаблона "виртуальный конструктор копирования"
	DerivativeIncreaseLeft* clone() const { return new DerivativeIncreaseLeft(*this); }

	// Функция присваивает данному ЭУ ЭУ other и возвращает true, если у них совпадает тип
	// Иначе возвращает false
	bool assign(const ElemCondition *other) { return assign_impl(this, other); }
	
}; // of class

}; // of namespace



#endif /* __AXIOMS_LIB_EL_DerivativeIncreaseLeft_HXX */
