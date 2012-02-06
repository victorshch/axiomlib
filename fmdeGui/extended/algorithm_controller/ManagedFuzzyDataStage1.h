/****************************************************************************
*			ManagedFuzzyDataStage1
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef MANAGEDFUZZYDATASTAGE1_H_
#define MANAGEDFUZZYDATASTAGE1_H_

#include <vector>
#include <string>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

#include <QtDebug>

#include "ManagedFuzzyDataStage.h"
#include "ManagedFuzzyDataUtility.h"
#include "ElemCondPlusStat.h"
#include "../utilities.h"


class ManagedFuzzyDataStage1: public ManagedFuzzyDataStage {

	//объявления, необходимые для сериализации
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &archive, const unsigned int /*version*/)
	{
		archive & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ManagedFuzzyDataStage);
//		archive & BOOST_SERIALIZATION_NVP(maxAxiomPopSize);
//		archive & BOOST_SERIALIZATION_NVP(maxIterNum);
//		archive & BOOST_SERIALIZATION_NVP(maxAxiomRes);
//		archive & BOOST_SERIALIZATION_NVP(percentBestAxioms);
		
		archive & BOOST_SERIALIZATION_NVP(bestECForAllAbnormalTypes);
		archive & BOOST_SERIALIZATION_NVP(abnormalTypesSelected);
	}

	ManagedFuzzyDataStage1() {}
public:
        ManagedFuzzyDataStage1(AxiomLib::FuzzyMultiDataExtAlgorithm* algorithm,
	                                               AxiomLib::Environment* env) : 
	    ManagedFuzzyDataStage(algorithm, env) 
	{ }
	
	int getStageNumber() const { return 1; }
	
	void initImpl() {
		
		//qDebug()<<"Initializing stage 1";
		
//		m_env->getMandatoryParamValue(maxAxiomPopSize, "maxAxiomPopSize");
//		m_env->getMandatoryParamValue(maxIterNum, "axiomAlgIterNum");
//		m_env->getMandatoryParamValue(maxAxiomRes, "maxCostForAxiom");
//		m_env->getMandatoryParamValue(percentBestAxioms, "percentBestAxioms");
		
		std::vector<std::vector<std::vector<int> > > sizeVector;
		
		m_algorithm->getECSize(sizeVector);
		
		TransformMultiVectorHelper<3>::transform(sizeVector, this->bestECForAllAbnormalTypes,
											   [](int size) -> std::vector<AxiomLib::ElemCondPlusStat>{ 
			
		    std::vector<AxiomLib::ElemCondPlusStat> ecVector;
		    ecVector.clear();
			ecVector.reserve(size);
			return ecVector;
		}
		);
		
		TransformMultiVectorHelper<3>::transform(sizeVector, this->abnormalTypesSelected,
											   [](int size) ->std::vector<bool> { 
		    std::vector<bool> v;
			v.resize(size);
			return v;
		}
		);
		
		
		forEachEC(m_algorithm, 
				  [this](int aType, int dim, int ecType, int n, const AxiomLib::ElemCondPlusStat& ec, bool selected)
				  {
			this->bestECForAllAbnormalTypes[aType][dim][ecType].push_back(ec);
			abnormalTypesSelected[aType][dim][ecType][n] = selected;
		}
		);
		
		qDebug()<<"Stage 1 initialized.";
	}
	
	void saveImpl() {
		m_algorithm->setECs(bestECForAllAbnormalTypes);

		forEachEC(m_algorithm, 
				  [this](int aType, int dim, int ecType, int n, const AxiomLib::ElemCondPlusStat&, bool selected)
				  {
			m_algorithm->setECSelected(aType, dim, ecType, n,
				abnormalTypesSelected[aType][dim][ecType][n]);
		}
		);		
	}

	// Параметры алгоритма настройки аксиом
	// Максимальное число аксиом в популяции
	int maxAxiomPopSize;
	// Максимальное число итераций алгоритма поиска аксиом
	int maxIterNum;
	// Максимальное значение целевой функции для аксиомы, если оно превышается, то поиск новых аксиом останавливается
	double maxAxiomRes;
	// Процент лучших аксиом популяции, которые сохраняются при сокращении популяции
	double percentBestAxioms;		
	
	ECMultiVector bestECForAllAbnormalTypes;
	ECSelectedMultiVector abnormalTypesSelected;
};




#endif /* MANAGEDFUZZYDATASTAGE1_H_ */
