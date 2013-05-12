/****************************************************************************
*				Volatility
*
*	Description:	Элементарное условие - волатильность в заданной окрестности размечаемой точки находится в заданных пределах
*	Author:		dk
*	History:
*
****************************************************************************/

#ifndef __AXIOMS_LIB_EL_Variance_HXX
#define __AXIOMS_LIB_EL_Variance_HXX

#include "ElemCondition.h"

namespace AxiomLib { 

class Variance : public ElemCondition {
  private:
	//объявления, необходимые для сериализации
	friend class boost::serialization::access;
	//функция сериализации - в ней должны быть указаны члены-данные,
	//характеризующие состояние объекта
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ElemCondition);
		archive & BOOST_SERIALIZATION_NVP(levelDelta);
		archive & BOOST_SERIALIZATION_NVP(minLevelLimit);
		archive & BOOST_SERIALIZATION_NVP(maxLevelLimit);
		archive & BOOST_SERIALIZATION_NVP(leftLimit);
		archive & BOOST_SERIALIZATION_NVP(rightLimit);
		//archive & BOOST_SERIALIZATION_NVP(minLevel);
		archive & BOOST_SERIALIZATION_NVP(maxLevel);
		archive & BOOST_SERIALIZATION_NVP(left);
		archive & BOOST_SERIALIZATION_NVP(right);
	}

	// Минимальная граница изменения для параметра minLevel, которая учитывается при подборе параметров перебором
	double minLevelLimit;
	
	// Максиомальная граница изменения для параметра maxLevel, которая учитывается при подборе параметров перебором
	double maxLevelLimit;

	// Шаг изменения параметров minLevel и maxLevel, при подборе параметров перебором
	double levelDelta;
	
	// Максимальное значение параметра left, которое учитывается при подборе параметров перебором
	int leftLimit;
	
	// Максимальное значение параметра right, которое учитывается при подборе параметров перебором
	int rightLimit;

  protected:
	  
	// Минимальное значение численного интеграла
	//double minLevel;
	
	// Максиомальное значение численного интеграла
	double maxLevel;
	
	// Число точек требуемых для элементарного условия - слева
	int left;
	
	// Число точек требуемых для элементарного условия - справа
	int right;
	
  public:
	
	// Пустой конструктор
    Variance(void);
	
	// Деструктор - удаляает все созданные динамические объекты
    ~Variance(void) {};
	
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

	// Функция устанавливает максимальные значения параметров leftLimit и rightLimit
	signed int setLimits (const int newLeftLimit, const int newRightLimit);

	// Функция устанавливает значения параметров minLevelLimit, maxLevelLimit и levelDelta
	signed int setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels);

	// Функция изменяет параметры элементарного условия на число шагов, указанное в параметрах
	signed int changeParams (const int numOfSteps);

	// Функция клонирования - реализация шаблона "виртуальный конструктор копирования"
    Variance * clone() const { return new Variance(*this); }

	// Функция присваивает данному ЭУ ЭУ other и возвращает true, если у них совпадает тип
	// Иначе возвращает false
	bool assign(const ElemCondition *other) { return assign_impl(this, other); }
	
}; // of class

}; // of namespace



#endif /* __AXIOMS_LIB_EL_Variance_HXX */
