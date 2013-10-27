/***************************************************************************
*			Axiom
*
*	Description:	Конъюнкция элементарных аксиом
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/

/*!
  @file Axiom.h
  @author Eugene A. Vasin
  Сonjunction of elementary axioms
 */

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

/*!
   @class     Axiom
   @brief     Class for axiom
   @author    Eugene A. Vasin
 */
class Axiom {

 private:

 // Данный друг необходим для проверки совместимости заданных элементарных условий в аксиомах
    /*!
      @brief Friend is needed for checking compatibility of given elemetary
             conditions in axioms
      */
    friend class Check;

 protected:

  // Вектор, содержит экземпляры элементарных аксиом, входящих в данную аксиому
    /// Vector contains instances of elemetary axioms, which are part of given axiom
    std::vector<ElemCondition *> elemConditions;

  // Название аксиомы
    /// Name of axiom
    std::string nameOfAxiom;

  // Минимальное число элементарных условий в аксиоме
    /// Minimal number of elemetary conditions in axiom
    unsigned int minNumOfEC;
/// Maximal number of elemetary conditions in axiom
  // Максимальное число элементарных условий в аксиоме
    unsigned int maxNumOfEC;

 public:

  // Пустой конструктор: создает нулевой вектор
        /*!
          @brief Empty constructor: creates null vector
          Empty constructor with determinition name of axiom as no_name
          */
    Axiom(void);

  // Пустой конструктор: создает нулевой вектор и записывает имя данной аксиомы
    /*!
      Empty constructor with definition name of axiom as input parameter
      @brief Empty constructor: creates null vector and writes name of given axiom
      @param sName - name of given axiom
      */
    Axiom(const std::string sName);

  // Декструктор, уничтожающий все экземпляры объектов
    /*!
      @brief destructor - deletes each dynamic generated objects
      */
    ~Axiom(void);

  // Название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
    /*!
      Getting name of axiom. Name is independently of parameters.
      Name is using for axiom library determition.
      @return std::string - name of axiom
      */
    std::string name (void) const;

  // Функция устанавливает значения атрибутов класса
    /*!
      Function sets values of class attributes
      @return 0 - if success
      */
    signed int setAttrValue (const unsigned int min, const unsigned int max);

  // Функция возвращает значения атрибутов класса
    /*!
      Function returns values of class attributes
      @return 0 - if success
      */
    signed int getAttrValue (unsigned int &min, unsigned int &max);

  // Функция возвращает текущеее число элемнтарных условий в данной аксиоме
    /*!
        Function returns current number of elementary conditions in given axiom
        @return int - number of elementary conditions in given axiom
      */
    inline int size(void) const {return (int) elemConditions.size();};
    inline int getNumOfEC (void) const {return (int) elemConditions.size();};

  // Функция удаляет все динамически созданные элементарные условия
    /*!
      Fuction deletes each dynamic created elementary conditions
      @brief  Fuction deletes each dynamic created elementary conditions
      @author dk
      */
    signed int clear (void);

  // Проверяет, размечается ли данной сложной аксиомой точка k ряда ts
  // Возвращает:  1  если размечается
  //              0  если не размечается
  //		 -1  если данная сложная аксиома не применима
    /*!
      Checks for marking point k in row ts by given compound axiom
      @author dk
      @param
      @param     k -    point in checking row
      @param     ts -   time row
      @retval 1  point is marking
      @retval 0  point is not marking
      @retval -1 if given axiom does not apply
      */
    signed int check (const unsigned long k, const std::vector<double>& ts);

  // Задает данную аксиому как конъюнкцию элементарных аксиом с номерами, заданными в векторе axiom_nums
  // Эта штука используется для создания "случайных" аксиом
  // При создании используются конструкторы аксиом без параметров
    /*!

      Sets given axiom as conjunction of elementary conditions with numbers
      from vector axiom_nums. Numbers correspond to position of elementary
      condition in array of each used elementary conditions.
      This is used for creation of "random" axioms. Constructors of elementary
      conditions without parameters are used in creation.
      @author dk
      @param axiomNums - vector of numbers correspond to elementary conditions
                         which will be added in axiom.
      @return 0 if success. Otherwise - exit and throw exception
      @throw AxiomLibException 1. If creation one of elementary conditions
      failed. 2. If listed axioms from input parameters are not compatibility
      */
    signed int initAxiomByNums (const std::vector<signed int>& axiomNums);

