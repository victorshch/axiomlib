/****************************************************************************
*			Функции класса RefMarkupSetSimple.h
*
*	Description:	Эталонная разметка для класса неисправностей - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "RefMarkupSetSimple.h"

using namespace AxiomLib;

// Конструктор  - с заданием внутренних переменных по умолчанию
RefMarkupSetSimple::RefMarkupSetSimple () {
	refMarkups.resize(0);
}


// Пустой деструктор
RefMarkupSetSimple::~RefMarkupSetSimple () {
}


/****************************************************************************
*                       RefMarkupSetSimple::setParamsFromEnv
*
*       Description:	Инициализировать параметры данными из env
*       Parameters:	const Environment& env - среда
*       Returns:		0
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int RefMarkupSetSimple::setParamsFromEnv (const Environment& env) {
	// Передаем установку параметров на уровень ниже
	rmu.setParamsFromEnv (env);
	return 0;
}


/****************************************************************************
*					RefMarkupSetSimple::addRefMarkup
*
*	Description:	Добавление новой эталонной разметки данной системой аксиом
*	Parameters:	refPattern - временной ряд для разметки системой аксиом и добавления 
*				к эталонной разметке
*				as - система аксиом для разметки
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSetSimple::addRefMarkup(std::vector<double>& refPattern, AxiomSet& as, const bool comments) {
	// Создаем ряд для хранения разметки
	std::vector<int> axiomTypes;
	// Размечаем временной ряд
	as.enter(axiomTypes, refPattern, 0, (unsigned long) refPattern.size());
	// Тестовый вывод разметки эталонной траектории
	if (comments) {
		std::cout << "\n	Axioms";
		for (unsigned int i = 0; i  < axiomTypes.size(); i++) {
			std::cout << "  " << axiomTypes[i];
		}
	}
	// Запись произведенной разметки
	int flag = 0;
	std::vector<RefMarkupSimple>::iterator setIter;
	// Пробуем добавить параметры к уже существуеющей такой же разметкой
	for (setIter = refMarkups.begin(); setIter != refMarkups.end(); ++setIter) {
		if (*setIter == axiomTypes) {
			flag = 1;
			break;
		}
	}
	// Если такой же разметки не нашлось - то добавляем ее как новую
	if (flag == 0) {
		//RefMarkupSimple rmu;
		rmu.create(axiomTypes);
		refMarkups.resize(refMarkups.size() + 1);
		refMarkups[refMarkups.size() - 1] = rmu;
	}
	if (comments) std::cout << "New size is " << refMarkups.size() << "\n";
}


/****************************************************************************
*					RefMarkupSetSimple::addRefMarkup
*
*	Description:	Добавление новой эталонной разметки к данному классу
*	Parameters:	refRes - добавляемая разметка в стандартном виде - последовательность аксиом без ограничений
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSetSimple::addRefMarkup(std::vector<int> &axiomTypes, const bool comments) {
	// Тестовый вывод разметки эталонной траектории
	if (comments) {
		std::cout << "\n	Axioms";
		for (unsigned int i = 0; i  < axiomTypes.size(); i++) {
			std::cout << "  " << axiomTypes[i];
		}
	}
	// Запись произведенной разметки
	int flag = 0;
	std::vector<RefMarkupSimple>::iterator setIter;
	// Пробуем добавить параметры к уже существуеющей такой же разметкой
	for (setIter = refMarkups.begin(); setIter != refMarkups.end(); ++setIter) {
		if (*setIter == axiomTypes) {
			flag = 1;
			break;
		}
	}
	// Если такой же разметки не нашлось - то добавляем ее как новую
	if (flag == 0) {
		//RefMarkupSimple rmu;
		rmu.create(axiomTypes);
		refMarkups.resize(refMarkups.size() + 1);
		refMarkups[refMarkups.size() - 1] = rmu;
		//rmu.setOut();
	}
	if (comments) std::cout << "New size is " << refMarkups.size() << "\n";
}


/****************************************************************************
*					RefMarkupSetSimple::clear
*
*	Description:	Функция очищения класса - удаление всех добавленных разметок 
*	Parameters:	-
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkupSetSimple::clear () {
	if (refMarkups.size() > 0) {
		for (unsigned int i = 0; i < refMarkups.size(); i++) {
			(refMarkups[i]).clear();
		}
		refMarkups.clear();
	}
	return 0;
}


/****************************************************************************
*					RefMarkupSetSimple::check 
*
*	Description:	Функция проверки - подходит ли какая либо из разметок
*				класса к вектору расшиеренной разметки во вхоном параметре.
*				То есть проверяется - найдется ли в конце входного ряда аксиом 
*				какая либо из раметок данного класса. (нахождение в конце - 
*				подразумевает, что последняя аксиома разметки 
*				входного ряда должна совпадать с последней аксиомой 
*				эталонной разметки)
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkupSetSimple::check (const double newMaxDistance, std::vector<int> &axiomTypes, long int upTo) {
	// Пробегаем по всем разметкам и смотрим - найдется ли подходящая
	//RefMarkupSimple temp;
	std::vector<RefMarkupSimple>::iterator setIter;
	int flag = 0;
	// Сравниваем с каждой из разметок
	for (setIter = refMarkups.begin(); setIter != refMarkups.end(); ++setIter) {
		//temp = *setIter; 
		//flag = temp.check(axiomTypes, upTo);
		flag = (*setIter).check(newMaxDistance, axiomTypes, upTo);
		if (flag != 0) {
			return flag;
		}
	}
	return 0;
}


/****************************************************************************
*					RefMarkupSetSimple::operator=
*
*	Description:	Оператор присваивания
*	Parameters:		Присваиваемый объект класса разметок
*	Returns:		*this
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
RefMarkupSetSimple& RefMarkupSetSimple::operator= (const RefMarkupSetSimple& second) {
	rmu = second.rmu;
	refMarkups.clear();
	refMarkups = second.refMarkups;
	return *this;
}
