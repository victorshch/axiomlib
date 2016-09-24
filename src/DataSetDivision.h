/****************************************************************************
*				DataSetDivision
*
*	Description: Класс, соответствующий разделу набора данных
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef DATASETDIVISION_H
#define DATASETDIVISION_H

#include <vector>
#include <boost/tuple/tuple.hpp>

#include "MultiTS.h"
#include "IntInterval.h"

namespace AxiomLib {

class DataSetDivision
{
public:
    DataSetDivision() {}
    DataSetDivision(const ReferenceClassesTS &abnormalTS, const ClassTS &normalTS);
	
	std::string getMultiTSName(int indexClass, int indexMultiTS) const;

	// Возвращает число классов нештатного поведения
	int getClassCount() const; 
	
	// Число мультирядов в классе
	int getMultiTSCount(int classNo) const;
	
	// Возвращает одномерный ряд по индексам
	bool getTSByIndex(std::vector<double> &v, int behaviourType, int multiTSNo, int tsNo, bool clip = true) const;
	
	// Возвращает длину мультиряда
	int getMultiTSLength(int behaviourType, int multiTSNo) const;
	
	// Работа с интервалом ограничения
	IntInterval getClippingInterval(int behaviourType, int multiTSNo) const;
	void setClippingInterval(const IntInterval &interval,int behaviourType, int multiTSNo);
	
	// Возвращает размерности траекторий нештатного поведения для каждого класса нештатного поведения
	bool getAbnormalSize(std::vector<std::vector<int> > &v) const;
	
	// Возвращает размерности траекторий заданного класса
	bool getClassSize(int classNo, std::vector<int> &v) const;
	
	// Возвращает размерности траекторий нормального поведения
	bool getNormalSize(std::vector<int> &v) const;
private:
	typedef boost::tuples::tuple<MultiTS, IntInterval> ClippedMultiTS;
	typedef std::vector<ClippedMultiTS> ClippedClassTS;
	typedef std::vector<ClippedClassTS> ClippedClassTSSet;
	
	ClippedClassTSSet m_tsSet;
	
	const ClippedClassTS &getClippedClassTS(int behaviourType) const;
	const ClippedMultiTS &getClippedMultiTS(int behaviourType, int multiTSNo) const;
	
	ClippedClassTS &getClippedClassTS(int behaviourType);
	ClippedMultiTS &getClippedMultiTS(int behaviourType, int multiTSNo);
	
};

}

#endif // DATASETDIVISION_H
