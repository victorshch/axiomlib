/****************************************************************************
*				IntervalSet.h
*
*	Description: В данном файле описан класс, содержащий набор интервалов
*				отсечения, соответствующих нештатному и нормальному поведению,
*				а также структура, соответствующая набору интервалов отсечения
*				для обучающей и для контрольной выборки
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef INTERVALSET_H
#define INTERVALSET_H

#include "FuzzyDataSet.h"
#include "IntInterval.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

namespace AxiomLib {
	
// Класс для набора интервалов отсечения нештатного и нормального поведения
class IntervalSet {
private:
	// Объявления, необходимые для сериализации
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & archive, const unsigned int /*version*/) {
		archive & BOOST_SERIALIZATION_NVP(m_intervals);
	}

	// Набор интервалов отсечения для траекторий нештатного поведения
	std::vector<std::vector<IntInterval> > m_intervals;
	
	static void clipClassNo(int& classNo);
	
	IntervalSet() {}
public:
	
	IntervalSet(const FuzzyDataSet& dataSet, FuzzyDataSet::DataSetDivisionType division);
	
	// Сохраняет интервалы и сбрасывает их в dataset'е
	static IntervalSet reset(FuzzyDataSet& dataSet, FuzzyDataSet::DataSetDivisionType division);
	
	// Применяет данный набор ограничений к датасету
	void apply(FuzzyDataSet& dataSet, FuzzyDataSet::DataSetDivisionType division) const;
	// Возвращает интервал отсечения для заданного класса и траектрии
	// (classNo < 0 означает нормальное поведение)
	const IntInterval &getClippingInterval(int classNo, int multiTSNo) const;
	// Устанавливает интервал отсечения для заданного класса и траектрии
	// (classNo < 0 означает нормальное поведение)
	void setClippingInterval(const IntInterval &interval, int classNo, int tsNo);
	
	// Возвращает число траекторий заданного класса
	int getMultiTSCount(int classNo) const;
};

}

#endif // INTERVALSET_H
