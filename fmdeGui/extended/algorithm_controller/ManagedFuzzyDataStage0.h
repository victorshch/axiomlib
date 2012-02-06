/****************************************************************************
*			ManagedFuzzyDataStage0
*
*	Description:	Структура, содержащая параметры первой стадии
*	Author:		wictor
*	History:
*
****************************************************************************/


#ifndef MANAGEDFUZZYDATASTAGE0_H_
#define MANAGEDFUZZYDATASTAGE0_H_


#include "ManagedFuzzyDataStage.h"
#include "ElemCondPlusStat.h"
#include <vector>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/foreach.hpp>

typedef std::pair<std::string, bool> ECTypeSpec;

class ManagedFuzzyDataStage0 : public ManagedFuzzyDataStage {

	//объявления для сериализации
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &archive, const unsigned int /*version*/) {
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ManagedFuzzyDataStage);
		archive & BOOST_SERIALIZATION_NVP(initialECTypes);
		archive & BOOST_SERIALIZATION_NVP(checkedECTypes);
	}

	ManagedFuzzyDataStage0() { }
public:
	virtual int getStageNumber() const {
		return 0;
	}


	ManagedFuzzyDataStage0(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, AxiomLib::Environment* env) : ManagedFuzzyDataStage(algorithm, env)
	{
		setCalculated(true);
		
//		m_myEnv.setParamValue("", "numBestECs");
//		m_myEnv.setParamValue("", "leftLimit");
//		m_myEnv.setParamValue("", "rightLimit");
//		m_myEnv.setParamValue("", "numOfLevels");
//		m_myEnv.setParamValue("", "reserve");
		
//		m_myEnv %= *env;
	}

	~ManagedFuzzyDataStage0()
	{
		
	}
	
	void initImpl() {
		ManagedFuzzyDataStage0& stage0 = *this;

		AxiomLib::FuzzyMultiDataExtAlgorithm& fuzzyMultiDataLearnAlgorithm = *m_algorithm;
		
//		env.getMandatoryParamValue(stage0.maxECs, "numBestECs");
//		env.getMandatoryParamValue(stage0.leftLimit, "leftLimit");
//		env.getMandatoryParamValue(stage0.rightLimit, "rightLimit");
//		env.getMandatoryParamValue(stage0.numOfLevels, "numOfLevels");
//		env.getMandatoryParamValue(stage0.reserve, "reserve");
		
		//stage0.clippingIntervals = fuzzyMultiDataAccessor.fuzzyDataSet().getClippingIntervals();
		
		stage0.initialECTypes.clear();
		stage0.initialECTypes.reserve(fuzzyMultiDataLearnAlgorithm.getECTypeSize());
		stage0.checkedECTypes.clear();
		stage0.checkedECTypes.resize(fuzzyMultiDataLearnAlgorithm.getECTypeSize());
		for(int i = 0; i < fuzzyMultiDataLearnAlgorithm.getECTypeSize(); i++) {
			stage0.initialECTypes.push_back(fuzzyMultiDataLearnAlgorithm.getECType(i));
			stage0.checkedECTypes[i] = fuzzyMultiDataLearnAlgorithm.isECTypeSelected(i);
		}
	}
	
	void saveImpl() {
		
		std::string numBestECsStr = getParamValue("numBestECs");
		int numBestECs;
		if(numBestECsStr.empty()) {
			numBestECs = 10;
		} else {
			numBestECs = boost::lexical_cast<int>(numBestECsStr);
		}
		
		setParamValue("numBestECsMax", boost::lexical_cast<std::string>(2 * numBestECs));
				
		m_algorithm->setECTypes(initialECTypes);
		for(int i = 0; i < checkedECTypes.size(); i++) {
			m_algorithm->setECTypeSelected(i, checkedECTypes[i]);
		}
	}

	//Параметры данного этапа

	//Набор элементарных условий
	std::vector<AxiomLib::ElemCondPlus> initialECTypes;
	
	//Выбранные ЭУ
	std::vector<bool> checkedECTypes;
};



#endif /* MANAGEDFUZZYDATASTAGE0_H_ */
