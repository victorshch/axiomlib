/****************************************************************************
*
*			Функции класса AxiomSetPop
*
****************************************************************************/

#include "AxiomSetPop.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

/****************************************************************************
*			AxiomSetPop::initFromEnv
*
*	Description:	Инициализирует начальную популяцию систем аксиом,
*				используя данные из конфигурационного файла.
*				А именно:
*				    - определяет количество аксиом в популяции
*				    - определяет, как создавать системы аксиомы для
*				      начальной популяции
*				    - при необходимости, читает часть систем аксиом из
*				      файлов
*				    - при необходимости, инициализирует часть систем
*				      аксиом случайным образом
*	Parameters:	const Environment& env - окружение, параметры:
*				    * popSize - размер популяции
*				    * popPreset - процент аксиом, которые надо создать
*				      из заданной экспертом популяции
*	Returns:		0
*	Throws:		AxiomLibException
*	Author:		gevor
*	History:
*
****************************************************************************/
int AxiomSetPop::initFromEnv(const Environment& env)
{
  // Для начала, определяем размер выборки
  if (env.getIntParamValue(popSize, "popSize") < 0)
    throw AxiomLibException("AxiomSetPop::initFromEnv : population size is not set.");
  //asPop = new vector<AxiomSetWithStats>;
  //asPop.resize(popSize);
  asPop.clear();

  // Смотрим, надо ли брать какое-то количество аксиом из заданной
  // экспертом популяции
  double pPresetAS=0;
  env.getDoubleParamValue(pPresetAS, "popPreset");
  int nPresetAS = (int)((pPresetAS) * (double)(popSize)); // кол-во аксиом, которые надо сгенерировать из заданных
  if (nPresetAS < 0)
    throw AxiomLibException("AxiomSetPop::initFromEnv : nPresetAS < 0 (maybe, popPreset < 0).");
  
  // Проверяем, если pPresetAS != 0, пробуем читать аксиомы из популяции etc.
  if (nPresetAS != 0) {
    // Часть аксиом должны читать из файла
    // 1. Ищем AxiomBase - она должна быть задана, читаем ее
    std::string axiomBaseDir;
    if (env.getStringParamValue(axiomBaseDir, "AxiomBaseDir") < 0)
      throw AxiomLibException("AxiomSetPop::initFromEnv : popPreset > 0 but axiom base dir is not set.");
    AxiomBase axiomBase;
    axiomBase.readAxioms(axiomBaseDir.c_str());
    
    // 2. ищем базовую директорию с наборами систем аксиом
    std::string axiomSetDir;
    if (env.getStringParamValue(axiomSetDir, "AxiomSetBaseDir") < 0)
      throw AxiomLibException("AxiomSetPop::initFromEnv : popPreset > 0 but axiom set base dir is not set.");

    // 3. получаем множество наборов аксиом
    std::set<std::string> axiomSetNames;
    if (env.getStringSetParamValue(axiomSetNames, "AxiomSet") < 0)
      throw AxiomLibException("AxiomSetPop::initFromEnv : popPreset > 0 but no axiom set is given.");
    int nGivenAxiomSets = (int) axiomSetNames.size();

    // 4. смотрим, может мы не сможем их все поместить
	if (nGivenAxiomSets > nPresetAS) {
	  //throw AxiomLibException("AxiomSetPop::initFromEnv : number of given axiom sets > given number of slots in the population.");
		nPresetAS = nGivenAxiomSets;
	}

    // 5. Начинаем заполнять слоты аксиомами, безо всякого random-а, строго последовательно
    std::set<std::string>::const_iterator curAx = axiomSetNames.begin();
    for (int i = 0; i < nPresetAS; i++) {
	  std::cout << "AxiomSetPop::initFromEnv. Filling slot " << i+1 << " with axiom '" << *curAx << "'" << std::endl;
      
      // По имени, получаем наш axiomSet
	  //(asPop)[i].initAxiomSetFromFile(axiomSetDir, *curAx, axiomBase);
	  asPop.push_back(AxiomSetWithStats());
	  asPop.back().initAxiomSetFromFile(axiomSetDir, *curAx, axiomBase);
    
      // Наконец, берем следующее имя из набора
      curAx++;
      if (curAx == axiomSetNames.end())
        curAx = axiomSetNames.begin();
    }
  }
  
  // Ok, теперь если надо какие-то создавать случайным образом - создаем их
  for (int i = nPresetAS; i < popSize; i++) {
    int nOfAxioms = 10;
    int maxECs = 5;
    std::cout << "AxiomSetPop::initFromEnv. Filling slot " << i+1 << " with random axiom, nOfAxioms = " << nOfAxioms << ", maxECs = " << maxECs << ".\n";
	asPop.push_back(AxiomSetWithStats());
	(asPop).back().initAxiomSetByRand(nOfAxioms, maxECs);
	(asPop).back().saveAxiomSetToFile (".", "x");
  }

  popSize = asPop.size();

  return 0;
}


