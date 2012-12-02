/****************************************************************************
*					Функции класса AxiomExprSet
*
*	Description:	Система аксиом от аксиом типа AxiomExpr - Реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include "AxiomExprSet.h"

using namespace AxiomLib;

#define str_default_axiomSet_name	"no_name_AxiomExprSet"
#define str_default_axiomBank_name	"no_name_AxiomBank"


// Пустой конструктор с заданием имени системы аксиом по default'у
AxiomExprSet::AxiomExprSet () {
	nameOfAxiomSet.assign (str_default_axiomSet_name);
	nameOfAxiomBank.assign (str_default_axiomBank_name);
	axioms.clear();
}


// Пустой конструктор с заданием имени системы аксиом в параметре
AxiomExprSet::AxiomExprSet (const std::string sName) {
	nameOfAxiomSet = sName;
	nameOfAxiomBank.assign (str_default_axiomBank_name);
	axioms.clear();
}

// Конструктор копирования - клонирует аксиомы
AxiomExprSet::AxiomExprSet(const AxiomExprSet &other) {
	this->nameOfAxiomBank = other.nameOfAxiomBank;
	this->nameOfAxiomSet = other.nameOfAxiomSet;
	
	this->axioms.reserve(other.axioms.size());
	
	for(unsigned int i = 0; i < other.axioms.size(); i++) {
		this->axioms.push_back(other.axioms[i]->clone());
	}
}

// Деструктор - удаляет все созданные динамические объкты в системе аксиом
AxiomExprSet::~AxiomExprSet(void) {
	this->clear();
}


// Функция удаляет все созданные динамические объкты в системе аксиом
void AxiomExprSet::clear(void) {
	for (unsigned int i = 0; i < this->axioms.size(); i++) {
		(axioms[i])->clear();
		delete (axioms[i]);
	}
	axioms.clear();
}


// Добавляет аксиому к существующему ветору аксиом в данной системе
void AxiomExprSet::addAxiom (const AxiomExpr& newAxiom) {
	unsigned int oldSize = axioms.size();
	axioms.resize (oldSize + 1);
	axioms[oldSize] = new AxiomExpr;
	*(axioms[oldSize]) = newAxiom;
}


// Добавляет аксиомы к существующему ветору аксиом в данной системе
void AxiomExprSet::addAxioms (std::vector <AxiomExpr*> &newAxiomExpr) {
	unsigned int oldSize = axioms.size();
	axioms.resize (oldSize + newAxiomExpr.size());
	unsigned int j = 0;
	for (unsigned int i = oldSize; i < axioms.size(); i++, j++) {
		*axioms[i] = *newAxiomExpr[j];
	}
}


// Устанавливает новый вектор аксиом
void AxiomExprSet::setAxioms (std::vector <AxiomExpr*> &newAxiomExpr) {
	clear();
	addAxioms (newAxiomExpr);
}


// Возвращает название системы аксиом
std::string AxiomExprSet::name (void) const {
	return nameOfAxiomSet;
}


// Функция возвращает число аксиом в данной системе
int AxiomExprSet::size (void) const {
	return axioms.size();
}


/****************************************************************************
*					AxiomExprSet::enter
*
*	Description:	Функция разметки многомерного ряда row начиная с отсчета begin до отсчета end
*	Parameters:		marked - вектор, куда кладем разметку
*					row    - многомерный ряд
*					begin  - отсчет с которого начинать разметку
*					end    - отсчет до которого вести разметку
*	Returns:		0
*	Throws:			AxiomLibExeption - если begin и end заданы некорректно
*	Author:			dk
*	History:
*
****************************************************************************/
int AxiomExprSet::enter (std::vector<int>& marked, const std::vector < std::vector<double> > &row, const unsigned long begin, const unsigned long end) const {
	// Проверка входных параметров
	unsigned int len = 0;
	for (unsigned int i = 0; i < row.size(); i++) {
		if (row[i].size() > len)
			len = row[i].size();
	}
	if ((end <= begin) || (begin < 0) || (end > len)) {
		throw AxiomLibException("Error in AxiomExprSet::enter : wrong input parameters.");
	}	
	// Разметка ряда аксиомами системы
	int curRes;
	marked.resize(end - begin);
	unsigned long k = 0;
	for (unsigned long i = begin; i < end; i++, k++) {
		marked[k] = 0;
		for (unsigned int j = 0; j < axioms.size(); j++) {
			curRes = (axioms[j])->check (i, row);
			if (curRes > 0) {
				marked[k] = j + 1;
				break;
			}
		}
	}
 	return 0;
}


