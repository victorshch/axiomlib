/****************************************************************************
*			ManagedFuzzyDataStage
*
*	Description:	Базовый класс для структур, содержащих информацию
*					о стадии алгоритма
*	Author:		wictor
*	History:
*
****************************************************************************/


#ifndef MANAGEDFUZZYDATASTAGE_H_
#define MANAGEDFUZZYDATASTAGE_H_

#include <stack>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "vectortypes.h"
#include "FuzzyMultiDataExtAlgorithm.h"
#include "FuzzyDataSet.h"
#include "Environment.h"
#include "IntervalSet.h"
#include "EnvironmentSerialization.h"

typedef boost::shared_ptr<AxiomLib::IntervalSet> PIntervalSet;

class ManagedFuzzyDataStage {

	//объявления, необходимые для сериализации
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &archive, const unsigned int /*version*/)
	{

		archive & BOOST_SERIALIZATION_NVP(m_calculated);
		archive & BOOST_SERIALIZATION_NVP(clippingIntervals);
		archive & BOOST_SERIALIZATION_NVP(m_myEnv);
	}

	bool m_calculated;
	
protected:
	ManagedFuzzyDataStage();
	
	AxiomLib::FuzzyMultiDataExtAlgorithm* const m_algorithm;
	AxiomLib::Environment* const m_env;
	AxiomLib::Environment m_myEnv;
	
	virtual void initImpl() = 0;
	virtual void saveImpl() = 0;
public:
	ManagedFuzzyDataStage(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, AxiomLib::Environment* env) : 
			m_algorithm(algorithm), m_env(env), m_calculated(false)
	{ }
	
	// Интервалы отсечения для обучающей выборки
	PIntervalSet clippingIntervals;
	
	virtual int getStageNumber() const { return -1; }
	
	void init();
	void save();
	
	
	std::string getParamValue(const std::string& name) const;
	void setParamValue(const std::string& name, const std::string& newValue);
	
	void initClipping() {
		clippingIntervals = PIntervalSet(
		            new AxiomLib::IntervalSet(m_algorithm->getDataSet(), 
							AxiomLib::FuzzyDataSet::Reference)
		            );
	}

	void saveClipping() {
		if(clippingIntervals != PIntervalSet()) {
			clippingIntervals->apply(m_algorithm->getDataSet(), AxiomLib::FuzzyDataSet::Reference);
		} else {
			throw std::runtime_error("Clipping intervals not loaded into stage!");
		}
	}

	bool calculated() { return m_calculated; }

	void setCalculated(bool value) { m_calculated = value; }

	virtual ~ManagedFuzzyDataStage() {}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ManagedFuzzyDataStage);


#endif /* MANAGEDFUZZYDATASTAGE_H_ */