/****************************************************************************
*				AxiomSetPop::addSets
*
*	Description:	Добавляет сгенерированные систмы аксиом в популцию
*	Parameters:	newSets - матрица с новыми добаляаемыеми системами аксиом 
*				setsMask - матрица маски - указывает какие систем аксиом из матрицы newSets действительно добавлять в популяцию
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void AxiomSetPop::addSets (std::vector < std::vector<AxiomSetWithStats> > &newSets, std::vector < std::vector <int> > &setsMask) {
	// временный вектор для объединения результатов
	std::vector<AxiomSetWithStats> temp;
	temp.clear();
	// подсчет числа реально добавляемых систем аксиом
	int count = 0;
	//std::cout << "\n addSets::setsMask is\n ";
	for (unsigned int i = 0; i < setsMask.size(); i++) {
		//std::cout << "	";
		for (unsigned int j = 0; j < (setsMask[i]).size(); j++) {
			if (setsMask[i][j] > 0) count++;
			//std::cout << "  " << setsMask[i][j];
		}
		//std::cout << "\n";
	}
	// заполнение временного вектора - для объединения старых и новых систем аксиом
	temp.resize(asPop.size() + count);
	for (unsigned int h = 0; h < asPop.size(); h++)
		temp[h] = asPop[h];
	int t = (int) asPop.size();
	for (unsigned int g = 0; g < newSets.size(); g++) 
		for (unsigned int f = 0; f < (newSets[g]).size(); f++)
			if (setsMask[g][f] > 0) {
				temp[t] = newSets[g][f];
				t++;
			}
	// очищение переменных для заполнение объединенными данными
	asPop.clear();
	asPop.resize(temp.size());
	for (unsigned int h = 0; h < temp.size(); h++) {
		asPop[h] = temp[h];
	}

//	 тестовый вывод данных о числе аксиом в системах аксиом
//	 std::cout << "\n		AxiomSetPop::num of axioms in axiom-sets\n		";
//	 for (int i = 0; i < asPop.size(); i++) 
//		std::cout << "  " << (asPop[i]).getNumOfAxioms();
//	std::cout << "\n";

}


/****************************************************************************
*				AxiomSetPop::addSets
*
*	Description:	Добавляет сгенерированные систмы аксиом в популцию
*	Parameters:	newSets - матрица с новыми добаляаемыеми системами аксиом 
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
void AxiomSetPop::addSets (std::vector<AxiomSetWithStats>  &newSets) {
	// временный вектор для объединения результатов
	std::vector<AxiomSetWithStats> temp;
	temp.clear();
	// заполнение временного вектора - для объединения старых и новых систем аксиом
	temp.resize(asPop.size() + newSets.size());
	for (unsigned int h = 0; h < asPop.size(); h++) {
		temp[h] = asPop[h];
	}
	unsigned int t = (unsigned int) asPop.size();
	for (unsigned int g = 0; g < newSets.size(); g++)  {
		temp[t] = newSets[g];
		t++;
	}
	// очищение переменных для заполнение объединенными данными
	asPop.resize(temp.size());
	for (unsigned int h = 0; h < temp.size(); h++) {
		asPop[h] = temp[h];
	}
	
	/*for (unsigned int i = 0; i < newSets.size(); i++) {
		asPop.push_back (newSets[i]);
	}
	return 0;*/
}


