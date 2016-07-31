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
    friend class Check;

	// Устанавливает дефолтное имя для экземпляра класса
	signed int setDefaultName (void);

 protected:

 public:

	// Название аксиомы
    std::string nameOfAxiomExpr;

	// Имя используемого банка элементарных условий
    std::string nameOfECBank;

	// Матрица ЭУ - выражение над ЭУ. Условия в столбцах связаны конъюнкциями, столбцы между собой - дизъюкциями.
	// Отрицания в структуре элементов матрицы.
	std::vector< std::vector <ElemCondPlus> > expression;
	
		// Функции класса: //

	int index;
	
	// Конструктор: не создает выражение для аксиомы
    AxiomExpr(void);  
  
	// Конструктор: не создает выражение для аксиомы, записывает новое имя аксиомы
    AxiomExpr(const std::string sName);

	AxiomExpr(const ElemCondPlus& ec);
  
	// Декструктор, уничтожающий все экземпляры объектов
    ~AxiomExpr(void);

	AxiomExpr *clone() const { return new AxiomExpr(*this); }

	// Возвращает название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
    std::string name (void) const;
	
	// Функция возвращает текущеее число элемнтарных условий в данной аксиоме
    signed int size(void) const;
    signed int getNumOfEC (void) const;

	// Функция удаляет все элементарные условия и очищает содержимое аксиомы
    signed int clear (void);
    
	// Проверяет, размечается ли данной сложной аксиомой точка k ряда ts (для одномерного и многомерного рядов)
	// Возвращает:  1  если размечается
	//              0  если не размечается
	//				-1  если данная сложная аксиома не применима
    signed int check (const unsigned long k, const std::vector<double>& ts) const;
	signed int check (const unsigned long k, const std::vector < std::vector<double> >& ts) const;

	// Инициализация параметров элементарных условий, входящих в аксиому
    //signed int setECParameters (const std::map <std::string, std::map <std::string, std::string> >& ecParams);
    
	// Функция возвращает параметры акиомы
    //signed int getECParameters (std::map <std::string, std::map <std::string, std::string> > &ecParams) const;
  
	// Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
	// По дефолту, четкое сравнение
    double operator< (const AxiomExpr& second);
    
	// Оператор присваивания
	// В явном виде для данного класса его описание не требуется
    // AxiomExpr & operator= (const AxiomExpr& second);

	// Слитие аксиом по оператору И
	AxiomExpr operator& (const AxiomExpr& second) const;

	// Слитие аксиом по оператору ИЛИ
	AxiomExpr operator| (const AxiomExpr& second) const;

	AxiomExpr& operator &=(const AxiomExpr& second);

	AxiomExpr& operator |=(const AxiomExpr& second);

	// Слитие аксиом по оператору И в данную аксиому
	signed int andExpr (const AxiomExpr& first, const AxiomExpr& second);

	// Слитие аксиом по оператору ИЛИ в данную аксиому
	signed int orExpr (const AxiomExpr& first, const AxiomExpr& second);

	// Вычисление необходимых размерностей датасета
	void getDimensions(std::vector<bool> &v) const;
	
	// Проверка слабой структурной идентичности двух аксиом
    signed int checkWeakEquivalence(const AxiomExpr& second) const;

	// Функция сохраняет структуру выбранной аксиомы в AxiomExprStructures
	signed int createAxiomExprStructure (AxiomExprStructure &aes) const;

	// Функция заполенения аксиомы случайным образом элеметраныйми условиями
	// На вход принимается параметр - число элем условий в создаваемой аксиоме
	// в случае - если не удалось произвести заполнение аксиомы таким образом  - функция возвращает -1
    signed int initAxiomByRand (const int numOfEC);

	// Случайное образование аксиомы с указанием размерности набора данных для каждого условия из заданного диапазона
	signed int initAxiomByRand (const int numOfEC, const int dimensionMin, const int dimensionMax);

	// Функция задания аксиомы по данным из структуры с описанием системы аксиом
	signed int initAxiomFromStructure (AxiomExprStructure &aes);

	// Функция задания аксиомы из файла
	signed int initAxiomFromFile (const std::string baseDir, const std::string axiomName);

	// Функция задания аксиомы из файла (с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
	signed int initAxiomFromFile (const std::string baseDir, const std::string axiomName, const std::vector <std::string> &paramNames);

	// Функция сохранения системы аксиом в файл
	signed int saveAxiomToFile (const std::string baseDir, const std::string axiomName) const;

	// Функция сохранения системы аксиом в файл (с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
	signed int saveAxiomToFile (const std::string baseDir, const std::string axiomName, std::vector <std::string> &paramNames) const;

	std::string toString() const;
    
};

}; //  end of namespace

std::ostream& operator<<(std::ostream& ostr, const AxiomLib::AxiomExpr& axiomExpr);

#endif /* _Axiom_Expr_HXX */
