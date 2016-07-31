/****************************************************************************
*			Функции класса Check.h
*
*	Description:	Класс для проверок - Реализация
*	Author:		dk
*	History:	
*
****************************************************************************/
#include "Check.h"

using namespace AxiomLib;

// Пустой конструктор
Check::Check() {}

// Пустой деструктор
Check::~Check() {}

/****************************************************************************
*				Check::checkAndCompatibility
*
*	Description:	Функция проверки на совсестимость по "и" двух элементарных условий из двух аксиом
*	Parameters:	ax1 - указатель на первую аксиому
*				ecNum1 - номер первого сравнивамого элементарного условия в первой аксиом
*				ax2 - указатель на вторую аксиому
*				ecNum2 - номер второго элементарного условия во второй аксиоме
*	Returns:		1  - если совместимы
*				0  если не совместимы
*				-1  если неопределено
*	Throws:		AxiomLibException - если входные параметры не удовлетворяют диапазону значений
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Check::checkAndCompatibility (Axiom* ax1, int ecNum1, Axiom* ax2, int ecNum2) {
	if ( (ecNum1 < 0) || (ecNum2 < 0) || ( ecNum1 >= ax1->getNumOfEC() ) || ( ecNum2 >= ax2->getNumOfEC() ) )
		throw AxiomLibException ("Error in Check::checkAndCompatibility :given positions of ecs' are out of range.");
	return (ax1->elemConditions[ecNum1])->checkAndCompatibility( *(ax2->elemConditions[ecNum2]) );
}


/****************************************************************************
*				Check::change
*
*	Description:	Функция обмена местами двух элементаных условий из двух аксиом
*	Parameters:	ax1 - указатель на первую аксиому
*				ecNum1 - номер первого элементарного условия в первой аксиоме
*				ax2 - указатель на вторую аксиому
*				ecNum2 - номер второго элементарного условия во второй аксиоме
*	Returns:		0
*	Throws:		AxiomLibException - если входные параметры не удовлетворяют диапазону значений
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Check::change (Axiom* ax1, int ecNum1, Axiom* ax2, int ecNum2) {
	if ( (ecNum1 < 0) || (ecNum2 < 0) || ( ecNum1 >= ax1->getNumOfEC() ) || ( ecNum2 >= ax2->getNumOfEC() ) )
		throw AxiomLibException ("Error in Check::change :given positions of ecs' are out of range.");
	ElemCondition* tmp;
	tmp = ax2->elemConditions[ecNum2];
	ax2->elemConditions[ecNum2] = ax1->elemConditions[ecNum1];
	ax1->elemConditions[ecNum1] = tmp;
	return 0;
}
