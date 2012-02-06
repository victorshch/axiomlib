/****************************************************************************
*			QnaryBase.h
*
*	Description:	Базовый класс для q-нарного перцептрона и q-нарной ассоц. сети
*					в нем вводятся некоторые определения типов и значение Q
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef QNARY_BASE_H_
#define QNARY_BASE_H_

#include <boost/numeric/ublas/matrix.hpp>

#include "NeuralCommon.h"

namespace AxiomLib {

class QnaryBase {
public:
		// Тип элемента матриц весов, векторов порогов и локальных полей
		typedef double Elem;

		// Тип веса - матрица
		typedef boost::numeric::ublas::matrix<Elem> Weight;
		// Тип всех весов сети - матрица матриц
		typedef boost::numeric::ublas::matrix<Weight> NetWeights;

		// Тип порога
		typedef boost::numeric::ublas::vector<Elem> Threshold;
		
		// Тип локального поля - совпадает с типом порога
		typedef Threshold LocalField;
		
		// Тип нулевого порога
		typedef boost::numeric::ublas::zero_vector<Elem> ZeroThreshold;
		// Тип всех порогов сети
		typedef boost::numeric::ublas::vector<Threshold> NetThresholds;

		// Конструктор - в нем задается значение q
		QnaryBase(int q = 0): q(q) {}
		
		void construct(int q_) { q = q_; }
		
		// Возвращает значение q
		int getQ() const { return q; }
		
protected:
		// Значение q
		int q;
};

};

#endif /* QNARY_BASE_H_ */
