/****************************************************************************
*			Функции класса RefMarkupSet.h
*
*	Description:	Эталонная разметка для класса неисправностей - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "RefMarkupSet.h"
#include "RefMarkup.h"

using namespace AxiomLib;

// Конструктор  - с заданием внутренних переменных по умолчанию
RefMarkupSet::RefMarkupSet () {
	precision = 0.0;
	refMarkups.resize(0);
}


// Пустой (пока) деструктор
RefMarkupSet::~RefMarkupSet () {

}


// Функция задания новых значений внутренних переменных
int RefMarkupSet::setPrecision (const double newPrecision) {
	if ((newPrecision < 0.0) || (newPrecision > 1.0)) {
		throw AxiomLibException ("Error in RefMarkupSet::setPrecision: input parameter out of range");
	}
	precision = newPrecision;
	return 0;
}


/****************************************************************************
*					RefMarkupSet::processRes
*
*	Description:	Функция генерации расширенной разметки ряда по 
*				обычной разметке - когда каждой точке соответсвует 
*				какая-либо аксиома
*	Parameters:	resRef - исходная разметка ряда
*				axiomTypes - заполняемая разметка ряда
*				axiomMinLen - заполняемые параметры разметки
*				axiomMaxLen - заполняемые параметры разметки
*	Returns:		0
*	Throws:		AxiomLibException - если исходная разметка пуста
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkupSet::processRes (std::vector<int> &resRef, std::vector<int> &axiomTypes, std::vector<int> &axiomMinLen, std::vector<int> &axiomMaxLen) {
	// Проверка на корректность параметров
	if (resRef.size() <= 0) {
		throw AxiomLibException ("Error in RefMarkupSet::processRes: empty resRef - nothing to process.");
	}
	// Подсчитываем число элементов в заполняемых векторах
	int numOfCng = 1;
	for (unsigned int j = 1; j < resRef.size(); j++) {
		if (resRef[j] != resRef[j-1]) {
			numOfCng++;
		}
	}
	// Подготавливаем к заполнению вектора
	int curMin = 1;
	int curMax = 1;
	axiomTypes.resize(numOfCng);
	axiomTypes[0] = resRef[0];
	axiomMinLen.resize(numOfCng);
	axiomMaxLen.resize(numOfCng);
	// Переходим к итеративному заполнению векторов
	int h = 1;
	for (int i = 1; i < (int) resRef.size(); i++) {
		if (resRef[i] != resRef[i-1]) {
			axiomTypes[h] = resRef[i];
			axiomMinLen[h-1] = (int) ( (double) curMin * (1.0 - precision) );
			//std::cout << "\n 		curMin is " << curMin << "\n";
			//std::cout << " 		minLen[h-1] is " << axiomMinLen[h-1] << "\n";
			curMin = 1;
			axiomMaxLen[h-1] =(int) ( (double) curMax * (1.0 + precision) );
			curMax = 1;
			h++;
		}
		else {
			curMin++;
			curMax++;
		} 
	}
	axiomMinLen[h-1] = (int) ( (double) curMin * (1.0 - precision) );
	axiomMaxLen[h-1] = (int) ( (double) curMax * (1.0 + precision) );
	return 0;
}


/****************************************************************************
*					RefMarkupSet::addRefMarkup
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
void RefMarkupSet::addRefMarkup(std::vector<double>& refPattern, AxiomSet& as, const bool comments) {
	//std::cout << "Adding reference markup to the set with size " << refMarkups.size() << ": ";

	// Создаем ряд для хранения разметки
	std::vector<int> resRef;
	// Размечаем временной ряд
	as.enter(resRef, refPattern, 0, (unsigned long) refPattern.size());
	// Подготавливаем ряды для записи расширенной разметки
	std::vector<int> axiomTypes;
	std::vector<int> axiomMinLen;
	std::vector<int> axiomMaxLen;
	// Производство расширенной разметки
	processRes (resRef, axiomTypes, axiomMinLen, axiomMaxLen);
	if (comments) {
		std::cout << "\n	Axioms";
		for (unsigned int i = 0; i  < axiomTypes.size(); i++) {
			std::cout << "  " << axiomTypes[i];
		}
		std::cout << "\n	MinLen";
		for (unsigned int i = 0; i  < axiomMinLen.size(); i++) {
			std::cout << "  " << axiomMinLen[i];
		}
		std::cout << "\n	MaxLen";
		for (unsigned int i = 0; i  < axiomMaxLen.size(); i++) {
			std::cout << "  " << axiomMaxLen[i];
		}
		std::cout << "\n";
	}
	// Запись произведенной разметки
	int flag = 0;
	RefMarkup temp;
	std::vector<RefMarkup>::iterator setIter;
	// Пробуем добавить параметры к уже существуеющей такой же разметкой
	for (setIter = refMarkups.begin(); setIter != refMarkups.end(); ++setIter) {
		temp = *setIter; 
		flag = temp.addLens (axiomTypes, axiomMinLen, axiomMaxLen);
		if (flag != 0) {
			break;
		}
	}
	// Если такой же разметки не нашлось - то добавляем ее как новую
	if (flag == 0) {
		RefMarkup rmu;
		rmu.create(axiomTypes, axiomMinLen, axiomMaxLen);
		refMarkups.resize(refMarkups.size() + 1);
		refMarkups[refMarkups.size() - 1] = rmu;
//		cout << " flag = 0.";
	}
//	cout << "New size is " << refMarkups.size() << "\n";	
}


/****************************************************************************
*					RefMarkupSet::addRefMarkup
*
*	Description:	Добавление новой эталонной разметки к данному классу
*	Parameters:	refRes - добавляемая разметка в стандартном виде - последовательность аксиом без ограничений
*	Returns:		void
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void RefMarkupSet::addRefMarkup(std::vector<int> &resRef, const bool comments) {
	// Подготавливаем ряды для записи расширенной разметки
	std::vector<int> axiomTypes;
	std::vector<int> axiomMinLen;
	std::vector<int> axiomMaxLen;
	// Производство расширенной разметки
	processRes (resRef, axiomTypes, axiomMinLen, axiomMaxLen);
	if (comments) {
		std::cout << "\n	Axioms";
		for (unsigned int i = 0; i  < axiomTypes.size(); i++) {
			std::cout << "  " << axiomTypes[i];
		}
		std::cout << "\n	MinLen";
		for (unsigned int i = 0; i  < axiomMinLen.size(); i++) {
			std::cout << "  " << axiomMinLen[i];
		}
		std::cout << "\n	MaxLen";
		for (unsigned int i = 0; i  < axiomMaxLen.size(); i++) {
			std::cout << "  " << axiomMaxLen[i];
		}
		std::cout << "\n";
	}
	// Запись произведенной разметки
	int flag = 0;
	RefMarkup temp;
	std::vector<RefMarkup>::iterator setIter;
	// Пробуем добавить параметры к уже существуеющей такой же разметкой
	for (setIter = refMarkups.begin(); setIter != refMarkups.end(); ++setIter) {
		temp = *setIter; 
		flag = temp.addLens (axiomTypes, axiomMinLen, axiomMaxLen);
		if (flag != 0) {
			break;
		}
	}
	// Если такой же разметки не нашлось - то добавляем ее как новую
	if (flag == 0) {
		RefMarkup rmu;
		rmu.create(axiomTypes, axiomMinLen, axiomMaxLen);
		refMarkups.resize(refMarkups.size() + 1);
		refMarkups[refMarkups.size() - 1] = rmu;
//		cout << " flag = 0.";
	}
//	cout << "New size is " << refMarkups.size() << "\n";	
}


/****************************************************************************
*					RefMarkupSet::check 
*
*	Description:	Функция проверки - подходит ли какая либо из разметок
*				класса к вектору расшиеренной разметки во вхоном параметре.
*				То есть проверяется - найдется ли в конце входного ряда аксиом 
*				какая либо из раметок данного класса. (нахождение в конце - 
*				подразумевает, что последняя аксиома расширенной разметки 
*				входного ряда должна совпадать с последней аксиомой 
*				эталонной разметки)
*	Parameters:	axiomTypes - входной ряд аксиом (разметка ряда)
*				axiomLen - параметры разметки
*				upTo - конец ряда (ряд возможно заполнен лишь до этой точки)
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int RefMarkupSet::check (std::vector<int> &axiomTypes, std::vector<int> &axiomLen, long int upTo) {
	// Пробегаем по всем разметкам и смотрим - найдется ли подходящая
	RefMarkup temp;
	std::vector<RefMarkup>::iterator setIter;
	int flag = 0;
//	cout << "Checking the set with size" << refMarkups.size() << ": ";
	// Пробуем добавить параметры к уже существуеющей такой же разметкой
	for (setIter = refMarkups.begin(); setIter != refMarkups.end(); ++setIter) {
		temp = *setIter; 
		flag = temp.check(axiomTypes, axiomLen, upTo);
		if (flag != 0) {
			return flag;
		}
//		cout << ".";
	}
//	cout << "\n";
	return 0;
}