/****************************************************************************
*				AxiomExprSet::enter
*
*	Description:	Функция разметки ряда row начиная с отсчета begin до отсчета end
*	Parameters:		marked - вектор, куда кладем разметку
*					row    - ряд для разметки
*					begin - отсчет с которого начинать разметку
*					end    - отсчет до которого вести разметку
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*					-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:			AxiomLibExeption - если begin и end заданы не корректно
*	Author:			dk
*	History:
*
****************************************************************************/
int AxiomExprSet::enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end) {
	// Проверка входных параметров
	if ((end <= begin) || (row.size() < end)) {
		marked.clear();
		throw AxiomLibException("Error in AxiomExprSet::enter : wrong input parameters.");
	}
	// Разметка ряда аксиомами системы
	int curRes;
	marked.resize(end - begin);
	unsigned long k = 0;
	for (unsigned long i = begin; i < end; i++, k++) {
		marked[k] = 0;
		curRes = 0;
		for (unsigned int j = 0; j < axioms.size(); j++ ) {
			curRes = (axioms[j])->check(i, row);
			if (curRes > 0) {
				marked[k] = j + 1;
				break;
			}
		}
	}
 	return 0;
}


/****************************************************************************
*				AxiomExprSet::enter
*
*	Description:	Функция разметки ряда row начиная с отсчета begin до отсчета end
*	Parameters:		marked - вектор, куда кладем разметку
*					row    - ряд для разметки
*					begin - отсчет с которого начинать разметку
*					end    - отсчет до которого вести разметку
*					stat - статистика по аксиомам системы, какие были использованы при разметке
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*					-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:			AxiomLibExeption - если begin и end заданы не корректно
*	Author:			dk
*	History:
*
****************************************************************************/
int AxiomExprSet::enter (std::vector<int>& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat) {
	// Проверка входных параметров
	if ((end <= begin) || (row.size() < end)) {
		marked.resize(0);
		throw AxiomLibException("Error in AxiomExprSet::enter : wrong parameters.");
	}
	// Разметка ряда аксиомами системы со сбором статистики
	int curRes;
	marked.resize(end - begin);
	stat.resize(axioms.size());
	for (unsigned int i = 0; i < axioms.size(); i++)
		stat[i] = false;
	unsigned long k = 0;
	for (unsigned long i = begin; i < end; i++, k++) {
		marked[k] = 0;
		curRes = 0;
		for (unsigned int j = 0; j < axioms.size(); j++ ) {
			curRes = (axioms[j])->check(i, row);
			if (curRes > 0) {
				marked[k] = j + 1;
				if (!stat[j]) stat[j] = true;
				break;
			}
		}
	}
 	return 0;
}

/****************************************************************************
*				AxiomExprSet::enter
*
*	Description:	Функция разметки ряда в случае разметки подмножествами аксиом
*                               row начиная с отсчета begin до отсчета end
*	Parameters:		marked - вектор, куда кладем разметку
*					row    - ряд для разметки
*					begin - отсчет с которого начинать разметку
*					end    - отсчет до которого вести разметку
*					stat - статистика по аксиомам системы, какие были использованы при разметке
*	Returns:		Функция вовзращает размеченный ряд - ряд с номером аксиомы выполненной на каждом из отсчетов исходного ряда
*					-1 - означает что ни одна аксиома не выполнена - то есть нулевая аксиома записывается под этим значением
*	Throws:			AxiomLibExeption - если begin и end заданы не корректно
*	Author:			armkor
*	History:
*
****************************************************************************/

