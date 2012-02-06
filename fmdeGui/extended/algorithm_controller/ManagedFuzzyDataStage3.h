/****************************************************************************
 *			ManagedFuzzyDataStage3
 *
 *	Description:
 *	Author:		wictor
 *	History:	
 *
 ****************************************************************************/

#ifndef MANAGEDFUZZYDATASTAGE3_H_
#define MANAGEDFUZZYDATASTAGE3_H_

#include "ManagedFuzzyDataStage.h"
#include "ManagedFuzzyDataUtility.h"
#include "AxiomExprSetPlus.h"
#include "StringPlus.h"

class ManagedFuzzyDataStage3: public ManagedFuzzyDataStage {
	//объявления, необходимые для сериализации
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ManagedFuzzyDataStage);
		archive & BOOST_SERIALIZATION_NVP(bestAxiomSets);
	}

	ManagedFuzzyDataStage3() {}
public:
	AxiomSetVector bestAxiomSets;

	int getStageNumber() const { return 3; }
	
	void initImpl() {
		bestAxiomSets.clear();
		bestAxiomSets.reserve(m_algorithm->getASSize());
		
		forEachAxiomSet(m_algorithm,
						[this](int i, const AxiomLib::AxiomExprSetPlus& as) {
			bestAxiomSets.push_back(as);
		}
		);
	}
	
	void saveImpl() {
		m_algorithm->setAxiomSets(bestAxiomSets);
	}

	ManagedFuzzyDataStage3(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm, AxiomLib::Environment* env): ManagedFuzzyDataStage(algorithm, env) {
	}

	~ManagedFuzzyDataStage3() {
	}
};


#endif /* MANAGEDFUZZYDATASTAGE3_H_ */
