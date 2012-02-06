/****************************************************************************
*					ElemCondPlusStat 
*
*	Description:	Класс описания элементарного условия с заданной статистикой
*					выполнения на траекториях, содержащих траектории нештатного
*					поведения, и на траекториях нормального поведения.
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ElemCondPlusStat_HXX
#define __AXIOMS_LIB_ElemCondPlusStat_HXX

#include "ElemCondPlus.h"

namespace AxiomLib {

class ElemCondPlusStat : public ElemCondPlus {
	
	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ElemCondPlus);
		archive & BOOST_SERIALIZATION_NVP(goal);
		archive & BOOST_SERIALIZATION_NVP(statAbnorm);
		archive & BOOST_SERIALIZATION_NVP(statNormal);
		archive & BOOST_SERIALIZATION_NVP(statOccurence);
		archive & BOOST_SERIALIZATION_NVP(statOccVector);
	}	
	
	void setDefaultStatistics();
	
 public:

	//	Расширение исходного класса ElemCondPlus за счет следующих данных по статистике:

	// Значение целевой функции
    double goal;

	// Частота выполнения на траекториях, содержащих траектории нештатного поведения
	double statAbnorm;

	// Частота выполнения на траекториях нормального поведения
	double statNormal;
	
	// Встречаемость
	double statOccurence;
	
	// Флаги встречаемости для каждой траектории обучающей выборки
	std::vector<bool> statOccVector;


	//	Функции класса:
 
    // Пустой конструктор с заданием параметров по умолчанию
    ElemCondPlusStat (void);  
	
	// Конструктор, создающий объект по ElemCondPlus
	explicit ElemCondPlusStat(const ElemCondPlus &ecPlus);
	
    // Пустой деструктор
    ~ElemCondPlusStat(void);

    // Функция обнуляет значения всех переменных
    void clear (void);
  
	// Оператор присваивания элементарного условия со статистикой
    //ElemCondPlusStat& operator= (const ElemCondPlusStat &second);
	
	// Функция сохранения системы аксиом в файл
	signed int saveECToFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames) const;
	
	// Функция задания аксиомы по данным из структуры с описанием системы аксиом
	signed int initFromFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames);
}; // end of class

}; //  end of namespace

//BOOST_CLASS_EXPORT_GUID(AxiomLib::ElemCondPlusStat, "ElemCondPlusStat");

#endif /* __AXIOMS_LIB_ElemCondPlusStat_HXX */
