/****************************************************************************
*			ElemCondition
*
*	Description:	Общий предок классов элементарных условий
*	Author:		Eugene A. Vasin
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_EL_SUPER_AXIOM_HXX
#define __AXIOMS_LIB_EL_SUPER_AXIOM_HXX

#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <map>
#include "boost/serialization/assume_abstract.hpp"
#include "boost/serialization/access.hpp"
#include "boost/serialization/nvp.hpp"


namespace AxiomLib {

//#pragma warning (disable:4996)

class ElemCondition {
  private:

	//объявления, необходимые для сериализации
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		std::string name = this->name();
		archive & BOOST_SERIALIZATION_NVP(name);
	}

  protected:
	// Реализация assign -- она для всех одинакова с точностью
	// до названия класса, поэтому она вынесена в шаблонную функцию
	template<class Derived>
	static bool assign_impl(Derived* first, const ElemCondition* second);
  public:

      // Стандартный конструктор
    ElemCondition(void) {}
      // Стандартный деструктор
    virtual ~ElemCondition() {}

      // Возвращает имя данной аксиомы.
    virtual std::string name (void) const = 0;

      // Проверка выполнения данной аксиомы. На входе - точка временного ряда k
      // и сам временной ряд x. На выходе 1, если выполняется, 0 если не
      // выполняется и -1, если не применимо (выход аксиомы за границы ряда
      // слева или справа).
      // Будем считать ряды одномерными, ибо все аксиомы пока у нас будут одномерными,
      // а факт многомерности исходного ряда мы будем использовт, на более
      // высоком уровне.
    virtual signed int check (const unsigned long k, const std::vector<double>& x) const = 0;

      // Функция проверки - выполняется ли данное элементарное условие в диапазоне
	//virtual signed int enter (std::vector<int>& markup,const std::vector<double>& x, const unsigned long begin, const unsigned long end) const=0;
    
      // Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
      // Возвращает 1, если первая аксиома является частным случаем второй, 0 если аксиомы
      // не сравнимы или тождественны, -1, если вторая является частным случаем первой
    virtual double operator< (const ElemCondition& second) const = 0;
    
      // Операция мутации: изменение каких-то внутренних параметров условия (что именно и
      // как меняется от наблюдателя скрыто). p определяет степень изменения (должно находиться
      // на отрезке [0,1]). Возвращает 0 если изменения не произошло, -1, если изменения не получилось (например,
      // у этой аксиомы нет параметров), 1, если изменение произошло
    virtual signed int transmute (const double p) = 0;
    
      // Задает параметры элементарного условия
      // Возвращает 0, если все ok, -1 иначе
    virtual signed int setParams (const std::map<std::string,std::string>& newParams) = 0;
    
      // Задает параметры элементарного условия
      // Возвращает 0, если все ok, -1 иначе
    virtual signed int getParams (std::map<std::string,std::string> &newParams) const = 0;
    
      // Функция устанавливает значение одного параметра элементарного условия,
      // если такой параметр присутствует в данном элементарном условии
   virtual signed int setParamValue (const double param, const std::string &paramName) = 0;
   
      // Функция записывает значение одного параметра элементарного условия,
      // если такой параметр присутствует в данном элементарном условии
   virtual signed int getParamValue (double &param, const std::string &paramName) const = 0;
   
      // Функция заполняет входной вектор строками с названиями параметров данной элементарной аксиомы
   virtual signed int getECParamNames (std::vector<std::string> &ecParamNames) const = 0;
     
      // Проверка совместимости с другим элементарным условием на связку "И"
      // 0 - если не совместима
      // 1 - если совместима
      // -1 - если undefined
    virtual signed int checkAndCompatibility (const ElemCondition& toCheck) const = 0;

      // Скрещивание двух элементарных условий (меняет их состояние)
    void crossEC(ElemCondition& second, const double crossProb, const double crossParam);

	  // Функция устанавливает максимальные значения для границ left и right,
	  // которые определяют окрестность текущей точки в которой проверяется элементарное условие
	virtual signed int setLimits (const int leftLimit, const int rightLimit) = 0;

	  // Функция устанавливает максимальные значения для параметров элементарного условия,
	  // отличных от left и right
	virtual signed int setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) = 0;

	  // Функция изменяет параметры элементарного условия в заданных пределах на число шагов,
	  // указанное в параметрах. Изменение на один шаг - это изменение на одно значение delta.
	  // Значение delta определяется при установке лимитов изменения.
	virtual signed int changeParams (const int numOfSteps) = 0;

	// Функция клонирования - реализация шаблона "виртуальный конструктор копирования"
	virtual ElemCondition * clone() const = 0;

	// Функция присваивает данному ЭУ ЭУ other и возвращает true, если у них совпадает тип
	// Иначе возвращает false
	// В реализации не будут использоваться дорогие операции,
	// такие, как new
	virtual bool assign(const ElemCondition* other) = 0;
	
}; // class ElemCondition

template<class Derived>
bool ElemCondition::assign_impl(Derived* first, const ElemCondition* second) {
	// Кастуем к нашему типу
	const Derived* secondDerived = dynamic_cast<const Derived*>(second);
	if(secondDerived == 0) {
		return false;
	}
	// Дефолтная реализация копирует все поля класса
	*first = *secondDerived;
	return true;	
}

}; // namespace AxiomLib

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AxiomLib::ElemCondition);

#endif /* __AXIOMS_LIB_EL_SUPER_AXIOM_HXX */
