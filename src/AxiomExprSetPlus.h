/****************************************************************************
*					AxiomExprSetPlus 
*
*	Description:	Класс описания системы аксиом на основе выражений он 
*					элементарных условий с некоторой статистикой.
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AXIOM_EXPR_SET_PLUS_HXX
#define __AXIOMS_LIB_AXIOM_EXPR_SET_PLUS_HXX


#include "AxiomExprSet.h"
#include <vector>
#include <utility>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>

#include <boost/unordered_map.hpp>

namespace AxiomLib {

class AxiomExprSetPlus : public AxiomExprSet {
	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive &archive, unsigned int /*version*/) {
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AxiomExprSet);
		archive & BOOST_SERIALIZATION_NVP(goal);
		archive & BOOST_SERIALIZATION_NVP(errFirst);
		archive & BOOST_SERIALIZATION_NVP(errSecond);
		archive & BOOST_SERIALIZATION_NVP(axiomsIndex);
		archive & BOOST_SERIALIZATION_NVP(markUps);
		archive & BOOST_SERIALIZATION_NVP(multiMarkUps);
		archive & BOOST_SERIALIZATION_NVP(errors);
	}

	boost::unordered_map<int, boost::unordered_map<int, std::pair<int, int> > > detailedErrors;

 public:
	
	void addAxiom(const AxiomExpr& newAxiom);

	// Добавляет аксиому с заданным приоритетом (от 1) и обновляет разметки
	void addAxiom(const AxiomExpr &newAxiom, int priority);

	// Удаляет аксиому из системы по номеру (от 1),
	// обновляя значения в разметке
	void removeAxiom(int axiomNumber);

	bool operator <(const AxiomExprSetPlus& other) const;

	//	Расширение исходного класса AxiomExprSet за счет следующих переменных:

	// Значение целевой функции
    double goal;

	// Число ошибок I рода
	int errFirst;

	// Число ошибок II рода
	int errSecond;

	// Индексы входящих в систему аксиом в некотором представлении
	std::vector <unsigned int> axiomsIndex;

	// Вектора разметок для различных типов нештатного поведения
	std::vector < std::vector <int> > markUps;
	
	// Разметки подмножествами для каждого класса нештатного поведения
	std::vector < std::vector <std::vector<bool> > > multiMarkUps;
	
	// Ошибки первого и второго рода для каждого типа нештатного поведения
	std::vector<std::pair<unsigned int, unsigned int> > errors;


	//	Функции класса:
 
    // Пустой конструктор с заданием имени по default'у
    AxiomExprSetPlus(void);  
  
    // Пустой конструктор с заданием имени в параметре
    AxiomExprSetPlus(const std::string sName);
	
	AxiomExprSetPlus(const AxiomExprSet& as);
 
    // Деструктор удаляющий все созданные в рамках данной системы аксиом динамические объекты
    ~AxiomExprSetPlus(void);

	void setErrorsForTraj(int type, int trajNo, int errI, int errII);

	std::pair<int, int> getErrorsForTraj(int type, int trajNo) const;
 
    // Функция удаления всех аксиом из системы аксиом
    void clear (void);
  
	// Оператор присваивания системы аксиом
	// В явном виде его описывать в данном классе нет необходимости
    //AxiomExprSetPlus& operator= (const AxiomExprSetPlus &second);
	
	// Функция задания системы аксиом строго определенным образом (по конфигурации из файла)
    int initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName, const std::vector<std::string> &paramNames);
	
	// Функция сохранения системы аксиом в файл (с указанием размерости исходных данных для каждого элементарного условия)
	int saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, 
							const std::vector <std::string> &dataSetParams, int first = -1, int second = -1) const;
        
}; // end of class

}; //  end of namespace

//BOOST_CLASS_EXPORT_GUID(AxiomLib::AxiomExprSetPlus, "AxiomExprSetPlus");

#endif /* __AXIOMS_LIB_AXIOM_EXPR_SET_PLUS_HXX */
