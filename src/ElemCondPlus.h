/***************************************************************************
*					ElemCondPlus
*
*	Description:	Структура определяет элементароне условие с отрицанием
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Elem_Cond_Plus_HXX
#define _Elem_Cond_Plus_HXX

#include "./elem_conditions/ElemCondition.h"
#include "all_axioms.h"
#include "AxiomBase.h"
#include "ElemCondPlusStruture.h"
#include <vector>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

BOOST_CLASS_TRACKING(AxiomLib::ElemCondition, boost::serialization::track_never);

namespace AxiomLib {

class ElemCondPlus {

	//Объявления для сериализации (см. документацию к boost::serialization)
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_NVP(elemCondition);
		archive & BOOST_SERIALIZATION_NVP(sign);
		archive & BOOST_SERIALIZATION_NVP(dimension);
		archive & BOOST_SERIALIZATION_NVP(name);
	}
 
 public:

	 // ссылка на элементарное условие
	 ElemCondition* elemCondition;

	 // знак условия (если false то оно идет с отрицанием)
	 bool sign;

	 // размерность набора данных, на которой проверять данное элементарное условие
	 int dimension;
	 
	 // имя конкретного ЭУ
	 std::string name;

	 size_t index;
	 
		 // Функции класса //

	 // конструктор
	 ElemCondPlus(void);

	 // Декструктор
	 ~ElemCondPlus(void);

	 // Конструктор копирования
	 ElemCondPlus(const ElemCondPlus &second);

	 ElemCondPlus operator!() const;

	 // Возвращает имя типа данного ЭУ
	 std::string ecTypeName (void) const;

	 // Вычисление необходимых размерностей датасета
	 void getDimensions(std::vector<bool> &v) const;

	 // Функция проверки выполнимости ЭУ с учетом знака отрицания, если он есть
	 signed int check (const unsigned long k, const std::vector<double>& x) const;
	 
	 // Функция проверки выполнимости ЭУ с учетом отрицания и размерности
	 signed int check (unsigned long k, const std::vector<std::vector<double> > &x) const;

	 // Функция разметки ряда ЭУ
	 unsigned long markUp(const std::vector<double>& x, std::vector<int> &markup) const;

	 // Фнукция обнуляет содержимое данной структуры
	 signed int clear (void);

	 // Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
	 double operator< (const ElemCondPlus& second) const;

	 // Оператор присваивания
	 ElemCondPlus& operator= (const ElemCondPlus& second);

	 // Функция задания аксиомы по данным из структуры с описанием системы аксиом
	 signed int initFromStructure (ElemCondPlusStruture &ecps);

	 // Функция задания аксиомы по данным из структуры с описанием системы аксиом
	 signed int initFromFile (const std::string &fullName);

	 // Функция задания аксиомы по данным из структуры с описанием системы аксиом
	 signed int initFromFile (const std::string baseDir, const std::string ecName);

	 // Функция задания аксиомы по данным из структуры с описанием системы аксиом
	 signed int initFromFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames);

	 // Функция задания аксиомы по данным из структуры с описанием системы аксиом
	 signed int initFromFile (const std::string &filePath, const std::vector <std::string> &paramNames);

	 // Функция сохраняет структуру выбранной аксиомы в AxiomExprStructures
	 signed int createStructure (ElemCondPlusStruture &ecps) const;

	 // Функция сохранения системы аксиом в файл
	 signed int saveECToFile (const std::string baseDir, const std::string ecName) const;

	 // Функция сохранения системы аксиом в файл
	 signed int saveECToFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames) const;

};

}; //  end of namespace

//BOOST_CLASS_EXPORT_GUID(AxiomLib::ElemCondPlus, "ElemCondPlus");

#endif /* _Elem_Cond_Plus_HXX */
