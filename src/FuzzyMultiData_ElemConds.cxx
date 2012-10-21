/****************************************************************************
*
*				Функции класса FuzzyMultiDataLearnAlgorithm
*
****************************************************************************/

#include "FuzzyMultiDataLearnAlgorithm.h"
#include "Defines.h"

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

//#define DEBUG_FUZZYMULTYDATA

#ifdef DEBUG_FUZZYMULTYDATA
#include <iostream>
#endif

using namespace AxiomLib;

// Набор определений, специфичных для данного класса
#define str_Dimension			"_dim_"	// Обозначение измерения при составлении имени сохраняемого элементарного условия
#define str_AbnormalType		"_at_"	// Обозначение типа нештатного поведения при составлении имени файла с описанием элементарного условия
#define name_max_size			200 // максимальное число символов в имени файла с описанием элементарного условия (лучше чтобы было с запасом!)
//эта величина сделана параметром
//#define numBestECs				20	// данное количество экземпляров элементарных условий по каждому типу условий сохраняется после первого этапа работы алгоритма
//#define eps						0.0001 // используется в некоторых формулах, чтобы исплючить ошибку машинного округления
#define eps						0.0000001 // используется в некоторых формулах, чтобы исплючить ошибку машинного округления
#define max_goal_val			10000 // максимальное значение целевой функции для элементарного условия (просто достаточно большое число)

#define replacementProbability	0.5 // Вероятность замещения одного из элементов вектора bestECs новым, если этот вектор уже раздут и заполнен условиями с таким же значением целевой функции
#define additionalECItems		10	// > 0 !!! Число дополнительных слотов, на которое увеличивать bestECs, если этот вектор заполнен

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::selectElemCond
*
*	Description:	Функция подбора и настройки элементарных условий.
*	Parameters:		void
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::selectElemCond (void) {
	//	Определение ранга текущего процесса
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// MPI-распараллеливание вычислений на первых двух шагах алгоритма
	// Для этого: Определение для какого типа нештатного поведения данному процессу процессу производить обучение элементарных условий и аксиом
	int from, upTo, rankFrom, rankUpTo;
	splitAbnormalTypeRange (rank, size, from, upTo, rankFrom, rankUpTo);
	// Вывод информации о назначенных для данного шаблона типов нештатного поведения - для них алгоритм будет подбирать элементырне условия и строить аксиомы
	std::cout << "\n\tProcess: " << rank + 1 << " from " <<  size << " set for abnormal behaviour: " << from + 1 << " - " << upTo << " \n";
	std::cout.flush();

	// подбор параметров условий для типов нештатного поведения с from до upTo
	bestECForAllAbnormalTypes.resize (upTo - from);
#ifdef DEBUG_FUZZYMULTYDATA
		std::cout<<"Starting loop..."<<std::endl;
#endif
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = from; i < upTo; i++) {
		// выбор параметров условий
		bestECForAllAbnormalTypes[i - from].clear();
#ifdef DEBUG_FUZZYMULTYDATA
		std::cout<<"Selecting elem cond..."<<std::endl;
#endif
		selectElemCond (i, rank, rankFrom, rankUpTo, bestECForAllAbnormalTypes[i - from]);
		// Вывод на экран стадии процесса
		std::cout << "\n\tProcess: " << rank + 1 << " from " <<  size <<  ":\tAbnormal type\t" << i+1 << " done.\n";
		std::cout.flush();
	}
#ifdef DEBUG_FUZZYMULTYDATA
		std::cout<<"Exiting loop..."<<std::endl;