// Функция очищения параметров класса
void AxiomSetPop::clear () {
	for (unsigned int i = 0; i < asPop.size(); i++) {
		(asPop[i]).clear();
	}
	asPop.clear();
}


/****************************************************************************
*				AxiomSetPop::exit
*
*	Description:	функция проверяет - существует ли система аксиом с целевой функцией ниже заданного значения
*	Parameters:	exitCond - параметр, ниже которого идет поиск значений целевой функции
*				bestAxiomSet - выбираемая лучшая система аксиом
*	Returns:		true - если таковая система аксиом нашлась
*				false - если не нашлось системы аксиом с целевой функцией ниже заданного параметра
*	Throws:		AxiomLibException - целевая функция не подсчитана хотя бы у одной из систем аксиом (это может быть важно)
*	Author:		dk
*	History:
*
****************************************************************************/
bool AxiomSetPop::exit (double exitCond, AxiomSetWithStats &bestAxiomSet) {
	int tmpFirst, tmpSecond;
	double gBest, gCurBest, gCur;
	unsigned int indCurBest;
	// проверка длины популяции - не может быть меньше 1
	if (asPop.size() <= 0) {
		throw AxiomLibException("AxiomSetPop::exit - population of zero size.\n");
		return true;
	}
	// получение статистики по первой системе аксиом
	(asPop[0]).unsafeGetStats(tmpFirst, tmpSecond, gCurBest);
	indCurBest = 0;
	// получение и анализ статистики по последнему циклу генетического алгоритма
	for (unsigned int i = 1; i < asPop.size(); i++) {
		// получение статистики по текущей системе аксиом
		(asPop[i]).unsafeGetStats(tmpFirst, tmpSecond, gCur);
		// проверка - была ли особт инициализирована
		if (gCur < 0)
			throw AxiomLibException("AxiomSetPop::exit - goal stat was not initialized.\n");
		// поиск лучшей системы аксиом по значению целевой функции
		if (gCur < gCurBest) {
			gCurBest = gCur;
			indCurBest = i;
		}
	}
	// чтение информации о статистике лучшей системы аксиом по резульатам всехъ прдыдущих циклов
	bestAxiomSet.unsafeGetStats(tmpFirst, tmpSecond, gBest);
	// сравнение получнной статистики из популяции с лучшей системой аксимо за всю историю - замена, если неоходимо таковой
	//std::cout << "\n AxiomSetPop::exit : indCurBest is " << indCurBest << "     gCurBest is " << gCurBest << "     gBest is " << gBest << "\n";
	if ( (gCurBest < gBest) || (gBest < 0) ) {
		bestAxiomSet = asPop[indCurBest];
	}
	
	// возвращение условия досрочного выхода
	if (gCurBest <= exitCond) 
		return true; 
	return false;
}


/****************************************************************************
*			AxiomSetPop::enter
*
*	Description:	Размеяает ряд row начиная с индеска begin заканичвая 
*				индексом end системой аксиом номер asNum из вектора asPop 
*				и записывает результаты разметки в ряд markup
*	Parameters:	markup - вектор для записи результатов разметки
*				row - исходный врменной ряд для разметки системой аксиом
*				begin - номер отсчтера ряда row откуда вести разметку
*				end - номер отсчета ряда row докуда вести разметку
*				asNum - номер системы аксим в asPop которой проводить разметку ряда
*	Returns:		0
*	Throws:		AxiomLibException - если asNum - out of range
*	Author:		dk
*	History:
*
****************************************************************************/
int AxiomSetPop::enter (std::vector<int>& markup, const std::vector<double>& row, const unsigned long begin, const unsigned long end, int asNum) {
	if ( (asNum < 0) || (asNum >= (int) asPop.size()) ) {
		throw AxiomLibException("Error in AxiomSetPop::enter : wrong parameter asNum - out of range.");
	}
	((asPop)[asNum]).enter (markup, row, begin, end);
	return 0;
}


