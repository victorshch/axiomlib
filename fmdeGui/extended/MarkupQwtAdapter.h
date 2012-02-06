/****************************************************************************
 *			MarkupQwtAdapter
 *
 *	Description:
 *	Author:		wictor
 *	History:	
 *
 ****************************************************************************/

#ifndef MARKUPQWTADAPTER_H_
#define MARKUPQWTADAPTER_H_

//#include "VectorQwtAdapter.h"
#include <qwt_data.h>
#include "AxiomLibException.h"
#include "utilities.h"

#include <QtDebug>



class MarkupQwtAdapter: public QwtData {
	PMarkupVector xVector;

	PCVectorType pDataVector;
	PCMultiVector pDataMultiVector;
	CVectorType &dataVector;

	double minValue, maxValue;

	void initialize(const std::vector<int>& markupVector) {
		if(markupVector.size() != dataVector.size())
		{
			throw AxiomLib::AxiomLibException("MarkupQwtAdapter(): Markup and data vector sizes don't match");
		}

		xVector = PMarkupVector(new std::vector<int>());

		xVector->reserve(markupVector.size());

		bool valuesSet = false;
		for(unsigned int i = 0; i < markupVector.size(); i++) {
			if(markupVector[i]!=0) {
				xVector->push_back(i);
				if(!valuesSet) {
					minValue = maxValue = (dataVector)[i];
					valuesSet = true;
				} else {
					if(minValue > (dataVector)[i]) {
						minValue = (dataVector)[i];
					} else if(maxValue < (dataVector)[i]) {
						maxValue = (dataVector)[i];
					}
				}
			}
		}

		//qDebug()<<"xVector->size() == "<<xVector->size();
	}

public:
	MarkupQwtAdapter(PCVectorType dataVector, const std::vector<int>& markupVector)
	:pDataVector(dataVector), dataVector(*dataVector) {
		initialize(markupVector);
	}

	MarkupQwtAdapter(CVectorType &dataVector, const std::vector<int>& markupVector)
	:pDataVector(), dataVector(dataVector) {
		initialize(markupVector);
	}

	MarkupQwtAdapter(const PCMultiVector &multiTS, int dimension, const std::vector<int> markupVector)
		: pDataMultiVector(multiTS), dataVector((*multiTS)[dimension]) {
		initialize(markupVector);
	}
	
	virtual MarkupQwtAdapter * copy() const {
		MarkupQwtAdapter * result = new MarkupQwtAdapter(*this);
		return result;
	}

	virtual size_t size() const {
		return xVector->size();
	}

	virtual double x(size_t i) const {
		return (double) (*xVector)[i];
	}

	virtual double y(size_t i) const {
		return (dataVector)[(*xVector)[i]];
	}

	virtual QwtDoubleRect boundingRect() const {
		return QwtDoubleRect(
				QwtDoublePoint(0, maxValue),
				QwtDoublePoint(xVector->size()-1, minValue)
				);
	}

	virtual ~MarkupQwtAdapter() {}
};

#endif /* MARKUPQWTADAPTER_H_ */
