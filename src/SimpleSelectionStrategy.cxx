/****************************************************************************
*			SimpleSelectionStrategy
*
*	Description:	Функции класса SimpleSelectionStrategy - Реализация
*	Author:		gevor
*	History:		
*
****************************************************************************/
#include "SimpleSelectionStrategy.h"
#include "AxiomLibException.h"

using namespace AxiomLib;
using namespace std;

/****************************************************************************
*			SimpleSelectionStrategy::setParamsFromEnv
*
*	Description:	Установка параметров селекции из среды
*	Parameters:	const Environment& env - откуда ставить
*	Returns:	0
*	Throws:		AxiomLibException - если параметра нет
*	Author:		gevor
*	History:
*
****************************************************************************/
int SimpleSelectionStrategy::setParamsFromEnv (const Environment& env)
{
  if (env.getDoubleParamValue(pBest, "selectNumBest") < 0) {
    throw AxiomLibException ("Error in SimpleSelectionStrategy::setParamsFromEnv - can not find all needed parameners.");
  }
  return 0;
}


/****************************************************************************
*			SimpleSelectionStrategy::sort
*
*	Description:	Функция сортировки вектора индексов mask по вектороу значений  goals 
*	Parameters:	mask - сотрируемый вектор
*				goals - вектора значений - по которому ведется сортировка
*	Returns:		0
*	Throws:		AxiomLibException - если вектора разной длины
*	Author:		dk
*	History:
*
****************************************************************************/
inline int SimpleSelectionStrategy::sort (std::vector<int> &mask, std::vector<double> &goals) const {
	// проверка входных парметров
	if ( (mask.size() != goals.size()) || (mask.size() < 1) )
		throw AxiomLibException ("Error in SimpleSelectionStrategy::sort: input parameters are of wrong length.");
	// собственно сортировка по принципу - размещения в порядке неубывания по goals индексов в mask
	int j = 0;
	for (int i = 0; i < (int) goals.size(); i++){
		int j = 0;
		// поиск размешения
		while ( j < i ) {
			if (goals[i] < goals[mask[j]]) {
				break;
			}
			j++;
		}
		// смещение всех, у кого goal меньше
		for (int k = i; k > j; k--) {
			mask[k] = mask[k-1];
		}
		// собственно размещение индекса
		mask[j] = i;
	}
	return 0;
}


/****************************************************************************
*			SimpleSelectionStrategy::quickSort
*
*	Description:	Функция сортировки вектора индексов mask по вектороу значений  goals по методу "быстрой сортирови"
*	Parameters:	mask - сотрируемый вектор
*				goals - вектора значений - по которому ведется сортировка
*	Returns:		0
*	Throws:		AxiomLibException - если вектора разной длины
*	Author:		dk
*	History:
*
****************************************************************************/
inline int SimpleSelectionStrategy::quickSort (std::vector<int> &mask, std::vector<double> &goals) const {
	// проверка входных парметров
	if ( (mask.size() != goals.size()) || (mask.size() < 1) )
		throw AxiomLibException ("Error in SimpleSelectionStrategy::sort: input parameters are of wrong length.");
	// собственно сортировка по принципу - размещения в порядке невозрастания по goals индексов в mask
	int l;
	int d = 1;
	int l1, r1, l2, r2;
	div_t divRes;
	std::vector <int> tmpMask;
	tmpMask.resize(mask.size());
	while (d < (int) goals.size()){
		divRes = div ((int) goals.size() - 1,d);
		l = divRes.quot;
		for (int j = 0; j < l; j+=2){
			l1 = d * j;
			r1 = d * (j+1) - 1;
			l2 = d * (j+1);
			r2 = d * (j+2) - 1;
			if (r2 >= (int) goals.size()) r2 = (int) goals.size() - 1;
			for (int h = l1; h <= r2; h++) {
				if (l1 > r1) {
					tmpMask[h] = mask[l2];
					l2++;
					continue;
				}
				if (l2 > r2) {
					tmpMask[h] = mask[l1];
					l1++;
					continue;
				}
				if (goals[mask[l1]] > goals[mask[l2]]) {
					tmpMask[h] = mask[l1];
					l1++;
				}
				else {
					tmpMask[h] = mask[l2];
					l2++;
				}
				// throw AxiomLibException ("Error in SimpleSelectionStrategy::quickSort: this cannot happen!");
				//std::cerr << "\n This cannot happen! \n";
			}
		}
		mask = tmpMask;
		d *= 2;
	}
	return 0;
}


