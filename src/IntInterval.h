/****************************************************************************
*				IntInterval.h
*
*	Description: Класс, описывающий целочисленный интервал
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef INTINTERVAL_H
#define INTINTERVAL_H

#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include "AxiomLibException.h"

namespace AxiomLib {
	// Класс для целочисленного интервала
	class IntInterval {
		int left_, right_;
		
		// Объявления, необходимые для сериализации
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int /*version*/)
		{
			archive & BOOST_SERIALIZATION_NVP(left_);
			archive & BOOST_SERIALIZATION_NVP(right_);
		}
		
	public:
		// Конструктор умолчания
		IntInterval() : left_(-1), right_(0) { }
		
		// Конструктор, задаюищй интервал
		IntInterval(int left, int right) : left_(left), right_(right) {
			if(left < 0 || left > right) {
				throw AxiomLib::AxiomLibException("IntInterval::IntInterval(): invalid range");
			}
		}
		
		// Конструктор, задающий интервал в соответствии с длиной вектора
		template<class T>
		explicit IntInterval(const std::vector<T> &v) : left_(0), right_(v.size() - 1) { }
		
		// Левая граница
		int left() const { return left_; }
		
		// Правая граница
		int right() const { return right_; }
		
		// Длина интервала (обе границы включаются)
		int length() const { return right() - left() + 1; }
		
		// Перевод индекса в с.о., в которой задан интервал, в
		// индекс внутри интервала
		int translateIndex(int index) const { return index - left(); }
		
		// Перевод индекса внутри интервала в индекс в с.о., в
		// которой задан интервал
		int reverseIndex(int clippedIndex) const { return clippedIndex + left(); }
		
		// Оператор сравнения на равенство
		bool operator==(const IntInterval &other) const {
			return left() == other.left() && right() == other.right();
		}
	};
	
}

#endif // INTINTERVAL_H
