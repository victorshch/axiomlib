/****************************************************************************
*			utilities
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef UTILITIES_H_
#define UTILITIES_H_

//TODO: разбить на несколько модулей

#include "AxiomLibException.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <QByteArray>
#include <QString>
#include <QLabel>
#include <QLayout>
#include "AxiomExprSetPlus.h"
#include "ElemCondPlusStat.h"
#include "AxiomExpr.h"

#include "undefs.h"

#include "relaxed_transform.h"

typedef std::vector<double> VectorType;
typedef const std::vector<double> CVectorType;
typedef boost::shared_ptr<VectorType> PVectorType;
typedef boost::shared_ptr<CVectorType> PCVectorType;
typedef std::vector<int> MarkupVector;
typedef boost::shared_ptr<MarkupVector> PMarkupVector;
typedef std::vector<VectorType> MultiVector;
typedef const std::vector<VectorType> CMultiVector;
typedef boost::shared_ptr<CMultiVector> PCMultiVector;

typedef boost::shared_ptr<AxiomLib::AxiomExprSetPlus> PAxiomExprSetPlus;
typedef boost::shared_ptr<AxiomLib::AxiomExpr> PAxiomExpr;

//template<class Iterator, class BinaryFunction>
//auto fold(Iterator begin, Iterator end, BinaryFunction aggregateFunction) -> decltype(aggregateFunction(*begin, *begin)) {
//	if(begin == end) {
//		throw std::logic_error("Can't apply fold to empty sequence");
//	}
	
//	Iterator it = begin;
//	++it;
//	auto result = aggregateFunction(*begin, *it);
//}

template<class TreeItem, class Functor>
void IterateChildren(TreeItem &treeItem, Functor functor) {
	int size = treeItem.childCount();
	for(int i = 0; i < size; i++) {
		functor(&(treeItem.child(i)));
	}
}

template<class TreeItem, class Functor>
void IterateNodes(TreeItem &treeItem, Functor functor) {
	functor(&treeItem);
	int count = treeItem.childCount();
	for(int i = 0; i < count; i++) {
		IterateNodes(*(treeItem.child(i)), functor);
	}
}

template<class TreeItem, class Functor> class ApplyToLeaves {
	Functor functor;
public:
	ApplyToLeaves(const Functor & functor) : functor(functor) {}
	void operator() (TreeItem *treeItem) {
		if(treeItem->childCount() == 0) {
			functor(treeItem);
		}
	}
};

template<class TreeItem, class Functor>
void IterateLeaves(TreeItem &treeItem, Functor functor) {
	IterateNodes(treeItem, ApplyToLeaves<TreeItem, Functor>(functor));
}

void displayException(const AxiomLib::AxiomLibException &exception);

void displayException(const std::exception &exception);

//Функция для кодировки из Koi8-r в unicode
QString encode(const QByteArray &array);

void clearLayout(QLayout *layout);

template<class Container1, class Container2, class Transformer>
Container2 &copyTransformed(const Container1 &container1, 
							Container2 &container2,
							Transformer transformer) {
	container2.clear();
	container2.reserve(container1.size());
	relaxed_transform(
			container1.begin(),
			container1.end(),
			std::back_inserter(container2),
			transformer
			);
	return container2;
}

template<class MultiVector, int depth>
struct GlobalSizeHelper {
	static int getGlobalSize(const MultiVector &v) {
		int result = 0;
		for(int i = 0; i < v.size(); i++) {
			result += 
					GlobalSizeHelper<typename MultiVector::value_type, depth - 1>::getGlobalSize(v[i]);
		}
		return result;
	}	
};

template<class MultiVector>
struct GlobalSizeHelper<MultiVector, 1> {
	static int getGlobalSize(const MultiVector &v) {
		return v.size();
	}	
};

template<class MultiVector, class Value, int depth>
struct GlobalIndexHelper {
	static Value& getByGlobalIndex(MultiVector &v, int index) {
		int currMaxIndex = 0;
		for(int i = 0; i < v.size(); i++) {
			index -= currMaxIndex;
			int size = GlobalSizeHelper<MultiVector, depth-1>::getGlobalSize(v[i]);
			if(size > index) {
				return GlobalIndexHelper<MultiVector, typename MultiVector::value_type, depth-1>::getByGlobalIndex(v[i], index);
			}
			currMaxIndex = size;
		}
		throw std::range_error("MultiVector index out of range");
	}
};

template<class MultiVector>
struct GlobalIndexHelper<MultiVector, typename MultiVector::value_type, 1> {
	static typename MultiVector::value_type& getByGlobalIndex(MultiVector &v, int index) {
		return v[index];
	}
};

template<int depth>
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
	
	
	template<class MultiVector2, class TreeItem, class Functor>
	static MultiVector2 &transformTree(const TreeItem& treeItem, MultiVector2 &v2, Functor functor) {
		int size = treeItem->childCount();
		//v2.clear();
		v2.resize(size);
		
		for(int i = 0; i < size; i++) {
			TransformMultiVectorHelper<depth - 1>::transform(v2[i], treeItem->child(i), functor);
		}
		
		return v2;
	}
	
	template<class MultiVector1, class MultiVector2>
	static void makeSameSize(const MultiVector1& v1, MultiVector2& v2) {
		v2.resize(v1.size());
		
		for(typename MultiVector1::size_type i = 0; i < v1.size(); ++i) {
			makeSameSize<depth - 1>(v1[i], v2[i]);
		}
	}
};

template<>
struct TransformMultiVectorHelper<1> {
	template<class MultiVector1, class MultiVector2, class Transform>
	static MultiVector2 &transform(const MultiVector1 &v1, MultiVector2 &v2, Transform transform) {
		v2.clear();
		v2.reserve(v1.size());
		
		::relaxed_transform(v1.begin(), v1.end(),
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
	
	template<class MultiVector2, class TreeItem, class Functor>
	static MultiVector2 &transformTree(const TreeItem& treeItem, MultiVector2 &v2, Functor functor) {
		int size = treeItem->childCount();
		v2.resize(size);
		
		for(int i = 0; i < size; i++) {
			v2[i] = functor(treeItem->child(i));
		}
		
		return v2;		
	}	
	
	template<class MultiVector1, class MultiVector2>
	static void makeSameSize(const MultiVector1& v1, MultiVector2& v2) {
		v2.resize(v1.size());
	}
};

const double eps = 1e-8;

bool doubleEq(double a, double b);

template<class Axiom>
double statDifference(const Axiom &ecPlus) { 
	return ecPlus.statAbnorm - ecPlus.statNormal;
}

std::string getFullECTypeName(const AxiomLib::ElemCondPlus &ecPlus);

#endif /* UTILITIES_H_ */
