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
#include <string.h>

using namespace AxiomLib;

// Максимальная длина названия файла с описанием системы аксиом (достаточно большое число)
#define int_AxiomSet_Name_Max_Size				200

// Порог отброса элементов при построении расширенной разметки - 
//  если значение взвешеной суммы для элемента =< этот порог
//  - то данный элемент не войдет в расширенную обощенную разметку
// Порог отброса состоит из двух элементов: вклад в критерий отброса длины масимального участка в разметке
//#define double_weight_markUps_length	0.05
//  вклад в критерий числа повторений в разметке некоторой аксиомы
//#define double_weight_markUps_sens	0.5
// Критерий отсечения - если значение взвешенной суммы от длины и максимальной встречаемости меньше, чем максимальное значение взвешенной суммы * на данный коэффициент, то элемент не войдет в решение
//#define double_threshold_func	0.33

// [0.0; 1.0] - добавка (в своей метрике; значения от 0 - нет добавки до 1 - максимальное значение) к критерию отсечения в функции упрощения обобщенных разметок
//#define double_simplify_coeff	0.3


// Параметр показывает максимальный размер вектора с лучшими системами аксилом, который хранить для всех процессов
//  (должен быть достаточно большой, так как из сохраненных все равно потом выбиарются лучшие)
#define int_max_size_of_vector_axiomExprSet		100

// Максимальное число систем аксиом, которое может быть внесено на каждом шаге алгоритма для одной исходной системы аксиом
#define int_max_number_of_renew_axiom_set		10

