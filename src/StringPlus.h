/****************************************************************************
*						StringPlus
*
*	Description:	Описание структуры, содержащей строку и набор параметров,
*					некоторым образом с ней связанным
*	Author:			dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_StringPlus_HXX
#define __AXIOMS_LIB_StringPlus_HXX

#include <vector>
#include <string>

#include <boost/serialization/nvp.hpp>

namespace AxiomLib {

struct StringPlus {
private:
	//объявления, необходимые для сериализации
	friend class boost::serialization::access;
	//функция сериализации - в ней должны быть указаны члены-данные,
	//характеризующие состояние объекта
	template<class Archive>
	void serialize(Archive & archive, const unsigned int version)
	{
		archive & BOOST_SERIALIZATION_NVP(str);
		archive & BOOST_SERIALIZATION_NVP(doubleVal);
	}
public:
	
	// Строка
	std::string str;
	// Числовая переменная
	double doubleVal;
	
}; // end of struct

}; // end of namespace

#endif /* __AXIOMS_LIB_StringPlus_HXX */
