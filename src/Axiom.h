/***************************************************************************
*			Axiom
*
*	Description:	Конъюнкция элементарных аксиом
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_HXX
#define __AXIOMS_LIB_AXIOM_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "AxiomStructure.h"
#include "Round.h"
#include "all_axioms.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class Axiom {

 private:
  
 // Данный друг необходим для проверки совместимости заданных элементарных условий в аксиомах 
    friend class Check;

 protected:
 
  // Вектор, содержит экземпляры элементарных аксиом, входящих в данную аксиому
    std::vector<ElemCondition *> elemConditions;
    
  // Название аксиомы
    std::string nameOfAxiom;

  // Минимальное число элементарных условий в аксиоме
	unsigned int minNumOfEC;

  // Максимальное число элементарных условий в аксиоме
	unsigned int maxNumOfEC;

 public:
	double weight;
 
  // Пустой конструктор: создает нулевой вектор
    Axiom(void);  
  
  // Пустой конструктор: создает нулевой вектор и записывает имя данной аксиомы
    Axiom(const std::string sName);
  
  // Декструктор, уничтожающий все экземпляры объектов
    ~Axiom(void);

  // Название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
    std::string name (void) const;

  // Функция устанавливает значения атрибутов класса
	signed int setAttrValue (const unsigned int min, const unsigned int max);

  // Функция возвращает значения атрибутов класса
	signed int getAttrValue (unsigned int &min, unsigned int &max);

  // Функция возвращает текущеее число элемнтарных условий в данной аксиоме
    inline int size(void) const {return (int) elemConditions.size();};
    inline int getNumOfEC (void) const {return (int) elemConditions.size();};

  // Функция удаляет все динамически созданные элементарные условия
    signed int clear (void);
    
  // Проверяет, размечается ли данной сложной аксиомой точка k ряда ts
  // Возвращает:  1  если размечается
  //              0  если не размечается
  //		 -1  если данная сложная аксиома не применима
    signed int check (const unsigned long k, const std::vector<double>& ts);
  
  // Задает данную аксиому как конъюнкцию элементарных аксиом с номерами, заданными в векторе axiom_nums
  // Эта штука используется для создания "случайных" аксиом
  // При создании используются конструкторы аксиом без параметров
    signed int initAxiomByNums (const std::vector<signed int>& axiomNums);

  // Функция заполенения аксиомы случайным образом элеметраныйми условиями
  // На вход принимается параметр - число элем условий в создаваемой аксиоме
  // в случае - если не удалось произвести заполнение аксиомы таким образом  - функция возвращает -1
    signed int initAxiomByRand (const int numOfEC);

  // Задает данную аксиому как конъюнкцию элементарных аксиом с именами, заданными в векторе axiom_names
  // В принципе, сюда и параметры стоит добавить
  // Эта штука используется для создания фиксированных экспертом аксиом
  // При создании используются конструкторы аксиом без параметров
    signed int initAxiomByNames (const std::vector<std::string>& axiomNames);

  // Инициализация параметров элементарных условий, входящих в аксиому
    signed int setECParameters (const std::map <std::string, std::map <std::string, std::string> >& ecParams);
    
  // Функция возвращает параметры акиомы
    signed int getECParameters (std::map <std::string, std::map <std::string, std::string> > &ecParams) const;
    
  // Функция устанавливает значение определенного параметра определенного элементарного условия
    signed int setParamValue (const double param, const int ecNum, const std::string &paramName);
    
  // Функция возвращает значение определенного параметра определенного элементарного условия
    signed int getParamValue (double &param, const int ecNum, const std::string &paramName) const;
  
  // Функция заполняет входной вектор именами параметров элементарного условия с заданным номером в векторе элементарных условий
    signed int getECParamNames(int ecNum, std::vector<std::string> &ecParamNames) const;
    
  // Функция возвращает номер элементарного условия в векторе условий по его имени
    signed int getECNumByName (std::string &ecName) const;
    
  // Функция заполняет входной вектор именами элементарных условий данной аксиомы
    signed int getECNames(std::vector<std::string> &ecNames) const;

  // Функция сохраняет структуру данной аксиомы в AxiomStructure
	signed int createAxiomStructure (AxiomStructure &as) const;

  // Мутация на уровне параметров элементраных условий, входящих в состав аксиомы (ectp). и на
  // уровне структуры самой аксиомы (параметр atp)
    signed int transmute (const double ectp, const double atp);
      
  // Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
  // По дефолту, четкое сравнение
    double operator< (const Axiom& second);
    
  // Оператор присваивания
    Axiom & operator= (const Axiom& second);
	
	Axiom(const Axiom& other);

  // Проверка слабой структурной идентичности двух аксиом
    signed int checkWeakEquivalence(const Axiom& second) const;
    
};

}; //  end of namespace

#endif /* __AXIOMS_LIB_AXIOM_HXX */