// Малая величина чтобы работать с действительными числами
#define eps			0.0000000001


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::createAxiomSet
*
*	Description:	функция формирования системы аксиом из аксиом
*	Parameters:		param - размерность траекторий набора данных с которой работаем 
*					в данном алгоритме
*	Returns:		0 - если алгоритм отработал нормально
*					1 - если алгоритм не был запущен, так как его запуск приводил бы к дублированию функций
*						(если другой процесс уже делает те же действия)
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::createAxiomSet (void) {
	// подготавливаем необходимые переменные
	std::vector <std::string> dataSetParams;
	dataSetParams = fuzzyDataSet.getParamNames();
	
	//  Создаем вектор всех аксиом - чтобы его можно было использовать при составленнии систем аксиом
	int numOfAxioms = 0;
	for (unsigned int i = 0; i < bestAxiomsFileNames.size(); i++) {
		numOfAxioms += bestAxiomsFileNames[i].size();
	}

	// Вектор аксиом, создаваемый по мере формирования аксиом из элементраных условий
	std::vector <AxiomExpr> bestAxioms;

	/* // Для случая - если определение bestAxioms вынесено в определение класса
	// Очищаем содержимое переменной, если это необходимо
	for (unsigned int i = 0; i < bestAxioms.size(); i++)
		bestAxioms[i].clear();
	bestAxioms.clear();
	*/

	// Создаем набор всех аксиом
	bestAxioms.resize(numOfAxioms);
	int ax = 0;
	for (unsigned int i = 0; i < bestAxiomsFileNames.size(); i++) {
		for (unsigned int j = 0; j < bestAxiomsFileNames[i].size(); j++, ax++) {
			// читаем аксиому из файла
			bestAxioms[ax].initAxiomFromFile (axiomBaseDir, bestAxiomsFileNames[i][j], dataSetParams);
		}
	}

	//	Определение ранга текущего процесса
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	// Распараллеливание вычсилений по процессам - выбираем из каких систем аксиом стартовать выполнение процедуры обучения на данном этапе
	int from, upTo, rankFrom, rankUpTo;
	splitRange (rank, size, 0, numOfAxioms, from, upTo, rankFrom, rankUpTo);
	// Для всех процессов - очищаем содержимое переменной для хранения набора лучших систем аксиом
	bestAxiomSets.clear();
	if (rank == rankFrom) { // Обучение производим таким образом, чтобы не дублировать вычисления в различных процессах
		// Создание начальной популяции - создаем системы аксиом только для выбранного диапазона аксиом
		// Создаем набор систем аксиом, каждая из которых будет содержать только одну аксиому
		std::vector <AxiomExprSetPlus> axiomSets, newAxiomSets;
		std::vector < std::vector <AxiomExprSetPlus> > nextStepAxiomSets;
		axiomSets.resize(upTo - from);
		int as = 0;
		#pragma omp parallel for schedule(dynamic, 1)
		for (int i = 0; i < (upTo - from); i++) {
			// Формируем систему аксиом из одной аксиомы
			axiomSets[i].addAxiom (bestAxioms[i + from]);
			// Устанавливаем статистику
			axiomSets[i].axiomsIndex.push_back(i + from);
			// Вычисляем значение целевой функции для такой системы аксиом
			matterAxiomSetFunc (axiomSets[i]);
			/*
			AxiomExprPlus aep;
			AxiomExprStructure aes;
			bestAxioms[i + from].createAxiomExprStructure (aes); 
			aep.initAxiomFromStructure (aes);
			matterAxiomFunc (aep, 0);
			matterAxiomFunc (aep, 1);
			aes.clear();
			aep.clear();
			*/
			// Вывод текущей информации, если это необходимо
			if (this->comments) {
				char buf[128];
				sprintf (buf, "\r\tCurrent axiomSet: %d out of %d. Goal value is %.2f (%d, %d).\tThread %d.\t", from + i + 1, numOfAxioms, axiomSets[i].goal, axiomSets[i].errFirst, axiomSets[i].errSecond, omp_get_thread_num());
				//printf ("\n\tCurrent axiomSet: %d out of %d. Goal value is %.2f (%d, %d).\tThread %d.\t", from + i + 1, numOfAxioms, axiomSets[i].goal, axiomSets[i].errFirst, axiomSets[i].errSecond, omp_get_thread_num());
				std::cout<<buf<<std::endl;
				//std::cout.flush();
			}
		}

		// Итерационный алгоритм построения системы аксиом
		bool condition = true;
		std::vector <int> indicesOfBestSets;
		nextStepAxiomSets.clear();
		int iterNum = 0;
		double defMaxGoal;
		defMaxGoal = -1.0; // (axiomSets[axSet].goal + 1)*2.0; // Делаем defMaxGoal достаточно большим (заведомо больше axiomSets[axSet].goal - даже с учетом вычислительной погрешности)
		// Устанавливаем размер вектора по числу аксиом
		unsigned int wholeNum = 0;
		newAxiomSets.resize (bestAxioms.size());
		while (condition) {
			// Подготавливаем переменную для вектора будущих систем аксиом
			wholeNum = 0;
			nextStepAxiomSets.resize (axiomSets.size());
			for (unsigned int axSet = 0; axSet < axiomSets.size(); axSet++) {
				// Отображаем на экран стадию процесса
				if (this->comments) {
					char buf[128];
					sprintf (buf, "\r\tProc start point: %d - %d\tIteration: %d\tCurrent axiomSet: %d out of %d.\t Thread %d.\t", from + 1, upTo, iterNum+1, axSet+1, axiomSets.size(), omp_get_thread_num());
					std::cout<<buf<<std::endl;
					//std::cout.flush();
				}
				// Очищаем содержимое вектора с промежуточными результатами
				for (unsigned int u = 0; u < newAxiomSets.size(); u++)
					newAxiomSets[u].clear();
				// Для каждой аксиомы - создаем новую систему на основе данной аксиомы и системы axiomSets[axSet]
				#pragma omp parallel for schedule(dynamic, 1)
				for (int ax = 0; ax < (int) bestAxioms.size(); ax++) {
					// Для всех аксиом, еще не входящих в рассматриваемую систему аксиом создаем новую систему
					if (isIn ((unsigned int) ax, axiomSets[axSet].axiomsIndex) < 0) {
						// создаем новую систему аксиом на основе выбранной - добавляя в нее еще невходивщую аксиому
						newAxiomSets[ax] = axiomSets[axSet];
						newAxiomSets[ax].addAxiom (bestAxioms[ax]);
						newAxiomSets[ax].axiomsIndex.push_back(ax);
						// считаем значение целевой функции для полученной системы аксиом и сохраняем его
						matterAxiomSetFunc (newAxiomSets[ax]);
					} else {
						// Если аксиома уже входит в систему - то не создаем новой системы. Заполняем значение целевой функции по умолчанию значением значительно больше axiomSets[axSet].goal .
						newAxiomSets[ax].goal = defMaxGoal;
					}
				}
				// выбираем заданное число лучших систем аксиом и добавляем их в популяцию на следующем шаге алгоритма
				indicesOfBestSets.clear();
				chooseIndicesOfBestAxiomSets (newAxiomSets, indicesOfBestSets, axiomSets[axSet].goal);
				wholeNum += indicesOfBestSets.size();
				addAxiomSets(nextStepAxiomSets[axSet], newAxiomSets, indicesOfBestSets);
			}
			// Переносим исходные системы аксиом в специальный вектор результатов и по ходу дела очищаем их содержимое
			addToBestAxiomSets (axiomSets);
			// Переносим системы аксиом для следующего шага в axiomSets
			axiomSets.resize(wholeNum);
			unsigned int cur = 0;
			double curBestVal = -1.0;
			for (unsigned int y = 0; y < nextStepAxiomSets.size(); y++) {
				for (unsigned int u = 0; u < nextStepAxiomSets[y].size(); u++, cur++) {
					axiomSets[cur] = nextStepAxiomSets[y][u];
					nextStepAxiomSets[y][u].clear();
					if ((curBestVal < -eps) || ((curBestVal - axiomSets[cur].goal) > eps))
						curBestVal = axiomSets[cur].goal;
				}
				nextStepAxiomSets[y].clear();
			}
			nextStepAxiomSets.clear();
			
			// - Убрано, так как рост числа систем аксиом уже ограничивается в функции chooseIndicesOfBestAxiomSets
			// Ограничиваем число систем аксиом, чтобы оно не могло разрастаться со скоростью геометрической прогрессии
			cutDownAxiomSets (axiomSets);

			// Увеличиваем число итераций, проделанных алгоритмом
			iterNum++;

			// Проверяем критерии останова:
			// - выходим, если не было создано ни одной новой системы аксиом с лучшим значением целевой функции
			if (axiomSets.size() < 1) {
				condition = false; 
				break;
			}
			// - выходим, если число итераций больше заданного предела
			if ((iterNum >= maxNumberOfSteps) || ((curBestVal - bestAxiomSetGoal) < eps)) {
				// Переносим исходные системы аксиом в специальный вектор результатов и по ходу дела очищаем их содержимое
				addToBestAxiomSets (axiomSets);
				condition = false;
				break;
			}
		}

		// Сортировка лучших систем аксиом до заданного числа элементов
		sortBestAxiomSets ();

		// Сохранение сформированных систем аксиом в файлы
		saveBestAxiomSets (rank, dataSetParams);
	}

	// Объединение списка сохраненных систем аксиом и статистики по ним
	gatherBestAxiomsSets (rank, size);

	// Синхронизация процессов
	MPI_Barrier (MPI_COMM_WORLD);

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::cutDownAxiomSets
*
*	Description:	Функция сокращает число систем аксиом во входном векторе до 
*					заданного в параметрах класса значения.
*	Parameters:		axiomSets - вектор систем аксиом, который подлежит сокращению 
*					до заданного размера
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::cutDownAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets) const {
	// Проверка входных параметров
	if ((maxAxiomSetPopSize < 1) || ( (int) axiomSets.size() <= maxAxiomSetPopSize))
		return 0;
	
	// Сортировка аксиом по значению целевой функции
	std::vector <int> indecies;
	sortAxiomSets (axiomSets, indecies);
	
	// Определение числа лучших аксиом, сохраняемых в популяции
	unsigned int numBestToSave, numToChoose, numFrom;
	numBestToSave = (unsigned int) round (percentBestAxiomSets*maxAxiomSetPopSize);
	numToChoose = (unsigned int) maxAxiomSetPopSize - numBestToSave;
	
	numFrom = axiomSets.size() - numBestToSave;
	std::vector <bool> chosen;
	chosen.resize (numFrom, false);
	int curChoice;
	std::vector <int> toSave;
	toSave.resize (maxAxiomSetPopSize);
	// Заносим в вектор сохраняемых элементов - заданное число лучших
	for (unsigned int i = 0; i < numBestToSave; i++) {
		toSave[i] = indecies[i];
	}
	// Случайный выбор оставшихся
	for (unsigned int i = 0; i < numToChoose; i++) {
		curChoice = round (((double) rand() / (double) RAND_MAX) * (double) (numFrom - i - 1));
		for (unsigned int t = (unsigned int) curChoice; t < numFrom; t++)
			if (!chosen[t]) {
				chosen[t] = true;
				toSave[numBestToSave + i] = indecies[numBestToSave + t];
				break;
			}
	}

	// Сортировка массива выбранных для сохранения элементов по убыванию
	unsigned int tmpInt;
	for (unsigned int i = maxAxiomSetPopSize - 1; i > 0; i--) {
		for (unsigned int j = 0; j < i; j++) {
			if (toSave[j] < toSave[j+1]) {
				tmpInt = toSave[j+1];
				toSave[j+1] = toSave[j];
				toSave[j] = tmpInt;
			}
		}
	}
	
	// Удаление невыбранных систем аксиом
	// Очищаем содержимое элементов вектора аксиом
	if ((toSave[0] + 1) < (int) axiomSets.size()) {
		for (unsigned int j = toSave[0] + 1; j < axiomSets.size(); j++)
			axiomSets[j].clear();
		axiomSets.erase (axiomSets.begin() + toSave[0] + 1, axiomSets.end());
	}
	for (int i = 1; i < maxAxiomSetPopSize; i++) {
		if ((toSave[i] + 1) < toSave[i-1]) {
			// Очищаем содержимое элементов вектора аксиом
			for (int j = toSave[i] + 1; j < toSave[i-1]; j++)
				axiomSets[j].clear();
			// Удаляем элементы векторов
			axiomSets.erase (axiomSets.begin() + toSave[i] + 1, axiomSets.begin() + toSave[i-1]);
		}
	}
	// Очищаем содержимое элементов вектора аксиом
	if (toSave[maxAxiomSetPopSize-1] > 0) {
		for (int j = 0; j < toSave[maxAxiomSetPopSize-1]; j++) {
			axiomSets[j].clear();
		}
		axiomSets.erase (axiomSets.begin(), axiomSets.begin() + toSave[maxAxiomSetPopSize-1]);
	}

	return 0;
}	


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::chooseIndicesOfBestAxiomSets
*
*	Description:	Функция выбирает из вектора систем аксиом заданное число
*					лучших, причем их значение целевой функции должно быть
*					меньше заданного значения.
*	Parameters:		newAxiomSets - вектор систем аксиом, из которых выбриаются лучшие
*					indicesOfBestSets - заполняемый вектор индексов лучших систем аксиом
*					goal - значение целевой функции, для критерия выбора лучших систем
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::chooseIndicesOfBestAxiomSets (std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets, double &goal) const {
	// Сортируем вектор систем аксиом по значению целевой функции
	std::vector <int> indecies;
	sortAxiomSets (newAxiomSets, indecies);
	// Ищем первую содержательную систему аксиом
	int first = 0; 
	while (first < (int) indecies.size()) {
		if (newAxiomSets[indecies[first]].goal > -eps)
			break;
		first++;
	}
	// Ищем крайний индекс, начиная с которого все системы аксиом имеют значение целевой функции больше заданного goal
	int last = first;
	while (last < (int) indecies.size()) {
		if ((newAxiomSets[indecies[last]].goal - goal) > eps)
			break;
		last++;
	}
	// Если таких не нашлось - то выходим
	if ((first == (int) indecies.size()) || (first == last)) {
		indicesOfBestSets.clear();
		return 0;
	}
	
	// Определяем максимальное число элементов в результирующем векторе
	//  Выбираем число лучших систем аксиом и случайно выбранных
	int numBest, numRand, num;
	numBest = round ((double) int_max_number_of_renew_axiom_set * percentBestAxiomSets);
	if (numBest > (last - first)) {
		numBest = last - first;
		numRand = round ((double) numBest * (1.0 - percentBestAxiomSets) / percentBestAxiomSets);
		num = numBest + numRand;
	} else {
		numRand = int_max_number_of_renew_axiom_set - numBest;
		num = int_max_number_of_renew_axiom_set;
	}

	// Если доступно меньшее число систем аксиом
	if (((int) newAxiomSets.size() - first) <= num) {
		num = (int) newAxiomSets.size() - first;
		indicesOfBestSets.resize (num);
		for (int i = 0; i < num; i++, first++)
			indicesOfBestSets[i] = indecies[first];
		return 0;
	}

	// Отбираем случайные системы аксиом
	int chooseDelta;
	chooseDelta = indecies.size() - first - numBest;
	std::vector <int> jRand;
	jRand.resize (numRand);
	int cur;
	int condition;
	for (int t = 0; t < numRand; t++) {
		// Случайно выбираем число из отрезка [0, chooseDelta-1]
		cur = first + numBest + (int) (((double) rand () / ((double) RAND_MAX + 1.0)) * (double) chooseDelta);
		// проверяем - не выбирали ли его раньше - если выбирали, то сдвигаем
		do {
			condition = false;
			for (int s = 0; s < t; s++) {
				if (cur == jRand[s]) {
					cur++;
					condition = true;
					break;
				}
			}
		} while (condition);
		// Сохраняем сделанный выбор
		jRand[t] = cur;
		// Уменьшаем диапазон выбора
		chooseDelta--;
	}
	// Сохраняем все выбранные индексы
	indicesOfBestSets.resize (num);
	cur = first;
	for (int i = 0; i < numBest; i++, cur++)
		indicesOfBestSets[i] = indecies[cur];
	cur = 0;
	for (int i = numBest; i < num; i++, cur++)
		indicesOfBestSets[i] = indecies[jRand[cur]];
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::addAxiomSets
*
*	Description:	Функция добавляет системы аксиом из одного вектора, индексы 
*					которых перечислены во входном векторе, в другой вектор.
*	Parameters:		nextStepAxiomSets - вектор систем аксиом, в который добавляются новые системы
*					newAxiomSets - вектор систем, из которого переносятся системы аксиом
*					indicesOfBestSets - вектор индексов переносимых систем аксиом
*	Returns:		0
*	Throws:			AxiomLibException - если вектор, в который сохранять системы аксиом не пуст
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::addAxiomSets(std::vector <AxiomExprSetPlus> &nextStepAxiomSets, std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets) const {
	if (nextStepAxiomSets.size() > 0)
		throw AxiomLibException("Error in FuzzyMultiDataLearnAlgorithm::addAxiomSets: input arguments out of consistency.");
	// Определяем сколько систем аксиом добавлять
	unsigned int toAdd = 0;
	for (unsigned int i = 0; i < indicesOfBestSets.size(); i++) {
		if ((indicesOfBestSets[i] >= 0 ) && (indicesOfBestSets[i] < (int) newAxiomSets.size()))
			toAdd++;
	}
	// Заносим все новые системы аксиом в вектор всех систем в соответствии с указанным индексом
	unsigned int curSize = 0; // nextStepAxiomSets.size();
	nextStepAxiomSets.resize (toAdd);
	for (unsigned int i = 0; i < indicesOfBestSets.size(); i++) {
		if ((indicesOfBestSets[i] >= 0 ) && (indicesOfBestSets[i] < (int) newAxiomSets.size())) {
			nextStepAxiomSets[curSize] = newAxiomSets[indicesOfBestSets[i]];
			curSize++;
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::sortBestAxiomSets
*
*	Description:	Функция сокращает число элементов вектора лучших систем аксиом
*					до заданного по умолчанию или в конфигурационном файле значения
*	Parameters:		-
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::sortBestAxiomSets (void) {
	// Если число элементов в векторе лучших систем аксиом меньше заданного параметра - то просто выходим
	if ((int) bestAxiomSets.size() <= numberOfBestAxiomSets)
		return 0;
	// Если число тех, которые должны выжить - меньше 1, то просто удаляем все элементы
	if (numberOfBestAxiomSets < 1) {
		for (unsigned int i = 0; i < bestAxiomSets.size(); i++) 
			bestAxiomSets[i].clear();
		bestAxiomSets.clear();
		return 0;
	}
	// Выбираем из вектора заданный набор худших систем аксиом и удаляем их
	int numToDel;
	numToDel = (int) bestAxiomSets.size() - numberOfBestAxiomSets;
	std::vector <int> indeciesToDel;
	indeciesToDel.resize(numToDel);
	for (int i = 0; i < numToDel; i++) {
		indeciesToDel[i] = i;
	}
	int curIndex; 
	double curMin;
	for (int i = numToDel; i < (int) bestAxiomSets.size(); i++) {
		curMin = bestAxiomSets[i].goal;
		curIndex = -1;
		for (int j = 0; j < numToDel; j++) {
			if (curMin > bestAxiomSets[indeciesToDel[j]].goal) {
				curMin = bestAxiomSets[indeciesToDel[j]].goal;
				curIndex = j;
			}
		}
		if (curIndex >= 0) {
			indeciesToDel[curIndex] = i;
		}
	}
	// Вектор индексов составлен - Сортируем его по убыванию
	for (int i = numToDel - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (indeciesToDel[j] < indeciesToDel[j+1]) {
				curIndex = indeciesToDel[j+1];
				indeciesToDel[j+1] = indeciesToDel[j];
				indeciesToDel[j] = curIndex;
			}
		}
	}
	// Удаляем системы аксиом в соответствии с выбранным вектором индексов
	for (int i = 0; i < numToDel; i++) {
		bestAxiomSets[indeciesToDel[i]].clear();
		bestAxiomSets.erase (bestAxiomSets.begin() + indeciesToDel[i]);
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::saveBestAxiomSets
*
*	Description:	Сохранение лучших сформированных систем аксиом
*	Parameters:		dataSetParams - названия размерностей набора данных
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::saveBestAxiomSets (const int rank, std::vector <std::string> &dataSetParams) {
	// Сохранение построенных систем аксиом в файлы
	bestAxiomSetsNamesPlus.resize (bestAxiomSets.size());
	std::string curAxiomSetName;
	char tmss[20];
	for (unsigned int asNum = 0; asNum < bestAxiomSets.size(); asNum++) {
		curAxiomSetName = axiomSetName;
		tmss[0]='\0';
		sprintf(tmss, "%d_%d", rank, asNum + 1);
		curAxiomSetName.append (tmss);
		bestAxiomSets[asNum].saveAxiomSetToFile (axiomSetBaseDir, curAxiomSetName, dataSetParams);
		// Сохраняем название файла и статистику по данной системе аксиом в специальную внутреннюю переменную
		bestAxiomSetsNamesPlus[asNum].str = curAxiomSetName;
		bestAxiomSetsNamesPlus[asNum].doubleVal = bestAxiomSets[asNum].goal;
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::gatherBestAxiomsSets
*
*	Description:	Функция собирает имена файлов с описаниями лучших систем 
*					аксиом с разных процессов, на которые был распараллелен 
*					основной алгоритм.
*	Parameters:		rank - ранг текущего процесса
*					size - общее число процессов, на которые был распараллелен алгоритм
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::gatherBestAxiomsSets (const int rank, const int size) {
	if (size > 1) { // значит параметры условий для разных типов нештатного поведения подбирались независимо - необходимо объединить данные
		// Для этого - Создание собственного типа MPI для передачи названий файлов не по одному, а целыми массивами
		struct FileDescrPlus {
			char name[int_AxiomSet_Name_Max_Size]; // file name
			double goal[1]; // value of goal-function for axiomSet
		};
		struct FileDescrPlus fDescr[2];
		/* build datatype describing structure */
		MPI_Datatype FileDescrPlusType;
		MPI_Aint base;
		MPI_Datatype type[3] = {MPI_CHAR, MPI_DOUBLE, MPI_UB};
		int blocklen[3] = {int_AxiomSet_Name_Max_Size, 1, 1};
		MPI_Aint disp[3];
		/* compute displacements of structure components */
		MPI_Address(fDescr, disp);
		MPI_Address(fDescr[0].goal, disp+1);
		MPI_Address(fDescr+1, disp+2);
		base = disp[0];
		for (int i = 0; i < 3; i++) disp[i] -= base;
		/* build datatype describing structure */
		MPI_Type_struct( 3, blocklen, disp, type, &FileDescrPlusType);
		// Регистрация типа в MPI
		MPI_Type_commit(&FileDescrPlusType);

		// Передача или прием списка с названиями файлов соответственно
		unsigned int buffer[1];
		if (rank == 0) { // этот процесс собирает данные
			struct FileDescrPlus *fDescrips;
			unsigned int st, vecSize;
			MPI_Status status;
			for (int procNum = 1; procNum < size; procNum++) {
				// получаем число векторов, содержащих вектора со строками - первую размерность всей структуры
				MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 400 + procNum, MPI_COMM_WORLD, &status);
				vecSize = buffer[0];
				st = bestAxiomSetsNamesPlus.size();
				bestAxiomSetsNamesPlus.resize (st + vecSize);
				fDescrips = new struct FileDescrPlus[vecSize];
				MPI_Recv( fDescrips, vecSize, FileDescrPlusType, procNum, 401 + procNum, MPI_COMM_WORLD, &status);
				for (unsigned int i = 0; i < vecSize; i++, st++) {
					// Сохраняем полученные значения - вектор имен файлов с описанием элементарных условий
					bestAxiomSetsNamesPlus[st].str.assign (fDescrips[i].name);
					bestAxiomSetsNamesPlus[st].doubleVal = fDescrips[i].goal[0];
				}
				delete fDescrips;
			}
		} else { // этот процесс посылает данные
			struct FileDescrPlus *fDescrips;
			// посылаем число размерностей, по которым в этом процессе велся поиск параметров элементарных условий
			buffer[0] = bestAxiomSetsNamesPlus.size();
			MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 400 + rank, MPI_COMM_WORLD);
			// Если число строк с именами файлов > 0, то посылаем их
			if (bestAxiomSetsNamesPlus.size() > 0) {
				// Копируем строки с именами файлов в созданную структуру для передачи средствами MPI
				fDescrips = new struct FileDescrPlus[bestAxiomSetsNamesPlus.size()];
				for (unsigned int t = 0; t < bestAxiomSetsNamesPlus.size(); t++) {
					strncpy(fDescrips[t].name, bestAxiomSetsNamesPlus[t].str.c_str(), int_AxiomSet_Name_Max_Size - 1);
					fDescrips[t].goal[0] = bestAxiomSetsNamesPlus[t].doubleVal;
				}
				// посылаем собственно вектор с именами файлов
				MPI_Send( fDescrips, bestAxiomSetsNamesPlus.size(), FileDescrPlusType, 0, 401 + rank, MPI_COMM_WORLD);
				delete fDescrips;
			}
		}
	}

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::isIn
*
*	Description:	Функция возвращает первый индекс вхождения элемента в вектор, 
*					или -1 если такого элемента в векторе нет
*	Parameters:		elem - искомый элемент
*					vec - вектор, в котором ведется поиск
*	Returns:		-1 - если элемент не входит в вектор
*					>=0  - индекс первого вхождения элемента elem в вектор vec
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::isIn (const unsigned int elem, const std::vector <unsigned int> &vec) const {
	for (int i = 0; i < (int) vec.size(); i++)
		if (elem == vec[i]) {
			return i;
		}
	return -1;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::addToBestAxiomSets
*
*	Description:	Функция сохраняет поданную набор систем аксиом в векторе лучших систем, 
*					если там есть еще место или заменяет систему аксиом из вектора на данную
*					систему.
*	Parameters:		axiomExprSetPlus - сохраняемая система аксиом
*	Returns:		-1 - если в ходе выполнения возникла ошибка
*					0  - если система аксиом была сохранена
*					1  - если целевая функция у данной системы аксиом больше, чем у системе 
*						 в векторе лучших систем, поэтому данная система не была сохранена
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::addToBestAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets) {
	// Копируем содержимое вектора bestAxiomSets во временную переменную
	std::vector <AxiomExprSetPlus> tmpBest;
	tmpBest.resize (bestAxiomSets.size());
	for (unsigned int i = 0; i < bestAxiomSets.size(); i++) {
		tmpBest[i] = bestAxiomSets[i];
		bestAxiomSets[i].clear();
	}
	// Выстраиваем вектора tmpBest и axiomSets по возрастанию
	std::vector <int> indeciesSets, indeciesBest;
	sortAxiomSets (axiomSets, indeciesSets);
	sortAxiomSets (tmpBest, indeciesBest);
	// Выбираем какие системы аксиом сохранять
	unsigned int s = 0;
	unsigned int b = 0;
	unsigned int sMax = axiomSets.size();
	unsigned int bMax = tmpBest.size();
	unsigned int upTo = int_max_size_of_vector_axiomExprSet;
	if ((sMax + bMax) < upTo) {
		upTo = sMax + bMax;
		s = sMax;
		b = bMax;
	}
	while ((s + b) < upTo) {
		if (b >= bMax) {
			s++;
			continue;
		}
		if (s >= sMax) {
			b++;
			continue;
		}
		if (tmpBest[indeciesBest[b]].goal < axiomSets[indeciesSets[s]].goal)
			b++;
		else
			s++;
	}

	// Переносим выбранные системы аксиом в вектор лучших
	bestAxiomSets.resize (upTo);
	for (unsigned int i = 0; i < b; i++)
		bestAxiomSets[i] = tmpBest[i];
	for (unsigned int i = 0; i < s; i++, b++)
		bestAxiomSets[b] = axiomSets[i];

	// Очищаем использованные вектора
	for (unsigned int i = 0; i < axiomSets.size(); i++)
		axiomSets[i].clear();
	axiomSets.clear();
	for (unsigned int i = 0; i < tmpBest.size(); i++)
		tmpBest[i].clear();
	tmpBest.clear();

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::sortAxiomSets
*
*	Description:	Функция сортирует вектор систем аксиом по неубыванию целевой фунекции
*	Parameters:		axiomSets - вектор сортируемых систем аксиом
*					indecies - вектор индексов систем аксиом в порядке неубывания значения целевой функции
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::sortAxiomSets (const std::vector <AxiomExprSetPlus> &axiomSets, std::vector <int> &indecies) const {
	indecies.resize (axiomSets.size());
	for (int i = 0; i < (int) axiomSets.size(); i++)
		indecies[i] = i;
	int tmp;
	for (int i = (int) axiomSets.size() - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (axiomSets[indecies[j]].goal > axiomSets[indecies[j+1]].goal) {
				tmp = indecies[j+1];
				indecies[j+1] = indecies[j];
				indecies[j] = tmp;
			}
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом:
*					1. разметка траекторий обучающей выборки и формирование
*					   расщиренных обобщенных разметок
*					2. разметка траекторий контрольной выборки и подсчет 
*					   значения целевой функции
*	Parameters:		as - система аксиом, для которой считается значение целевой функции
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc (AxiomExprSetPlus &as) const {
	int numOfClasses;
	std::vector <int> numOfMultiTS;
	std::vector < std::vector <int> > numOfTS;
	std::vector <double> curTS;
	std::vector <int> tempLen, corrMarkUp, curMarkUp, result;
	// Получение информации об обучающем наборе данных
	fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	int dataSetParamsSize = fuzzyDataSet.paramNamesSize();
	// Полная проверка корректности полученных данных
	/* // Эти проверки уже были сделаны - при каждом обращении к функции их делать не обязательно
	if ((numOfClasses != numOfMultiTS.size()) || (numOfClasses != numOfTS.size()))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc : incorrect response from internal function of dataSet.");
	for (int u = 0; u < numOfClasses; u++) {
		if (numOfMultiTS[u] != (int) numOfTS[u].size()) 
			throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc : incorrect response from internal function of dataSet (level2).");
		for (int v = 0; v < (int) numOfTS[u].size(); v++) {
			if (dataSetParamsSize != numOfTS[u][v])
				throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc: incorrect response from some of dataSet function (level3).");
		}
	}
	*/
	// для каждого класса нештатного поведения формируем расширенные обобщенные разметки
	std::vector < std::vector <int> > resMarkUps, genMarkUps;
	int upToNumTS;
	int errFirstVal, errSecondVal;
	as.errFirst = 0;
	as.errSecond = 0;
	as.markUps.resize(numOfClasses);
	for (int abType = 0; abType < numOfClasses; abType++) {
		// Определение числа используемых траекторий для формирования разметок
		if ((numberOfUsedClassTraj > 0) && (numberOfUsedClassTraj < numOfMultiTS[abType]))
			upToNumTS = numberOfUsedClassTraj;
		else
			upToNumTS = numOfMultiTS[abType];
		// Подготавливаем переменные для записи разметок траекторий, содержащих участки нештатного поведения
		genMarkUps.clear();
		resMarkUps.resize (upToNumTS);
		for (int multiTS = 0; multiTS < upToNumTS; multiTS++) {
			createRefMarkUp (resMarkUps[multiTS], as, abType, multiTS, numOfTS);
		}
		// Упрощаем разметки - удаляем 0 в начале и в конце векторов - чтобы общую часть было искать проще
		simplifyMarkUps (resMarkUps);
		// На основании полученных разметок траекторий аномального поведения - производим их сдвиг и формируем варианты обобщенных разметок
		createMarkUpVariants (genMarkUps, resMarkUps);
		// Упрощаем вектора разметок - удаляем 0 в начале и в конце. Плюс возможно внесение некоторых изменений в саму разметку
		simplifyMarkUps (genMarkUps);
		// Добавляем минимальные варианты разметок - если общую часть выделить не удалось
		if (genMarkUps.size() < 1)
			createSimpleMarkUpVariants (genMarkUps, (int) as.axioms.size());
		// Выбираем из всех разметок лучшую для данного класса нештатного поведения и сохраняем ее
		chooseBestMarkUp (as, abType, as.markUps[abType], genMarkUps, errFirstVal, errSecondVal);
		as.errFirst += errFirstVal;
		as.errSecond += errSecondVal;
	}
	// Вычисление общего значения целевой фунекции для данной системы аксиом с учетом выбранных разметок эталонных траекторий
	//matterAxiomSetFunc (as, as.markUps);
	as.goal = goalStrategy->compute(as.errFirst, as.errSecond);

	return as.goal;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом с заданным вариантом 
*					разметок эталонных траекторий
*	Parameters:		as - система аксиом, для которой считается значение целевой функции
*					markUps - разметки эталонных траекторий
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc (AxiomExprSetPlus &as, std::vector <std::vector <int> > &markUps) const {
	int tmpFirst, tmpSecond;
	double tmpGoal;
	as.errFirst = 0;
	as.errSecond = 0;
	for (int abType = 0; abType < (int) markUps.size(); abType++) {
		matterAxiomSetFunc (as, abType, markUps[abType], tmpGoal, tmpFirst, tmpSecond);
		as.errFirst += tmpFirst;
		as.errSecond += tmpSecond;
	}
	// Вычисление значения целевой функции для полученного числа ошибок I и II рода
	as.goal = goalStrategy->compute(as.errFirst, as.errSecond);
	return as.goal;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом для заданного 
*					типа нештатного поведения и заданного варианта разметки 
*					эталонной траектории этого типа
*	Parameters:		as - система аксиом, для которой считается значение целевой функции
*					abType - тип нештатного поведения, для которого считается значение целевой функции
*					genMarkUp - вариант разметки эталонной траектории заданного типа нештатного поведения
*					goalVal - заполняемое значение целевой функции
*					errFirstVal - заполняемое значение число ошибок I рода
*					errSecondVal - заполняемое значение число ошибок II рода
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc (AxiomExprSetPlus &as, int abType, std::vector <int> &genMarkUp, double &goalVal, int &errFirstVal, int &errSecondVal) const {
	int numOfClasses;
	std::vector <int> numOfMultiTS;
	std::vector < std::vector <int> > numOfTS;
	// получаем информацию о числе траекторий контрольной выборки
	fuzzyDataSet.getTestSize (numOfClasses, numOfMultiTS, numOfTS);
	if ((abType < 0) || (abType >= numOfClasses))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc: incorrect input number of abnormal type.");
	// Подготавливаем данные - какие измерения рядов задействованы системой аксиом
	std::vector <bool> dims;
	dims.resize (fuzzyDataSet.paramNamesSize(), false);
	for (unsigned int axNum = 0; axNum < as.axioms.size(); axNum++) {
		for (unsigned int ecNumI = 0; ecNumI < as.axioms[axNum]->expression.size(); ecNumI++) {
			for (unsigned int ecNumII = 0; ecNumII < as.axioms[axNum]->expression[ecNumI].size(); ecNumII++) {
				if ((as.axioms[axNum]->expression[ecNumI][ecNumII].dimension < 0) || (as.axioms[axNum]->expression[ecNumI][ecNumII].dimension >= (int) dims.size()))
					throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc : incomplete definition of elementary condition, it's data set dimension is out of range.");
				dims[as.axioms[axNum]->expression[ecNumI][ecNumII].dimension] = true;
			}
		}
	}
	// подготавливаем перменные для хранения числа ошибок
	errFirstVal = 0;
	errSecondVal = 0;
	// Выбираем очередную траекторию для заданного типа нештатного поведения и запускаем распознаватель
	std::vector <int> curMarkUp, result;
	int num;
	for (int t = 0; t < (int) numOfTS[abType].size(); t++) {
		// разметка траектории контрольной выборки системой аксиом as
		createTestMarkUp (curMarkUp, as, dims, abType, t, numOfTS[abType][t]);

		// Распознавание нештатного поведения в разметке ряда
		recognizer->run (curMarkUp, genMarkUp, result);
		
		// Вычисление числа ошибок первого и второго рода
		num = getStatistic (result);
		
		// Суммирование числа ошибок
		if (num == 0)
			errSecondVal++;
		else
			errFirstVal += num - 1;
	}
	// Пробегаем по всем рядам нормального поведения
	int numNormalMultiTS;
	std::vector <int> numNormalTS;
	fuzzyDataSet.getNormalTestSize (numNormalMultiTS, numNormalTS);
	for (int t = 0; t < (int) numNormalTS.size(); t++) {
		// размечаем траекторию нормального поведения
		createTestMarkUp (curMarkUp, as, dims, t, numNormalTS[t]);

		// Распознавание нештатного поведения в разметке ряда
		recognizer->run (curMarkUp, genMarkUp, result);
		
		// Вычисление числа ошибок первого и второго рода
		num = getStatistic (result);
		
		// Суммирование числа ошибок
		errFirstVal += num;
	}

	// Вычисление значения целевой функции для полученного числа ошибок I и II рода
	goalVal = goalStrategy->compute(errFirstVal, errSecondVal);

	return goalVal;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом описанной в файле
*	Parameters:		baseDir - название каталога, где лежит требуемое описание системы аксиом
*					fileName - файл с описанием системы аксиом, для которой считается значение целевой функции
*					errFirst - число ошибок I рода
*					errSecond - число ошибок II рода
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyMultiDataLearnAlgorithm::matterAxiomSetFunc (const std::string baseDir, const std::string fileName, int &errFirst, int &errSecond) const {
	// Инициализация системы аксиом из указанного файла
	AxiomExprSetPlus aesp;
	AxiomSetBase axiomSetBase;
	AxiomExprSetStructure aess;
	std::string axiomSetPath = baseDir;
	axiomSetPath.append("/");
	axiomSetPath.append(fileName);
	// Считываем информацию о системе аксиом в aess
	std::vector <std::string> dataSetParams;
	dataSetParams = fuzzyDataSet.getParamNames();
	axiomSetBase.readFromAS (axiomSetPath, aess, dataSetParams);
	aesp.initAxiomSetFromStructure (aess);
	// Вычисление значения целевой фукнции
	matterAxiomSetFunc (aesp);
	// Заполнение результатов и выход из функции
	errFirst = aesp.errFirst;
	errSecond = aesp.errSecond;
	double toReturn = aesp.goal;
	aesp.clear();
	return toReturn;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::getStatistic
*
*	Description:	Функция считает число вхождейни ненулевых областей
*	Parameters:		row - ряд, в котором ищутся ненулевые образы
*	Returns:		int - число ненулевых областей
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::getStatistic (std::vector <int> &row) const {
	int num = 0;
	int i = 0;
	int j;
	int len = (int) row.size();
	while (i < len) {
		if (row[i] > 0) {
			num++;
			j = i+1;
			while (j < len) {
				if (row[j]!=row[j-1])
					break;
				j++;
			}
			if ((j - i) > ccNumPoints)
				num += j - i - ccNumPoints;
			i = j;
		} else {
			i++;
		}
	}
	return num;
}

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::chooseBestMarkUp
*
*	Description:	Функция выбирает из всех вариантов для разметок лучшую
*	Parameters:		as - система аксиом, для которой выбирается разметка траектории нештатного типа 
*					abType - тип нештатного поведения, для которого выбирается разметка 
*					markUp - результат работы функции, т.е. выбранный вариант разметки эталонной траектории данного типа
*					genMarkUps - сформированные ранее кандидаты в разметки траекторий нештатного поведения
*					errFirstVal - заполняемое число ошибок I рода для лучшей по значению целевой функции эталонной разметки
*					errSecondVal - заполняемое число ошибок II рода для лучшей по значению целевой функции эталонной разметки
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::chooseBestMarkUp (AxiomExprSetPlus &as, int abType, std::vector <int> &markUp, std::vector <std::vector <int> > &genMarkUps, int &errFirstVal, int &errSecondVal) const {
	// Проверяем размер вектора кандидатов в разметки эталонной траектории заданного типа
	if (genMarkUps.size() < 1) { 
		markUp.clear();
		errFirstVal = 0;
		errSecondVal = 0;
		return 1;
	}
	if (genMarkUps.size() == 1) {
		markUp = genMarkUps[0];
		double tmpGoalVal;
		matterAxiomSetFunc (as, abType, genMarkUps[0], tmpGoalVal, errFirstVal, errSecondVal);
		return 0;
	}
	// Подготавливаем необходимые перменные
	double goalVal, goalTmp;
	int errFirstTmp, errSecondTmp;
	int curBestIndex = 0;
	// Вычисляем число ошибок I и II рода для заданного варианта разметки эталонной траектории заданного типа и Выбираем лучшее значение целевой функции
	matterAxiomSetFunc (as, abType, genMarkUps[0], goalVal, errFirstVal, errSecondVal);
	for (unsigned int i = 1; i < genMarkUps.size(); i++) {
		matterAxiomSetFunc (as, abType, genMarkUps[i], goalTmp, errFirstTmp, errSecondTmp);
		if (goalTmp < goalVal) {
			goalVal = goalTmp;
			errFirstVal = errFirstTmp;
			errSecondVal = errSecondTmp;
			curBestIndex = i;
		}
	}
	// Сохраняем соотвествующую лучшую разметку
	markUp = genMarkUps[curBestIndex];
	return 0;
}

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::createMarkUpVariants
*
*	Description:	Функция создает вариант разметок траекторий нештатного поведения
*					по разметкам траекторий, содержащих участки нештатного поведения.
*	Parameters:		genMarkUps - заполняемый вектор возможных вариантов эталонной разметки 
*						для некоторого типа нештатного поведения
*					resMarkUps - можество разметок траекторий, содержащих участки 
*						нештатного поведения.
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::createMarkUpVariants (std::vector < std::vector <int> > &genMarkUps, std::vector < std::vector <int> > &resMarkUps) const {
	// Если число траекторий меньше 2, то объединять нечего - выходим из функции
	if (resMarkUps.size() < 2) {
		genMarkUps = resMarkUps;
		return 0;
	}
	// Если число траекторий хотя бы 2, то выделяем из них наибольшую общую подпоследовательность
	std::vector < std::vector <int> > square;
	getSquare (resMarkUps[0], resMarkUps[1], square);
	getSubsequences (resMarkUps[0], resMarkUps[1], resMarkUps[0].size() - 1, resMarkUps[1].size() - 1, square, genMarkUps);
	// Для всех остальных траекторий - также выделяем общую подпоследовательность с уже выделенной последовательностью
	std::vector <bool> toSave;
	std::vector <std::vector <int> > curRes, toAdd;
	unsigned int cRes;
	for (unsigned int i = 2; i < resMarkUps.size(); i++) {
		toAdd.clear();
		toSave.resize (genMarkUps.size(), true);
		for (unsigned int j = 0; j < genMarkUps.size(); j++) {
			// Очищаем используемые перменные
			curRes.clear();
			square.clear();
			// Запускаем вычисление общих подстрок
			getSquare (genMarkUps[j], resMarkUps[i], square);
			getSubsequences (genMarkUps[j], resMarkUps[i], genMarkUps[j].size() - 1, resMarkUps[i].size() - 1, square, curRes);
			// Анализируем полученные значения
			cRes = curRes.size();
			if (cRes == 1) {
				// Значит просто заменяем данную строку общей подстрокой 
				genMarkUps[j].clear();
				genMarkUps[j] = curRes[0];
			} else {
				// Если число наибольших подстрок не 1, то сохраняем их в отедельную переменную toAdd
				toSave[j] = false;
				toAdd.insert (toAdd.begin(), curRes.begin(), curRes.end());
			}
		}
		// Вносим изменения, полученные после поиска общих подпоследовательностей с уже выделенными подстроками
		for (int m = (int) toSave.size() - 1; m >= 0; m--) {
			if (!toSave[m]) {
				genMarkUps.erase (genMarkUps.begin() + m);
				toSave[m] = true;
			}
		}
		genMarkUps.insert (genMarkUps.begin(), toAdd.begin(), toAdd.end());
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::createSimpleMarkUpVariants
*
*	Description:	Функция создает самый простой вариант разметок траекторий нештатного поведения
*					путем создания их, состоящи ищ одной акиомы.
*	Parameters:		genMarkUps - заполняемый вектор возможных вариантов эталонной разметки 
*						для некоторого типа нештатного поведения
*					numOfAxioms - число аксиом в системе аксиом, для которой создаются варианты разметки
*	Returns:		int - число созданных разметок
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::createSimpleMarkUpVariants (std::vector < std::vector <int> > &genMarkUps, const int numOfAxioms) const {
	genMarkUps.resize(numOfAxioms);
	for (int i = 0; i < numOfAxioms; i++) {
		genMarkUps[i].resize(1);
		genMarkUps[i][0] = i+1;
	}
	return numOfAxioms;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::createRefMarkUp
*
*	Description:	Разметка многомерного ряда multiTSNumber системой 
*					аксиом as с записью результата в markUp.
*	Parameters:		resMarkUp - разметка траектории
*					as - система аксиом, которой происходит разметка
*					abType - номер класса нештатного поведения
*					multiTSNumber - номер многомерного ряда для данного 
*					класса нештатного поведения, разметка которого происходит
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::createRefMarkUp (std::vector <int> &markUp, AxiomExprSetPlus &as, const int abType, const int multiTSNumber, std::vector < std::vector <int> > &numOfTS) const {
	// перед началом пробегаем по содержимому системы аксиом и составляем спосок необходимых траекторий
	std::vector <bool> dims;
	dims.resize (fuzzyDataSet.paramNamesSize(), false);
	for (unsigned int axNum = 0; axNum < as.axioms.size(); axNum++) {
		for (unsigned int ecNumI = 0; ecNumI < as.axioms[axNum]->expression.size(); ecNumI++) {
			for (unsigned int ecNumII = 0; ecNumII < as.axioms[axNum]->expression[ecNumI].size(); ecNumII++) {
				if ((as.axioms[axNum]->expression[ecNumI][ecNumII].dimension < 0) || (as.axioms[axNum]->expression[ecNumI][ecNumII].dimension >= (int) dims.size()))
					throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::createRefMarkUp : incomplete definition of elementary condition, it's data set dimension is out of range.");
				dims[as.axioms[axNum]->expression[ecNumI][ecNumII].dimension] = true;
			}
		}
	}
	// подготавливаем необходимые переменные
	std::vector < std::vector<double> > multiTS;
	multiTS.resize (fuzzyDataSet.paramNamesSize());
	// вытаскиваем траекторию из обучающей выборки для данного класса нештатного поведения (вытаскиваем только нужные размерности)
	int rowLen = 0;
	for (int t = 0; t < numOfTS[abType][multiTSNumber]; t++) {
		if (dims[t]) {
			if (!fuzzyDataSet.getTSByIndexFromClass (multiTS[t], abType, multiTSNumber, t))
				throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::createRefMarkUp : unable to get concrete time series from dataSet.");
			rowLen = multiTS[t].size();
		} else {
			multiTS[t].clear();
		}
	}
	// вытащили нужные размерности ряда, теперь производим его разметку
	as.enter (markUp, multiTS, 0, rowLen);
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::createTestMarkUp
*
*	Description:	Разметка многомерного ряда из контрольной выборки системой 
*					аксиом as с записью результата в markUp.
*	Parameters:		markUp - разметка траектории
*					as - система аксиом, которой происходит разметка
*					dims - данные о системе аксиом: какие размерности рядов зайдествованы
*					abType - тип нештатного поведения для которого выбирается ряд для анализа
*					indexTest - номер ряда для некоторого класса нештатного поведения
*					numOfTestTS - номер индекса данного многомерного ряда
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::createTestMarkUp (std::vector <int> &markUp, AxiomExprSetPlus &as, std::vector <bool> &dims, int &abType, int &indexTest, int &numOfTestTS) const {
	// подготавливаем необходимые переменные
	std::vector < std::vector<double> > multiTS;
	multiTS.resize (fuzzyDataSet.paramNamesSize());
	// вытаскиваем траекторию из обучающей выборки для данного класса нештатного поведения (вытаскиваем только нужные размерности)
	int rowLen = 0;
	for (int t = 0; t < numOfTestTS; t++) {
		if (dims[t]) {
			if (!fuzzyDataSet.getTSByIndexFromTest (multiTS[t], abType, indexTest, t))
				throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::createTestMarkUp : unable to get concrete time series from dataSet.");
			rowLen = multiTS[t].size();
		} else {
			multiTS[t].clear();
		}
	}
	// вытащили нужные размерности ряда, теперь производим его разметку
	return as.enter (markUp, multiTS, 0, rowLen);
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::createTestMarkUp
*
*	Description:	Разметка многомерного ряда нормального поведения из контрольной 
*					выборки системой аксиом as с записью результата в markUp.
*	Parameters:		markUp - разметка траектории
*					as - система аксиом, которой происходит разметка
*					dims - данные о системе аксиом: какие размерности рядов зайдествованы
*					indexTest - номер ряда для некоторого класса нештатного поведения
*					numOfTestTS - номер индекса данного многомерного ряда
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::createTestMarkUp (std::vector <int> &markUp, AxiomExprSetPlus &as, std::vector <bool> &dims, int &indexTest, int &numOfTestTS) const {
	// подготавливаем необходимые переменные
	std::vector < std::vector<double> > multiTS;
	multiTS.resize (fuzzyDataSet.paramNamesSize());
	// вытаскиваем траекторию из обучающей выборки для данного класса нештатного поведения (вытаскиваем только нужные размерности)
	int rowLen = 0;
	for (int t = 0; t < numOfTestTS; t++) {
		if (dims[t]) {
			if (!fuzzyDataSet.getNormalTSFromTest (multiTS[t], indexTest, t))
				throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::createTestMarkUp : unable to get concrete time series from dataSet.");
			rowLen = multiTS[t].size();
		} else {
			multiTS[t].clear();
		}
	}
	// вытащили нужные размерности ряда, теперь производим его разметку
	return as.enter (markUp, multiTS, 0, rowLen);
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::simplifyMarkUps
*
*	Description:	Упрощение разметки путем удаления 0-ей в начале и в конце
*					вектора разметки
*	Parameters:		genMarkUpы - преобразуемые разметки
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::simplifyMarkUps (std::vector <std::vector <int> > &markUps) const {
	int k, l; 
	for (int i = (int) markUps.size() - 1; i >= 0; i--) {
		// Убираем все 0 в начале вектора - их не учитываем
		k = 0;
		while (k < (int) markUps[i].size()) {
			if (markUps[i][k] > 0)
				break;
			k++;
		}
		// не учитываем все 0 в конце вектора разметки
		l = (int) markUps[i].size() - 1;
		while (l >= k) {
			if (markUps[i][l] > 0)
				break;
			l--;
		}
		if (l < k) {
			// Если были только 0 - то удаляем разметку из набора
			markUps.erase(markUps.begin() + i);
		} else {
			// Удаляем 0 в начале и в конце вектора разметки
			markUps[i].erase(markUps[i].begin() + l + 1, markUps[i].end());
			markUps[i].erase(markUps[i].begin(), markUps[i].begin() + k);
		}
	}	
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::simplifyMarkUp
*
*	Description:	Упрощение разметки и приведение ее в сокращенное описание
*	Parameters:		markUp - сокращенный и упрощенный вариант разметки
*					markUpLen - вектор длин вхождений элементов разметки из simp в 
*						исходную разметку row
*					genMarkUp - преобразуемая разметка
*					genMarkUpSens - частота встречаемости элементов разметки
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
/*int FuzzyMultiDataLearnAlgorithm::simplifyMarkUp (std::vector <int> &markUp, std::vector <int> &markUpLen, std::vector <int> &genMarkUp, std::vector <int> &genMarkUpSens) const {
	markUp.clear();
	markUpLen.clear();
	// Убираем все 0 в начале вектора - их не учитываем
	unsigned int i = 0;
	while (i < genMarkUp.size()) {
		if (genMarkUp[i] > 0)
			break;
		i++;
	}
	// не учитываем все 0 в конеце вектора
	unsigned int upTo = genMarkUp.size() - 1;
	while (upTo >= i) {
		if (genMarkUp[upTo] > 0)
			break;
		upTo--;
	}
	upTo++;
	// Если были только 0 - то выходим из функции
	if (((i+1) == upTo) && (genMarkUp[i] == 0))
		return -1;
	// Пробегаем по всем содержательным элементам и запоминаем максимальное значение вектора + длина участка повторяющихся символов разметки
	int curMaxVal = genMarkUpSens[i];
	int curLen = 1;
	double cutCriteriaMax = countCutCriteria (curLen, curMaxVal);
	for (unsigned int k = i+1; k < upTo; k++) {
		if (genMarkUp[k] == genMarkUp[k-1]) {
			curLen++;
			if (genMarkUpSens[k] > curMaxVal)
				curMaxVal = genMarkUpSens[k];
		} else {
			if (countCutCriteria (curLen, curMaxVal) > cutCriteriaMax)
				cutCriteriaMax = countCutCriteria (curLen, curMaxVal);
			curMaxVal = genMarkUpSens[k];
			curLen = 1;
		}
	}
	// Устанавливаем значение критерия отсечения - по максимальному значению критерия
	double cutCriteria = cutCriteriaMax * double_threshold_func;
	// Цикл по всем содержательным элементам ветора row
	curMaxVal = genMarkUpSens[i];
	curLen = 1;
	for (unsigned int k = i+1; k < upTo; k++) {
		if (genMarkUp[k] == genMarkUp[k-1]) {
			curLen++;
			if (genMarkUpSens[k] > curMaxVal)
				curMaxVal = genMarkUpSens[k];
		} else {
			if (countCutCriteria (curLen, curMaxVal) > cutCriteria) {
				markUp.push_back (genMarkUp[k-1]);
				markUpLen.push_back (curLen);
			}
			curMaxVal = genMarkUpSens[k];
			curLen = 1;
		}
	}
	if (countCutCriteria (curLen, curMaxVal) > cutCriteria) {
		markUp.push_back (genMarkUp[upTo-1]);
		markUpLen.push_back (curLen);
	}
	return 0;
}


// Немного другой вариант реализации той же функции упрощения разметок (по другому выделяет разметки)
int FuzzyMultiDataLearnAlgorithm::simplifyMarkUp2 (std::vector <int> &markUp, std::vector <int> &markUpLen, std::vector <int> &genMarkUp, std::vector <int> &genMarkUpSens) const {
	markUp.clear();
	markUpLen.clear();
	// Убираем все 0 в начале вектора - их не учитываем
	unsigned int i = 0;
	while (i < genMarkUp.size()) {
		if (genMarkUp[i] > 0)
			break;
		i++;
	}
	// не учитываем все 0 в конеце вектора
	unsigned int upTo = genMarkUp.size() - 1;
	while (upTo >= i) {
		if (genMarkUp[upTo] > 0)
			break;
		upTo--;
	}
	upTo++;
	// Если были только 0 - то выходим из функции
	if (((i+1) == upTo) && (genMarkUp[i] == 0))
		return -1;

	// Пробегаем по всем содержательным элементам и запоминаем максимальное значение вектора + длина участка повторяющихся символов разметки
	std::vector <int> entrance, repeat, maxSens;
	entrance.resize(2,0);
	repeat.resize (2,0);
	maxSens.resize (2,0);
	for (unsigned int k = i; k < upTo; k++) {
		if (genMarkUp[k] >= (int) entrance.size()) {
			entrance.resize (genMarkUp[k] + 1, 0);
			repeat.resize (genMarkUp[k] + 1, 0);
			maxSens.resize (genMarkUp[k] + 1, 0);
		}
		entrance[genMarkUp[k]]++;
		repeat[genMarkUp[k]] += genMarkUpSens[k];
		// По ходу дела сохраняем максимальное значение для sens
		if (genMarkUpSens[k] > maxSens[genMarkUp[k]])
			maxSens[genMarkUp[k]] = genMarkUpSens[k];
	}
	// Считаем среднее значение для отношения суммарного числа повторений к суммарному числу наблюдений в обобщенной разметке
	std::vector <double> average;
	average.resize (entrance.size(), 0);
	for (unsigned int i = 0; i < entrance.size(); i++) {
		if (entrance[i] > 0)
			average[i] = ((double) repeat[i]) / ((double) entrance[i]);
	}
	// average - будет выполнять роль критерия отсечения - поэтому добавляем степень свободы - и изменяем значения на некоторый коэффициент
	for (unsigned int i = 0; i < average.size(); i++) {
		if (average[i] > 0)
			average[i] += ((double) maxSens[i] - average[i]) * double_simplify_coeff;
	}
	
	// Цикл по всем содержательным элементам ветора genMarkUp
	std::vector <int> tmpMarkUp; 
	tmpMarkUp.resize (genMarkUp.size());
	int cur = 0;
	for (unsigned int k = i; k < upTo; k++) {
		if ((double) genMarkUpSens[k] >= average[genMarkUp[k]]) {
			tmpMarkUp[cur] = genMarkUp[k];
			cur++;
		}
	}
	// Переносим элементы из tmpMarkUp в markUp
	if (cur > 0) {
		markUp.resize (1, tmpMarkUp[0]);
		markUpLen.resize (1, 1);
		int loc = 0;
		for (int j = 1; j < cur; j++) {
			if (tmpMarkUp[j] == markUp[loc]) {
				markUpLen[loc]++;
			} else {
				markUp.push_back (tmpMarkUp[j]);
				markUpLen.push_back (1);
				loc++;
			}
		}
	}
	return 0;
}*/


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::countCutCriteria
*
*	Description:	Функция вычисляет взвешенную сумму двух компонент. 
*					Используется для вычисления критерия отброса элементов 
*					в функции построения разметок для некотрой системы аксиом
*					по нечетко заданной выборке.
*	Parameters:		len - первая компонента (длина участка разметки)
*					val - максимальная частота встречаемости символа в разметке
*	Returns:		double - взвешенная сумма двух компонент. Коэффициенты взвшивания задаются при помозщи define'ов.
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
/*inline double FuzzyMultiDataLearnAlgorithm::countCutCriteria (int &len, int &val) const {
	return ((double) len * double_weight_markUps_length + (double) val * double_weight_markUps_sens);
}*/


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::extendMarkUpLen
*
*	Description:	Преобразует элементы вектора в интервал из минимального и 
*					максимального значения с заданным коэффиентом
*	Parameters:		stdLen - преобразуемые элементы
*					minLen - заполняемый вектор минимальных значений
*					maxLen - заполняемый вектор максимальных значений
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
/*inline int FuzzyMultiDataLearnAlgorithm::extendMarkUpLen (std::vector <int> &row, std::vector <int> &minLen, std::vector <int> &maxLen, std::vector <int> &stdLen) const {
	// Проверяем входные данные
	if (row.size() != stdLen.size())
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::extendMarkUpLen : incorrect input parameters.");
	if (row.size() < 1)
		return 0;
	// Расширяем минимальное и максимальное число вхождений аксиомы
	std::vector <int> lenghtMin, lenghtMax;
	lenghtMin.resize (stdLen.size());
	lenghtMax.resize (stdLen.size());
	int maxVal = 0;
	for (unsigned int i = 0; i < stdLen.size(); i++) {
		lenghtMin[i] = max (0, round (((double) stdLen[i]) * (1.0 - extendMarkUpLensLevel)));
		lenghtMax[i] = round (((double) stdLen[i]) * (1.0 + extendMarkUpLensLevel));
		// По ходу считаем максимальное значение для верхнего числа повторений
		if (lenghtMax[i] > maxVal)
			maxVal = lenghtMax[i];
	}
	// Вычисляем значение числа повторений для 0
	int zLen = round ((double) maxVal * 0.5);
	if (zLen < 1) 
		zLen = 1;
	// Копируем вектор для его расширения
	std::vector <int> preRow;
	preRow = row;
	// Копируем первые значения
	row.clear();
	row.push_back(preRow[0]);
	minLen.clear();
	minLen.push_back(lenghtMin[0]);
	maxLen.clear();
	maxLen.push_back(lenghtMax[0]);
	int cur = 0;
	// Добавляем в результат текущую разметку и разбавляем ее 0-ми
	for (unsigned int i = 1; i < preRow.size (); i++) {
		if ((row[cur] != 0) && (preRow[i] != 0)) {
			row.push_back (0);
			minLen.push_back (0);
			maxLen.push_back (zLen);
			cur++;
		}
		row.push_back (preRow[i]);
		minLen.push_back (lenghtMin[i]);
		maxLen.push_back (lenghtMax[i]);
		cur++;
	}
	return 0;
}*/


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::getSubsequences
*
*	Description:	Функция находит все максимальные общие подпоследовательности двух строк. 
*					(Может быть несколько различных подпоследовательностей максимальной длины)
*	Parameters:		str1 - первая строка, для которой ищется подпоследовательность
*					str2 - вторая строка, для которой ищется подпоследовательность
*					i - индекс элемента в первой строке, на котором заканчивать поиск подпоследовательностей
*					j - индекс элемента во второй строке, на котором заканчивать поиск подпоследовательностей
*					square - матрица соответствия элементов строк str1 и str2
*					res - заполняемый вектор наибольших общих подпоследовательностей
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::getSubsequences (const std::vector <int> &str1, const std::vector <int> &str2, int i, int j, const std::vector < std::vector <int> > &square, std::vector < std::vector <int> > &res) const {
	if (square[i][j] == 0) {
		return 0;
	}

	if (i == 0) {
		res.resize(1);
		res[0].push_back (str1[0]);
		return 0;
	}

	if (j == 0) {
		res.resize(1);
		res[0].push_back (str2[0]);
		return 0;
	}

	// идем по диагонали
	if ((square[i][j] > square[i-1][j]) && (square[i][j] > square[i][j-1])) {
		if (square[i][j] == 1) {
			res.resize(1);
			res[0].push_back (str1[i]);
		} else {
			getSubsequences (str1, str2, i-1, j-1, square, res);
			for (unsigned int k = 0; k < res.size(); k++)
				res[k].push_back (str1[i]);
		}
		return 0;
	} else {
		// идем по диагонали
		if (square[i][j] == square[i-1][j-1]) {
			getSubsequences (str1, str2, i-1, j-1, square, res);
		}
		std::vector < std::vector <int> > resNew;
		// идем вверх
		int i2 = i;
		while (i2 > 0) {
			if (square[i2][j] > square[i2-1][j])
				break;
			i2--;
		}
		if (square[i2][j] > square[i2][j-1]) {
			if (i2 > 0) {
				if (square[i2][j] == 1) {
					resNew.resize(1);
					resNew[0].push_back (str1[i2]);
				} else {
					getSubsequences (str1, str2, i2-1, j-1, square, resNew);
					for (unsigned int k = 0; k < resNew.size(); k++)
						resNew[k].push_back (str1[i2]);
				}
				for (unsigned int k = 0; k < resNew.size(); k++)
					res.push_back (resNew[k]);
				resNew.clear();
			} else {
				res.resize(res.size() + 1);
				res[res.size() - 1].push_back (str1[0]);
			}
		}
		// идем влево
		while (j > 0) {
			if (square[i][j] > square[i][j-1])
				break;
			j--;
		}
		if (square[i][j] > square[i-1][j]) {
			if (j > 0) {
				if (square[i][j] == 1) {
					resNew.resize(1);
					resNew[0].push_back (str1[i]);
				} else {
					getSubsequences (str1, str2, i-1, j-1, square, resNew);
					for (unsigned int k = 0; k < resNew.size(); k++)
						resNew[k].push_back (str1[i]);
				}
				for (unsigned int k = 0; k < resNew.size(); k++)
					res.push_back (resNew[k]);
				resNew.clear();
			} else {
				res.resize(res.size() + 1);
				res[res.size() - 1].push_back (str2[0]);
			}
		}
		// выходим из функции
		return 0;
	}
	// По идеи - сюда уже дойти не можем
	return -1;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::getSquare
*
*	Description:	Функция вычисляет матрицу соответствия элементов двух строк.
*					Эта матрица может использоваться для нахождения наибольшей 
*					общей подпоследовательности двух строк.
*	Parameters:		str1 - первоя строка, для которой строится матрица
*					str2 - вторая строка, для которой строится матрица
*					square - матрица соответствия для строк str1 и str2
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::getSquare (const std::vector <int> &str1, const std::vector <int> &str2, std::vector < std::vector <int> > &square) const {
	int s1 = (int) str1.size();
	int s2 = (int) str2.size();
	// Проврека входных данных
	if ((s1 < 1) || (s2 < 1)) {
		square.clear ();
		return 0;
	}
	
	int preVal = 0;
	// заполянем первый элемент матрицы
	square.resize (s1);
	square[0].resize(s2, 0);
	if (str1[0] == str2[0]) {
		preVal = 1;
		square[0][0] = 1;
	}
	// заполняем левый столбец
	int i, j;
	for (i = 1; i < s1; i++) {
		square[i].resize(s2, 0);
		if (preVal > 0)
			square[i][0] = 1;
		else 
			if (str1[i] == str2[0]) {
				preVal = 1;
				square[i][0] = 1;
			}
	}
	// заполняем верхнюю строку матрицы
	preVal = square[0][0];
	for (j = 1; j < s2; j++) {
		if (preVal > 0)
			square[0][j] = 1;
		else 
			if (str1[0] == str2[j]) {
				preVal = 1;
				square[0][j] = 1;
			}
	}

	// Заполняем всю матрицу
	for (i = 1; i < s1; i++) {
		for (j = 1; j < s2; j++) {
			if ((square[i-1][j-1] >= square[i][j-1]) && (square[i-1][j-1] >= square[i-1][j])) {
				square[i][j] = square[i-1][j-1];
				if (str1[i] == str2[j])
					square[i][j]++;
			} else 
				if (square[i-1][j] >= square[i][j-1])
					square[i][j] = square[i-1][j];
				else
					square[i][j] = square[i][j-1];		
		}
	}

	return 0;
}
