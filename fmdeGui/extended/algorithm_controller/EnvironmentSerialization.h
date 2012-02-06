/************************************************
*   EnvironmentSerialization.h
*   Объявления в данном файле необходимы для того,
*   чтобы иметь возможность сохранять класс Environment
*   при помощи библиотеки boost_serialization
*
*************************************************/

#ifndef ENVIRONMENTSERIALIZATION_H
#define ENVIRONMENTSERIALIZATION_H

#include <vector>
#include <string>

#include <QString>

#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>

#include "Environment.h"

namespace boost {
	namespace serialization {
		template<class Archive>
		void save(Archive& archive, const AxiomLib::Environment& env, unsigned int /*version*/) {
			std::vector<std::string> paramNames;
			std::set<std::string> values;
			env.getParamNames(paramNames);
			archive << BOOST_SERIALIZATION_NVP(paramNames);
			
			for(auto it = paramNames.begin(); it != paramNames.end(); ++it) {
				values.clear();
				env.getStringSetParamValue(values, *it);
				archive << BOOST_SERIALIZATION_NVP(values);
			}
		}
		
		template<class Archive>
		void load(Archive& archive, AxiomLib::Environment& env, unsigned int /*version*/) {
			env.clear();
			
			std::vector<std::string> paramNames;
			std::set<std::string> values;

			archive >> BOOST_SERIALIZATION_NVP(paramNames);
			
			for(auto it = paramNames.begin(); it != paramNames.end(); ++it) {
				archive >> BOOST_SERIALIZATION_NVP(values);
				env.setParamValues(values, *it);
			}
		}
	}
}

BOOST_SERIALIZATION_SPLIT_FREE(AxiomLib::Environment);


#endif // ENVIRONMENTSERIALIZATION_H