/****************************************************************************
*			AxiomSetPop::getBasePopSize
*
*	Description:	Возвращает "базовый" (указанный в конфиге) размер популяции
*	Parameters:	-
*	Returns:		popSize
*	Throws:		AxiomLibException - если axiomPop не проинициализирована
*	Author:		gevor
*	History:
*
****************************************************************************/
int AxiomSetPop::getBasePopSize(void) const
{
  if (popSize < 0)
    throw AxiomLibException("AxiomSetPop::getBasePopSize - population has not been initialized.");
  return popSize;
}


/****************************************************************************
*			AxiomSetPop::size
*
*	Description:	Возвращает текущий размер популяции
*	Parameters:	-
*	Returns:	pop.size()
*	Throws:		AxiomLibException - если axiomPop не проинициализирована
*	Author:		gevor
*	History:
*
****************************************************************************/
int AxiomSetPop::size(void) const
{
  //if (popSize < 0)
  //  throw AxiomLibException("AxiomSetPop::size - population has not been initialized.");
  return (int) asPop.size();
}


/****************************************************************************
*			AxiomSetPop::name
*
*	Description:	Функция возвращающая имя системы аксиом по номеру в популяции
*	Parameters:		index - номер системы аксиом в популяции
*	Returns:		std::string - название системы аксиом или пустую строку для некорректного значения index
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string AxiomSetPop::name (int index) const {
	if ((index >=0) && (index < (int) asPop.size()))
		return asPop[index].name();
	return (std::string) "";
}


/****************************************************************************
*				AxiomSetPop::voidStats
*	
*	Description:	Сброс статистики системы аксиом под номером asNum в векторе asPop
*	Parameters:	asNum - номер системы аксиом в векторе asPop к которой идет обращение
*	Returns:		-
*	Throws:		AxiomLibException - если asNum находится за возможными границами
*	Author:		dk
*	History:
*	
****************************************************************************/
void AxiomSetPop::voidStats (const int asNum) {
	if ( (asNum < 0) || (asNum >= (int) asPop.size()) )
		throw AxiomLibException("Error in AxiomSetPop::voidStats: input number of axiomSet is out of range.");
	(asPop)[asNum].voidStats();
}
 

/****************************************************************************
*				AxiomSetPop::setStats
*	
*	Description:	Установка статистики системы аксиом под номером asNum в векторе asPop
*	Parameters:	asNum - номер системы аксиом в векторе asPop к которой идет обращение
*				nf - устанавливаемое значение числа ошибок первого рода
*				ns - устанавливаемое значение числа ошибок второго рода
*				g - устанавливаемое значение целевой функции
*	Returns:		-
*	Throws:		AxiomLibException - если asNum находится за возможными границами
*	Author:		dk
*	History:
*	
****************************************************************************/
void AxiomSetPop::setStats (const int asNum, signed int nf, signed int ns, double g) {
	if ( (asNum < 0) || (asNum >= (int) asPop.size()) )
		throw AxiomLibException("Error in AxiomSetPop::setStats: input number of axiomSet is out of range.");
	(asPop)[asNum].setStats (nf, ns, g);
}


/****************************************************************************
*				AxiomSetPop::getStats
*	
*	Description:	Получение статистики, без проверок, для системы аксиом под номером asNum в векторе asPop
*	Parameters:	asNum - номер системы аксиом в векторе asPop к которой идет обращение
*				nf - считываемое значение числа ошибок первого рода
*				ns - считываемое значение числа ошибок второго рода
*				g - считываемое значение целевой функции
*	Returns:		-
*	Throws:		AxiomLibException - если asNum находится за возможными границами
*	Author:		dk
*	History:
*	
****************************************************************************/
void AxiomSetPop::getStats (const int asNum, signed int& nf, signed int& ns, double& g) const {
	if ( (asNum < 0) || (asNum >= (int) asPop.size()) )
		throw AxiomLibException("Error in AxiomSetPop::getStats: input number of axiomSet is out of range.");
	(asPop)[asNum].unsafeGetStats (nf, ns, g);
}


