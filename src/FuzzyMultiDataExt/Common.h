/****************************************************************************
*				Common
*
*	Description: Функции, используемые в других классах в данной папке
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef FUZZYMULTIDATAEXT_COMMON_H
#define FUZZYMULTIDATAEXT_COMMON_H

#include <vector>
#include <algorithm>
#include <iterator>
#include <string>

#include <boost/lexical_cast.hpp>

#include "../ElemCondPlusStat.h"
#include "../AxiomExprPlus.h"
#include "../AxiomExprSetPlus.h"
#include "../Logger.h"
#include "../Common.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

template<class T>
class ElementSelection {
public:
	ElementSelection() : m_selected(false) { }
	
	ElementSelection(const T& element, bool selected = true) : 
			m_element(element), m_selected(selected) { }
	
	T& element() { return m_element; }
	const T& element() const { return m_element; }
	bool selected() const { return m_selected; }
	void setSelected(bool value) { m_selected = value; }
private:
	T m_element;
	bool m_selected;
	
//	friend class boost::serialization::access;
	
//	template<class Archive>
//	void serialize(Archive &archive, unsigned int /*version*/) {
//		archive & BOOST_SERIALIZATION_NVP(m_element);
//		archive & BOOST_SERIALIZATION_NVP(m_selected);
//	}
};

// своя версия std::transform для того, чтобы гарантировать порядок обработки
// последовательности (в стандарте C++ порядок не специфицирован)
template <class InputIterator, class OutputIterator, class Transformator>
OutputIterator relaxed_transform(InputIterator first, InputIterator last,
                         OutputIterator result, Transformator trans) {
  for ( ; first != last; ++first, ++result)
    *result = trans(*first);
  return result;
}

template <class InputIterator1, class InputIterator2,
           class OutputIterator, class BinaryOperator>
  OutputIterator relaxed_transform(InputIterator1 first1, InputIterator1 last1,
							 InputIterator2 first2, InputIterator2 last2,
                             OutputIterator result, BinaryOperator op) {
  while (first1 != last1 || first2 != last2)
    *result++=binary_op(*first1++,*first2++);
  return result;
}

// Класс, предоставляющий функции для обработки многомерных векторов
template<unsigned int depth>
struct TransformMultiVectorHelper {
	template<class MultiVector1, class MultiVector2, class Transform>
	static MultiVector2 &transform(const MultiVector1 &v1, MultiVector2 &v2, Transform transform) {
		v2.resize(v1.size());
		
		for(unsigned int i = 0; i < v1.size(); i++) {
			TransformMultiVectorHelper<depth - 1>::transform(v1[i], v2[i], transform);
		}
		
		return v2;
	}
	
	template<class MultiVector1, class MultiVector2, class MultiVector3, class BinaryOp>
	static MultiVector3 &transform(const MultiVector1 &v1, const MultiVector2 &v2, MultiVector3 &v3, BinaryOp transform) {
		size_t size = std::min(v1.size(), v2.size());
		v3.resize(size);
		
		for(int i = 0; i < size; i++) {
			TransformMultiVectorHelper<depth - 1>::transform(v1[i], v2[i], v3[i], transform);
		}
		
		return v3;
	}
	
	template<class MultiVector1, class MultiVector2, class Functor>
	static MultiVector1& forEach(MultiVector1 &multiVector1, MultiVector2 &multiVector2,
								Functor functor) {
		int size = std::min(multiVector1.size(), multiVector2.size());
		for(int i = 0; i < size; i++) {
			TransformMultiVectorHelper<depth - 1>::forEach(multiVector1[i], multiVector2[i], functor);
		}
		return multiVector1;
	}
	
	template<class MultiVector, class Functor>
	static MultiVector& forEach(MultiVector &multiVector, Functor functor) {
		for(unsigned int i = 0; i < multiVector.size(); i++) {
			TransformMultiVectorHelper<depth - 1>::forEach(multiVector[i], functor);
		}
		return multiVector;
	}
	
	template<class Tuple, class Function>
	static void tupleForEach(Tuple& t1, Tuple& t2, Function function) {
                function(t1.template get<depth - 1>(), t2.template get<depth - 1>());
		TransformMultiVectorHelper<depth - 1>::tupleForEach(t1, t2, function);
	}
};

//Специализация шаблона TransformMultiVectorHelper, соответствующая случаю одномерного вектора
template<>
struct TransformMultiVectorHelper<1> {
	template<class MultiVector1, class MultiVector2, class Transform>
	static MultiVector2 &transform(const MultiVector1 &v1, MultiVector2 &v2, Transform transform) {
		v2.clear();
		v2.reserve(v1.size());
		
		relaxed_transform(v1.begin(), v1.end(),
							   std::back_inserter(v2),
							   transform);
		
		return v2;
	}
	
	template<class MultiVector1, class MultiVector2, class MultiVector3, class BinaryOp>
	static MultiVector3 &transform(const MultiVector1 &v1, const MultiVector2 &v2, MultiVector3 &v3, BinaryOp transform) {
		v3.clear();
		v3.reserve(std::min(v1.size(), v2.size()));
		
		return relaxed_transform(v1.begin(), v1.end(), v2.begin(), v2.end(), 
						  std::back_inserter(v3), transform);
	}	
	
	template<class MultiVector, class Functor>
	static MultiVector& forEach(MultiVector &multiVector, Functor functor) {
		std::for_each(multiVector.begin(), multiVector.end(), functor);
		return multiVector;
	}
	
	template<class MultiVector1, class MultiVector2, class Functor>
	static MultiVector1& forEach(MultiVector1 &multiVector1, MultiVector2 &multiVector2,
								Functor functor) {
		int size = std::min(multiVector1.size(), multiVector2.size());
		for(int i = 0; i < size; i++)
		{
			functor(multiVector1[i], multiVector2[i]);
		}
		return multiVector1;
	}
	
	template<class Tuple, class Function>
	static void tupleForEach(Tuple& t1, Tuple& t2, Function f) {
                f(t1.template get<0>(), t2.template get<0>());
	}
};


typedef std::vector<AxiomLib::AxiomExprSetPlus> ASVector;		

typedef std::vector<AxiomLib::ElemCondPlus> ECTypeVector;

template<class T>
std::string str(const T& t) {
	return boost::lexical_cast<std::string>(t);
}

};

};

#endif // FUZZYMULTIDATAEXT_COMMON_H
