/****************************************************************************
*					AxiomExprPlus 
*
*	Description:	Класс описания аксиомы на основе выражений от 
*					элементарных условий с некоторой статистикой.
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_EXPR_PLUS_HXX
#define __AXIOMS_LIB_AXIOM_EXPR_PLUS_HXX

#include "AxiomExpr.h"
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/extended_type_info.hpp>

namespace AxiomLib {

class AxiomExprPlus : public AxiomExpr {
	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive &archive, const unsigned int /*version*/) {
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AxiomExpr);
		archive & BOOST_SERIALIZATION_NVP(statNormal);
		archive & BOOST_SERIALIZATION_NVP(statAbnorm);
		archive & BOOST_SERIALIZATION_NVP(statVariation);
		archive & BOOST_SERIALIZATION_NVP(goal);
		archive & BOOST_SERIALIZATION_NVP(statOccurence);
	}
	
	void setDefaultStatistics();
	
 public:

	//	Расширение исходного класса AxiomExpr за счет следующих переменных:
	// Значение целевой функции
    double goal;

	// Частота выполнения на траекториях, содержащих траектории нештатного поведения
	double statAbnorm;

	// Частота выполнения на траекториях нормального поведения
	double statNormal;
	
	// Стандартное отклонение частот встречаемости данной аксиомы
	// на различных траекториях типа нештатного поведения,
	// к которому применяется данная аксиома
	double statVariation;
	
	// Отношение числа траекторий, соответствующих нештатному поведению, на которых 
	// аксиома выполняется, к общему числу траекторий, соответствующих нештатному
	// поведению
	double statOccurence;

		// Функции класса:
 
    // Пустой конструктор с заданием имени по default'у
    AxiomExprPlus(void);  
  
    // Пустой конструктор с заданием имени в параметре
    AxiomExprPlus(const std::string sName);
	
	explicit AxiomExprPlus(const ElemCondPlus& ec);
	
	explicit AxiomExprPlus(const AxiomExpr& ax);
 
    // Деструктор удаляющий все созданные в рамках данной системы аксиом динамические объекты
    ~AxiomExprPlus(void);

    // Функция очищения содержимого переменных данного объекта
    void clear (void);
  
	// Оператор присваивания системы аксиом
	// В явном виде для данного класса он не требуется
    // AxiomExprPlus& operator= (const AxiomExprPlus &second);
	
	// Функция сохранения системы аксиом в файл (с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
	signed int saveAxiomToFile (const std::string& baseDir, const std::string& axiomName, const std::vector <std::string> &paramNames) const;
	
	// Функция задания аксиомы из файла (с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
	signed int initAxiomFromFile (const std::string baseDir, const std::string axiomName, const std::vector <std::string> &paramNames);
	

}; // end of class

}; //  end of namespace

//BOOST_CLASS_EXPORT_GUID(AxiomLib::AxiomExprPlus, "AxiomExprPlus");

#endif /* __AXIOMS_LIB_AXIOM_EXPR_PLUS_HXX */
