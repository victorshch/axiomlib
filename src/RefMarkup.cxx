/****************************************************************************
*			Функции класса RefMarkup.h
*
*	Description:	Эталонная разметка - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "RefMarkup.h"

using namespace AxiomLib;

// Пустой конструктор
RefMarkup::RefMarkup () {
	errorLimit = 1;
}

// Пустой (пока) деструктор
RefMarkup::~RefMarkup () {

}


/****************************************************************************
*					RefMarkup::create
*
*	Description:	Функция заполнения переменных класса
*	Parameters:	newAxiomTypes - вектор разметки ряда аксиомами
*				newMinLen - вектор параметров разметки (минимальное число учитываемых аксиом)
*				newMaxLen - вектор параметров разметки (максимальное число учитываемых аксиом)
*	Returns:		void
*	Throws:		AxiomLibException - если входные параметры разной длины
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkup::create (std::vector<signed int> &newAxiomTypes, std::vector<signed int> &newMinLen, std::vector<signed int> &newMaxLen) {
	// проверка корректности параметров
	if ((newAxiomTypes.size() <= 0) || (newAxiomTypes.size() != newMinLen.size()) || (newAxiomTypes.size() != newMinLen.size())) {
		throw AxiomLibException ("Error in RefMarkup::create: wrong parameters to create RefMurkup ");
	}
	// заолнение переменных данного класса
	this->axiomTypes.resize(0);
	this->axiomTypes = newAxiomTypes;
	this->axiomMinLen.resize(0);
	this->axiomMinLen = newMinLen;
	this->axiomMaxLen.resize(0);
	this->axiomMaxLen = newMaxLen;
}

// Функция изменения параметров разметки
/****************************************************************************
*					RefMarkup::addLens
*
*	Description:	Функция изменения параметров разметки. Если входная разметка 
*				совпадает с разметкой, хранящейся в классе, то параметры разметки 
*				объединяются.
*	Parameters:	newAxiomTypes - вектор разметки ряда аксиомами
*				newMinLen - вектор параметров разметки (минимальное число учитываемых аксиом)
*				newMaxLen - вектор параметров разметки (максимальное число учитываемых аксиом)
*	Returns:		0 - если разметки не совпали и расширения разметок не было
*				1 - если разметки совпали и параметры разметок были объединены
*	Throws:		AxiomLibException - если входный параметры разной длины или пусты
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkup::addLens (std::vector<signed int> &newAxiomTypes, std::vector<signed int> &newMinLen, std::vector<signed int> &newMaxLen) {
	// проверка корректности параметров
	if ((newAxiomTypes.size() <= 0) || (newAxiomTypes.size() != newMinLen.size()) || (newAxiomTypes.size() != newMinLen.size())) {
		throw AxiomLibException ("Error in RefMarkup::addLens: wrong parameters to change RefMurkup ");
	}
	if (this->axiomTypes.size() <= 0) {
		throw AxiomLibException ("Error in RefMarkup::addLens: wrong proccessing of addLens functuion");
	}
	// сравнение разметки рядов 
	if (newAxiomTypes.size() != this->axiomTypes.size()) {
		return 0;
	}
	int flag = 0;
	for (unsigned int i = 0; i < this->axiomTypes.size(); i++) {
		if (this->axiomTypes[i]!= newAxiomTypes[i]) {
			flag = 1;
			break;
		}
	}
	// Если разметка разная - то выходим с false - то есть с 0
	if (flag != 0) {
		return 0;
	}
	// Если разметка ряда одинаковая - то добавляем данные к длинам разметки и выходим с true - то есть 1
	for (unsigned int j = 0; j < this->axiomMinLen.size(); j++) {
		if (this->axiomMinLen[j] > newMinLen[j]) {
			this->axiomMinLen[j] = newMinLen[j];
		}
		if (this->axiomMaxLen[j] < newMaxLen[j]) {
			this->axiomMaxLen[j] = newMaxLen[j];
		}
	}
	return 1;
}


/****************************************************************************
*					RefMarkup::getCopy 
*
*	Description:	Выдает текущее состояние разметки
*	Parameters:		axTypes - заполняемый ряд аксиом (разметка ряда)
*					axMinLen - заполняемое число минимальных вхождений аксиом в разметку
*					axMaxLen - заполняемое число максимальных вхождений аксиом в разметку
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int RefMarkup::getCopy (std::vector<int> &axTypes, std::vector<int> &axMinLen, std::vector<int> &axMaxLen) const {
	axTypes = axiomTypes;
	axMinLen = axiomMinLen;
	axMaxLen = axiomMaxLen;
	return 0;
}


/****************************************************************************
*					RefMarkup::check 
*
*	Description:	Функция проверки - подходит ли разметка
*				класса к вектору расшиеренной разметки во вхоном параметре.
*				То есть проверяется - найдется ли в конце входного ряда аксиом 
*				раметка данного класса. (нахождение в конце - подразумевает,
*				что последняя аксиома расширенной разметки 
*				входного ряда должна совпадать с последней аксиомой 
*				эталонной разметки)
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				axiomLen - параметры разметки
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*	Returns:		0 - false
*				1 - true
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkup::check (std::vector<int> &rowAxiomTypes, std::vector<int> &rowAxiomLen, long int upTo) {
	// Проверка на корректность входных данных
	if (upTo >= (long int) rowAxiomTypes.size()) {
		throw AxiomLibException ("Error in RefMarkup::check: wrong parameters of check functuion");
	}
	// Поиск разметки в ряду
	int toReturn = 0;
	toReturn = subcheck (rowAxiomTypes, rowAxiomLen, upTo, (int) axiomTypes.size() - 1);
	return toReturn;
}


/****************************************************************************
*					RefMarkup::subcheck 
*
*	Description:	Вспомогательная функция для check - рекурсивный вызов 
*				функции проверки совпадания разметок - для учета 
*				того, что число вхождений аксиомы может быть равным 0.
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				axiomLen - параметры разметки
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*				curEnd - текущее положение в эталонной разметке для сравнения
*	Returns:		0 - false
*				1 - true
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkup::subcheck (std::vector<int> &rowAxiomTypes, std::vector<int> &rowAxiomLen, long int upTo, int curEnd) {
	if (curEnd < 0) {
		return 1;
	}
	long int j = upTo;
	int flag = 0;
	for (int i = curEnd; i >= 0; i--) {
		// Не учитываем разметку эталонного ряда - если там -1
		if (axiomTypes[i] == -1) {
			continue;
		}
		// Не учитываем разметку ряда - если там -1 - что означает пустую аксиому
		if ((rowAxiomTypes[j] == -1) && (j != upTo))  {
			j--;
			if (j < 0) {
				// Ряд слишком короткий чтобы что-то определить
				return 0;
			}
		}
		// Если аксиома может и не входить в размету - то проверяем - совпадают ли ряды ее при отсутствии 
		if (axiomMinLen[i] == 0) {
			flag = this->subcheck (rowAxiomTypes, rowAxiomLen, j, curEnd - 1);
			if (flag != 0) {
				return flag;
			}
		}
		// Просто проверка вхождения эталонной последовательности в разметку
		if (axiomTypes[i] != rowAxiomTypes[j]) {
			// Несовпадают типы аксиом
			return 0;
		}
		// Проверка - совпадают ли при этом длины - то есть ссответствие расширенной разметки
		if ((axiomMinLen[i] > rowAxiomLen[j]) || ((axiomMaxLen[i] < rowAxiomLen[j]) && (i != 0) ) ) {
			// Неподходят длины аксиомы
			return 0;
		}
		// Сдвигаемся по ряду - для сравнения следующей аксиомы
		j--;
		if ((j < 0) && (i != 0)) {
			// Ряд слишком короткий чтобы что-то определить
			return 0;
		}
	}
	// раз не выпрыгнули из функции раньше - значит разметка подходит - выходим с true	
	return 1;
}
