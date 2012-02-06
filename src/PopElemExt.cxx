/****************************************************************************
*
*			Функции класса PopElemExt
*
****************************************************************************/

#include "PopElemExt.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

/****************************************************************************
*			PopElemExt::initFromEnv
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
int PopElemExt::initFromEnv (const Environment& env) {
	std::string axiomBaseDir;
	if (env.getStringParamValue(axiomBaseDir, "AxiomBaseDir") < 0)
		throw AxiomLibException("PopElemExt::initFromEnv : popPreset > 0 but axiom base dir is not set.");
	AxiomBase axiomBase;
	axiomBase.readAxioms(axiomBaseDir.c_str());

	// 2. ищем базовую директорию с наборами систем аксиом
	std::string axiomSetDir;
	if (env.getStringParamValue(axiomSetDir, "AxiomSetBaseDir") < 0)
		throw AxiomLibException("PopElemExt::initFromEnv : popPreset > 0 but axiom set base dir is not set.");

	// 3. получаем множество наборов аксиом
	std::set<std::string> axiomSetNames;
	if (env.getStringSetParamValue(axiomSetNames, "AxiomSet") < 0)
		throw AxiomLibException("PopElemExt::initFromEnv : popPreset > 0 but no axiom set is given.");
	int nGivenAxiomSets = (int) axiomSetNames.size();
	if (nGivenAxiomSets > 1) std::cout << "\n	PopElemExt::initFromEnv: Too many diclarations of AxiomSet.\n" ;
	if (nGivenAxiomSets < 1) throw AxiomLibException("PopElemExt::initFromEnv : no axiom set is given.");
	// По имени, получаем наш axiomSet
	axSet.initAxiomSetFromFile(axiomSetDir, *(axiomSetNames.begin()), axiomBase);

	double tempDB;
	if (env.getDoubleParamValue(tempDB, "precision") < 0)
		throw AxiomLibException("PopElemExt::initFromEnv : cannot find value of precision.");
	// Получаем значение prec
	if ( ((double) rand() / (double) RAND_MAX) > tempDB ) {
		prec = tempDB + (1.0 - tempDB) * ((double) rand() / (double) RAND_MAX) / 3.0;
	} else {
		prec = tempDB -  tempDB * ((double) rand() / (double) RAND_MAX) / 3.0;
	}

	return 0;
}


/****************************************************************************
*			PopElemExt::enter
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
int PopElemExt::enter (std::vector<int>& markup, const std::vector<double>& row, const unsigned long begin, const unsigned long end) {
	axSet.enter (markup, row, begin, end);
	return 0;
}


/****************************************************************************
*				PopElemExt::voidStats
*	
*	Description:	Сброс статистики системы аксиом под номером asNum в векторе asPop
*	Parameters:	asNum - номер системы аксиом в векторе asPop к которой идет обращение
*	Returns:		-
*	Throws:		AxiomLibException - если asNum находится за возможными границами
*	Author:		dk
*	History:
*	
****************************************************************************/
void PopElemExt::voidStats () {
	axSet.voidStats();
}
 

/****************************************************************************
*				PopElemExt::setStats
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
void PopElemExt::setStats (signed int nf, signed int ns, double g) {
	axSet.setStats (nf, ns, g);
}


/****************************************************************************
*				PopElemExt::getStats
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
void PopElemExt::getStats (signed int& nf, signed int& ns, double& g) const {
	axSet.unsafeGetStats (nf, ns, g);
}
