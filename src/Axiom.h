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
    The function sets this axiom as a conjunction of elementary conditions
    selected randomly. The implementation involves an appeal to a lower
    level - the level of creating the basic conditions - and receiving a
    randomly selected set of compatible elementary conditions, the number
    of elementary conditions is defined as parameter
        @brief Function to fill randomly the axiom with elementary conditions
        Takes the input parameter - the number of elementary conditions in
        created axiom. In case - if you can not make the filling the axiom in
        such way - the function returns -1
        @param numOfEC - the number of elementary conditions that will be
                included in this axiom
        @author dk
        @retval -1 - If creation is fault
        @retval 0 - if all goes well
        @throws AxiomLibExeption - if you can not find numOfEC compatible
        elementary conditions
      */
    signed int initAxiomByRand (const int numOfEC);

  // Задает данную аксиому как конъюнкцию элементарных аксиом с именами, заданными в векторе axiom_names
  // В принципе, сюда и параметры стоит добавить
  // Эта штука используется для создания фиксированных экспертом аксиом
  // При создании используются конструкторы аксиом без параметров
    /*!
        Sets this axiom as a conjunction of elementary axioms with the
        names given in the axiom_names vector. In principle , here parameters
        should be added. This function is used to create axioms which are fixed
        by expert. Constructors of the axioms without parameters are used when
        creating.
        @brief Sets this axiom as a conjunction of elementary axioms with the
        names given in the axiom_names vector. In principle , here parameters
        should be added. This function is used to create axioms which are fixed
        by expert. Constructors of the axioms without parameters are used when
        creating
        @param axiomNums - vector of numbers to appropriate elementary
        conditions that will be added to the axiom.
        @author dk
        @retval 0 - if all is successful
        @retval otherwise - exit from the functions by throw
        @throws AxiomLibException - 1) if it is unable to create some of the
        elementary conditions listed in the list
        2 ) If listed axioms in the input parameter is not compatible
  */

    signed int initAxiomByNames (const std::vector<std::string>& axiomNames);
    /*!
        Initialize the parameters of the elementary conditions included in the
        axiom
        @brief Initialize the parameters of the elementary conditions included
        in the axiom
        @param
        @author dk
        @retval 0 - if at least one parameter was changed
        @retval -2 - in ecParams there are no proper reflection
        @throws AxiomLibExeption - if the ecParams reflection is empty
      */
    signed int setECParameters (const std::map <std::string, std::map <std::string, std::string> >& ecParams);

  // Функция возвращает параметры акиомы
    /*!
        This function returns the parameters of axiom
        @brief This function returns the parameters of axiom
        @param ecParams - filling - sets the reflection of the names elementary
        conditions in the "low-mapping". "Low-mapping" is a reflection of the
        names of the parameters of the elementary conditions in the values ??of
        these parameters. The values of parameters are ??given in the form of
        lines, for convenience (if this are numbers then they are transferred
        by function atof ())
        @author dk
        @return 0
      */
    signed int getECParameters (std::map <std::string, std::map <std::string, std::string> > &ecParams) const;

  // Функция устанавливает значение определенного параметра определенного элементарного условия

    //!
    //! \brief This function sets the value of a specific parameter of certain
    //!  elementary condition
    //! \param param
    //! \param ecNum
    //! \param paramName
    //! \return
    //!
    signed int setParamValue (const double param, const int ecNum, const std::string &paramName);

  // Функция возвращает значение определенного параметра определенного элементарного условия
    //!
    //! \brief This function returns the value of a specific parameter of certain
    //! elementary condition
    //! \param param
    //! \param ecNum
    //! \param paramName
    //! \return
    //!
    signed int getParamValue (double &param, const int ecNum, const std::string &paramName) const;

  // Функция заполняет входной вектор именами параметров элементарного условия с заданным номером в векторе элементарных условий
    /*!
        This function fills the input vector of the parameter names of the
        elementary conditions for a given number in the vector of elementary
        conditions
        @brief This function fills the input vector of the parameter names of the
        elementary conditions for a given number in the vector of elementary
        conditions
        @param ecNum - number of elementary conditions for requesting
                information
        @param EcParamNames - filled vector of strings with the names of the
                parameters
        @author dk
        @return 0
        @throws AxiomLibException - if the specified number of elementary
        condition goes beyond the number of elementary conditions of this axiom
      */
    signed int getECParamNames(int ecNum, std::vector<std::string> &ecParamNames) const;

  // Функция возвращает номер элементарного условия в векторе условий по его имени
    /*!
        This function returns the number of elementary conditions in the
        vector of conditions by its name
        @brief This function returns the number of elementary conditions in the
        vector of conditions by its name
        @param ecName - the name of the elementary conditions
        @author dk
        @retval >=0 - if elementary condition with the specified name was found
        @retval <0 - if elementary condition with the specified name was not found
      */
    signed int getECNumByName (std::string &ecName) const;

  // Функция заполняет входной вектор именами элементарных условий данной аксиомы
    /*!
        This function fills the input vector in the names of the elementary
        conditions
        @brief This function fills the input vector in the names of the
        elementary conditions
        @param ecParamNames - filled with a vector of strings with the names of
               the elementary conditions
        @author dk
        @return a positive integer - the number of elements of the vector
      */

    signed int getECNames(std::vector<std::string> &ecNames) const;

  // Функция сохраняет структуру данной аксиомы в AxiomStructure
    /*!
        This function will save the structure of the system of axioms in
        AxiomStructure
        @brief This function will save the structure of the system of axioms
               in AxiomStructure
        @param as - a structure in which to write the description of the axiom
        @author dk
      */

    signed int createAxiomStructure (AxiomStructure &as) const;

  // Мутация на уровне параметров элементраных условий, входящих в состав аксиомы (ectp). и на
  // уровне структуры самой аксиомы (параметр atp)
    /*!
        A mutation in the parameter level of elementary conditions that are
        part of the axiom (ectp) and at the level of the structure of the
        axioms (parameter atp). A mutation at the level of the elementary
        conditions determines the (usually) a change in the parameters.
        A mutation at the level of axioms assumes replacement, removal or
        addition some cases selected elementary condition.
        @brief Mutation in the parameter level of elementary conditions that
        are part of the axiom (ectp) and in the level of the structure of the
        axioms (parameter atp)
        @param ectp - parameter determines the degree of mutation at the level
              of elementary conditions - the parameter value must vary a 0 to 1
        @param atp - parameter determines the degree of mutation at the level
               of axioms - the parameter value must vary a 0 to 1
        @author dk
        @retval 0 - if the mutation is successful
        @retval otherwise - if something goes wrong - a mutation is just not
                happening
      */



    signed int transmute (const double ectp, const double atp);

  // Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
  // По дефолту, четкое сравнение
    /*!
        Comparison of given axiom with its internal parameters and other axiom
        By default comparison is accurate
        @brief Comparison
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
