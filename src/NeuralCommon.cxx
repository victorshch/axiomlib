/****************************************************************************
*			NeuralCommon.cxx
*
*	Description:	Реализация того, что объявлено в NeuralCommon.h
*	Author:		wictor
*	History:	
*
****************************************************************************/

#include <boost/algorithm/string/case_conv.hpp>
#include "NeuralCommon.h"
#include "math.h"

namespace AxiomLib {

/****************************************************************************
*					hamming_distance
*
*	Description:	Возвращает расстряние Хемминга между более короткой строкой и префиксом той же длины более длинной
*	Parameters:	str1, str2 - q-нарные строки
*	Returns:	расстояние Хемминга (int)
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
int hammingDistance(const Qstring &str1, const Qstring &str2) {
	int res=0, m,  s1 = str1.size(), s2=str2.size();
	// Определяем размер наименьшей строки
	m = (s1 > s2) ? s2 : s1;
	// Итерируем по символам строк, сравнивая их на равенство
	for (int i = 0; i < m; i++) {
		if(str1(i)!=str2(i)) res++;
	}
	return res;
}

/****************************************************************************
*					difference
*
*	Description:	Возвращает расстряние Хемминга между более короткой строкой и префиксом той же длины более длинной,
*					деленное на длину более короткой строки
*	Parameters:	str1, str2 - q-нарные строки
*	Returns:	расстояние Хемминга (int)
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
double difference(const Qstring &str1, const Qstring &str2) {
	int res=0, m,  s1 = str1.size(), s2=str2.size();
	// Определяем размер наименьшей строки
	m = (s1 > s2) ? s2 : s1;
	// Итерируем по символам строк, сравнивая их на равенство
	for (int i = 0; i < m; i++) {
		if(str1(i)!=str2(i)) res++;
	}
	return double(res)/double(m);
}

/****************************************************************************
*					numDigits
*
*	Description:	Возвращает число цифр в записи num в q-ичной системе счисления
*	Parameters:	int q, int num
*	Returns:	int
*	Throws:		AxiomLibException, если q<=1
*	Author:		wictor
*	History:
*
****************************************************************************/
int numDigits(int q, int num)
{
	if(q<=1) {
		throw AxiomLibException("numDigits(): invalid q value");
	}
	return (int) floor(log(double(num))/log(double(q)) + 1.0);
}

/****************************************************************************
*					numToQnaryVector
*
*	Description:	Записывает в str число num в q-ичной системе счисления как q-нарный вектор
*	Parameters:	int q, int num, Qstring &str
*	Returns:	-
*	Throws:		AxiomLibException, если q<=1
*	Author:		wictor
*	History:
*
****************************************************************************/
void numToQnaryVector(int q, int num, Qstring &str)
{
	if(q<=1) {
		throw AxiomLibException("numToQnaryVector(): invalid q value");
	}
	int sz=str.size();
	// number will be written with normal digits' order
	for(int i=sz-1; i>=0; i--)
	{
		str[i]=num%q;
		num/=q;
	}
}

/****************************************************************************
*					qnaryVectorToNum
*
*	Description:	Преобразует q-нарный вектор в число, рассматривая его как число в q-ичной СС
*	Parameters:	int q, const Qstring &str
*	Returns:	int - указанное число
*	Throws:		AxiomLibException, если q<=1
*	Author:		wictor
*	History:
*
****************************************************************************/
int qnaryVectorToNum(int q, const Qstring &str)
{
	if(q<=1) {
		throw AxiomLibException("qnaryVectorToNum(): invalid q value");
	}
	int res=0;
	for(unsigned int i=0; i<str.size()-1; i++)
	{
		res+=str[i];
		res*=q;
	}
	res+=str[str.size()-1];
	return res;
}

/****************************************************************************
*					makeZero
*
*	Description:	Забивает q-нарный вектор v нулями
*	Parameters:	Qstring &v
*	Returns:	-
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
void makeZero(Qstring &v)
{
	for(unsigned int i=0; i<v.size(); ++i)
	{
		v[i]=0;
	}
}

/****************************************************************************
*					scanDelta
*
*	Description:	Сканирует дельта-окрестность позиции pos в векторе на предмет вхождения toFind
*	Parameters:	const std::vector<int> &v - вектор который нужно сканировать
*				int pos - позиция
*				int delta - радиус окрестности
*				int toFind - значение, которое нужно найти
*	Returns:	true, если значение найдено; иначе false
*	Throws:		AxiomLibException, если pos выходит за рамки вектора
*	Author:		wictor
*	History:
*
****************************************************************************/
bool scanDelta(const std::vector<int> &v, int pos, int delta, int toFind) {
	if (pos < 0 || pos >= (int) v.size()) {
		throw AxiomLibException("scanDelta() : pos out of range");
	}
	
	int minI = pos - delta < 0 ? 0 : pos - delta;
	int maxI = pos + delta > ((int) v.size()) - 1 ? ((int) v.size()) - 1 : pos + delta;
	for(int i = minI; i <= maxI; i++) {
		if(v[i] == toFind) {
			return true;
		}
	}
	return false;
}

/****************************************************************************
*					getBoolParamValueFromEnv
*
*	Description:	Функция возвращает первое значение параметра с именем name как булевскую переменную
*					(правило преобразования - case-insensitive, "true" или "false")
*	Parameters:	Environment& env - окружение
*				bool& value - куда писать значение
*				string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось, или он не привелся к bool (по указанному правилу)
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
int getBoolParamValueFromEnv(const Environment &env, bool &value, 
	const std::string &name) {
	std::string str;
	if(env.getStringParamValue(str, name) < 0) {
		return -1;
	} else {
		boost::algorithm::to_lower(str);
		if (str == "true") {
			value = true;
		} else if (str == "false") {
			value = false;
		} else {
			return -1;	
		}
		return 0;	
	}	
}

/****************************************************************************
*					levensteinDistance
*
*	Description:	Возвращает расстряние Левенштейна между строками
*	Parameters:	str1, str2 - q-нарные строки
*	Returns:	расстояние Хемминга (int)
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
//int levensteinDistance(const Qstring &str1, const Qstring &str2) {
//	int sz1 = (int) str1.size() + 1;
//	int sz2 = (int) str2.size() + 1;
//	
//	boost::numeric::ublas::matrix<int> d;
//	d.resize(sz1, sz2, false);
//	
//	for (int i = 0; i < sz1; i++) {
//		d(i, 0) = i;
//	}
//	
//	for(int j = 0; j < sz2; j++) {
//			d(0, j) = j;
//	}
//	
//	for(int i = 1; i < sz1; i++) {
//		for(int j = 1; j < sz1; j++) {
//			int cost;
//			
//			if(str1[i] == str2[j]) {
//				cost = 0;
//			} else {
//				cost = 1;
//			}
//			
//			d(i, j) = std::min(d(i-1, j)+1, 
//						std::min(d(i, j-1) + 1, d(i-1, j-1) + cost));
//		}
//	}
//	return d(sz1 - 1, sz2 - 1);
//}

/****************************************************************************
*					levensteinDifference
*
*	Description:	Возвращает расстряние Левенштейна между более короткой строкой и префиксом той же длины более длинной,
*					деленное на длину более длинной строки
*	Parameters:	str1, str2 - q-нарные строки
*	Returns:	нормированное расстояние Левенштейна (double)
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
//double levensteinDifference(const Qstring &str1, const Qstring &str2) {
//	int maxSz =  (int) std::max(str1.size(), str2.size());
//	return (double) levensteinDistance(str1, str2) / (double) maxSz;
//}

};