/*****************************************************************************
*			SimpleSelectionStrategy::select
*
*	Description:	Функция селекции для данной популяции
*	Parameters:	AxiomSetPop - изменяемая популяция
*	Returns:		0 - если отсев произошел
*				1 - если размер исходной популяции совпадает с размером ожидаемой на выходе - селекции не было
*	Throws:		AxiomLibException - если класс не был проинициализирован или некорректны начальные данные, если хотя бы для одной из особей не подсчитана целевая функция
*	Author:		dk
*	History:
*
****************************************************************************/
int SimpleSelectionStrategy::select (AxiomSetPop& pop) const {
	// Итак, из популяции pop мы должны выбрать ровно pop.basePopSize() систем
	// 1. получаем номинальное значение
	int basePopSize = pop.getBasePopSize();
	int curPopSize = pop.size();
	// проверка корректности начальных данных
	if ( (pBest < 0) || (basePopSize > curPopSize) ) {
		throw AxiomLibException ("Error in SimpleSelectionStrategy::select - selection class has not been initialized or wrong input parameters.");
	}
	
	// 2. проверяем, надо ли сокращать
	if (basePopSize == curPopSize) {
		// сокаращать не надо - значит и селекта делать не надо
		// только выводим значения целевой функции
		int tmpFirst, tmpSecond;
		double goal;
		std::cout << " 	Goals on this stage are \n	";
		for (int asNum = 0; asNum < curPopSize; asNum++) {
			pop.getStats (asNum, tmpFirst, tmpSecond, goal);
			std::cout << "  " << goal;
		}
		return 1;
	}
	
	// Если сокращать надо. то - приступим: 
	// 3. определяем число лучших - которые пройдут точно в новую популяцию
	int nBest = (int) ((double) pBest * (double) basePopSize); // сколько лучших оставлять (остальные - рэндомно
	
	// Объявление внутренних вспомогательных перменных
	int numOthers, curOther, wholeOthers;
	int tmpFirst, tmpSecond;
	std::vector <double> goals;
	std::vector <int> mask;
	std::vector <int> newPop;
	std::vector <bool> stayAlive;
	
	// 4. вытащим значения целевой функции по каждой из систем аксиом 
	goals.resize(curPopSize);
//	if (comments) std::cout << "\n 		Goals and first and second: \n";
	for (int asNum = 0; asNum < curPopSize; asNum++) {
		pop.getStats (asNum, tmpFirst, tmpSecond, goals[asNum]);
// 		if (comments) { // вывод результатов систем аксиом в порядке убывания целевой функции
// 			std::cout << "\n		" << goals[asNum] << "	" << tmpFirst << "	" << tmpSecond;
// 		}
		if (goals[asNum] < 0) {
			throw AxiomLibException ("Error in SimpleSelectionStrategy::select: Goal function is not computed in of the AxiomSetsWithStat.");
		}
	}
//	if (comments) std::cout << "\n";
	
	// 5. Составим вектор маски - по убыванию целевой фунции - для определения какие особи включать сразу в новую популяцию, а из каких выбирать по рэндому
	mask.resize(curPopSize);
	// инициализация маски
	for (int asNum = 0; asNum < curPopSize; asNum++) {
		mask[asNum] = asNum;
	}
	// сортировка mask по значению целевой функции
	sort (mask, goals); // ну или quickSort (mask, goals);
	
	if (comments) { // вывод результатов систем аксиом в порядке убывания целевой функции
		std::cout << " 	Goals on this stage are \n	";
		for (unsigned int g = 0; g < mask.size(); g++) {
			std::cout << "  " << goals[mask[g]];
		}
		std::cout << "\n";
	}
	
	// 6. теперь есть вектор указаний последовательности номеров систем аксиом в порядке увеличения целевой функции - переходим к выбору выживших особей
	stayAlive.resize(curPopSize);
	// инициализация вектора - показателя какие особи выживут после селекции
	for (int a = 0; a < curPopSize; a++) {
		stayAlive[a] = false;
	}
	// Оставляем требуемое число лучших особей
	for (int alive = 0; alive < nBest; alive++) {
		stayAlive[mask[alive]] = true;
	}
	// Оставляем случайно оставшиеся особи
	numOthers = basePopSize - nBest; // число выбираемых по рэндому
	wholeOthers = curPopSize - nBest; // число кандидатов
	for (int i = 0; i < numOthers; i++) {
		curOther = (int) ( ((double) rand() / (double) RAND_MAX) * ((double) wholeOthers - (double) i - 1.0) );
		while (stayAlive[mask[nBest + curOther]] == true) {
			curOther++;
		}
		stayAlive[mask[nBest + curOther]] = true;
	}
	
	pop.cutDown (stayAlive);
	return 0;
}
