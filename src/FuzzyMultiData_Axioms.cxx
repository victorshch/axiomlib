/****************************************************************************
*
*				Функции класса FuzzyMultiDataLearnAlgorithm
*
****************************************************************************/

#include <cmath>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include "FuzzyMultiDataLearnAlgorithm.h"
#include "Defines.h"

#define FUZZYMULTIDATA_AXIOMGOAL_EXPERIMENTAL

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

using namespace AxiomLib;

#define str_AbnormalType_AxiomNaming	"_at_" // Обозначение типа нештатного поведения при создании имен файлов с описаниями систем аксиом
#define int_Axiom_Name_Max_Size			200 // максимальное число символов в имени файла с описанием аксиомы (лучше чтобы было с запасом!)
#define eps						0.0000001 // используется в некоторых формулах, чтобы исплючить ошибку машинного округления
#define max_goal_val			10000 // максимальное значение целевой функции для аксиомы (просто достаточно большое число)

/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::formAxioms
*
*	Description:	Функция формирования аксиом из элементарных условий для 
*					всех типов нештатного поведения
*	Parameters:		-
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::formAxioms (void) {
	//	Определение ранга текущего процесса
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// MPI-распараллеливание вычислений на первых двух шагах алгоритма
	// Для этого: Определение для какого типа нештатного поведения данному процессу процессу производить обучение аксиом
	int from, upTo, rankFrom, rankUpTo;
	splitAbnormalTypeRange (rank, size, from, upTo, rankFrom, rankUpTo);
	// Вывод информации о назначенных для данного шаблона типов нештатного поведения - для них алгоритм будет подбирать элементырне условия и строить аксиомы
	std::cout << "\n\tProcess: " << rank + 1 << " from " <<  size << " set for abnormal behaviour: " << from + 1 << " - " << upTo << " \n";
	std::cout.flush();

	if (bestECForAllAbnormalTypes.size() != (upTo - from))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function*.");

	if (rank == rankFrom) { // запускаем формирование аксиом без дублирования в разных процессах
		std::vector < std::vector <AxiomExprPlus> > axioms;
		axioms.resize (upTo - from);
		bestAxiomsFileNames.resize(upTo - from);
		// подбор параметров условий для типов нештатного поведения с from до upTo
		#pragma omp parallel for schedule(dynamic, 1)
		for (int abType = 0; abType < (upTo - from); abType++) {
			// выбор параметров условий
			formAxioms (from + abType, bestECForAllAbnormalTypes[abType], axioms[abType], bestAxiomsFileNames[abType]);
			// Очищение содержимого аксиом 
			for (unsigned int u = 0; u < axioms[abType].size(); u++)
				axioms[abType][u].clear();
			axioms[abType].clear();
			// Вывод на экран стадии процесса
			std::cout << "\n\tProcess: " << rank + 1 << " from " <<  size <<  ":\tAbnormal type\t" << abType+1 << " done.\n";
			std::cout.flush();
		}
	} else { 
		// Для данного процесса - нет необходимости формировать аксиомы, т.к. другой процесс уже это делает. Это так, потому что:
		//  функция splitAbnormalTypeRange устроена таким образом, что число классов разбиения равно min (NumOfProcesses, NumOfClasses)
		bestAxiomsFileNames.clear();
	}

	// Объединение списков с именами файлов, содержащих описания аксиом, полученных в рамках разных процессов
	gatherBestAxioms ();

	// Синхронизация процессов
	MPI_Barrier (MPI_COMM_WORLD);

	// Добавление в набор аксиом, перечисленных в конфигурационном файле
	updateBestAxiomsListFromEnv();

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::gatherBestAxioms
*
*	Description:	Функция сбора списков с названиями файлов, в которые были 
*					записаны описания лучших аксиом, с различных 
*					процессов, учавствующих во втором этапе работы основного алгоритма.
*	Parameters:		rank - ранг текущего процесса
*					size - общее число процессов
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::gatherBestAxioms (void) {
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size > 1) { // значит параметры условий для разных типов нештатного поведения подбирались независимо - необходимо объединить данные
		// Для этого - Создание собственного типа MPI для передачи названий файлов не по одному, а целыми массивами
		struct FileDescr {
			char name[int_Axiom_Name_Max_Size]; /* file name */
		};
		struct FileDescr fDescr[2];
		/* build datatype describing structure */
		MPI_Datatype FileDescrType;
		MPI_Aint base;
		MPI_Datatype type[2] = {MPI_CHAR, MPI_UB};
		int blocklen[2] = {int_Axiom_Name_Max_Size, 1};
		MPI_Aint disp[2];
		/* compute displacements of structure components */
		MPI_Address(fDescr, disp);
		MPI_Address(fDescr+1, disp+1);
		base = disp[0];
		for (int i = 0; i < 2; i++) disp[i] -= base;
		/* build datatype describing structure */
		MPI_Type_struct( 2, blocklen, disp, type, &FileDescrType);
		// Регистрация типа в MPI
		MPI_Type_commit(&FileDescrType);

		// Определение необходимых для передачи переменных
		int vec[1];
		int *vecs;
		vecs = new int[size];
		int secSize, secMax, wholeLen;
		// Определение максимального размера вектора передавамого параметра
		//  Локальное определение максимального размера передавамого параметра
		secMax = 0;
		for (unsigned int i = 0; i < bestAxiomsFileNames.size(); i++) {
			if (secMax < (int) bestAxiomsFileNames[i].size())
				secMax = (int) bestAxiomsFileNames[i].size();
		}
		//  Глобальное определение максимального размера передавамого параметра
		vec[0] = secMax;
		MPI_Allgather(vec, 1, MPI_INT, vecs, 1, MPI_INT, MPI_COMM_WORLD);
		for (int u = 0; u < size; u++) {
			if (secMax < vecs[u])
				secMax = vecs[u];
		}		
		// Создаем буффер, через который будут передаваться вектора переменных
		struct FileDescr *fDescrips;
		fDescrips = new struct FileDescr[secMax];
		// Определение первой размерности всех передаваемых переменных и соответственно общей длины получаемого результата
		vec[0] = (int) bestAxiomsFileNames.size();
		MPI_Allgather(vec, 1, MPI_INT, vecs, 1, MPI_INT, MPI_COMM_WORLD);
		wholeLen = 0;
		for (int u = 0; u < size; u++) {
			wholeLen += vecs[u];
		}
		// Первый индекс результата
		int allFirst = 0;
		std::vector <std::vector <std::string> > allNames;
		allNames.resize (wholeLen);
		// Цикл по первой размерности передаваемого вектора
		for (int proc = 0; proc < size; proc++) {
			for (int first = 0; first < vecs[proc]; first++) {
				// Получаем число пересылаемых во второй размерности векторов
				if (proc == rank) {
					for (int sec = 0; sec < (int) bestAxiomsFileNames[first].size(); sec++) {
						strncpy(fDescrips[sec].name, bestAxiomsFileNames[first][sec].c_str(), int_Axiom_Name_Max_Size - 1);
					}
					vec[0] = (int) bestAxiomsFileNames[first].size();
				}
				// Посылаем число передаваемых данных
				MPI_Bcast(vec, 1, MPI_INT, proc, MPI_COMM_WORLD);
				secSize = vec[0];
				// Посылаем собственно данные
				MPI_Bcast(fDescrips, secSize, FileDescrType, proc, MPI_COMM_WORLD);
				// Обрабатываем полученные данные
				allNames[allFirst].resize(secSize);
				for (int sec = 0; sec < secSize; sec++) {
					allNames[allFirst][sec].assign(fDescrips[sec].name);
				}
				allFirst++;
			}
		}
		// Очищиение задействованной для передачи динамической памяти
		delete vecs;
		// Удаляем созданнй буффер
		delete fDescrips;
		// Копируем собранные со всех процессов данные в локальную переменную класса
		bestAxiomsFileNames = allNames;

		/* // OLD VARIANT
		// Передача или прием списка с названиями файлов соответственно
		unsigned int buffer[1];
		if (rank == 0) { // этот процесс собирает данные
			struct FileDescr *fDescrips;
			unsigned int st, vecSize;
			MPI_Status status;
			for (int procNum = 1; procNum < size; procNum++) {
				// получаем число векторов, содержащих вектора со строками - первую размерность всей структуры
				MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 300 + procNum, MPI_COMM_WORLD, &status);
				vecSize = buffer[0];
				st = bestAxiomsFileNames.size();
				bestAxiomsFileNames.resize (st + vecSize);
				for (unsigned int i = 0; i < vecSize; i++, st++) {
					MPI_Recv( buffer, 1, MPI_UNSIGNED, procNum, 301 + procNum, MPI_COMM_WORLD, &status);
					fDescrips = new struct FileDescr[buffer[0]];
					// получаем вектор с именами условий
					MPI_Recv( fDescrips, buffer[0], FileDescrType, procNum, 302 + procNum, MPI_COMM_WORLD, &status);
					// Сохраняем полученные значения - вектор имен файлов с описанием элементарных условий
					bestAxiomsFileNames[st].resize(buffer[0]);
					for (unsigned int t = 0; t < buffer[0]; t++) {
						bestAxiomsFileNames[st][t].assign (fDescrips[t].name);
					}
					delete fDescrips;
				}
			}
		} else { // этот процесс посылает данные
			struct FileDescr *fDescrips;
			unsigned int curSize;
			// посылаем число размерностей, по которым в этом процессе велся поиск параметров элементарных условий
			buffer[0] = bestAxiomsFileNames.size();
			MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 300 + rank, MPI_COMM_WORLD);
			for (unsigned int i = 0; i < bestAxiomsFileNames.size(); i++) {
				curSize = bestAxiomsFileNames[i].size();
				// Если число строк с именами файлов в выбранной размерности - 0, то пропускаем ее
				if (curSize < 1)
					continue;
				// Копируем строки с именами файлов в созданную структуру для передачи средствами MPI
				fDescrips = new struct FileDescr[curSize];
				for (unsigned int t = 0; t < curSize; t++)
					strncpy(fDescrips[t].name, bestAxiomsFileNames[i][t].c_str(), int_Axiom_Name_Max_Size - 1);
				// посылаем число строк в векторе
				buffer[0] = curSize;
				MPI_Send( buffer, 1, MPI_UNSIGNED, 0, 301 + rank, MPI_COMM_WORLD);
				// посылаем собственно вектор с именами условий
				MPI_Send( fDescrips, curSize, FileDescrType, 0, 302 + rank, MPI_COMM_WORLD);
				delete fDescrips;
			}
		} // END OF OLD VARIANT
		*/ 
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::updateBestAxiomsListFromEnv
*
*	Description:	Функция обновляет список имен файлов с описаниями лучших 
*					аксиом - именами из конфигурационного файла.
*	Parameters:		-
*	Returns:		0
*	Throws:			AxiomLibException - если 
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::updateBestAxiomsListFromEnv(void) {
	int at, index;
	std::set <std::string> :: const_iterator c_iter;
	for (c_iter = predefinedAxiomsFileName.begin(); c_iter != predefinedAxiomsFileName.end(); ++c_iter) {
		disintegrateAxiomFileName(*c_iter, at, index);
		if ((at < (int) bestAxiomsFileNames.size()) && (at >= 0)) {
			// добавляем имя в массив
			bestAxiomsFileNames[at].push_back (*c_iter);
		} else {
			// Вообще говоря - здесь нужно выдавать warning - т.к. файл не относится ни к одному типу нештатного поведения, указанному в используемом наборе данных
			std::cout << "\n\tAxiom File '" << *c_iter << "' is incompatible with current dataSet properties: wrong number of abnormalType or no such number in file's name.\n";
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::disintegrateAxiomFileName
*
*	Description:	функция раскладывает имя файла на составляющие и выдает, 
*					если находит, два числовых параметра.
*	Parameters:		fileName - имя файла для анализа
*					at - номер аномального типа поведения 
*					index - размерность структуры, из который файл были сохранен.
*						Этот параметр может быть не задан, в таком случае
*						его значение определяется как -1. 
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::disintegrateAxiomFileName(const std::string fileName, int &at, int &index) const {
	// Устанавливаем значения по умолчанию: 
	at = -1;
	index = -1;
	// Начинаем поиск
	std::string :: size_type posAT, first;
	// Пытаемся считать AbnormalType
	posAT = fileName.rfind (str_AbnormalType_AxiomNaming);
	if ( posAT != std::string::npos ) {
		at = getNearestNumber (fileName, posAT) - 1; // -1 - т.к. при кодировании имени файла ко всем числам прибавляется 1
	}
	// Пытаемся считать оставшийся индекс
	// пропускаем число относящиеся к AT
	first = fileName.find_first_of ("0123456789", posAT);
	first = fileName.find_first_not_of ("0123456789", first);
	// ищем первое следующее попавшееся число
	first = fileName.find_first_of ("0123456789", first);
	// если такое число нашли - то сохраняем его
	if (first != std::string::npos) {
		index = getNearestNumber (fileName, first) - 1; // -1 - т.к. при кодировании имени файла ко всем числам прибавляется 1
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::formAxioms
*
*	Description:	Функция формирования аксиом из элементарных условий для 
*					заданного типа нештатного поведения
*	Parameters:		abType - тип нештатного поведения, для которого формируются аксиомы
*					ecFileNames - названия файлов с описаниями элементарных условий, 
*						которые будут использованы для построения аксиом
*					axioms - заполняемый вектор лучших аксиом, полученных в данной функции
*					axiomFileNames - заполняемый вектор с именами файлов с описаниями лучших аксиом
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::formAxioms (const int abType, std::vector <std::vector <std::vector <std::string> > > &ecFileNames, std::vector <AxiomExprPlus> &axioms, std::vector <std::string> &axiomFileNames) const {
	// Подсчет числа различных элементарных условий
	unsigned int totSize = 0;
	for (unsigned int i = 0; i < ecFileNames.size(); i++)
		for (unsigned int j = 0; j < ecFileNames[i].size(); j++)
			totSize += ecFileNames[i][j].size();
	if (totSize < 1)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::formAxioms : set of prepared elementary conditions is indeed empty.");

	// Создание начальной популяции аксиом, из которых буду строиться аксиомы дальше в итеративном алгоритме
	axioms.resize(totSize);
	unsigned int t = 0;
	std::vector <std::string> dataSetParamNames;
	dataSetParamNames = fuzzyDataSet.getParamNames();
	for (unsigned int i = 0; i < ecFileNames.size(); i++) {
		for (unsigned int j = 0; j < ecFileNames[i].size(); j++) {
			for (unsigned int k = 0; k < ecFileNames[i][j].size(); k++, t++) {
				axioms[t].expression.resize(1);
				axioms[t].expression[0].resize(1);
				// Создание элементарного условия, по его описанию из файла
				axioms[t].expression[0][0].initFromFile (axiomBaseDir, ecFileNames[i][j][k], dataSetParamNames);
			}
		}
	}
	// Заполняем начальную статистику по популяции
	if (this->comments)	std::cout << "\n\tCounting initial statistics.\n";
	#pragma omp parallel for schedule(dynamic, 1)
	for (int i = 0; i < (int) totSize; i++) {
		if (this->comments) {
			char buf[128];
			sprintf (buf, "\r\t\taxiomNum=%d\tby thread %d.\t", i, omp_get_thread_num());
			std::cout<<buf<<std::endl;
			//std::cout.flush();
		}
		matterAxiomFunc (axioms[i], abType);
	}
	// Подготовка переменных для поиска аксиом
	AxiomExprPlus axiomExprPlus;
	int iterNum = 0; // Счетчик итераций алгоритма построения аксиом
	bool criteriaToContinue = true; // Флаг продолжения процедуры построения аксиом
	int curSize, curBest;
	std::vector <AxiomExprPlus> bestAxiomsNew;
	// Проверка корректности указания максимального размера популяции
	if (maxAxiomPopSize < 2) 
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::formAxioms : incorrect value of parameter 'maxAxiomPopValue'.");
	// Задаем размер множества аксиом, которые могут быть получены на каждом шаге алгоритма - размер задаем достаточным для худшего случая
	bestAxiomsNew.resize(maxAxiomPopSize + round ((double)((maxAxiomPopSize - 1) * maxAxiomPopSize) / 2.0));
	// Итеративное изменение популяции 
	while (criteriaToContinue) {
		iterNum++;
		// Отображаем на экран стадию процесса
		if (this->comments) {
			char buf[128];
			sprintf (buf, "\r\tAbType: %d\tIteration: %d\tNumber of axioms: %d\tby thread %d.\t", abType, iterNum, axioms.size(), omp_get_thread_num());
			std::cout<<buf<<std::endl;
			//std::cout.flush();
		}
		// Урезаем, если необходимо, вектор аксиом до максимально допустимых размеров
		cutDownBestAxioms (axioms);
		// Проверяем - достаточное ли число элементов вектора
		curSize = axioms.size();
		if (curSize < 2) {
			criteriaToContinue = false;
			if(this->comments) {
				std::cout<<"Stopping because axioms vector size < 2"<<std::endl;
			}
			break;
		}
		// Переносим существующие аксиомы в результат на данном шаге		
		for (int i = 0; i < curSize; i++) {
			bestAxiomsNew[i] = axioms[i];
		}
		curBest = curSize;
		// Создаем новые элементы и вычисляем их значение целевой функции
		for (unsigned int i = 0; i < (unsigned int) (curSize - 1); i++) {
			for (unsigned int j = i+1; j < (unsigned int) curSize; j++) {
				if (combineAxioms (axioms[i], axioms[j], axiomExprPlus, abType) > 0) {
					bestAxiomsNew[curBest] = axiomExprPlus;
					curBest++;
				}
			}
			axioms[i].clear();
		}
		axioms[curSize - 1].clear();
		axioms.clear();
		// Записываем полученные значения в вектор для следующей итерации
		axioms.resize(curBest);
		for (int i = 0; i < curBest; i++) {
			axioms[i] = bestAxiomsNew[i];
			bestAxiomsNew[i].clear();
		}

		// Проверяем критерии останова
		if (curBest == curSize) {
			criteriaToContinue = false;
			if(comments) {
				std::cout<<"Stopping because no new axioms created"<<std::endl;
			}
		}
		if (iterNum > maxIterNum) {
			criteriaToContinue = false;
			if(comments) {
				std::cout<<"Stopping because maximum number of iterations exceeded"<<std::endl;
			}
			
		}
		for (int i = 0; i < curBest; i++) {
			if (axioms[i].goal > maxAxiomRes) {
				criteriaToContinue = false;
				if(comments) {
					std::cout<<"Stopping because goal value "<<
							axioms[i].goal<<" exceeds maximum goal value "<<
							maxAxiomRes<<std::endl;
				}
				break;
			}			
		}
	}

	// Очищаем использованные промежуточные вектора
	bestAxiomsNew.clear();

	// Уменьшаем размер популяции аксиом
	cutDownBestAxioms (axioms);

	// Сохраняем лучшие аксиомы в файлах
	saveBestAxiomsInFiles (abType, axioms, axiomFileNames);
	
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::saveBestAxiomsInFiles
*
*	Description:	Функция сохраняет в файлы лучшие сформированные на втором этапе файлы
*	Parameters:		abnormalBehaviourType - тип нештатного поведения для которого подбирались аксиомы
*					axioms - вектор лучших аксиом
*	Returns:		0
*	Throws:			AxiomLibException - если функция получает несогласованные параметры от набора данных
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::saveBestAxiomsInFiles (const int abnormalBehaviourType, std::vector <AxiomExprPlus> &axioms, std::vector <std::string> &axiomFileNames) const {
	// Формируем название файла с описанием аксиомы
	std::string curAxiomName, tmpAxiomName;
	char tmss[10];
	curAxiomName = axiomNameTemplate;
	// дописываем в название тип нештатного поведения, для которого подбиралась аксиома
	curAxiomName.append (str_AbnormalType_AxiomNaming);
	tmss[0]='\0';
	sprintf(tmss, "%d", abnormalBehaviourType + 1);
	curAxiomName.append (tmss);
	curAxiomName.append ("_");
	// получаем вектор названий измерений набора данных
	std::vector <std::string> dataSetParams;
	dataSetParams = fuzzyDataSet.getParamNames();
	// Подготавливаем переменную для хранения имен записанных файлов
	axiomFileNames.resize(axioms.size());
	// Сохраняем все акисомы
	for (unsigned int axNum = 0; axNum < axioms.size(); axNum++) {
		// Дописываем в название индекс аксиомы во внутреннем представлении - в перемнной класса
		tmpAxiomName = curAxiomName;
		tmss[0]='\0';
		sprintf(tmss, "%d", axNum + 1);
		tmpAxiomName.append (tmss);
		// Сохраняем описанием аксиомы в файл
		axioms[axNum].saveAxiomToFile(axiomBaseDir, tmpAxiomName, dataSetParams);
		// Сохраняем имена файлов с описанием аксиомы
		axiomFileNames[axNum].assign (tmpAxiomName);
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::matterAxiomFunc
*
*	Description:	Подсчет целевой функции для аксиомы
*	Parameters:		ax - аксиома, для которой вычисляется значение целевой функции
*					param - параметр набора данных, на котором проверяется аксиома
*	Returns:		double - значение целевой функции
*	Throws:			AxiomLibException - если функция получает несогласованные параметры от набора данных
*	Author:			dk
*	History:
*
****************************************************************************/
double FuzzyMultiDataLearnAlgorithm::matterAxiomFunc (AxiomExprPlus &ax, const int abType) const {
	// Объявление необходимых переменных
	int numOfClasses, numOfNormalMultiTS;
	std::vector <int> numOfMultiTS, numOfNormalTS;
	std::vector < std::vector <int> > numOfTS;
	std::vector < std::vector <double> > curTS;
	int classCount, classLen, classLenCur;
	// Получение информации о наборе данных
	fuzzyDataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTS);
	/* // Эти проверки слишком частые, кроме того это проверяется и в других функциях, т.о. они здесь не обязательны
	if ((numOfClasses != numOfMultiTS.size()) || (numOfClasses != numOfTS.size()))
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function.");
	if (numOfMultiTS[abType] != numOfTS[abType].size())
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function*.");
	*/
	// перед началом пробегаем по содержимому аксиомы и составляем спосок необходимых размерностей траектории
	std::vector <bool> dims;
	dims.resize (fuzzyDataSet.paramNamesSize(), false);
	for (unsigned int a = 0; a < ax.expression.size(); a++) {
		for (unsigned int b = 0; b < ax.expression[a].size(); b++) {
			if ((ax.expression[a][b].dimension < 0) || (ax.expression[a][b].dimension >= (int) dims.size()))
				throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : incomplete definition of elementary condition, it's data set dimension is out of range.");
			dims[ax.expression[a][b].dimension] = true;
		}
	}
	// цикл по всем траекториям из обучающей выборки для данного класса нештатного поведения
	classCount = 0;
	classLen = 0;
	curTS.resize (fuzzyDataSet.paramNamesSize());
		
	boost::accumulators::accumulator_set<double, 
		boost::accumulators::features<boost::accumulators::tag::variance> > varianceAccumulator;
	
	// Число траекторий, соответствующих нештатному поведению, на которых аксиома сработалаы
	int numOccured = 0;
	
	// Общее число траекторий, соответствующих нештатному поведению
	int numTraj = (int) numOfTS[abType].size();
	
	for (int j = 0; j < numTraj; j++) {
		/* // Эта проверка происходит не один раз - здесь возможно исключить
		if (numOfTS[abType][j] != fuzzyDataSet.paramNamesSize())
			throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : dataSet is not full - some time series contains not enought dimensions.");
		*/
		// вытаскиваем траекторию из обучающей выборки для данного класса нештатного поведения (вытаскиваем только нужные размерности)
		classLenCur = 0;
		for (int t = 0; t < numOfTS[abType][j]; t++) {
			if (dims[t]) {
				if (!fuzzyDataSet.getTSByIndexFromClass (curTS[t], abType, j, t))
					throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : unable to get concrete time series from dataSet.");
				classLenCur = curTS[t].size();
			} else {
				curTS[t].clear();
			}
		}
		// проверяем как срабатывает аксиома на выбранных размерностях выбранной траектории и считаем статистику для подсчета значения целевой функции
		int currNumOfCarriedOutItems = numOfCarriedOutItems (ax, curTS, classLenCur);
		
		if(currNumOfCarriedOutItems > 0) {
			++numOccured;
		}
		
		// Вычисляем частоту срабатывания на текущей траектории
		double currFreq = (double) currNumOfCarriedOutItems / (double) classLenCur;
		
		// Аккумулируем значение частоты для дальнейшего подсчета вариации
		varianceAccumulator(currFreq);
		
		classCount += currNumOfCarriedOutItems;
		classLen += classLenCur;
	}
	// Заполняем статистику по нештатному поведению
	
	if (classLen > 0) {
		ax.statAbnorm = (double) classCount/ (double) classLen;
		ax.statVariation = std::sqrt(
				boost::accumulators::variance(varianceAccumulator)
				);
		ax.statOccurence = (double) numOccured / (double) numTraj;
	} else {
		ax.statAbnorm = -1.0; // - значит данные не определены
		ax.statVariation = -1.0;
		ax.statOccurence = -1.0;
		std::cout << "\nWarning in  FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : incorrect dstaSet request for abnormal type.\n";
	}	

	// Цикл по траекториям нормального поведения
	fuzzyDataSet.getNormalClassSize (numOfNormalMultiTS, numOfNormalTS);
	/* // Это уже много где проверяется - в целях оптимизации здесь исключаем проверку
	if (numOfNormalMultiTS != numOfNormalTS.size())
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : incorrect response from internal function getNormalClassSize");
	*/
	classCount = 0;
	classLen = 0;
	for (int j = 0; j < (int) numOfNormalTS.size(); j++) {
		// вытаскиваем траекторию из обучающей выборки для данного класса нештатного поведения (вытаскиваем только нужные размерности)
		classLenCur = 0;
		for (int t = 0; t < numOfNormalTS[j]; t++) {
			if (dims[t]) {
				if (!fuzzyDataSet.getNormalTSFromClass (curTS[t], j, t))
					throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::matterAxiomFunc : unable to get concrete normal time series from dataSet.");
				classLenCur = curTS[t].size();
			} else {
				curTS[t].clear();
			}
		}
		// Получаем статистику по разметки выбранной траектории
		classCount += numOfCarriedOutItems (ax, curTS, classLenCur);
		classLen += classLenCur;
	}
	// Заполняем статистику по нештатному поведению
	if (classLen > 0)
		ax.statNormal = (double) classCount/ (double) classLen;
	else {
		ax.statNormal = -1.0; // - значит данные не определены
		std::cout << "\nWarning in FuzzyMultiDataLearnAlgorithm::matterAxiomFunc: incorrect dstaSet request for normal data.\n";
	}

#ifndef FUZZYMULTIDATA_AXIOMGOAL_EXPERIMENTAL
	// Определение значения целевой функции
	if (ax.statAbnorm < eps)
		ax.goal = 0.0;
	else
		if (ax.statNormal < eps)
			ax.goal = max_goal_val;
		else
			ax.goal = min (ax.statAbnorm / ax.statNormal, max_goal_val);
#else
	const double delta = 0.000005;
	if (ax.statAbnorm < eps)
		ax.goal = 0.0;
	else
		ax.goal = min (ax.statAbnorm / (ax.statNormal + delta), max_goal_val);
#endif
	return ax.goal;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::numOfCarriedOutItems
*
*	Description:	Подсчет числа точек в ряду, на которых аксиома выполняется
*	Parameters:		ax - проверяемоя аксиома
*					row - ряд для порверки
*	Returns:		int - число точек, на которых аксиома выполнена
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::numOfCarriedOutItems (AxiomExpr &ax, std::vector < std::vector <double> > &row, int &rowLen) const {
	int count = 0;
	for (unsigned long i = 0; i < (unsigned long) rowLen; i++) {
		if (ax.check(i, row) > 0)
			count++;
	}
	return count;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::getPriority
*
*	Description:	Вспомогательная функция сортировки массива по убыванию 
*					с занесением результатов в отдельный вектор индексов.
*					Значения исходного сортируемого вектора не изменяются и не 
*					перемещаются.
*	Parameters:		vec - сортируемый вектор аксиом по значению целевой функции
*					pos - вектор индексов для указания результата
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int FuzzyMultiDataLearnAlgorithm::getPriority (std::vector <AxiomExprPlus> &vec, std::vector <unsigned int> &pos) const {
	pos.resize(vec.size()); 
	for (unsigned int i = 0; i < vec.size(); i++) 
		pos[i] = i;
	unsigned int temp;
	for (unsigned int i = vec.size() - 1; i > 0; i--) {
		for (unsigned int j = 0; j < i; j++) {
			if (vec[pos[j]].goal < vec[pos[j+1]].goal) {
				temp = pos[j+1];
				pos[j+1] = pos[j];
				pos[j] = temp;
			}
		}
	}
	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::cutDownBestAxioms
*
*	Description:	Функция сокращает популяцию существующих аксиом 
*					до определенного в параметрах (из конфиг-файла) значения
*	Parameters:		axioms - вектор лучших аксиом
*	Returns:		0 - если урезания популяции не было
*					1 - в противном случае
*	Throws:			AxiomLibException - если внутренние переменные класса не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::cutDownBestAxioms (std::vector <AxiomExprPlus> &axioms) const {
	// Проверка входных параметров
	if ((maxAxiomPopSize < 1) || (axioms.size() <= (unsigned int) maxAxiomPopSize))
		return 0;
	
	// Сортировка аксиом по значению целевой функции
	std::vector <unsigned int> pos;
	getPriority (axioms, pos);
	
	// Определение числа лучших аксиом, сохраняемых в популяции
	unsigned int numBestToSave, numToChoose, numFrom;
	numBestToSave = (unsigned int) round (percentBestAxioms*maxAxiomPopSize);
	numToChoose = (unsigned int) maxAxiomPopSize - numBestToSave;
	
	numFrom = axioms.size() - numBestToSave;
	std::vector <bool> chosen;
	chosen.resize (numFrom, false);
	int curChoice;
	std::vector <unsigned int> toSave;
	toSave.resize (maxAxiomPopSize);
	// Заносим в вектор сохраняемых элементов - заданное число лучших
	for (unsigned int i = 0; i < numBestToSave; i++) {
		toSave[i] = pos[i];
	}
	// Случайный выбор оставшихся
	for (unsigned int i = 0; i < numToChoose; i++) {
		curChoice = round (((double) rand() / (double) RAND_MAX) * (double) (numFrom - i - 1));
		for (unsigned int t = (unsigned int) curChoice; t < numFrom; t++)
			if (!chosen[t]) {
				chosen[t] = true;
				toSave[numBestToSave + i] = pos[numBestToSave + t];
				break;
			}
	}

	// Сортировка массива выбранных для сохранения элементов по убыванию
	unsigned int tmpInt;
	for (unsigned int i = maxAxiomPopSize - 1; i > 0; i--) {
		for (unsigned int j = 0; j < i; j++) {
			if (toSave[j] < toSave[j+1]) {
				tmpInt = toSave[j+1];
				toSave[j+1] = toSave[j];
				toSave[j] = tmpInt;
			}
		}
	}
	
	// Удаление невыбранных аксиом
	// Удаляем все элементы после последнего по индексу в списке
	if ((toSave[0] + 1) < axioms.size()) {
		for (unsigned int j = toSave[0] + 1; j < axioms.size(); j++)
			axioms[j].clear();
		axioms.erase (axioms.begin() + toSave[0] + 1, axioms.end());
	}
	for (int i = 1; i < maxAxiomPopSize; i++) {
		if ((toSave[i] + 1) < toSave[i-1]) {
			// Очищаем содержимое элементов вектора аксиом
			for (unsigned int j = toSave[i] + 1; j < toSave[i-1]; j++)
				axioms[j].clear();
			// Удаляем элементы векторов
			axioms.erase (axioms.begin() + toSave[i] + 1, axioms.begin() + toSave[i-1]);
		}
	}
	// Удаляем все элементы до первого по индексу в составленном списке
	if (toSave[maxAxiomPopSize-1] > 0) {
		for (unsigned int j = 0; j < toSave[maxAxiomPopSize-1]; j++) {
			axioms[j].clear();
		}
		axioms.erase (axioms.begin(), axioms.begin() + toSave[maxAxiomPopSize-1]);
	}

	return 0;
}


/****************************************************************************
*					FuzzyMultiDataLearnAlgorithm::combineAxioms
*
*	Description:	Объединение двух аксиом и вычисление результата для полученных новых аксиом.
*	Parameters:		axiomFirst - первая из аксиом, на основе которой будут построены новые варианты аксиом
*					axiomSec - вторая из аксиом, на основе которой будут построены новые варианты аксиом
*					axiomExpr - заполняемая переменная аксиомы, в случае если удалось построить лучшую аксиому
*					abType - тип нештатного поведения для которого производится построение аксиомы
*	Returns:		0 - если новые аксиомы, созданные из двух указанных оказались не лучше
*					>0 - если созданная аксиома оказалась лучше предшественников
*					-1 - если не удалось провести комбинирование указанных аксиом
*	Throws:			AxiomLibException - если указанные входные параметры не согласованы с внутренними пременными класса
*	Author:			dk
*	History:
*
****************************************************************************/
int FuzzyMultiDataLearnAlgorithm::combineAxioms (AxiomExprPlus &axiomFirst, AxiomExprPlus &axiomSec, AxiomExprPlus &axiomExprPlus, const int abType) const {
	// Создание новых аксиом
	AxiomExprPlus axiomExprAnd, axiomExprOr;
	axiomExprAnd.andExpr(axiomFirst, axiomSec);
	axiomExprOr.orExpr (axiomFirst, axiomSec);
	
	// Вычисление целевых функций
	matterAxiomFunc (axiomExprAnd, abType);
	matterAxiomFunc (axiomExprOr, abType);

	// Вывод отладочной информации
	//std::cout << "\n\t andRes = " << axiomExprAnd.goal << "\torRes = " << axiomExprOr.goal << "\taxF = " << fi << "\taxS = " << se << "\tand.size() = " << axiomExprAnd.expression.size() << "\tand[0].size() = " << axiomExprAnd.expression[0].size() << "\tor.size() = " << axiomExprOr.expression.size() << "\tor[0].size() = " << axiomExprOr.expression[0].size();
	
	// Определение лучше ли получились значения целевых функций у новых аксиом
	if ((axiomExprAnd.goal >= axiomExprOr.goal) || (((axiomExprOr.goal - axiomExprAnd.goal) <= eps) && (axiomExprAnd.statAbnorm >= axiomExprOr.statAbnorm))) {
		// значит axiomExprAnd - лучше axiomExprOr
		if ((axiomFirst.goal >= axiomSec.goal) || (((axiomSec.goal - axiomFirst.goal) <= eps) && (axiomFirst.statAbnorm >= axiomSec.statAbnorm))) {
			// значит axiomFirst - лучше axiomSec
			if ((axiomFirst.goal >= axiomExprAnd.goal) || (((axiomExprAnd.goal - axiomFirst.goal) <= eps) && (axiomFirst.statAbnorm >= axiomExprAnd.statAbnorm))) {
				// значит лучшая из axiomExprAnd, axiomExprOr - хуже, чем одна из axiomFirst, axiomSec
				return 0;
			} else {
				// значит лучшая из axiomExprAnd, axiomExprOr - лучше, чем каждая из axiomFirst, axiomSec
				axiomExprPlus = axiomExprAnd;
				return 1;
			}
		} else {
			// значит axiomSec - лучше axiomFirst
			if ((axiomSec.goal >= axiomExprAnd.goal) || (((axiomExprAnd.goal - axiomSec.goal) <= eps) && (axiomSec.statAbnorm >= axiomExprAnd.statAbnorm))) {
				// значит лучшая из axiomExprAnd, axiomExprOr - хуже, чем одна из axiomFirst, axiomSec
				return 0;
			} else {
				// значит лучшая из axiomExprAnd, axiomExprOr - лучше, чем каждая из axiomFirst, axiomSec
				axiomExprPlus = axiomExprAnd;
				return 1;
			}			
		}
	} else {
		// значит axiomExprOr - лучше axiomExprAnd
		if ((axiomFirst.goal >= axiomSec.goal) || (((axiomSec.goal - axiomFirst.goal) <= eps) && (axiomFirst.statAbnorm >= axiomSec.statAbnorm))) {
			// значит axiomFirst - лучше axiomSec
			if ((axiomFirst.goal >= axiomExprOr.goal) || (((axiomExprOr.goal - axiomFirst.goal) <= eps) && (axiomFirst.statAbnorm >= axiomExprOr.statAbnorm))) {
				// значит лучшая из axiomExprAnd, axiomExprOr - хуже, чем одна из axiomFirst, axiomSec
				return 0;
			} else {
				// значит лучшая из axiomExprAnd, axiomExprOr - лучше, чем каждая из axiomFirst, axiomSec
				axiomExprPlus = axiomExprOr;
				return 2;
			}
		} else {
			// значит axiomSec - лучше axiomFirst
			if ((axiomSec.goal >= axiomExprOr.goal) || (((axiomExprOr.goal - axiomSec.goal) <= eps) && (axiomSec.statAbnorm >= axiomExprOr.statAbnorm))) {
				// значит лучшая из axiomExprAnd, axiomExprOr - хуже, чем одна из axiomFirst, axiomSec
				return 0;
			} else {
				// значит лучшая из axiomExprAnd, axiomExprOr - лучше, чем каждая из axiomFirst, axiomSec
				axiomExprPlus = axiomExprOr;
				return 2;
			}			
		}

	}
	
	/*if ((axiomExprAnd.goal > max (axiomFirst.goal, axiomSec.goal)) && (axiomExprAnd.goal > axiomExprOr.goal)) {
		axiomExprPlus = axiomExprAnd;
		return 1;
	}
	if (axiomExprOr.goal > max (axiomFirst.goal, axiomSec.goal)) {
		axiomExprPlus = axiomExprOr;
		return 2;
	}*/

	// Сюда прийти уже не должны 
	return -1;
}
