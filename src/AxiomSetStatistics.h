#ifndef AXIOMSETSTATISTICS_H
#define AXIOMSETSTATISTICS_H

#include "AxiomExprSetPlus.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace AxiomLib {

class AxiomSetStatistics
{
	friend class boost::serialization::access;
	
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
	
	// Число ошибок первого и второго рода для каждого типа нештатного поведения
	std::vector<std::pair<unsigned int, unsigned int> > errors;
	
	template<class Archive>
	void serialize(Archive &archive, unsigned int version) {
		archive & BOOST_SERIALIZATION_NVP(goal);
		archive & BOOST_SERIALIZATION_NVP(errFirst);
		archive & BOOST_SERIALIZATION_NVP(errSecond);
		archive & BOOST_SERIALIZATION_NVP(axiomsIndex);
		archive & BOOST_SERIALIZATION_NVP(markUps);
		archive & BOOST_SERIALIZATION_NVP(errors);
	}
	
public:
	
	AxiomSetStatistics(): goal(-1), errFirst(-1), errSecond(-1) {}
	
    AxiomSetStatistics(const AxiomExprSetPlus &as) {
		this->goal = as.goal;
		this->errFirst = as.errFirst;
		this->errSecond = as.errSecond;
		this->axiomsIndex = as.axiomsIndex;
		this->markUps = as.markUps;
		this->errors = as.errors;
	}
	
	void copyTo(AxiomExprSetPlus &as) const {
		as.goal = this->goal;
		as.errFirst = this->errFirst;
		as.errSecond = this->errSecond;
		as.axiomsIndex = this->axiomsIndex;
		as.markUps = this->markUps;
		as.errors = this->errors;
	}
	
	void saveToFile(const std::string& fileName) const {
		std::ofstream ofstr(fileName.c_str());
		if(ofstr.good())
		{
			boost::archive::xml_oarchive archive(ofstr);
			archive << boost::serialization::make_nvp("AxiomSetStatistics", *this);
		}
		else
		{
			throw AxiomLibException("AxiomSetStatistics::saveToFile(): Couldn't open file '"
									+ fileName + "' for writing");
		}		
	}
	
	// Загружает объект из файла
	void initFromFile(const std::string &fileName) {
		std::ifstream ifstr(fileName.c_str());
		if(ifstr.good())
		{
			boost::archive::xml_iarchive archive(ifstr);
			archive >> boost::serialization::make_nvp("AxiomSetStatistics", *this);
		}
		else
		{
			throw AxiomLibException("AxiomSetStatistics:initFromFile(): Couldn't open file '"
									+ fileName + "' for reading");				
		}
	}	
	
};

}

#endif // AXIOMSETSTATISTICS_H