#endif
	// Объединение списков с именами файлов, содержащих описания элементарных условий, полученных в рамках всех процессов
	//gatherBestECfromProcesses (rank, size);

	// Синхронизация процессов
	MPI_Barrier (MPI_COMM_WORLD);

	// добавление в список имен файлов с описанием элементарных условий - тех, которые были указаны в конфигурационном файле
	updateBestECListFromEnv();
	
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::splitAbnormalTypeRange
*
*	Description:	Функция определяет границы разбиения области поиска для данного процесса
*	Parameters:		rank - ранг текущего процесса
*					size - общее число процессов, учавствующих в работе алгоритма
*					from - заполняемое значение, левая граница назначенного интервала для данного процесса
*					upTo - заполняемое значение, правая граница назначенного интервала для данного процесса
*					rankFrom - минимальный ранг процесса, который совместно с данным будет подбирать параметры для одного типа нештатного поведения
*					rankUpTo - максимальный ранг процесса, который совместно с данным будет подбирать параметры для одного типа нештатного поведения
*	Returns:		0
*	Throws:			AxiomLibException - если используемый набор данных пуст
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::splitAbnormalTypeRange (int rank, int size, int &from, int &upTo, int &rankFrom, int &rankUpTo) const {
	int numOfClasses;
	std::vector <int> numOfMultiTS;
	std::vector < std::vector <int> > numOfTS;
	// Получение информации о наборе данных - если набор пуст, то выдаем ошибку
	if (!fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::splitAbnormalTypeRange : incorrect dataSet - it is probably empty");
	return splitRange (rank, size, 0, numOfClasses, from, upTo, rankFrom, rankUpTo);
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::gatherBestECfromProcesses
*
*	Description:	Функция сбора списков с названиями файлов, в которые были 
*					записаны описания лучших элементарных условий, с различных 
*					процессов, учавствующих в первом этапе работы основного алгоритма.
*	Parameters:		rank - ранг текущего процесса
*					size - общее число процессов, учавствующих в работе алгоритма
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
/*int FuzzyMultiDataLearnAlgorithm::gatherBestECfromProcesses (int rank, int size) {
	if (size > 1) { // значит параметры условий для разных типов нештатного поведения подбирались независимо - необходимо объединить данные
		// Для этого - Создание собственного типа MPI для передачи названий файлов не по одному, а целыми массивами
		struct FileDescr {
			char name[name_max_size]; // file name
		};
		struct FileDescr fDescr[2];
		// build datatype describing structure 
		MPI_Datatype FileDescrType;
		MPI_Aint base;
		MPI_Datatype type[2] = {MPI_CHAR, MPI_UB};
		int blocklen[2] = {name_max_size, 1};
		MPI_Aint disp[2];
		// compute displacements of structure components
		MPI_Address(fDescr, disp);
		MPI_Address(fDescr+1, disp+1);
		base = disp[0];
		for (int i = 0; i < 2; i++) disp[i] -= base;
		// build datatype describing structure
		MPI_Type_struct( 2, blocklen, disp, type, &FileDescrType);
		// Регистрация типа в MPI
		MPI_Type_commit(&FileDescrType);

		// Передача или прием списка с названиями файлов соответственно
		unsigned int buffer[1];
		if (rank == 0) { // этот процесс собирает данные
			struct FileDescr *fDescrips;
			unsigned int st, level1, level2, level3;
			MPI_Status status;
			for (int procNum = 1; procNum < size; procNum++) {
				// получаем число векторов, содержащих вектора со строками - первую размерность всей структуры
				MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 200 + procNum, MPI_COMM_WORLD, &status);
				level1 = buffer[0];
				st = bestECForAllAbnormalTypes.size();
				bestECForAllAbnormalTypes.resize (st + level1);
				for (unsigned int i = 0; i < level1; i++, st++) {
					// получаем число векторов со строками
					MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 201 + procNum, MPI_COMM_WORLD, &status);
					level2 = buffer[0];
					bestECForAllAbnormalTypes[st].resize (level2);
					for (unsigned int j = 0; j < level2; j++) {
						// получаем число векторов со строками
						MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 202 + procNum, MPI_COMM_WORLD, &status);
						level3 = buffer[0];
						bestECForAllAbnormalTypes[st][j].resize (level3);
						for (unsigned int k = 0; k < level3; k++) {
							MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 203 + procNum, MPI_COMM_WORLD, &status);
							fDescrips = new struct FileDescr[buffer[0]];
							// получаем вектор с именами условий
							MPI_Recv( fDescrips, buffer[0], FileDescrType, procNum, 204 + procNum, MPI_COMM_WORLD, &status);
							// Сохраняем полученные значения - вектор имен файлов с описанием элементарных условий
							bestECForAllAbnormalTypes[st][j][k].resize(buffer[0]);
							for (unsigned int t = 0; t < buffer[0]; t++) {
								bestECForAllAbnormalTypes[st][j][k][t].assign (fDescrips[t].name);
							}
							delete fDescrips;
						}
					}
				}
			}
		} else { // этот процесс посылает данные
			struct FileDescr *fDescrips;
			unsigned int curSize;
			// посылаем число размерностей, по которым в этом процессе велся поиск параметров элементарных условий
			buffer[0] = bestECForAllAbnormalTypes.size();
			MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 200 + rank, MPI_COMM_WORLD);
			for (unsigned int i = 0; i < bestECForAllAbnormalTypes.size(); i++) {
				// посылаем число векторов - каждый из которых будем по отдельности посылать
				buffer[0] = bestECForAllAbnormalTypes[i].size();
				MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 201 + rank, MPI_COMM_WORLD);
				for (unsigned int j = 0; j < bestECForAllAbnormalTypes[i].size(); j++) {
					// очередная размерность вектора с именами файлов - обрабатываем ее
					buffer[0] = bestECForAllAbnormalTypes[i][j].size();
					MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 202 + rank, MPI_COMM_WORLD);
					for (unsigned int k = 0; k < bestECForAllAbnormalTypes[i][j].size(); k++) {
						curSize = bestECForAllAbnormalTypes[i][j][k].size();
						// Если число строк с именами файлов в выбранной размерности - 0, то пропускаем ее
						if (curSize < 1)
							continue;
						// Копируем строки с именами файлов в созданную структуру для передачи средствами MPI
						fDescrips = new struct FileDescr[curSize];
						for (unsigned int t = 0; t < curSize; t++)
							strncpy(fDescrips[t].name, bestECForAllAbnormalTypes[i][j][k][t].c_str(), name_max_size - 1);
						// посылаем число строк в векторе
						buffer[0] = curSize;
						MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 203 + rank, MPI_COMM_WORLD);
						// посылаем собственно вектор с именами условий
						MPI_Send( fDescrips, curSize, FileDescrType, 0, 204 + rank, MPI_COMM_WORLD);
						delete fDescrips;	
					}					
				}
			}
		}
	}
	return 0;
}
*/

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::getNearestNumber
*
*	Description:	Функция достает ближайшее число из строки str начиная с индекса index
*	Parameters:		str - строка для анализа
*					index - индекс в строке, начиная с которго искать число
*	Returns:		int - считанное значение или -1, если числа не нашлось
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::getNearestNumber (const std::string str, std::string::size_type &index) const {
	int toReturn = -1;
	std::string::size_type first, last;
	first = str.find_first_of ("0123456789", index);
	if (first != std::string::npos) {
		last = str.find_first_not_of ("0123456789", first);
		if (last == std::string::npos)
			last = str.size();
		toReturn = atoi (str.substr(first, last - first).c_str());
	}
	return toReturn;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::disintegrateECFileName
*
*	Description:	Функция вытаскивает из имени файла четыре числовых параметра, 
*					если это удается. Иначе значения параметров определяются по 
*					умолчанию.
*	Parameters:		fileName - имя файла для анализа
*					at - номер аномального типа поведения 
*					dim - номер размерности выходных данных
*					vecI, vecJ - размерности структуры, из который файлы были 
*						сохранены. Эти параметры могут быть не заданы, в таком случае
*						их значение определяется как -1. 
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::disintegrateECFileName(const std::string fileName, int &at, int &dim, int &vecI, int &vecJ) {
	// Устанавливаем значения по умолчанию: 
	at = -1;
	dim = -1;
	vecI = -1;
	vecJ = -1;
	// Начинаем поиск
	std::string :: size_type indexAT, indexDIM, index, first;
	// Пытаемся считать AbnormalType
	indexAT = fileName.rfind (str_AbnormalType);
	if ( indexAT != std::string::npos ) {
		at = getNearestNumber (fileName, indexAT) - 1; // -1 - т.к. при кодировании имени файла ко всем числам прибавляется 1
	}
	// Пытаемся считать Dimension
	indexDIM = fileName.rfind (str_Dimension);
	if ( indexDIM != std::string::npos ) {
		dim = getNearestNumber (fileName, indexDIM) - 1; // -1 - т.к. при кодировании имени файла ко всем числам прибавляется 1
	}
	// Пытаемся считать оставшиеся два индекса
	index = max (indexAT, indexDIM);
	// пропускаем число относящиеся к DIM или AT
	index = fileName.find_first_of ("0123456789", index);
	index = fileName.find_first_not_of ("0123456789", index);
	first = fileName.find_first_of ("0123456789", index);
	if (first != std::string::npos) {
		vecI = getNearestNumber (fileName, first) - 1; // -1 - т.к. при кодировании имени файла ко всем числам прибавляется 1
		index = fileName.find_first_not_of ("0123456789", first);
		first = fileName.find_first_of ("0123456789", index);
		if (first != std::string::npos) {
			vecJ = getNearestNumber (fileName, first) - 1; // -1 - т.к. при кодировании имени файла ко всем числам прибавляется 1
		}		
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::updateBestECListFromEnv
*
*	Description:	функция обновляет список имен файлов с описанием параметров 
*					элементарных условий списков файлов из конфигурационного файла
*	Parameters:		-
*	Returns:		0
*	Throws:			AxiomLibException - если данные из конфигурационного файла 
*						не согласуются с данными, полученными при работе алгоритма
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::updateBestECListFromEnv(void) {
	int at, dim, inx, iny;
	std::set <std::string> :: const_iterator c_iter;
	for (c_iter = predefinedECFileName.begin(); c_iter != predefinedECFileName.end(); ++c_iter) {
		disintegrateECFileName(*c_iter, at, dim, inx, iny);
		if ((at < (int) bestECForAllAbnormalTypes.size()) && (at >= 0)) {
			if ((dim < (int) bestECForAllAbnormalTypes[at].size()) && (dim >= 0)) {
				// добавляем имя в массив
				if ((inx < (int) bestECForAllAbnormalTypes[at][dim].size()) && (inx >= 0)) {
					bestECForAllAbnormalTypes[at][dim][inx].push_back (*c_iter);
				} else {
					if (bestECForAllAbnormalTypes[at][dim].size() < 1)
						bestECForAllAbnormalTypes[at][dim].resize(1);
					bestECForAllAbnormalTypes[at][dim][0].push_back (*c_iter);
				}
			} else {
				// Вообще говоря - здесь нужно выдавать warning - но не обязательно
				std::cout << "\n\tElementary Condition File '" << *c_iter << "' is incompatible with current dataSet properties: wrong number of dataSet dimension.\n";
			}
		} else {
			// Вообще говоря - здесь нужно выдавать warning - но не обязательно
			//std::cout << "\n\tElementary Condition File '" << *c_iter << "' is incompatible with current dataSet properties: wrong number of abnormalType.\n";
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::storeBestECs
*
*	Description:	Функция сохранения лучших элементарных условий и соответствующих 
*					значений целевой функции
*	Parameters:		ec - сохраняемое элементарное условие
*					bestECs - вектор лучших условий того же типа
*	Returns:		1 - если условие сохранено
*					0 - если условие не входит в список лучших 
*	Throws:			AxiomLibException - если входные вектора не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::storeBestECs (std::vector <ElemCondPlusStat> &bestECs, ElemCondPlusStat &ec, double &goal, double &statNorm, double &statAbnorm) const {
	// Проверка - входит ли данное ЭУ в список лучших за историю
	double minVal = goal;
	int whereToSave = -1;
	double bestGoal = -1;
	for (int i = 0; i < (int) bestECs.size(); i++) {
		// Если есть свободный слот, то просто записываем в него
		if (bestECs[i].elemCondition == NULL) {
			bestECs[i] = ec;
			bestECs[i].goal = goal; 
			bestECs[i].statNormal = statNorm;
			bestECs[i].statAbnorm = statAbnorm;
			return 1;
		} else {
			// Определяем максимальное значение целевой функции среди bestECs
			if (bestGoal < bestECs[i].goal) 
				bestGoal = bestECs[i].goal;
			// Определяем - куда разместить данное условие ec
			if (minVal > bestECs[i].goal) {
				minVal = bestECs[i].goal;
				whereToSave = i;
			}
		}
	}
	// Если условие среди лучших - сохраняем его
	if (whereToSave >= 0) {
		// Сохраняем элементарное условие заменяя самое слабое условие в наборе
		bestECs[whereToSave] = ec;
		bestECs[whereToSave].goal = goal; 
		bestECs[whereToSave].statNormal = statNorm;
		bestECs[whereToSave].statAbnorm = statAbnorm;
		return 1;
	}
	// Если условие по факту среди лучших, но места для него нет - то находим это место
	if (abs(bestGoal - goal) < eps) {
		int curSize = (int) bestECs.size();
		if (curSize < numBestECsMax) {
			// Значит есть еще возможность расширить вектор bestECs
			std::vector <ElemCondPlusStat> tmpECs;
			tmpECs.resize (curSize);
			for (int i = 0; i < curSize; i++) {
				tmpECs[i] = bestECs[i];
				bestECs[i].clear();
			}
			bestECs.clear();
			bestECs.resize (curSize + additionalECItems);
			for (int i = 0; i < curSize; i++) {
				bestECs[i] = tmpECs[i];
				tmpECs[i].clear();
			}
			bestECs[curSize] = ec;
			bestECs[curSize].goal = goal; 
			bestECs[curSize].statNormal = statNorm;
			bestECs[curSize].statAbnorm = statAbnorm;
			return 1;
		} else {
			// Значит вектор bestECs уже раздут до предела 

			// Все условия - одинаковые, поэтому случайно выбираем - сохранять ли данное элементарное условие
			if (((double) rand() / (double) RAND_MAX) < replacementProbability) {
				// Выбираем элемент, который заместить
				whereToSave = round ( (((double) rand() + 1.0)/((double) RAND_MAX + 2.0)) * ((double) curSize) - 0.5);
				bestECs[whereToSave] = ec;
				bestECs[whereToSave].goal = goal; 
				bestECs[whereToSave].statNormal = statNorm;
				bestECs[whereToSave].statAbnorm = statAbnorm;
				return 1;
			}
			return 0;

			/*
			// Еще раз ищем место для данного условия, но уже по значению statAbnorm
			minVal = statAbnorm;
			for (int i = 0; i < curSize; i++) {
				if (minVal > bestECs[i].statAbnorm) {
					minVal = bestECs[i].statAbnorm;
					whereToSave = i;
				}
			}
			if (whereToSave >= 0) {
				// Сохраняем элементарное условие заменяя самое слабое условие в наборе
				bestECs[whereToSave] = ec;
				bestECs[whereToSave].goal = goal; 
				bestECs[whereToSave].statNormal = statNorm;
				bestECs[whereToSave].statAbnorm = statAbnorm;
				return 1;
			} else {
				// Все условия - одинаковые, поэтому случайно выбираем - сохранять ли данное элементарное условие
				if ((abs(bestECs[0].statAbnorm - statAbnorm) < eps) && (((double) rand() / (double) RAND_MAX) < replacementProbability)) {
					// Выбираем элемент, который заместить
					whereToSave = round ( (((double) rand() + 1.0)/((double) RAND_MAX + 2.0)) * ((double) curSize) - 0.5);
					bestECs[whereToSave] = ec;
					bestECs[whereToSave].goal = goal; 
					bestECs[whereToSave].statNormal = statNorm;
					bestECs[whereToSave].statAbnorm = statAbnorm;
					return 1;
				}
				return 0;
			}
			*/
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::numOfCarriedOutItems
*
*	Description:	Подсчет числа точек в ряду, на которых элементарное условие выполняется
*	Parameters:		ec - проверяемое элементарное условие
*					row - ряд для порверки элементарного условия
*	Returns:		int - число точек, на которых элементарное условие выполнено
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::numOfCarriedOutItems (ElemCondPlus &ec, std::vector <double> &row) const {
	int count = 0;
	for (unsigned long i = 0; i < row.size(); i++) {
		if (ec.check (i, row) > 0)
			count++;
	}
	return count;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::matterECFunc
*
*	Description:	Подсчет целевой функции для элементарного условия
*	Parameters:		ec - Элементарное условие
*					param - параметр набора данных, на котором проверяется элементароное условие
*					abnormalBehaviourType - тип нештатного поведения, для которого производится подсчет целевой функции
*	Returns:		double - значение целевой функции
*	Throws:			AxiomLibException - если функция получает несогласованные параметры от набора данных
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyMultiDataLearnAlgorithm::matterECFunc (ElemCondPlusStat &ec, const int param, const int abnormalBehaviourType) const {
	int numOfClasses, numOfNormalMultiTS;
	std::vector <int> numOfMultiTS, numOfNormalTS;
	std::vector < std::vector <int> > numOfTS;
	std::vector <double> curTS;
	int classCount, classLen;
	// Получение информации о наборе данных
	fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	// Цикл по траекториям из обучающей выборки, содержащим траектории нештатного поведения типа i
	/* // Эта проверка проходит не один раз - здесь возможно исключить
	if (numOfMultiTS[abnormalBehaviourType] != numOfTS[abnormalBehaviourType].size())
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterECFunc : incorrect response from internal function.");
	*/
	classCount = 0;
	classLen = 0;
	int numTS = (int) numOfTS[abnormalBehaviourType].size();
	//std::cout<<"numTS = "<<numTS<<std::endl;
	int numOccurred = 0;
	for (int j = 0; j < numTS; j++) {
		curTS.clear();
		if (fuzzyDataSet.getTSByIndexFromClass (curTS, abnormalBehaviourType, j, param)) {
			int currentClassCount = numOfCarriedOutItems (ec, curTS);
			classCount += currentClassCount;
			classLen += curTS.size();
			if(currentClassCount > 0) {
				++numOccurred;
			}
		}
	}
	//std::cout<<"numOccurred = "<<numOccurred<<std::endl;
	// Заполняем статистику по нештатному поведению
	if (classLen > 0) {
		ec.statAbnorm = (double) classCount/ (double) classLen;
		ec.statOccurence = (double) numOccurred / (double) numTS;
	}
	else {
		ec.statAbnorm = -1.0; // - значит данные не определены
		ec.statOccurence = -1.0;
		std::cout << "\nWarning in  FuzzyMultiDataLearnAlgorithm::matterECFunc : incorrect dstaSet request for abnormal type.\n";
	}

	// Цикл по траекториям нормального поведения
	fuzzyDataSet.getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	/* // Эта проверка проходит не один раз - здесь возможно исключить
	if (numOfNormalMultiTS != numOfNormalTS.size())
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterECFunc : incorrect response from internal function getNormalClassSize");
	*/
	classCount = 0;
	classLen = 0;
	for (int j = 0; j < numOfNormalMultiTS; j++) {
		curTS.clear();
		if (fuzzyDataSet.getNormalTSFromClass (curTS, j, param)) {
			classCount += numOfCarriedOutItems (ec, curTS);
			classLen += curTS.size();
		}
	}
	// Заполняем статистику по нештатному поведению
	if (classLen > 0)
		ec.statNormal = (double) classCount/ (double) classLen;
	else {
		ec.statNormal = -1.0; // - значит данные не определены
		std::cout << "\nWarning in FuzzyMultiDatadLearnAlgorithm::matterECFunc: incorrect dstaSet request.\n";
	}

	// Определение значения целевой функции
	if (ec.statAbnorm < eps)
		ec.goal = 0.0;
	else
		if (ec.statNormal < eps)
			ec.goal = max_goal_val;
		else
			ec.goal = min (ec.statAbnorm / ec.statNormal, max_goal_val);

	return ec.goal;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::saveBestECinFile
*
*	Description:	функция сохраняет описания элементарных условий 
*					из текущего набора лучших условий в файлы.
*	Parameters:		abnormalBehaviourType - тип нештатного поведения, 
*						для которого подбирались параметры элементарных условий
*					dimen - размерность временных рядов, по которой велось
*						определение нештатного поведения
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::saveBestECinFile (const int abnormalBehaviourType, const int  dimen, std::vector <std::vector <ElemCondPlusStat> > &bestECs, std::vector <std::vector <std::string> > &ecNames) const {
	// Формируем общее начало имени файла - в которое заключаем нормер типа нештатного поведения и номер размерности исходных данных
	char tmss[10];
	std::string fileName (ecNameTemplate);
	fileName.append (str_AbnormalType);
	sprintf(tmss, "%d", abnormalBehaviourType + 1);
	fileName.append (tmss);
	fileName.append (str_Dimension);
	tmss[0]='\0';
	sprintf(tmss, "%d", dimen + 1);
	fileName.append (tmss);
	std::string curFileName, tmpFileName;
	int numReal;
	// Получаем вектор названий параметров набора данных
	std::vector <std::string> dataSetParamNames;
	dataSetParamNames = fuzzyDataSet.getParamNames ();
	// Пробегаем по всем существующим лучшим элементарным условиям
	ecNames.resize (bestECs.size());
	for (unsigned int i = 0; i < bestECs.size(); i++) {
		// заносим в имя файла индекс массива с элементарными условиями
		tmpFileName = fileName;
		tmpFileName.append ("_");
		tmss[0]='\0';
		sprintf(tmss, "%d", i + 1);
		tmpFileName.append (tmss);
		// Ресайзим массив, в который будем заносить имена файлов с описаниями элементарных условий
		numReal = 0; // Число реальных элементарных условий - не пустышек
		for (unsigned int j = 0; j < bestECs[i].size(); j++) {
			// Если условие не было заполнено за время работы алгоритма - то не сохраняем его, т.к. это пустышка
			if (bestECs[i][j].goal < -eps)
				continue;
			numReal++;
		}
		ecNames[i].resize(numReal);
		numReal = 0;
		// Запускаем цикл по сохранению в файлы
		for (unsigned int j = 0; j < bestECs[i].size(); j++) {
			// Если условие не было заполнено за время работы алгоритма - то не сохраняем его, т.к. это пустышка
			if (bestECs[i][j].goal < -eps)
				continue;
			// заносим в имя файла второй индекс элементарного условия
			curFileName = tmpFileName;
			curFileName.append ("_");
			tmss[0]='\0';
			sprintf(tmss, "%d", j + 1);
			curFileName.append (tmss);
			// Сохраниение описания элементарного условия
			bestECs[i][j].saveECToFile (axiomBaseDir, curFileName, dataSetParamNames);
			ecNames[i][numReal] = curFileName;
			numReal++;
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::selectElemCond
*
*	Description:	Функция подбора и настройки элементарных условий.
*	Parameters:		void
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::selectElemCond (const int abnormalBehaviourType, const int rank, const int groupRankFrom, const int groupRankUpTo, std::vector <std::vector <std::vector <std::string> > > &bestECFileNames) {
	// Определяем размерность входных данных, т.е. размерность траекторий обучающей выборки
	int numOfDimens = dataSetParams.size();

	// Определяем параметры для распараллеливания
	int forOne, from, upTo, grIndex;
	forOne = (int) ((double) numOfDimens / (double) (groupRankUpTo - groupRankFrom));
	grIndex = numOfDimens - forOne * (groupRankUpTo - groupRankFrom);
	if ((rank - groupRankFrom) < grIndex) {
		from = (rank - groupRankFrom) * (forOne + 1);
		upTo = (rank - groupRankFrom + 1) * (forOne + 1);
	} else {
		from = grIndex * (forOne + 1) + (rank - groupRankFrom - grIndex) * forOne;
		upTo = grIndex * (forOne + 1) + (rank - groupRankFrom - grIndex + 1) * forOne;
	}

	// Запускаем поиск параметров элементарных условий на заданных размерностях исходных данных
	// Предварительно подготавливаем необходимые перменные
	std::vector <std::vector <std::vector <ElemCondPlusStat> > > bestECs;
	std::vector <std::vector <std::vector <std::string> > > ecFileNames;
	// Первое измерение - по числу размерностей исходных данных, которые выбраны для обработки в данном процессе
	bestECs.resize (upTo - from);
	ecFileNames.resize (upTo - from);
	// мульти-тредовость выполнения итераций циклов
	#pragma omp parallel for schedule(dynamic, 1)
	for (int dim = 0; dim < (upTo - from); dim++) {
		// Выбор параметров элементарных условий для заданного типа нештатного поведения по заданной размерности входных данных
		selectElemCond(abnormalBehaviourType, dataSetParams[from + dim], bestECs[dim]);
#ifdef DEBUG_FUZZYMULTYDATA
		std::cout << "Saving selected ecs..."<<std::endl;
#endif
		// Сохранение описаний лучших элементарных условий в файлы
		saveBestECinFile(abnormalBehaviourType, from + dim, bestECs[dim], ecFileNames[dim]);
#ifdef DEBUG_FUZZYMULTYDATA
		std::cout << "Selected ecs saved"<<std::endl;
#endif
		// Очищение созданных векторов с элементарными условиями для новой итерации цикла
		for (unsigned int i = 0; i < bestECs[dim].size(); i++) {
			for (unsigned int j = 0; j < bestECs[dim][i].size(); j++) {
				bestECs[dim][i][j].clear();
			}
			bestECs[dim][i].clear();
		}
		bestECs[dim].clear();
	}

	MPI_Barrier (MPI_COMM_WORLD);

	// Объединение списка с названиями файлов, в которые были записаны описания лучших элементарных условий
	gatherBestECNames (groupRankFrom, groupRankUpTo, ecFileNames, bestECFileNames);
	
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::selectElemCond
*
*	Description:	Функция подбора и настройки элементарных условий.
*	Parameters:		abnormalBehaviourType - тип нештатного поведения для 
*						которого происходит подбор параметров элементарных условий
*					dimension - измерение исходных данных, для которого производится
*						подбор параметров элементарных условий
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::selectElemCond (const int abnormalBehaviourType, const int dimension, std::vector <std::vector <ElemCondPlusStat> > &bestECs) const {
	ElemConditionsFactory ecf;
	// сначала забиваем вектор elemConds элементарными условиями из initialECTypes
	std::vector <ElemCondPlusStat> elemConds (initialECTypes);
	for (unsigned int i = 0; i < elemConds.size(); i++) {
		elemConds[i].dimension = dimension;
	}
	
	int elemCondsSize = elemConds.size();
	
	// Достаем необходимый для обучения ряд
	std::vector<double> teachRow;
	int numOfMultiTS;
	std::vector <int> numOfTS;
	fuzzyDataSet.getNormalClassSize (numOfMultiTS, numOfTS);
	if ((numOfTS.size() != numOfMultiTS) || (numOfMultiTS < 1))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::selectElemCond : not enough data for the algorithm.");
	for (int i = 0; i < numOfMultiTS; i++) {
		if (numOfTS[i] > dimension) {
			fuzzyDataSet.getNormalTSFromClass (teachRow, i, dimension);
			if (teachRow.size() > 0)
				break;
		}
	}
	if (teachRow.size() < 1)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::selectElemCond : incorrect normal data set.");
	// Данный участок ряда необходим чтобы собрать начальную статистику по максимальным и минимальным значениям для
	// некоторого набора переменных в элементарных условиях. Поэтому слишком большие объемы не нужны, можно ограничить 1000.
	/*
	if (teachRow.size() > 1000) {
		teachRow.resize (1000);
	}*/
	
	// изменяем размер по числу типов элементраных условий - для каждого типа будет сохранет набор лучших вариантов условий
	bestECs.resize (elemCondsSize);
	// Создаем перемные для каждого из типов условий - в которых накапливать размер текущей области с одним и тем же значением целевой функции
	std::vector <int> localExtremum;
	localExtremum.resize(elemCondsSize, 0);
	// Индикатор для каждого типа элементарного условия, который показывает, что значение целевой функции растет
	std::vector <bool> goalGrowing;
	goalGrowing.resize (elemCondsSize, true);
	// Создаем переменную для хранения предыдущего значения целевой функции для каждого типа элеметнарного условия
	std::vector <double> curGoal, curStatAbnorm, curStatNorm;
	curGoal.resize (elemCondsSize);
	curStatAbnorm.resize (elemCondsSize);
	curStatNorm.resize (elemCondsSize);

	// теперь в цикле обучаем каждое элементарное условие
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < elemCondsSize; i++) {
		if (this->comments) {
			int th_id = omp_get_thread_num();
			char buf[128];
			sprintf (buf, "\r\tAbType: %d\tDimension: %d\tElem condition: %d out of %d by thread %d.\t", abnormalBehaviourType, dimension, (i+1), elemConds.size(), th_id);
			std::cout<<buf<<std::endl;
		}
		// Изменяем размер по числу лучших элементарных условий, которое сохранять для каждого из типов условий
		bestECs[i].resize(numBestECs);
		// Устанавливаем границы сетки
		elemConds[i].elemCondition->setLimits (leftLimit, rightLimit);
		elemConds[i].elemCondition->setLimits (teachRow, reserve, numOfLevels);
		// Задаем первое значение в сетке параметров
		elemConds[i].elemCondition->changeParams (0);
		// Вычисляем значение целевой функции для условия с заданными параметрами
		curGoal[i] = matterECFunc (elemConds[i], dimension, abnormalBehaviourType);
		curStatAbnorm[i] = elemConds[i].statAbnorm; 
		curStatNorm[i] = elemConds[i].statNormal; 
		// Итеративно повторяем вычисление целевой фунекции и сохранение лучших для всех вариантов параметров условия
		while (elemConds[i].elemCondition->changeParams(1) >= 0) {
			// Считаем значение целевой функции для измененных параметров
			matterECFunc (elemConds[i], dimension, abnormalBehaviourType);
			// Определеяем необходимо ли сохранять элементарное условие с данным набором параметров
			if (abs(elemConds[i].goal - curGoal[i]) < eps) {
				// продолжается область одного значения целевой функции
				localExtremum[i]++;
			} else {
				// значение целевой функции относительно предыдущего значения изменилось
				if (elemConds[i].goal < curGoal[i]) {
					// Значение целевой функции упало - тогда сохраняем если необходимо элементарное условие из закончившейся области 
					if (goalGrowing[i]) {
						// Значит значение целевой функции на предыдущей области параметров было на максимуме - сохраняем условие с одним их тех параметров
						// Для этого определяем на сколько нужно сместить параметры условия "назад", чтобы сохранить центральный элемент области
						localExtremum[i] = round (((double) localExtremum[i]) / 2.0) + 1;
						// Откатываем параметры назад
						elemConds[i].elemCondition->changeParams (- localExtremum[i]);
						// Сохраняем условие среди лучших
						storeBestECs (bestECs[i], elemConds[i], curGoal[i], curStatNorm[i], curStatAbnorm[i]);
						// Возвращяем параметры условия, которые откатывали ранее
						elemConds[i].elemCondition->changeParams (localExtremum[i]);
						// Наконец сохраняем в соответствующей переменной указатель на то, что значение целевой функции начало падать
						goalGrowing[i] = false;
					}
				} else {
					// Значение целевой функции возростает - сохраняем это
					goalGrowing[i] = true;
					curStatNorm[i] = elemConds[i].statNormal; 
					curStatAbnorm[i] = elemConds[i].statAbnorm; 
				}
				// Устанавливаем текущее значение целеовой функции и обнуляем счетчик элементов в последней области с одним значением целевой функции
				curGoal[i] = elemConds[i].goal;
				localExtremum[i] = 0;
			}
		}
	}
	
	return 0;
}

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::gatherBestECNames
*
*	Description:	Функция сбора списков с названиями файлов, в которые были 
*					записаны описания лучших элементарных условий, с различных 
*					процессов, учавствующих в первом этапе работы основного алгоритма.
*	Parameters:		rank - ранг текущего процесса
*					size - общее число процессов, учавствующих в работе алгоритма
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::gatherBestECNames (const int rankFrom, const int rankUpTo, std::vector <std::vector <std::vector <std::string> > > &ecNames, std::vector < std::vector < std::vector <std::string> > > &allNames) const {
	// Если число процессов в группе меньше 2-х - то никаких пересылок делать не надо - все данные расположены локально
	if ((rankUpTo - rankFrom) < 2) {
		allNames = ecNames;
		return 0;
	}
	// значит параметры условий для разных типов нештатного поведения подбирались независимо - необходимо объединить данные
	// Для этого - Создание собственного типа MPI для передачи названий файлов не по одному, а целыми массивами
	struct FileDescr {
		char name[name_max_size];
	};
	struct FileDescr fDescr[2];
	// build datatype describing structure
	MPI_Datatype FileDescrType;
	MPI_Aint base;
	MPI_Datatype type[2] = {MPI_CHAR, MPI_UB};
	int blocklen[2] = {name_max_size, 1};
	MPI_Aint disp[2];
	// compute displacements of structure components
	MPI_Address(fDescr, disp);
	MPI_Address(fDescr+1, disp+1);
	base = disp[0];
	for (int i = 0; i < 2; i++) disp[i] -= base;
	// build datatype describing structure
	MPI_Type_struct( 2, blocklen, disp, type, &FileDescrType);
	// Регистрация типа в MPI
	MPI_Type_commit(&FileDescrType);

	// Создание коммуникатора - внутри которого процессы будут обмениваться сообщениями
	//  Для этого сначала создаем группу
	MPI_Group MPI_GROUP_WORLD, subgroup;
	// Список процессов, которые войдут в коммуникатор
	int ranges[1][3];
	ranges[0][0] = rankFrom;
	ranges[0][1] = rankUpTo - 1;
	ranges[0][2] = 1;
	// Создание группы со всеми процессами
	MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
	// Создание узкой группы процессов, которые между собой будут обмениваться данными
	MPI_Group_range_incl(MPI_GROUP_WORLD, 1, ranges, &subgroup);
	// Определение ранга текущего процесса в созданной группе
	int me;
	MPI_Group_rank(subgroup, &me);

	// Создание коммуникатора на основе созданной узкой группы процессов
	MPI_Comm local_comm;
	MPI_Comm_create(MPI_COMM_WORLD, subgroup, &local_comm);
	int commSize, commRank;
	MPI_Comm_size (local_comm, &commSize);
	MPI_Comm_rank (local_comm, &commRank);

	struct FileDescr *fDescrips;
	int vec[1];
	int *vecs;
	vecs = new int[commSize];
	int secSize, thirdSize, thirdMax, wholeLen;
	// Передача сообщений внутри созданной группы
	if(me != MPI_UNDEFINED) {
		// Определение максимального размера вектора передавамого параметра
		//  Локальное определение максимального размера передавамого параметра
		thirdMax = 0;
		for (unsigned int i = 0; i < ecNames.size(); i++) {
			for (unsigned int j = 0; j < ecNames[i].size(); j++) {
				if (thirdMax < (int) ecNames[i][j].size())
					thirdMax = (int) ecNames[i][j].size();
			}
		}
		//  Глобальное определение максимального размера передавамого параметра
		vec[0] = thirdMax;
		MPI_Allgather(vec, 1, MPI_INT, vecs, 1, MPI_INT, local_comm);
		for (int u = 0; u < commSize; u++) {
			if (thirdMax < vecs[u])
				thirdMax = vecs[u];
		}		
		// Создаем буффер, через который будут передаваться вектора переменных
		fDescrips = new struct FileDescr[thirdMax];
		
		// Определение первой размерности всех передаваемых переменных и соответственно общей длины получаемого результата
		vec[0] = (int) ecNames.size();
		MPI_Allgather(vec, 1, MPI_INT, vecs, 1, MPI_INT, local_comm);
		wholeLen = 0;
		for (int u = 0; u < commSize; u++) {
			wholeLen += vecs[u];
		}
		// Первый индекс результата
		int allFirst = 0;
		allNames.resize (wholeLen);
		// Цикл по первой размерности передаваемого вектора
		for (int proc = 0; proc < commSize; proc++) {
			for (int first = 0; first < vecs[proc]; first++) {
				// Получаем число пересылаемых во второй размерности векторов
				if (proc == commRank) {
					vec[0] = ecNames[first].size();
				}
				MPI_Bcast(vec, 1, MPI_INT, proc, local_comm);
				secSize = vec[0];
				allNames[allFirst].resize(secSize);
				// Посылаем все вектора второй размерности
				for (int sec = 0; sec < secSize; sec++) {
					if (proc == commRank) {				
						for (int third = 0; third < (int) ecNames[first][sec].size(); third++) {
							strncpy(fDescrips[third].name, ecNames[first][sec][third].c_str(), name_max_size - 1);
						}
						vec[0] = (int) ecNames[first][sec].size();
					}
					// Посылаем число передаваемых данных
					MPI_Bcast(vec, 1, MPI_INT, proc, local_comm);
					thirdSize = vec[0];
					// Посылаем собственно данные
					MPI_Bcast(fDescrips, thirdSize, FileDescrType, proc, local_comm);
					// Обрабатываем полученные данные
					allNames[allFirst][sec].resize(thirdSize);
					for (int third = 0; third < thirdSize; third++) {
						allNames[allFirst][sec][third].assign(fDescrips[third].name);
					}
				}
				allFirst++;
			}
		}
		// Удаляем созданнй буффер
		delete fDescrips;
		// Удаляем использованный коммуникатор
		MPI_Comm_free(&local_comm);
	} else {
		// Выход по throw, так как группа MPI была создана неверно
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::gatherBestECNames : incorrect work of MPI - this code should be unreacheble.");
	}
	// Очищиение задействованной для передачи динамической памяти
	delete vecs;
	// Удаление созданных групп
	MPI_Group_free(&MPI_GROUP_WORLD);
	MPI_Group_free(&subgroup);

	return 0;
}
