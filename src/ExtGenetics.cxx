/****************************************************************************
*
*			Функции класса ExtGenetics
*
****************************************************************************/

#include "ExtGenetics.h"

using namespace AxiomLib;

#define debug true

// Коструктор класса с заданием начальных параметров по умолчанию
ExtGenetics::ExtGenetics (void) {
	numOfIter = -1; 
	popSize = -1;
	ectp = 0.0;
	atp  = 0.0;
	comments = true;
	exitCond = 0;
	axiomSetBaseDir = NULL;
	whereToSave = NULL;
	//selectionStrategy = NULL;
	//transmuteStrategy = NULL;
	crossoverStrategy = NULL;
	goalStrategy = NULL;
	recognizer = NULL;
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
ExtGenetics::~ExtGenetics (void) {
	if (recognizer != NULL) delete recognizer;
	//if (selectionStrategy != NULL) delete selectionStrategy;
	//if (transmuteStrategy != NULL) delete transmuteStrategy;
	if (crossoverStrategy != NULL) delete crossoverStrategy;
	if (goalStrategy != NULL) delete goalStrategy;
	if (axiomSetBaseDir != NULL) delete axiomSetBaseDir;
	if (whereToSave != NULL) delete whereToSave;
}


/****************************************************************************
*			ExtGenetics::initFromEnv
*
*	Description:	Инициализация параметров с данными из env
*	Parameters:	const Environment& env - структура с параметрами конфигурации
*	Returns:	0
*	Throws:		AxiomLibException - если конфигурация некорректна
*	Author:		gevor
*	History:
*
****************************************************************************/
int ExtGenetics::initFromEnv(const Environment& env) {
	// проверяем число операций
	if (env.getIntParamValue(numOfIter, "numOfIter") < 0)
		throw AxiomLibException("ExtGenetics::initFromEnv : number of iterations has not been set.");

	int tempInt;
	if (env.getIntParamValue (tempInt, "popSize") < 0)
		throw AxiomLibException("ExtGenetics::initFromEnv : tempInt is undefined.");
	if (tempInt <= 0) 
		throw AxiomLibException("ExtGenetics::initFromEnv : tempInt has wrong value.");
	population.resize (tempInt);
	popSize = tempInt;
	// инициализируем начальную популяцию
	if (debug) std::cout << "\n	Generated elems:\n";
	for (int i = 0; i < tempInt; i++) {
		(population[i]).initFromEnv(env);
		if (debug) {
			std::cout << "	" << i+1 << ". ";
			if (i < 9) std::cout << " ";
			std::cout << population[i].prec << "	" << population[i].axSet.name() << "\n";
		}
	}
	if (debug) {
		std::cout << "\n";
		std::cout.flush();
	}

  // инициализируем распознаватель
  string recognizerName;
  RecognizerFactory rf;
  if (env.getStringParamValue(recognizerName, "Recognizer") < 0)
    throw AxiomLibException("ExtGenetics::initFromEnv : Recognizer is undefined.");
  recognizer = rf.create(recognizerName);
  recognizer->setParamsFromEnv(env);

  // Setting degrees of transmute
  double tempDB1, tempDB2;
  if ( (env.getDoubleParamValue(tempDB1, "elCondTransLevel") >= 0) && (env.getDoubleParamValue(tempDB2, "axiomTransLevel") >= 0) ) {
	if ( (tempDB1 < 0) || (tempDB1 > 1) || (tempDB2 < 0) || (tempDB2 > 1) )
		throw AxiomLibException ("Error in ExtGenetics::initFromEnv: transmute levels out of range.");
	ectp = tempDB1;
	atp = tempDB2;
  } else {
	if (env.getDoubleParamValue(tempDB1, "transLevel") >= 0) {
		if ((tempDB1 < 0) || (tempDB1 > 1))
			throw AxiomLibException ("Error in ExtGenetics::initFromEnv: transLevel out of range.");
		ectp = tempDB1;
		atp = tempDB1;
	} else {
		throw AxiomLibException ("Error in ExtGenetics::initFromEnv: transLevel is not set.");
	}
  }
  // Stting degree of precison Transmute Level
  if (env.getDoubleParamValue(tempDB1, "precisionTransLevel") >= 0) {
	if ( (tempDB1 < 0) || (tempDB1 > 1) )
		throw AxiomLibException ("Error in ExtGenetics::initFromEnv: precision transmute level is out of range.");
	prtp = tempDB1;
  } else {
	throw AxiomLibException ("Error in ExtGenetics::initFromEnv: precisionTransLevel is not set.");
  }

  // инициализируем стратегию скрещивания
  string crossoverStrategyName;
  CrossoverStrategyFactory csf;
  if (env.getStringParamValue(crossoverStrategyName, "crossClass") < 0)
    throw AxiomLibException("ExtGenetics::initFromEnv : crossover strategy is undefined.");
  crossoverStrategy = csf.create(crossoverStrategyName);
  crossoverStrategy->setParamsFromEnv(env);
  
	double allCrossLevel;
	if (env.getDoubleParamValue(allCrossLevel, "crossLevel") < 0) {
		env.getDoubleParamValue(eccl, "elemCondCrossLevel");
		if ( (eccl < 0.0) || (eccl > 1.0) ) 
			throw AxiomLibException("ExtGenetics::initFromEnv : elemCondCrossLevel isn't set in config file or elemCondCrossLevel is out of range.");
			
		env.getDoubleParamValue(acl, "axiomCrossLevel");
		if ( (acl < 0.0) || (acl > 1.0) ) 
			throw AxiomLibException("ExtGenetics::initFromEnv : axiomCrossLevel isn't set in config file or axiomCrossLevel is out of range.");
			
		env.getDoubleParamValue(ascl, "axiomSetCrossLevel");
		if ( (ascl < 0.0) || (ascl > 1.0) ) 
			throw AxiomLibException("ExtGenetics::initFromEnv : axiomSetCrossLevel isn't set in config file or axiomSetCrossLevel is out of range.");
	}
	else {
		if ( (allCrossLevel < 0.0) || (allCrossLevel > 1.0) ) 
			throw AxiomLibException("ExtGenetics::initFromEnv : crossLevel isn't set in config file or crossLevel is out of range.");
		// Если не заданы степени мутации по отдельности для каждого уровня мутации, а задан единый уровень - то его и присваиваем каждому из уровней
		eccl = allCrossLevel;
		acl = 0.0;
		ascl = 0.0;
	}

  // инициализируем стратегию вычисления целевой функции
  string goalStrategyName;
  GoalStrategyFactory gsf;
  if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
    throw AxiomLibException("ExtGenetics::initFromEnv : goal strategy is undefined.");
  goalStrategy = gsf.create(goalStrategyName);
  goalStrategy->setParamsFromEnv(env);
  
  // нинициализация условия досрочного выхожаи из цикла генетического алгоритма
  if (env.getDoubleParamValue (exitCond, "exitCond") < 0)
    throw AxiomLibException("ExtGenetics::initFromEnv : goal strategy is undefined.");
  
  // сохраняем путь по которому сохранять лучшую систему аксиом по результатам работы цикла
  std::string temp;
  if (env.getStringParamValue(temp, "AxiomSetBaseDir") >= 0) {
    axiomSetBaseDir = new string;
    *axiomSetBaseDir = temp;
  }
  if (env.getStringParamValue(temp, "saveTo") >= 0) {
    whereToSave = new string;
    *whereToSave = temp;
  }
  return 0;
}


/****************************************************************************
*				ExtGenetics::setNumOfIter
*
*	Description:	Установка нового значения числа итераций при подборе оптимальной системы аксиом
*	Parameters:	newNumOfIter - новое значение числа итераций
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int ExtGenetics::setNumOfIter (const int newNumOfIter) {
	numOfIter = newNumOfIter;
	return 0;
}


/****************************************************************************
*			ExtGenetics::run
*
*	Description:	Запуск ГА
*	Parameters:	-
*	Returns:		0
*	Throws:		AxiomLibException - если произошла ошибка или ГА не
*				был инициализирован
*	Author:		gevor, dk
*	History:
*
****************************************************************************/
int ExtGenetics::run(void) {
	// Классический генетический алгоритм:
	// 0. проверяем, установлены ли параметры
	// В цикле:
	// 1. Прогоняем на существующих системах аксиом распознаватель
	// 2. Вычисляем значение целевой функции по каждой из систем аксиом
	// 3. Селекционируем, Проверяем критерий останова
	// 4. Мутируем
	// 5. Скрещиваем

	// 0. проверка параметров
	if (numOfIter < 0)
		throw AxiomLibException("ExtGenetics::run : number of iterations < 0; probably GA has not been initialized.");
		
	// заводим переменную дял хранения лучшей системы аксиом за историю
	PopElemExt elemBest;
	if (population.size() < 1)
		throw AxiomLibException("ExtGenetics::run : population have not been initialized.");
	
	AxiomSetPop aspTemp;
	std::vector <double> precVec;
	// Основной Цикл

	for (int gaIter=0; gaIter < numOfIter; gaIter++) {
		if (debug) cout << "	ExtGenetics: iteration " << gaIter+1 << " out of " << numOfIter << ", pop size = " << population.size() << "\n";
		// 1. Прогоняем на существующей популяции алгоритм разметки и распознавания, получаем число ошибок первого и второго рода, заполняем статистику.
		aspTemp.clear();
		precVec.clear();
		
		// 2. Проводим Селекцию
		if (gaIter > 0) 
			select ();
				
		// 3. Мутируем
		if (gaIter > 0) 
			transmute ();
		
		// 4. Скрещиваем    
		if (gaIter > 0) 
			crossover ();

		// 5. recognize
		packAxiomSetPop (aspTemp, precVec);
		recognizer->run(aspTemp, precVec);
		
		// 6. Прогоняем вычисление H(first, second)
		goalStrategy->compute(aspTemp);
		unpackAxiomSetPop (aspTemp, precVec);
		writeResultsToDisplay();
		
		//
		if (gaIter == 0)
			elemBest = population[0];
		storeBest (elemBest);

		if (comments) { cout << "\n"; std::cout.flush();}
	}

	// сохраняем лучшую систему аксиом в файл
	AxiomSetWithStats bestAxiomSet;
	bestAxiomSet = elemBest.axSet;
	if (debug) {
		int  first, second;
		double goal;
		bestAxiomSet.unsafeGetStats (first, second, goal);
		std::cout << "\n	Best Results are:\n	    first and second and goal\n		" << first << "	"  << second << "	 " << goal;
		std::cout << "\n	Precision is " << population[0].prec << "\n";
		std::cout.flush();
	}
	if ( (axiomSetBaseDir != NULL) && (whereToSave != NULL) ) {
		bestAxiomSet.saveAxiomSetToFile (*axiomSetBaseDir, *whereToSave);
	}
	//std::cin.get();
	return 0;
}


int ExtGenetics::storeBest (PopElemExt &bestElem) {
	signed int nf, ns;
	double g;
	int change = 0;
	signed int nfBest, nsBest;
	double gBest;
	bestElem.getStats (nfBest, nsBest, gBest);
	for (unsigned int h = 0; h < population.size(); h++) {
		(population[h]).getStats (nf, ns, g);
		if (g < gBest) {
			gBest = g;
			nfBest = nf;
			nsBest = ns;
			bestElem = population[h];
			change++;
		}		
	}
	return change;
}


void ExtGenetics::writeResultsToDisplay (void) {
	signed int nf, ns;
	double g;
	std::cout << "\n	Results are \n";
	for (unsigned int h = 0; h < population.size(); h++) {
		(population[h]).getStats (nf, ns, g);
		std::cout << "	" << g << " (" << nf << ", " << ns << ")";
	}
	std::cout << "\n";
	std::cout.flush();
}


int ExtGenetics::packAxiomSetPop (AxiomSetPop &aspTemp) {
	std::vector <AxiomSetWithStats> axSetVec;
	axSetVec.resize(population.size());
	for (unsigned int h = 0; h < population.size(); h++) {
		axSetVec[h] = (population[h]).axSet;
	}
	aspTemp.addSets (axSetVec);
	return 0;
}

int ExtGenetics::packAxiomSetPop (AxiomSetPop &aspTemp, std::vector <double> &prec) {
	std::vector <AxiomSetWithStats> axSetVec;
	axSetVec.resize(population.size());
	prec.resize(population.size());
	for (unsigned int h = 0; h < population.size(); h++) {
		axSetVec[h] = (population[h]).axSet;
		prec[h] = (population[h]).prec;
	}
	aspTemp.addSets (axSetVec);
	return 0;
}

int ExtGenetics::unpackAxiomSetPop (AxiomSetPop &aspTemp) {
	signed int nf, ns;
	double g;
	if (aspTemp.size() != population.size()) 
		throw AxiomLibException("Error in ExtGenetics::unpackAxiomSetPop: internal error.");
	for (unsigned int h = 0; h < population.size(); h++) {
		aspTemp.getStats (h, nf, ns, g);
		(population[h]).setStats(nf, ns, g);
	}
	return 0;
}

int ExtGenetics::unpackAxiomSetPop (AxiomSetPop &aspTemp, std::vector <double> &prec) {
	signed int nf, ns;
	double g;
	if ( (prec.size() != population.size()) || (aspTemp.size() != population.size()) ) 
		throw AxiomLibException("Error in ExtGenetics::unpackAxiomSetPop: internal error.");
	for (unsigned int h = 0; h < population.size(); h++) {
		aspTemp.getStats (h, nf, ns, g);
		(population[h]).setStats(nf, ns, g);
		(population[h]).prec = prec[h];
	}
	return 0;
}


inline int ExtGenetics::sort (std::vector<int> &mask, std::vector<double> &goals) const {
	// проверка входных парметров
	if ( (mask.size() != goals.size()) || (mask.size() < 1) )
		throw AxiomLibException ("Error in SimpleSelectionStrategy::sort: input parameters are of wrong length.");
	// собственно сортировка по принципу - размещения в порядке неубывания по goals индексов в mask
	int j = 0;
	for (int i = 0; i < (int) goals.size(); i++){
		j = 0;
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


int ExtGenetics::transmute () {
	if ( (ectp > 0.0) || (atp > 0.0) ) {
		for (int i = 0; i < (int) population.size(); i++) {
			int nOfAxioms = population[i].axSet.getNumOfAxioms();
			int resTransmuteAxiom = 0;
			for (int j = 0; j < nOfAxioms; j++) {
				resTransmuteAxiom = population[i].axSet.transmuteAxiom(j, ectp, atp);
			}
			population[i].axSet.voidStats();
		}
	}
	if (prtp > 0.0) {
		double tr;
		for (unsigned int i = 0; i < population.size(); i++) {
			tr = (double) rand() / (double) RAND_MAX;
			if (tr < prtp) {
				if (population[i].prec * (1.0 + tr) >= 1.0) {
					population[i].prec = population[i].prec * (1.0 - tr);
				} else {
					if ( ((double) rand() / (double) RAND_MAX) > 0.5) {
						population[i].prec = population[i].prec * (1.0 + tr);
					} else {
						population[i].prec = population[i].prec * (1.0 - tr);
					}
				}
			}
		}
	}
	return 0;
}


int ExtGenetics::select () {
	//
	std::vector <double> goals;
	std::vector <int> positions;
	signed int nf, ns;
	double g;
	//
	positions.resize(population.size());
	goals.resize(population.size());
	for (int i = 0; i < (int) population.size(); i++) {
		population[i].getStats(nf, ns, g);
		goals[i] = g;
		positions[i] = i;
	}
	sort(positions, goals);
	// 
	unsigned int upTo = 0;
	if ( (int) positions.size() > popSize) {
		upTo = popSize;
	} else {
		upTo = (int) positions.size();
	}
	std::vector <PopElemExt> popTemp;
	popTemp.resize(upTo);
	for (unsigned int i = 0; i < upTo; i++) {
		popTemp[i].prec = population[positions[i]].prec;
		popTemp[i].axSet = population[positions[i]].axSet;
	}
	population.resize(popTemp.size());
	for (unsigned int i = 0; i < popTemp.size(); i++) {
		population[i].prec = popTemp[i].prec;
		population[i].axSet = popTemp[i].axSet;
	}	
	return 0;
}


int ExtGenetics::crossover () {
	if (population.size() < 2) {
		throw AxiomLibException ("Error in ExtGenetics::crossover: internal error - population is empty.");
	}
	AxiomSetPop aspTemp;
	packAxiomSetPop (aspTemp);
	crossoverStrategy->setParams(eccl, acl, ascl);
	crossoverStrategy->crossover(aspTemp);
	if (aspTemp.size() == population.size()) {
		return 1;
	}
	if (aspTemp.size() > (int) population.size()) {
		std::vector <PopElemExt> popTemp;
		popTemp.resize(aspTemp.size());
		for (unsigned int i = 0; i < population.size(); i++) {
			popTemp[i].prec = population[i].prec;
			popTemp[i].axSet = population[i].axSet;
		}
		int h = 0; 
		if (population.size() < 2) {
			throw AxiomLibException ("Error in ExtGenetics::crossover: internal error - the population is empty.");
		}
		for (int i = (int) population.size(); i < aspTemp.size(); i++) {
			popTemp[i].axSet = aspTemp.axiomSet(i);
			popTemp[i].prec = population[h].prec;
			h++; 
			if (h >= (int) population.size()) h = 0;
		}
		population.resize(popTemp.size());
		for (unsigned int i = 0; i < popTemp.size(); i++) {
			population[i].prec = popTemp[i].prec;
			population[i].axSet = popTemp[i].axSet;
		}
		return 2;
	}
	throw AxiomLibException ("Error in ExtGenetics::crossover: internal error - population size decreased during crossover.");
	return 0;
}