/****************************************************************************
*				AxiomSetPop::cutDown
*	
*	Description:	Функция прореживания популяции: удляются из популяции все особи (системы аксиом) 
*				- по индексу которых во входном векторе стоит false.
*	Parameters:	stayAlive - вектор указаний - особь с каким индексом оставить в популяции
*						(то есть если stayAlive[i] = false - то i-ая особь будет удалена из популяции)
*	Returns:		0
*	Throws:		AxiomLibException - если длина входного вектора не соответствует числу особей в популяции
*	Author:		dk
*	History:
*	
****************************************************************************/
int AxiomSetPop::cutDown (std::vector <bool> &stayAlive) {
	if ( stayAlive.size() != asPop.size() )
		throw AxiomLibException("Error in AxiomSetPop::cutDown: input vector's length is incorrect.");
	std::vector<AxiomSetWithStats>::iterator iter;
	std::vector <bool>::iterator witer;
	int flag = 0;
	while (flag == 0) {
		flag = 1;
		witer = stayAlive.begin();
		for (iter = asPop.begin(); iter != asPop.end(); iter++) {
			if (*witer == false) { 
				asPop.erase(iter);
				stayAlive.erase(witer);
				flag = 0;
				break;
			}
			witer++;
		}
	}
	return 0;
}


/****************************************************************************
*				AxiomSetPop::copyAxiomSet
*	
*	Description:	Copy axiom set with statistic by number asNum from this class
*	Parameters:		asNum - number of copiyng axiomSet in asPop
*	Returns:		copy of axiom Set
*	Throws:			AxiomLibException - if asNum is out of range
*	Author:			dk
*	History:
*	
****************************************************************************/
AxiomSetWithStats& AxiomSetPop::axiomSet (const int asNum) {
	if ( (asNum < 0) || (asNum >= (int) asPop.size()) )
		throw AxiomLibException("Error in AxiomSetPop::copyAxiomSet: input paramenters out of range.");
	return asPop[asNum];
}


/****************************************************************************
*				AxiomSetPop::saveAxiomSetPopToFiles
*	
*	Description:	Сохранение систем аксиом в файлы
*	Parameters:		env - класс окружения, из которого запрашивать необходимые параметры
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*	
****************************************************************************/
int AxiomSetPop::saveAxiomSetPopToFiles (Environment &env) {
	std::string saveName, saveDir, saveNameCur;
	if (env.getStringParamValue (saveDir, "PathToSaveResults") < 0)
		return -1;
	if (env.getStringParamValue (saveName, "saveTo") < 0)
		saveName.assign ("axiomSet_");
	char tmps[10];
	int first, second;
	double goal;
	for (int asNum = 0; asNum < (int) asPop.size(); asNum++) {
		tmps[0]='\0';
		sprintf(tmps,"%d",asNum + 1);
		saveNameCur = saveName;
		saveNameCur.append(tmps);
		asPop[asNum].unsafeGetStats (first, second, goal);
		asPop[asNum].saveAxiomSetToFile (saveDir, saveNameCur, first, second);
	}
	return 0;
}


/****************************************************************************
*				AxiomSetPop::operator=
*	
*	Description:	Оператор копирования популяции
*	Parameters:	second - копируемая популяция
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*	
****************************************************************************/
/*AxiomSetPop& AxiomSetPop::operator= (AxiomSetPop& second) {
	// Кипируем значения переменных класса - переменная popSize
	this->popSize = second.popSize;
	// Переменная asPop - вектор AxiomSetWithStat - для пкоторого определена операция копирования
	this->asPop.resize(second.asPop.size());
	for (int i = 0; i < second.asPop.size(); i++) {
		(this->asPop)[i] = second.asPop[i];
	}
	return *this;
}*/