  // Функция заполенения аксиомы случайным образом элеметраныйми условиями
  // На вход принимается параметр - число элем условий в создаваемой аксиоме
  // в случае - если не удалось произвести заполнение аксиомы таким образом  - функция возвращает -1
    /*!

/****************************************************************************
*				Axiom::initAxiomByRand
*
*	Description:	Функция задает данную аксиому как конъюнкцию элементарных условий выбранных случайным образом
*				Реализауия предполагает обращение к более низкому уровню - уровню создания элементарных условий -
*				и получение случайно выбранного набора совместимых элементарных условий,
*				число элементарных условий при этом задается как параметр
*	Parameters:	numOfEC - число элементарных условий, которые будут входить в данную аксиому
*	Returns:		-1 - Если  создать не удалось
*				 0 - Если все прошло нормально
*	Throws:		AxiomLibExeption - если невозможно подобрать numOfEC совместимых элементарных условий
*	Author:		dk
*	History:
*
****************************************************************************/

      */
    signed int initAxiomByRand (const int numOfEC);

  // Задает данную аксиому как конъюнкцию элементарных аксиом с именами, заданными в векторе axiom_names
  // В принципе, сюда и параметры стоит добавить
  // Эта штука используется для создания фиксированных экспертом аксиом
  // При создании используются конструкторы аксиом без параметров
    /*!

/****************************************************************************
*				Axiom::initAxiomByNames
*
*	Description:	Задает данную аксиому как конъюнкцию элементарных аксиом с именами, перечисленными в векторе axiom_nums.
*				Эта штука используется для создания экспертом аксиом. При создании используются конструкторы элементарных условий  без параметров
*	Parameters:	axiomNums - вектор номеров соответсвующих элементарным условиям, которые будут добавлены в аксиому.
*	Returns:		0 - если все выполнено успешно, в противном случае - выход из функции по throw
*	Throws:		AxiomLibException -  1)если не удалось создать некоторое из элементарных условий, перечисленных в списке
*								2)если перчисленные во входном параметре аксиомы не совместимы
*	Author:		dk
*	History:
*
****************************************************************************/

      */
    signed int initAxiomByNames (const std::vector<std::string>& axiomNames);

  // Инициализация параметров элементарных условий, входящих в аксиому
    /*!

/****************************************************************************
*				Axiom::setECParameters
*
*	Description:	Инициализация параметров элементарных условий, входящих в аксиому
*	Parameters:	setECParameters - задает отображение из имен элементарныз условий в "низкое отображение"
*				"низкое отображение" - это отобрадение из имен параметров элементарных условий в значения этих параметров
*				Значения параметров задаются в виде строк, для удобства ( если это числа то они переводятся функцией atof() )
*	Returns:		0  - если хотя бы один парамтр был изменен
*				-2 - в ecParams не нашлось ни одного подхдяшего отображения
*	Throws:		AxiomLibExeption - если отображение ecParams пусто
*	Author:		dk
*	History:
*
****************************************************************************/

      */
    signed int setECParameters (const std::map <std::string, std::map <std::string, std::string> >& ecParams);

  // Функция возвращает параметры акиомы
    /*!

/****************************************************************************
*				Axiom::getECParameters
*
*	Description:	Функция возвращает параметры акиомы
*	Parameters:	ecParams - заполняется - задает отображение из имен элементарныз условий в "низкое отображение"
*				"низкое отображение" - это отобрадение из имен параметров элементарных условий в значения этих параметров
*				Значения параметров задаются в виде строк, для удобства ( если это числа то они переводятся функцией atof() )
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/


      */
    signed int getECParameters (std::map <std::string, std::map <std::string, std::string> > &ecParams) const;

  // Функция устанавливает значение определенного параметра определенного элементарного условия
    /*!
      // Функция устанавливает значение определенного параметра определенного элементарного условия
      */
    signed int setParamValue (const double param, const int ecNum, const std::string &paramName);

  // Функция возвращает значение определенного параметра определенного элементарного условия
    /*!
      // Функция возвращает значение определенного параметра определенного элементарного условия

      */
    signed int getParamValue (double &param, const int ecNum, const std::string &paramName) const;

