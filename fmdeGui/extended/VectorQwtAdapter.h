/****************************************************************************
 *			VectorQwtAdapter
 *
 *	Description:
 *	Author:		wictor
 *	History:	
 *
 ****************************************************************************/

#ifndef VECTORQWTADAPTER_H_
#define VECTORQWTADAPTER_H_

#include <vector>
#include <algorithm>
#include <qwt_data.h>
#include <boost/shared_ptr.hpp>
#include "utilities.h"

const double amplifyingFactor = 1.2;

class VectorQwtAdapter: public QwtData {
	PCMultiVector pDataMultiVector;
	PCVectorType pDataVector;
	CVectorType &dataVector;
	double maxData, minData;
	QwtDoubleRect m_boundingRect;

	void initialize() {
		if(dataVector.size() == 0) {
			maxData = minData = 0;
			return;
		}
		maxData = minData = dataVector[0];
		for(unsigned int i = 1; i < dataVector.size(); i++) {
			if(dataVector[i] > maxData) {
				maxData = dataVector[i];
			} else if(dataVector[i] < minData) {
				minData = dataVector[i];
			}
		}

		m_boundingRect = QwtDoubleRect(
						QwtDoublePoint(0, maxData * amplifyingFactor),
						QwtDoublePoint(dataVector.size()-1, minData * amplifyingFactor)
						);
	}
public:
	VectorQwtAdapter(const PCVectorType& dataVector) : pDataVector(dataVector), dataVector(*pDataVector) {
		initialize();
	}

	//this constructor takes ownership of dataVector
	VectorQwtAdapter(CVectorType * dataVector): pDataVector(PCVectorType(dataVector)), dataVector(*pDataVector) {
		initialize();
	}

	//this constructor doesn't take ownership of dataVector (as if it could :))
	VectorQwtAdapter(CVectorType & dataVector): pDataVector(), dataVector(dataVector) {
		initialize();
	}
	
	VectorQwtAdapter(const PCMultiVector& multiTS, int dimension)
		: pDataMultiVector(multiTS), dataVector((*multiTS)[dimension]) {
		initialize();
	}

	virtual VectorQwtAdapter * copy() const {
		VectorQwtAdapter * result = new VectorQwtAdapter(*this);
		return result;
	}

	virtual size_t size() const {
		return dataVector.size();
	}

	virtual double x(size_t i) const {
		return (double) i;
	}

	virtual double y(size_t i) const {
		return dataVector[i];
	}

	virtual QwtDoubleRect boundingRect() const {
		return m_boundingRect;
	}

	~VectorQwtAdapter() {}
};

#endif /* VECTORQWTADAPTER_H_ */
