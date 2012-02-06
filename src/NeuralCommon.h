/****************************************************************************
*				NeuralCommon.h
*
*	Description:	Файл, содержащий некоторые общие для нейросетей объявления
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#undef min
#undef max

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>

#include "AxiomLibException.h"
#include "Environment.h"

namespace AxiomLib {

typedef int Symbol;
// q-нарная строка (q-нарный вектор)
typedef boost::numeric::ublas::vector<Symbol> Qstring;
typedef boost::numeric::ublas::zero_vector<Symbol> ZeroQstring;

// Возвращает расстряние Хемминга между более короткой строкой и префиксом более длинной строки
int hammingDistance(const Qstring &str1, const Qstring &str2);
double difference(const Qstring &str1, const Qstring &str2);

// Копирует в V2 windowSize (по умолчанию 0) элементов V1 с позиции startPos (по умолчанию 0)
// если windowSize = 0, то копируется весь V1.
// Функция сделана шаблоном, т.к. предполагается работа как с std::vector, так и с Qstring
template<class Vector1, class Vector2>
inline void copyVector(const Vector1 &v1, Vector2 &v2, unsigned int startPos = 0, unsigned int windowSize = 0) {
	int s1;
	
	if(windowSize + startPos > v1.size()) {
		windowSize = v1.size() - startPos;
	}
	
	s1 = windowSize ? windowSize : v1.size();
	
	if(s1 != v2.size()) {
		v2.resize(s1);
	}
	
	// Копируем в v2 часть v1 из окна
	for (int i = (int) startPos; i < (int) startPos + s1; i++) {
		v2[i - (int) startPos] = /*(Vector2::value_type)*/ v1[i];
	}
}

// отключение макроса min, т.к. далее везде используется функция std::min
//#undef min

inline void getWindow(const Qstring &v1, Qstring &v2, int pos, int size) {
	v2.resize(size);
	int m1 = std::min(size, ((int)v1.size()) - pos);
	for(int i = 0; i < m1; i++) {
		v2[i] = v1[pos + i];
	}
	for(int i = m1; i < size; i++) {
		v2[i] = 0;
	}	
}

// Возвращает число цифр в записи num в q-ичной системе счисления
int numDigits(int q, int num);

// Записывает в str число num в q-ичной системе счисления как q-нарный вектор
void numToQnaryVector(int q, int num, Qstring &str);

// Преобразует q-нарный вектор в число, рассматривая его как число в q-ичной СС
int qnaryVectorToNum(int q, const Qstring &str);

// Забивает q-нарный вектор v нулями
void makeZero(Qstring &v);

// Ищет вхождение toFind в векторе v в delta-окрестности pos
bool scanDelta(const std::vector<int> &v, int pos, int delta, int toFind);

// Считывание вектора значений из строки
template<class T>
void getVectorFromString(std::vector<T> &v, const std::string &s, const std::string &sep = "|") {
	std::string s1(s);
	std::size_t pos = s1.find_first_of(sep);
	while(pos != std::string::npos) {
		s1[pos] = '\t';
		pos = s1.find_first_of(sep);
	}
	
	std::istringstream istr(s1);
	T t;
	v.clear();
	istr.exceptions(std::ios::badbit | std::ios::failbit);
	try {
		istr>>std::skipws;
		while(!istr.eof()) {
			istr>>t;
			v.push_back(t);
		}
	} catch (...) {
		throw AxiomLibException("getVectorFromString(): syntax error");
	}
}

// Получение из окружения параметра с именем name как булевского значения
int getBoolParamValueFromEnv(const Environment &env, bool &value, 
	const std::string &name);

};

#endif