  // Функция заполняет входной вектор именами параметров элементарного условия с заданным номером в векторе элементарных условий
    /*!

/****************************************************************************
*				Axiom::getECParamNames
*
*	Description:	Функция заполняет входной вектор именами параметров элементарного
*				условия с заданным номером в векторе элементарных условий
*	Parameters:	ecNum - номер элементарного условия, о котором запрашивается информация
*				ecParamNames - заполняемый вектор строк с названиями параметров
*	Returns:		0
*	Throws:		AxiomLibException - если номер указанного элементарного условия
*				выходит за границы числа элементарных условий данной аксиомы
*	Author:		dk
*	History:
*
****************************************************************************/


      */
    signed int getECParamNames(int ecNum, std::vector<std::string> &ecParamNames) const;

  // Функция возвращает номер элементарного условия в векторе условий по его имени
    /*!
/****************************************************************************
*				Axiom::getECNumByName
*
*	Description:	Функция возвращает номер элементарного условия в векторе условий по его имени
*	Parameters:	ecName - имя элементарного условия
*	Returns:		>= 0 - если эелементарное условие с заданным именем нашлось
*				< 0 - если элементарного условия с заданным именем не нашлось
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/


      */
    signed int getECNumByName (std::string &ecName) const;

  // Функция заполняет входной вектор именами элементарных условий данной аксиомы
    /*!

/****************************************************************************
*				Axiom::getECNames
*
*	Description:	Функция заполняет входной вектор именами элементарных условий данной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор строк с названиями элементарных условий
*	Returns:		положительное целое - число элементов вектора
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/

      */
    signed int getECNames(std::vector<std::string> &ecNames) const;

  // Функция сохраняет структуру данной аксиомы в AxiomStructure
    /*!

 /****************************************************************************
*					Axiom::createAxiomStructure
*
*	Description:	Функция сохраняет структуру данной системы аксиом в AxiomStructure
*	Parameters:		as - структура, в которую записывает описание акисомы
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/

      */
    signed int createAxiomStructure (AxiomStructure &as) const;

  // Мутация на уровне параметров элементраных условий, входящих в состав аксиомы (ectp). и на
  // уровне структуры самой аксиомы (параметр atp)
    /*!

 /****************************************************************************
*				Axiom::transmute
*
*	Description:	Мутация на уровне параметров элементраных условий, входящих в состав аксиомы (ectp)
*				и на уровне структуры самой аксиомы (параметр atp)
*				Мутация на уровне элементарного условия определяет (как правило) некоторое изменение параметров
*				Мутация на уровне аксиомы пердполагает замену, удаление или добавлние некоторого случано выбранного элементарного условия
*	Parameters:	ectp - параметр определяет степень мутации на уровне элементарных условий - значения параметра обязаны варьироваться то 0 до 1
*				atp  - параметр определяет степень мутации на уровне аксиомы - значения параметра обязаны варьироваться то 0 до 1
*	Returns:		0 - если мутация прошла успешно, в противном случае - если что-то идет не так - мутации просто не происходит
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/


      */
    signed int transmute (const double ectp, const double atp);

  // Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
  // По дефолту, четкое сравнение
    /*!
        Comparison of given axiom with its internal parameters and other axiom
        By default comparison is accurate
        @brief Assignment operator
        @param second - second axiom to be compared
        @author dk
        @return double - real number from -1 to 1
        As the number is near 1, it could be certainty said first axiom is
        embedded in second axiom
      */
    double operator< (const Axiom& second);

  // Оператор присваивания
    /*!
        Assignment operator
        @brief Assignment operator
        @param second - axiom to be assignmented
        @author dk
        @return Axiom
      */
    Axiom & operator= (const Axiom& second);

    Axiom(const Axiom& other);

  // Проверка слабой структурной идентичности двух аксиом
    /*!
      Checking for weak structural identity of two axioms:
      sets of elemetary conditions are congruent. There is no checking for
      ligament, because AND ligament is not supposed for the time of being.
      @brief Checking for weak structural identity of two axioms
      @author gevor
      @param  second - axiom to be examined for comparison
      @retval 0 there is no structural identity
      @retval 1 there is structural identity
      */
    signed int checkWeakEquivalence(const Axiom& second) const;

};

}; //  end of namespace

#endif /* __AXIOMS_LIB_AXIOM_HXX */
