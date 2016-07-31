/****************************************************************************
*					TransmuteTpl
*
*	Description:	Функции класса TransmuteTpl - Реализация
*	Author:			dk
*	History:		
*
****************************************************************************/

#include "TransmuteTpl.h"

using namespace AxiomLib;

#define minProfit 0.5 // среднее значение для параметра profit некоторой аксиомы
#define epsilon 0.001 // чтобы избежать проблем при сравнении double'ов
#define cutOffCrit 0.8 // Критерий для величины вероятности мутации аксиомы
#define cutOffVal 0.5 // Во сколько согласно критерию уменьшать вероятность мутаци на уровне аксиом

// Конструктор класса
TransmuteTpl::TransmuteTpl () {
	/*
	axioms.clear();
	axiomsProfit.clear();
	axiomsMask.clear();
	axiomsMaskProb.clear();		
	*/
	
	canMutateOrder = true;
}

// Деструктор класса - пустой
TransmuteTpl::~TransmuteTpl () {
		
}

/****************************************************************************
*					TransmuteTpl::name
*
*	Description:	Возвращает имя данного класса мутации
*	Parameters:		-
*	Returns:		string - имя класса мутации
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string TransmuteTpl::name (void) const {
	return (std::string) "TransmuteTpl";
}


/****************************************************************************
*					TransmuteTpl::setParamsFromEnv
*
*	Description:	Установка параметров мутации из среды
*	Parameters:		const Environment& env - окружение, откуда выбирать параметры
*	Returns:		0
*	Throws:			AxiomLibException - если требуемых параметров нет
*	Author:			dk
*	History:
*
****************************************************************************/
int TransmuteTpl::setParamsFromEnv (const Environment& env) {
	
	if(env.getBoolParamValue(canMutateOrder, "canMutateOrder"))
	{
		canMutateOrder = true;
	}
	
	return 0;
}


/****************************************************************************
*					TransmuteTpl::sort
*
*	Description:	Сортирует элементы вектора vec и заносит отсортированную 
*					последовательность индексов в mask
*	Parameters:		vec - сортируемый вектор
*					mask - последовательности индексов
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/ 
inline void TransmuteTpl::sort (std::vector <double> &vec, std::vector <unsigned short> &mask) const {
	mask.resize(vec.size()); 
	for (unsigned short i = 0; i < vec.size(); i++) 
		mask[i] = i;
	unsigned short temp;
	for (unsigned short i = vec.size() - 1; i > 0; i-- ) {
		for (unsigned short j = 0; j < i; j++) {
			if (vec[mask[j]] < vec[mask[j+1]]) {
				temp = mask[j+1];
				mask[j+1] = mask[j];
				mask[j] = temp;
			}
		}
	}
}


/****************************************************************************
*					TransmuteTpl::transmute
*
*	Description:	Функция мутации системы аксиом, заданной с параметрами
*	Parameters:		axiomSetPlus - особь для мутации
*	Returns:		0 - если все произошло успешно
*					-1 - если мутация не прошла
*					1 - если согласно параметрам мутации и не должно было происходить
*	Throws:			AxiomLibExeption - если параметры мутации выходят за границы 
*					области определения
*	Author:			dk
*	History:
*
****************************************************************************/
int TransmuteTpl::transmute (AxiomSetPlus &axiomSetPlus) const {
	// Проверка значений параметров мутации на соответсвие области значений
	if ((axiomSetPlus.ecTransProb < 0) || (axiomSetPlus.ecTransProb > 1) || 
		(axiomSetPlus.axTransProb < 0) || (axiomSetPlus.axTransProb > 1) || 
		(axiomSetPlus.asTransProb < 0) || (axiomSetPlus.asTransProb > 1) || 
		(axiomSetPlus.prTransProb < 0) || (axiomSetPlus.prTransProb > 1)) {
		throw AxiomLibException("Error in TransmuteTpl::transmute : one or more parameteres is out of range.");
		return -1;
	}

	// Проверка значений параметров мутации - необходима ли мутация
	if ((axiomSetPlus.ecTransProb < epsilon) &&
		(axiomSetPlus.axTransProb < epsilon) &&
		(axiomSetPlus.asTransProb < epsilon) &&
		(axiomSetPlus.prTransProb < epsilon || !canMutateOrder)) {
			return 1;
	}

	// Подготовка внутренних переменных 
	std::vector <Axiom *> * axioms;
	axioms = &(axiomSetPlus.axiomSet->axioms);
	std::vector <unsigned short> axiomsMask;
	if (axioms->size() != axiomSetPlus.axiomProfit.size())
		throw AxiomLibException("TransmuteTpl::transmute : axioms profit is not set.");
	// Устанавливаем значение маски, которая содержит номера аксиом в векторе в порядке уменьшения функции значимости
	sort (axiomSetPlus.axiomProfit, axiomsMask);
	// Критерий выбора какие аксиомы мутировать сильнее, какие слабее - это 80% от максимальной полезности аксиомы (80 - параметр)
	double cutOff;
	cutOff = axiomSetPlus.axiomProfit[axiomsMask[0]] * cutOffCrit;
	double axTransProbMin;
	axTransProbMin = axiomSetPlus.axTransProb * cutOffVal;

	// Мутация на уровне аксиом и элементарных условий
	if ((axiomSetPlus.ecTransProb > 0) || (axiomSetPlus.axTransProb > 0)) {
		for (unsigned int i = 0; i < axioms->size(); i++) {
			if (axiomSetPlus.axiomProfit[i] > cutOff)
				(*axioms)[i]->transmute (axiomSetPlus.ecTransProb, axTransProbMin);
			else
				(*axioms)[i]->transmute (axiomSetPlus.ecTransProb, axiomSetPlus.axTransProb);
		}
	}

	// Мутация на уровне системы аксиом
	if (axiomSetPlus.asTransProb > 0) {
		double probOfTp = 0;
		int numOfEC = 0;
		for (unsigned int i = 0; i < axioms->size(); i++) {
			probOfTp = (double) rand() / (double) RAND_MAX;
			if (probOfTp < axiomSetPlus.asTransProb) {
				numOfEC = (*axioms)[i]->getNumOfEC();
				(*axioms)[i]->clear();
				(*axioms)[i]->initAxiomByRand(numOfEC);
			}
		}
	}

	// Мутация на уровне изменения приоритета аксиом в системе
	// TODO: сделать настройку алгоритма, отключающую эту мутацию (для
	// случая разметки подмножествами системы аксиом)
	if (axiomSetPlus.prTransProb > 0 && canMutateOrder) {
		int numOfSteps = 0;
		double probChange = 0.0;
		for (int i = 1; i < (int) axioms->size(); i++) {
			probChange = (double) rand() / (double) RAND_MAX;
			if (probChange < axiomSetPlus.prTransProb) {
				numOfSteps = round( ((double) rand() / (double) RAND_MAX) * i * axiomSetPlus.prTransProb );
				if (numOfSteps > 0) {
					axiomSetPlus.axiomSet->slide(i-numOfSteps, i);
				}
			}
		}
	}

	return 0;
}


/****************************************************************************
*					TransmuteTpl::transmute
*
*	Description:	Функция мутации для популяции
*	Parameters:		population - изменяемая популяция
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int TransmuteTpl::transmute (std::vector <AxiomSetPlus> &population) const {
	// Ходим по популяции, мутируем составляющие ее особи. При этом сбрасываем устаревшую статистику
	for (unsigned int i = 0; i < population.size(); i++) {
		transmute (population[i]);
		population[i].clearStat();
	}
	return 0;
}