int AxiomExprSet::enter (MultiMarking::MultiMark& marked, const std::vector<double>& row, const unsigned long begin, const unsigned long end, std::vector<bool> &stat){
    if ((end <= begin) || (row.size() < end)) {
            marked.resize(0);
            throw AxiomLibException("Error in AxiomExprSet::enter : wrong parameters.");
    }
    else {
        int curRes;
        marked.resize(end - begin);
        for (unsigned long i=begin;i<end;i++){
            marked[i-begin].resize(axioms.size());
            for (int j=0;j<axioms.size();j++){
                marked[i-begin][j]=false;
            }
        }
        stat.resize(axioms.size());
        for (unsigned int i = 0; i < axioms.size(); i++){
                stat[i] = false;
            }
        for (unsigned long i = begin; i < end; i++) {
                curRes = 0;
                for (unsigned int j = 0; j < axioms.size(); j++ ) {
                        curRes = (axioms[j])->check(i, row);
                        if (curRes > 0) {
                                marked[i-begin][j] = true;
                                if (!stat[j]) stat[j] = true;
                        }
                }
        }

}
    return 0;

}


/****************************************************************************
*				AxiomExprSet::operator=
*
*	Description:	Оператор присваивания системы аксиом
*	Parameters:		second - присваиваемая система аксиом
*	Returns:		*this
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
AxiomExprSet& AxiomExprSet::operator= (const AxiomExprSet &second) {
	// Копируем имя системы аксиом
	this->nameOfAxiomSet = second.nameOfAxiomSet;
	this->nameOfAxiomBank = second.nameOfAxiomBank;
	// Копируем входящие в систему аксиомы
	this->clear();
	axioms.resize(second.axioms.size());
	for (unsigned int i = 0; i < second.axioms.size(); i++) {
		axioms[i] = second.axioms[i]->clone();
	}
	return *this;
}


/****************************************************************************
*					AxiomExprSet::createAxiomExprSetStructure
*
*	Description:	Функция сохраняет структуру данной системы аксиом в AxiomExprSetStructures
*	Parameters:		aess - заполняемое поле структуры системы аксиом
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int AxiomExprSet::createAxiomExprSetStructure (AxiomExprSetStructure &aess) const {
	aess.axiomBankDir = nameOfAxiomBank;
	aess.axiomSetName = nameOfAxiomSet;
	aess.axioms.clear();
	aess.axioms.resize(this->axioms.size());
	for (unsigned int i = 0; i < this->axioms.size(); i++)
		axioms[i]->createAxiomExprStructure (aess.axioms[i]);
	return 0;
}


/****************************************************************************
*					AxiomExprSet::initAxiomSetFromFile
*
*	Description:	Функция задающая систему аксиом как набор строго определенных аксиом
*	Parameters:		axiomBaseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					axiomSetName - имя системы аксиом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExprSet::initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName) {
	AxiomSetBase axiomSetBase;
	AxiomExprSetStructure aess;
	// Формируем полный путь к файлу
	std::string axiomSetPath = axiomSetBaseDir;
	axiomSetPath.append("/");
	axiomSetPath.append(axiomSetName);
	// Считываем информацию о системе аксиом в aess
	axiomSetBase.readFromAS (axiomSetPath, aess);
	return initAxiomSetFromStructure (aess);
}

/****************************************************************************
*					AxiomExprSet::initAxiomSetFromFile
*
*	Description:	Функция задающая систему аксиом как набор строго определенных аксиом.
*					Сделана по аналогии с предыдущей, только добавлена обработка имен размерностей
*	Parameters:		axiomBaseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					axiomSetName - имя системы аксиом
*					paramNames - имена размерностей
*	Returns:		0
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
signed int AxiomExprSet::initAxiomSetFromFile (std::string axiomSetBaseDir, std::string axiomSetName, const std::vector<std::string> &paramNames) {
	AxiomSetBase axiomSetBase;
	AxiomExprSetStructure aess;
	// Формируем полный путь к файлу
	std::string axiomSetPath = axiomSetBaseDir;
	axiomSetPath.append("/");
	axiomSetPath.append(axiomSetName);
	
	// TODO: убрать это и добавить const в соответствующие места
	std::vector<std::string> temp(paramNames);
	
	// Считываем информацию о системе аксиом в aess
	axiomSetBase.readFromAS (axiomSetPath, aess, temp);
	return initAxiomSetFromStructure (aess);
}


/****************************************************************************
*					AxiomExprSet::initAxiomSetFromStructure
*
*	Description:	Функция задающая систему аксиом по специализированной структуре с описанием системы акисом
*	Parameters:		aess - структура с описанием системы аксиом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExprSet::initAxiomSetFromStructure (AxiomExprSetStructure &aess) {
	this->clear();
	if (aess.axiomSetName.size() > 0)
		nameOfAxiomSet = aess.axiomSetName;
	if (aess.axiomBankDir.size() > 0)
		nameOfAxiomBank = aess.axiomBankDir;
	AxiomExpr* axiomExpr;
	this->axioms.resize(aess.axioms.size());
	for (unsigned int i = 0; i < aess.axioms.size(); i++) {
		axiomExpr = new AxiomExpr(aess.axioms[i].axiomName);
		axiomExpr->initAxiomFromStructure (aess.axioms[i]);			
		this->axioms[i] = axiomExpr;
	}	
	return 0;
}


/****************************************************************************
*					AxiomExprSet::initAxiomSetByRand
*
*	Description:	Функция задания системы аксиом случайным образом
*	Parameters:		numOfAxioms  - число случано выбириаемых аксиом
*					maxECinAxiom - максимальное чилсо элементарных условий которые могут составлять одну аксиому
*	Returns:		0 - если все нормальо, в противном случае выход по throw 
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int AxiomExprSet::initAxiomSetByRand (const int numOfAxioms, const int maxECinAxiom) {
	return initAxiomSetByRand (numOfAxioms, maxECinAxiom, -1, -1);
}


/****************************************************************************
*					AxiomExprSet::initAxiomSetByRand
*
*	Description:	Функция задания системы аксиом случайным образом с указанием 
*					размерности набора данных для каждого условия из заданного диапазона
*	Parameters:		numOfAxioms  - число случано выбириаемых аксиом
*					maxECinAxiom - максимальное чилсо элементарных условий которые могут составлять одну аксиому
*					dimensionMin - минимальное значение для размерности исходных данных
*					dimensionMax - максимальное значение для размерности исходных данных
*	Returns:		0 - если все нормальо, в противном случае выход по throw 
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int AxiomExprSet::initAxiomSetByRand (const int numOfAxioms, const int maxECinAxiom, const int dimensionMin, const int dimensionMax) {
	this->clear();
	int numOfEC;
	// Изменяем число аксиом в данной системе
	this->axioms.resize(numOfAxioms);
	for (int i = 0; i < numOfAxioms; i++) {
		this->axioms[i] = new AxiomExpr();
		// Случайным образом определяем сколько Элементарных Условий будут входить в i-ую аксиому
		numOfEC = round(((double) rand() / (double) RAND_MAX) * (maxECinAxiom-1) + 1);
		// Определяем случайно новую аксиому
		this->axioms[i]->initAxiomByRand(numOfEC, dimensionMin, dimensionMax);
	}
	return 0;
}


/****************************************************************************
*					AxiomExprSet::saveAxiomSetToFile
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					AxiomSetName - имя, под которым сохрянять данную систему аксиом
*					first - число ошибок I рода данной системы аксиом на некоторой выборке
*					second - число ошибок II рода данной системы аксиом на некоторой выборке
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExprSet::saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, int first, int second) const {
	std::vector <std::string> dataSetParams;
	dataSetParams.clear();
	saveAxiomSetToFile (baseDir, axiomSetName, dataSetParams, first, second);
	return 0;
}


/****************************************************************************
*					AxiomExprSet::saveAxiomSetToFile
*
*	Description:	Функция сохранения системы аксиом в файл
*					(с указанием размерости исходных данных для каждого элементарного условия)
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					AxiomSetName - имя, под которым сохрянять данную систему аксиом
*					dataSetParams - вектор названий размерностей исходных данных
*					first - число ошибок I рода данной системы аксиом на некоторой выборке
*					second - число ошибок II рода данной системы аксиом на некоторой выборке
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExprSet::saveAxiomSetToFile (std::string baseDir, std::string axiomSetName, std::vector <std::string> &dataSetParams, int first, int second) const {
	AxiomExprSetStructure aess;
	createAxiomExprSetStructure (aess);
	aess.axiomSetName = axiomSetName;
	AxiomSetBase axiomSetBase;
	axiomSetBase.saveToAS (baseDir, aess, dataSetParams, first, second);
	return 0;
}
