/***************************************************************************
*					AxiomExpr
*
*	Description:	Аксиома в виде выражения от элементарных условий
*					(с дизъюнкциями, конъюнкциями и отрицаниями)
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Axiom_Expr_HXX
#define _Axiom_Expr_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemCondPlus.h"
#include "AxiomBase.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "AxiomExprStructure.h"
#include "Round.h"
#include "all_axioms.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace AxiomLib {

/*! AxiomExpr
   @class     AxiomExpr
   @brief     Class for axiom
   @author    dk

   Class AxiomExpr store axiom as expression of elementary conditions
   ( alternation, conjunction, negation )
 */
class AxiomExpr {

 private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &archive, const unsigned int /*version*/) {
        archive & BOOST_SERIALIZATION_NVP(nameOfAxiomExpr);
        archive & BOOST_SERIALIZATION_NVP(nameOfECBank);
        archive & BOOST_SERIALIZATION_NVP(expression);
    }

        // Данный друг необходим для проверки совместимости заданных элементарных условий в аксиомах
        /*! This friend is needed for checking compatibility input elementary
        conditions in axioms
        */
        friend class Check;

    // Устанавливает дефолтное имя для экземпляра класса
        /// Determine default name for direct instance of a class
        /// @return signed int - return 0 if determinition is ok
    signed int setDefaultName (void);

 protected:

 public:

    // Название аксиомы
        /// name of axiom
    std::string nameOfAxiomExpr;

    // Имя используемого банка элементарных условий
        /// name of bank of elementary conditions
    std::string nameOfECBank;

    // Матрица ЭУ - выражение над ЭУ. Условия в столбцах связаны конъюнкциями, столбцы между собой - дизъюкциями.
    // Отрицания в структуре элементов матрицы.
        /*! Matrix of elementary conditions - the expression above elementary
            condition. Conditions in columns are associated with conjunction,
            Columns are associated with alternation.
            Negations are in matrix structure

        */
    std::vector< std::vector <ElemCondPlus> > expression;

        // Функции класса: //

    int index;

    // Конструктор: не создает выражение для аксиомы
        /*!
            Constructor. Constructor doesn't create expression for axiom
            Empty constructor with creation name by default.
         */
    AxiomExpr(void);

    // Конструктор: не создает выражение для аксиомы, записывает новое имя аксиомы
        /*!
            Constructor. Constructor doesn't create expression for axiom.
            Constructor record new axiom name.
            @param sName - new axiom name
         */
    AxiomExpr(const std::string sName);

    /*!
        Constructor. Constructor creates expression for axiom.
        @param ec - elementary conditions
     */
    AxiomExpr(const ElemCondPlus& ec);

    // Декструктор, уничтожающий все экземпляры объектов
        /*!
          Class destructor
          destroys each direct instances,
          delete each dynamic generated objects
          */
    ~AxiomExpr(void);

    AxiomExpr *clone() const { return new AxiomExpr(*this); }

    // Возвращает название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
        /*!
          Name of axiom is irrespective of each parameters.
          Name of axiom is used for definition of axiom library.
          @return std::string - name of axiom
          */
    std::string name (void) const;

    // Функция возвращает текущеее число элемнтарных условий в данной аксиоме
        /*!
          Function returns current number of elementary conditions in given
          axiom.
          @return signed int - current number of elementary conditions
         */
    signed int size(void) const;
    /*!
      Function returns current number of elementary conditions in given
      axiom.
      @return signed int - current number of elementary conditions
     */
    signed int getNumOfEC (void) const;

    // Функция удаляет все элементарные условия и очищает содержимое аксиомы
    /*!
      Function deletes each of elementary conditions in given
      axiom.
      @return signed int - current number of elementary conditions
     */
    signed int clear (void);

    // Проверяет, размечается ли данной сложной аксиомой точка k ряда ts (для одномерного и многомерного рядов)
    // Возвращает:  1  если размечается
    //              0  если не размечается
    //				-1  если данная сложная аксиома не применима
    /*!
      Function checks for marking point in row by given compound axiom.
      (for one-dimensional rows)
      @author    dk
      @param     k -    point in checking row
      @param     ts -   time row
      @return signed int - return 1 if marking; 0 - not marking;
                                -1 if given axiom does not apply

     */
    signed int check (const unsigned long k, const std::vector<double>& ts) const;


    /*!
      Function checks for marking point in row by given compound axiom.
      (for several-dimensional rows)
      @author    dk
      @param     k -    point in checking row
      @param     ts -   time row
      @return signed int - return 1 if marking; 0 - not marking;
                                -1 if given axiom does not apply

     */
    signed int check (const unsigned long k, const std::vector < std::vector<double> >& ts) const;

    // Инициализация параметров элементарных условий, входящих в аксиому
    //signed int setECParameters (const std::map <std::string, std::map <std::string, std::string> >& ecParams);

    // Функция возвращает параметры акиомы
    //signed int getECParameters (std::map <std::string, std::map <std::string, std::string> > &ecParams) const;

    // Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
    // По дефолту, четкое сравнение


    /*!
      Comparison given axiom with its internal parameters with another axiom.
      Comparison is fair by default.
      @author    dk
      @param     second - second comparing axiom
      @return double - real number from -1 to 1
      As the number is near 1, it could be certainty said first axiom is
      embedded in second axiom
      (i.e. second axiom is greater than the first one).
      If number is -1 then first axiom is greater than the second one.
      If number is 0 then axioms are congruent or not congruent with each other
     */
    double operator< (const AxiomExpr& second);

    // Оператор присваивания
    // В явном виде для данного класса его описание не требуется
    // AxiomExpr & operator= (const AxiomExpr& second);

        // Слитие аксиом по оператору И

    /*!
      Merging axioms by AND operator
      @author    dk
      @param     second - second axiom in axiom merging
      @return AxiomExpr - new axiom as merging *this and second
     */
    AxiomExpr operator& (const AxiomExpr& second) const;

    // Слитие аксиом по оператору ИЛИ
        /*!
          Merging axioms by OR operator
          @author    dk
          @param     second - second axiom in axiom merging
          @return AxiomExpr - new axiom as merging *this and second
         */
    AxiomExpr operator| (const AxiomExpr& second) const;

    AxiomExpr& operator &=(const AxiomExpr& second);

    AxiomExpr& operator |=(const AxiomExpr& second);

    // Слитие аксиом по оператору И в данную аксиому
        /*!
          Merging axioms by AND operator in given axiom
          @author    dk
          @param     first -  first axiom in axiom merging
          @param     second - second axiom in axiom merging
          @return    0
         */
    signed int andExpr (const AxiomExpr& first, const AxiomExpr& second);

    // Слитие аксиом по оператору ИЛИ в данную аксиому
        /*!
          Merging axioms by OR operator in given axiom
          @author    dk
          @param     first -  first axiom in axiom merging
          @param     second - second axiom in axiom merging
          @return    0
         */
    signed int orExpr (const AxiomExpr& first, const AxiomExpr& second);

    // Вычисление необходимых размерностей датасета
        /*!
          Calculation of dataset dimensions, which are needed for current axiom
          @author    wictor
          @param     v      - True is writing on place of dimensions, which are
          using. If vector size is insufficient, than vector size doesn't
          increase(i.e it needs to set right vector size in advance).
         */
    void getDimensions(std::vector<bool> &v) const;

    // Проверка слабой структурной идентичности двух аксиом
        /*!
          Checking for weak structure equivalence for two axioms.
          Sets of elementary conditions are equal.
          @author    dk
          @param     second - second comparing axiom
          @retval    0 no weak structure equivalence
          @retval    1 weak structure equivalence
         */
    signed int checkWeakEquivalence(const AxiomExpr& second) const;

    // Функция сохраняет структуру выбранной аксиомы в AxiomExprStructures
    /*!
      Saving structure of choosing axiom in AxiomExprStructures
      @author    dk
      @param     aes - filling axiom structure field
      @return    0
     */
    signed int createAxiomExprStructure (AxiomExprStructure &aes) const;

    // Функция заполенения аксиомы случайным образом элеметраныйми условиями
    // На вход принимается параметр - число элем условий в создаваемой аксиоме
    // в случае - если не удалось произвести заполнение аксиомы таким образом  - функция возвращает -1
        /*!
          Function defines current axiom as expression above random
          elementary conditions. Realization assumes adress to lower layer -
          a layer of elementary conditions creation - and getting random
          selected sets of compatible elementary conditions. Number of
          elementary conditions define as parameter.
          @author    dk
          @param     numOfEC - number of elementary conditions, which will be
                     part of current axiom
          @throws    AxiomLibExeption - if it is impossible to select numOfEC
                     compatible elementary conditions
          @retval    0  creation ends with success
          @retval    -1  creation faults
         */
    signed int initAxiomByRand (const int numOfEC);

    // Случайное образование аксиомы с указанием размерности набора данных для каждого условия из заданного диапазона
    /*!
      Function defines current axiom as expression above random
      elementary conditions. For each condition dimension of data set
      (with which condition will be test) is defined in random from
      specified range. Realization assumes adress to lower layer -
      a layer of elementary conditions creation - and getting random
      selected sets of compatible elementary conditions. Number of
      elementary conditions define as parameter.
      @author    dk
      @param     numOfEC - number of elementary conditions, which will be
                 part of current axiom
      @throws    AxiomLibExeption - if it is impossible to select numOfEC
                 compatible elementary conditions
      @retval    0  creation ends with success
      @retval    -1  creation faults
     */
    signed int initAxiomByRand (const int numOfEC, const int dimensionMin, const int dimensionMax);

    // Функция задания аксиомы по данным из структуры с описанием системы аксиом
    /*!
      Function for definition of axiom from dedicated structure with
      axiom description
      @author    dk
      @param     aes - structure with axiom description
      @throws    AxiomLibExeption - if name of one of the elementary conditions
                 is wrong
      @return    0
     */
    signed int initAxiomFromStructure (AxiomExprStructure &aes);

    // Функция задания аксиомы из файла
    /*!
      Function for definition of axiom from file
      @author    dk
      @param     baseDir - directory, where askable file of description
                 of axioms is
      @param     AxiomSetName - name for saving axiom
      @return    0
     */
    signed int initAxiomFromFile (const std::string baseDir, const std::string axiomName);

    // Функция задания аксиомы из файла (с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
    /*!
      Function for definition of axiom from file.
      ( With indication of dimension of data set, on which each elementary
        conditions from current axiom is checking)
      @author    dk
      @param     baseDir - directory, where askable file of description
                 of axioms is
      @param     AxiomSetName - name for saving axiom
      @param     paramNames - vector of parameters of data set for indication
                 of dimension, on which each elementary conditions need to check
      @return    0
     */
    signed int initAxiomFromFile (const std::string baseDir, const std::string axiomName, const std::vector <std::string> &paramNames);

    // Функция сохранения системы аксиом в файл
    /*!
      Function for saving axioms in file
      @author    dk
      @param     baseDir - directory, where askable file of description
                 of axioms is
      @param     AxiomSetName - name for saving axiom
      @return    0
     */
    signed int saveAxiomToFile (const std::string baseDir, const std::string axiomName) const;

    // Функция сохранения системы аксиом в файл (с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
    /*!
      Function for saving axioms in file
      @author    dk
      @param     baseDir - directory, where askable file of description
                 of axioms is
      @param     AxiomSetName - name for saving axiom
      @param     paramNames - vector of parameters of data set for indication
                 of dimension, on which each elementary conditions need to check
      @return    0
     */
    signed int saveAxiomToFile (const std::string baseDir, const std::string axiomName, std::vector <std::string> &paramNames) const;

};

}; //  end of namespace

#endif /* _Axiom_Expr_HXX */
