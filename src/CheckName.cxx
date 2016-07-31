/****************************************************************************
*					CheckName.cxx
*
*	Description:	Функции из хедера CheckName - Реализация
*	Author:			dk
*	History:		
*
****************************************************************************/

#include "CheckName.h"

using namespace AxiomLib;

/****************************************************************************
*					checkName
*
*	Description:	Функция сравнения строк.
*				Кроме того отдельно указывается с какого символа в каждой 
*				строке начинать сравнение и до какого символа его вести.
*				Используется для сравнения с шаблоном имен файлов.
*	Parameters:	name1 - строка для сравнения
*				from -номер символа в строке name1 с которого начинать сравнение
*				upTo - номер символа в строке name1 до которого вести сравнение
*				name2 - строка для сравнения
*	Returns:	true  - если первый файл подходит под второй как под шаблон
*				false - в противном случае
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/

// VARIANT 0
bool checkName (std::string name1, signed int from, signed int upTo, std::string name2) {
	std::basic_string <char>::size_type indexCh1, indexCh2;
	static const std::basic_string <char>::size_type npos = (unsigned int) -1;
	const char * cName2 = name2.c_str();
	if ((from >= upTo) || (from > (signed int) name1.size())) {
		return false;
	}
	if (upTo > (int) name1.size()) {
		upTo = (int) name1.size() - 1;
	}
	indexCh1 = name1.rfind ( name2 , upTo );
	if (from < 0) {
		from = 0;
		indexCh2 = npos;
	}
	else {
		indexCh2 = name1.rfind ( name2 , from );
	}
	if ((indexCh1 != npos) && (indexCh2 == npos)) {
		// все верно - есть до upTo и нет до from
		return true;
	}
	return false;
};

// VARIANT 1
/*bool checkName (std::string name1, signed int from, signed int upTo, std::string name2) {
	std::basic_string <char>::size_type indexCh1, indexCh2;
	static const std::basic_string <char>::size_type npos = (std::basic_string <char>::size_type) -1;
	const char * cName2 = name2.c_str();
	if ((from >= upTo) || (from > (signed int) name1.size())) {
		return false;
	}
	if (upTo > (int) name1.size()) {
		upTo = (int) name1.size() - 1;
	}
	indexCh1 = name1.rfind ( name2 , upTo );
	if (from < 0) {
		from = 0;
		indexCh2 = npos;
	}
	else {
		indexCh2 = name1.rfind ( name2 , from );
	}
	if ((indexCh1 != npos) && (indexCh2 == npos)) {
		// все верно - есть до upTo и нет до from
		return true;
	}
	return false;
}*/

// VARIANT 2
/*bool checkName (std::string name1, signed int from, signed int upTo, std::string name2) {
	std::basic_string <char>::size_type indexCh1, indexCh2;
	static const std::basic_string <char>::size_type npos = (long) -1;
	const char * cName2 = name2.c_str();
	if ((from >= upTo) || (from > (signed int) name1.size())) {
		return false;
	}
	if (upTo > (int) name1.size()) {
		upTo = (int) name1.size() - 1;
	}
	indexCh1 = name1.rfind ( name2 , upTo );
	if (from < 0) {
		from = 0;
		indexCh2 = npos;
	}
	else {
		indexCh2 = name1.rfind ( name2 , from );
	}
	if ((indexCh1 != npos) && (indexCh2 == npos)) {
		// все верно - есть до upTo и нет до from
		return true;
	}
	return false;
}*/

// VARIANT 3
/*bool checkName (std::string name1, signed int from, signed int upTo, std::string name2) {
	std::basic_string <char>::size_type indexCh1, indexCh2;
	static const std::basic_string <char>::size_type npos = (unsigned long) -1;
	const char * cName2 = name2.c_str();
	if ((from >= upTo) || (from > (signed int) name1.size())) {
		return false;
	}
	if (upTo > (int) name1.size()) {
		upTo = (int) name1.size() - 1;
	}
	indexCh1 = name1.rfind ( name2 , upTo );
	if (from < 0) {
		from = 0;
		indexCh2 = npos;
	}
	else {
		indexCh2 = name1.rfind ( name2 , from );
	}
	if ((indexCh1 != npos) && (indexCh2 == npos)) {
		// все верно - есть до upTo и нет до from
		return true;
	}
	return false;
}*/

// VARIANT 4
/*#define maxAddrLen 1000
bool checkName (std::string name1, signed int from, signed int upTo, std::string name2) {
	std::basic_string <char>::size_type indexCh1, indexCh2;
	static const std::basic_string <char>::size_type npos = maxAddrLen;
	const char * cName2 = name2.c_str();
	if ((from >= upTo) || (from > (signed int) name1.size())) {
		return false;
	}
	if (upTo > (int) name1.size()) {
		upTo = (int) name1.size() - 1;
	}
	indexCh1 = name1.rfind ( name2 , upTo );
	if (from < 0) {
		from = 0;
		indexCh2 = npos;
	}
	else {
		indexCh2 = name1.rfind ( name2 , from );
	}
	if ((indexCh1 < npos) && (indexCh1 >= 0) && ((indexCh2 >= npos) || (indexCh2 < 0))) {
		// все верно - есть до upTo и нет до from
		return true;
	}
	return false;
}*/
