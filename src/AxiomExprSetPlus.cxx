/****************************************************************************
*					Функции класса AxiomExprSetPlus
*
*	Description:	Система аксиом от аксиом типа AxiomExpr со статистикой - Реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include "AxiomExprSetPlus.h"
#include "AxiomSetStatistics.h"

using namespace AxiomLib;




bool AxiomExprSetPlus::operator <(const AxiomExprSetPlus &other) const
{
	return this->goal < other.goal;
}

// Пустой конструктор с заданием имени системы аксиом по default'у
AxiomExprSetPlus::AxiomExprSetPlus () {
	goal = -1.0;
	errFirst = -1;
	errSecond = -1;
	axiomsIndex.clear();
	markUps.clear();
}


// Пустой конструктор с заданием имени системы аксиом в параметре
AxiomExprSetPlus::AxiomExprSetPlus (const std::string sName) {
	goal = -1.0;
	errFirst = -1;
	errSecond = -1;
	axiomsIndex.clear();
	markUps.clear();
	nameOfAxiomSet = sName;
}

AxiomExprSetPlus::AxiomExprSetPlus(const AxiomExprSet &as) : AxiomExprSet(as) {
	goal = -1.0;
	errFirst = -1;
	errSecond = -1;	
}

// Деструктор - удаляет все созданные динамические объкты в системе аксиом
AxiomExprSetPlus::~AxiomExprSetPlus(void) {
	axiomsIndex.clear();
	markUps.clear();
}

// Функция удаляет все созданные динамические объкты в системе аксиом
void AxiomExprSetPlus::clear(void) {
	AxiomExprSet::clear();
	goal = -1.0;
	errFirst = -1;
	errSecond = -1;
	axiomsIndex.clear();
	markUps.clear();
}


/****************************************************************************
*					AxiomExprSetPlus::operator=
*
*	Description:	Оператор присваивания системы аксиом со статистикой
*	Parameters:		second - присваиваемая система аксиом
*	Returns:		*this
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
//AxiomExprSetPlus& AxiomExprSetPlus::operator= (const AxiomExprSetPlus &second) {
//	AxiomExprSet::operator =(second);
////	// Копируем имя системы аксиом
////	this->nameOfAxiomSet = second.nameOfAxiomSet;
////	this->nameOfAxiomBank = second.nameOfAxiomBank;
////	// Копируем входящие в систему аксиомы
////	this->clear();
////	axioms.resize(second.axioms.size());
////	for (unsigned int i = 0; i < second.axioms.size(); i++) {
////		axioms[i] = new AxiomExpr;
////		*axioms[i] = *(second.axioms[i]);
////	}
//	// Копируем статистику по системе аксиом
//	this->goal = second.goal;
//	this->errFirst = second.errFirst;
//	this->errSecond = second.errSecond;
//	this->axiomsIndex = second.axiomsIndex;
//	this->markUps = second.markUps;
//	return *this;
//}

int AxiomExprSetPlus::saveAxiomSetToFile(std::string baseDir, std::string axiomSetName, 
										  const std::vector<std::string> &dataSetParams, 
										  int first, int second) const {
	// TODO: сделать везде const std::vector<std::string>& dataSetParams
	std::vector<std::string> temp(dataSetParams);
	int result = AxiomExprSet::saveAxiomSetToFile(baseDir, axiomSetName, temp, first, second);
	
	AxiomSetStatistics statistics(*this);
	statistics.saveToFile(
			baseDir + "/" +axiomSetName + ".stat"
			);
	//std::cout<<"saving as with stat"<<std::endl;
	
	return result;
}

int AxiomExprSetPlus::initAxiomSetFromFile(std::string axiomSetBaseDir, std::string axiomSetName,
										   const std::vector<std::string> &paramNames) {
	
	int result = AxiomExprSet::initAxiomSetFromFile(axiomSetBaseDir,
									   axiomSetName, paramNames);
	
	AxiomSetStatistics statistics;
	statistics.initFromFile(
			axiomSetBaseDir + "/" +axiomSetName + ".stat"
			);
	
	statistics.copyTo(*this);
	
	this->nameOfAxiomSet = axiomSetName;
	
	//std::cout<<"loading as with stat"<<std::endl;
	
	return result;
}

