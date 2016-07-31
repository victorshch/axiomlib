/****************************************************************************
*					CrossoverTpl.cxx
*
*	Description:	Функции класса CrossoverTpl - Реализация
*	Author:			dk
*	History:		
*
****************************************************************************/
#include "CrossoverTpl.h"

using namespace AxiomLib;

// Параметры, которые не предполагается менять
#define levelOfSub 0.55  // Уровень вложенности аксиом, по которому они сортируются внутри системы аксиом
#define minProfit 0.5 // среднее значение для параметра profit некоторой аксиомы
#define epsilon 0.001 // чтобы избежать проблем при сравнении double'ов
#define ecAverageValue 0.75 // Объем параметров ЭУ с типом double, которые будут усредняться при проведении скрещивания на уровне ЭУ

// Конструктор класса с заданием значений по умолчанию
CrossoverTpl::CrossoverTpl (void) {
	axioms.clear();
	axiomsProfit.clear();
	axiomsMask.clear();
	axiomsMaskProb.clear();
}

// Деструктор класса - пустой
CrossoverTpl::~CrossoverTpl (void) {

}


/****************************************************************************
*					CrossoverTpl::name
*
*	Description:	Возвращает имя данного класса
*	Parameters:		-
*	Returns:		std::string - имя данного класса
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
std::string CrossoverTpl::name (void) const {
	std::string toReturn("CrossoverTpl");
	return toReturn;
}


/****************************************************************************
*                       CrossovercTry::setParamsFromEnv
*
*       Description:	Инициализировать параметры данными из env
*       Parameters:		const Environment& env - среда
*       Returns:		0
*       Throws:			AxiomLibException - если нет какого-то параметра
*       Author:			dk
*       History:
*
****************************************************************************/
int CrossoverTpl::setParamsFromEnv (const Environment& env) {	

	return 0;
}


/****************************************************************************
*					CrossoverTpl::choosePairs
*
*	Description:	Функция выбора пар систем аксиом для скрещивания на 
*					уровне систем аксиом. Выбор происходит по рандому 
*					из всего набора систем аксиом. Число выбранных пар 
*					пропорционально параметру скрещивания axiomSetCrossLevel.
*	Parameters:		asPairs - вектор пар систем аксиом для скрещивания (заполняется в данной функцие)
*	Returns:		int - число сформированных пар
*	Throws:			- 
*	Author:			dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::choosePairs (std::vector <Pair> &asPairs, const std::vector <AxiomSetPlus> &population) {
	// Проверка на достаочное число систем аксиом
	if (population.size() < 2) return 0;
	// Подготовка внутренних вспомогательных переменных
	Pair tmpPair;
	unsigned short sec;
	int flag = 0;
	// Пробегаем по всему набору систем аксиом и определяем какие из систем аксиом будут чавствовать в скрещвании
	for (unsigned short i = 0; i < (int) population.size(); i++) {
		// для каждой системы аксиом случайным образом с заданным параметром отсечения определяем будет ли она учавствовать в скрещивании
		if (( (double) rand() / (double) RAND_MAX ) < population[i].asCrossProb) {
			sec = (unsigned short) ( ( (double) rand() / (double) RAND_MAX ) * ((double) population.size() - 2.0) );
			// выбрали системы аксиом - заполняем структуру пары и дополняем ее в asPairs
			if (sec == i) sec++;
			tmpPair.first = i;
			tmpPair.second = sec;
			asPairs.push_back(tmpPair);
			flag++;
		}
	}
	return flag;
}


/****************************************************************************
*				CrossoverTpl::crossAxiomSets
*
*	Description:	Функция скрещивания двух выбранных систем аксиом на 
*				уровне систем аксиом. Скрещивание происходит путем 
*				создания новой системы аксиом из аксиом составляющих
*				две системы, выбранные для скрещивания.
*	Parameters:	as1 - номер первой скрещиваемой системы аксиом
*				as1 - номер второй скрещиваемой системы аксиом
*				axiomSetPlus - особь популяции, которая может быть создана при скрещивании
*	Returns:		1 - если была создана новая особь
*					0 - если новая особь не была создана
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::crossAxiomSets (unsigned short as1, unsigned short as2, AxiomSetPlus &axiomSetPlus) {
	// Подготовка внутренних переменных функции
	int newSet1, newSet2; 
	std::vector <Axiom *> toInclude;
	std::vector <bool> mask;
	unsigned short curNum;
	// определение числа аксиом из каждой системы, которые будут входить во вновь созданную систему аксиом
	// (в новую систему будут входить от 1/3 до 2/3 аксиом из каждой из систем)
	newSet1 = (int) ( ( ( (double) rand() / (double) RAND_MAX ) * ( (double) axioms[as1]->size() - 1.0) / 3 ) + ( (double) axioms[as1]->size() - 1.0) / 3);
	newSet2 = (int) ( ( ( (double) rand() / (double) RAND_MAX ) * ( (double) axioms[as2]->size() - 1.0) / 3 ) + ( (double) axioms[as2]->size() - 1.0) / 3);
	if ( (newSet1 <= 0) && (newSet2 <= 0) ) return 0;
	// Набор аксиом, которые будут использоваться
	toInclude.resize( newSet1 + newSet2 );
	// Определение - какие аксиомы из первой системы аксиом будут входить во вновь создаваемую систему аксиом
	// Обнуление вектора маски - используется для препятствия вхождения в систему аксиом - одинаковых аксиом
	mask.resize (axioms[as1]->size());
	for (unsigned int t = 0; t < mask.size(); t++) {
		mask[t] = false;
	}
	double curProb;
	double curSum = 0.0;
	for (int i = 0; i < newSet1; i++) {
		curProb = ( (double) rand() / (double) RAND_MAX ) * (axiomsMaskProb[as1][axiomsMaskProb[as1].size() - 1] - curSum - epsilon);
		curNum = 0;
		while (curProb > axiomsMaskProb[as1][curNum])
			curNum++;
		while (mask[curNum])
			curNum++;
		if (curNum >= axioms[as1]->size())
			throw AxiomLibException("CrossoverTpl::crossAxiomSets : internal error - wrong logic in choosing axioms for new AxiomSet.");
		mask[curNum] = true;
		curSum += minProfit + axiomsProfit[as1][axiomsMask[as1][curNum]];
		toInclude[i] = (*axioms[as1])[axiomsMask[as1][curNum]];
	}
	// Определение - какие аксиомы из второй системы аксиом будут входить во вновь создаваемую систему аксиом
	// Обнуление вектора маски - используется для препятствия вхождения в систему аксиом - одинаковых аксиом
	mask.resize (axioms[as2]->size());
	for (unsigned int t = 0; t < mask.size(); t++) {
		mask[t] = false;
	}
	curSum = 0.0;
	for (int i = 0; i < newSet2; i++) {
		curProb = ( (double) rand() / (double) RAND_MAX ) * (axiomsMaskProb[as2][axiomsMaskProb[as2].size() - 1] - curSum - epsilon);
		curNum = 0;
		while (curProb > axiomsMaskProb[as2][curNum])
			curNum++;
		while (mask[curNum])
			curNum++;
		if (curNum >= axioms[as2]->size())
			throw AxiomLibException("CrossoverTpl::crossAxiomSets : internal error - wrong logic in choosing axioms for new AxiomSet.");
		mask[curNum] = true;
		curSum += minProfit + axiomsProfit[as2][axiomsMask[as2][curNum]];
		toInclude[newSet1 + i] = (*axioms[as2])[axiomsMask[as2][curNum]];
	}
	// инициализация системы аксиом путем копирования выбранных аксиом
	if (axiomSetPlus.axiomSet == NULL)
		axiomSetPlus.axiomSet = new AxiomSet;
	axiomSetPlus.axiomSet->initAxiomSetByCopy (toInclude, levelOfSub);
	return 1;
}


/****************************************************************************
*				CrossoverTpl::chooseAxQuads
*
*	Description:	Функция выбора пар аксиом для скрещивания на уровне аксиом.
*					Выбор происходит по рандому из всего набора аксиом. При этом от
*					каждой системы аксиом может быть выбрано в среднем 1-3 аксиомы
*					для скрещивания, при этом учитывается вероятность axCrossProb
*					участия аксиом данной системы в скрещивании.
*	Parameters:		axQuads - вектор из описаний Quad - какие аксиомы скрещивать
*	Returns:		int - число сформированных пар
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::chooseAxQuads (std::vector <Quad> &axQuads, const std::vector <AxiomSetPlus> &population) {
	// Подготовка внутрених переменных функции
	int tmpDiv;
	int fax, second, sax;
	double average;
	Quad tmpQuad;
	int flag = 0;

	// Подсчет среднего числа аксиом в системе аксиом в популяции
	average = 0.0;
	for (unsigned int j = 0; j < axioms.size(); j++) {
		average += (double) (axioms[j])->size();
	}
	if (axioms.size() < 1) return 0;
	average = average / (double) axioms.size();

	// пробегаем по всем аксиомам в популяции и случайно определяем будет ли использоваться аксиома в скрещивании
	for (int i = 0; i < (int) axioms.size(); i++) {
		tmpDiv = (int) ( 1.0 + (double) (axioms[i])->size() / average );
		for (int y = 0; y < tmpDiv; y++) {
			if (( (double) rand() / (double) RAND_MAX ) < population[i].axCrossProb) {
				fax = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (axioms[i])->size() - 1.0) );
				second = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) axioms.size() - 1.0) );
				sax = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (axioms[second])->size() - 1.0) );
				// Определили тех, кто используется в скрещивании - заполняем соответствующуюс структуру и дополняем вектор axQuads
				tmpQuad.first = i;
				tmpQuad.fax = fax;
				tmpQuad.second = second;
				tmpQuad.sax = sax;
				axQuads.push_back(tmpQuad);
				flag++;
			}
		}
	}
	
	return flag;
}


/****************************************************************************
*					CrossoverTpl::crossAxioms
*
*	Description:	Функция скрещивания двух выбранных аксиом 
*					на уровне аксиом. В данной реализации скрещивание 
*					предполагает обмен элементарными условиями из аксиом.
*					Выбор элементарных условий для обмена основывается на 
*					совместимости с элементарными условиями во вновь 
*					полученных аксиомах.
*	Parameters:		as1 - номер первой скрещиваемой системы аксиом
*					ax1 - номер аксиомы в первой системе аксиом
*					as1 - номер второй скрещиваемой системы аксиом
*					ax1 - номер аксиомы во второй системе аксиом
*					axiomSetPlus - особь популяции, которая может быть создана при скрещивании
*	Returns:		1 - если была создана новая особь
*					0 - если новая особь не была создана
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::crossAxioms (unsigned short as1, unsigned short ax1, unsigned short as2, unsigned short ax2, AxiomSetPlus &axiomSetPlus) {
	// Создаем новый вектор и заводим в нем копию аксиом из первой выбранной системы as1
	std::vector < Axiom* > newSet;
	newSet.resize (axioms[as1]->size());
	for (unsigned int i = 0; i < newSet.size(); i++) {
		newSet[i] = new Axiom;
		*(newSet[i]) = *(*axioms[as1])[i];
	}
	// Создаем копию второй аксиомы, выбранной для скрещивания
	Axiom* secondAxiom;
	secondAxiom = new Axiom;
	*secondAxiom = *(*axioms[as2])[ax2];
	// Для удобства заводим ссылку так же и на первую аксиому
	Axiom* firstAxiom;
	firstAxiom = newSet[ax1];

	// Определение числа элементарных условий в каждой из аксиом
	int numOfEc1, numOfEc2;
	numOfEc1 = firstAxiom->getNumOfEC();
	numOfEc2 = secondAxiom->getNumOfEC();

	// Определяем максимальное число ЭУ, которыми могут обменяться аксиомы
	int numToChange;
	if (numOfEc1 > 2*numOfEc2)
		numToChange = numOfEc2;
	else
		numToChange = round ( ((double) numOfEc1) / 2.0 );

	// Подготовка внутренних переменных класса
	Check checker;	// Класс для проверки совместимости ЭУ
	int chgDone = 0;	// Число сделанных замен ЭУ
	std::vector < std::vector <bool> > compatMatrix;	// Матрица совместимости ЭУ рассматриваемых аксиом
	compatMatrix.resize (numOfEc1);
	for (unsigned int k = 0; k < (unsigned int) numOfEc1; k++)
		compatMatrix[k].resize (numOfEc2);
	int numOfFalse;	// Число несовместимых ЭУ в одном столбце матрицы compatMatrix
	unsigned short lastFalse;	// Индекс последнего несовместимого ЭУ в столбце матрицы compatMatrix
	std::vector <Pair> ableToChg;	// Вектор пар индексов ЭУ в аксиомах, которые можно поменять
	int able;	// Позиция в векторе ableToChg, выбранная случайно
	unsigned short curSize;	// Переменная для хранения текущего размера вектора
	// Цикл по числу ЭУ, которыми могут обменяться аксиомы
	for (int t = 0; t < numToChange; t++) {
		// Поиск совместимых по обмену эелементарных условий - то есть таких, при обмене которыми получаются совместимые аксиомы (аксиомы из совместимых по and элементарных условий)
		able = 0;
		for (int i = 0; i < numOfEc1; i++) {
			for (int j = 0; j < numOfEc2; j++) {
				compatMatrix[i][j] = checker.checkAndCompatibility (firstAxiom, i, secondAxiom, j) != 0;
			}
		}
		// Определяем какие ЭУ на какие можно заменить и заносим соответствующие индексы в вектор пар ableToChg
		ableToChg.clear();
		for (unsigned short j = 0; j < numOfEc2; j++) {
			numOfFalse = 0;
			for (unsigned short i = 0; i < numOfEc1; i++) {
				if (!compatMatrix[i][j]) {
					lastFalse = i;
					numOfFalse++;
					if (numOfFalse > 1)
						break;
				}
			}
			if (numOfFalse == 0) {
				curSize = ableToChg.size();
				ableToChg.resize (curSize + numOfEc1);
				for (unsigned int i = 0; i < (unsigned int) numOfEc1; i++) {
					ableToChg[curSize + i].first = i;
					ableToChg[curSize + i].second = j;
				}
				continue;
			}
			if (numOfFalse == 1) {
				curSize = ableToChg.size();
				ableToChg.resize (curSize + 1);
				ableToChg[curSize].first = lastFalse;
				ableToChg[curSize].second = j;
				continue;
			}
		}
		// Теперь в ableToChg хранятся пары индексов ЭУ, которыми могут обменяться рассматриваемые аксиомы
		if (ableToChg.size() < 1) continue;
		// Выбираем какой именно обмен производить - случайно
		able = round ( ((double) ableToChg.size() - 1.0) * ((double) rand()) / ((double) RAND_MAX) );
		// Производим обмен элементарными условиями
		checker.change (firstAxiom, ableToChg[able].first, secondAxiom, ableToChg[able].second);
		chgDone++;
	}

	// В случае, если скрещивание произошло - создаем новую особь согласно данному скрещиванию
	if (chgDone > 0) {
		// инициализация системы аксиом путем копирования аксиом из сформированного вектора
		if (axiomSetPlus.axiomSet == NULL)
			axiomSetPlus.axiomSet = new AxiomSet;
		axiomSetPlus.axiomSet->initAxiomSetByCopy (newSet, levelOfSub);
	}
	
	// Удаление созданных аксиом 
	for (unsigned int i = 0; i < newSet.size(); i++) {
		newSet[i]->clear();
		delete newSet[i];
	}
	secondAxiom->clear();
	delete secondAxiom;
	
	// Выход со значением в зависимости от того - была ли создана новая особь
	if (chgDone > 0)
		return 1;
	return 0;
}


/****************************************************************************
*					CrossoverTpl::chooseEcQuads
*
*	Description:	Функция выбора пар элементарных условий для скрещивания 
*					на уровне параметров элементарных усовий.
*					Выбор происходит по рандому из всего набора элементарных условий.
*	Parameters:		ecQuads - вектор описаний QuadPlus - описаний какие элементарные условия 
*					были выбранны для скрещивания
*	Returns:		int - число сформированных пар
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::chooseECQuads (std::vector <QuadPlus> &ecQuads, const std::vector <AxiomSetPlus> &population) {
	// подготовка внутренних переменных данной функции
	std::map <std::string, std::vector<Pair> > ecMap;
	std::map <std::string, std::vector<Pair> >::iterator iter;
	std::vector<std::string> ecNames;
	std::vector<Pair> tmpVec;
	Pair tmpPair;
	ecMap.clear();
	tmpVec.resize(1);
	int overall = 0;
	double average = 0.0;
	int tmpDiv;
	int ec1, ec2;
	int flag;
	QuadPlus tmpQuad;
	// предварительный этап - сбор информации о наличии элементарных условий в аксиомах - заполнение структуры ecMap
	for (int i = 0; i < (int) axioms.size(); i++) {
		for (int j = 0; j < (int) (axioms[i])->size(); j++) {
			ecNames.clear();
			(*(axioms[i]))[j]->getECNames(ecNames);
			overall += (int) ecNames.size();
			for (int k = 0; k < (int) ecNames.size(); k++) {
				tmpPair.first = i;
				tmpPair.second = j;
				if (ecMap.count (ecNames[k]) > 0) {
					// то есть записи о элементарных условиях с тем же именем из данного набора систем аксиом уже имеется во множестве
					ecMap[ecNames[k]].push_back(tmpPair);
				}
				else {
					// то есть записи о таком элементарном условии еще нет - создаем ее в ecMap
					tmpVec[0] = tmpPair;
					ecMap.insert(make_pair (ecNames[k], tmpVec));
				}
			}
		}
	}
	// непосредствнно распределение - какие элементарные условия скрщивать - заполняем структуру ecQuad
	ecQuads.clear();
	flag = 0;
	if (ecMap.size() < 1) return 0;
	average = (double) overall / (double) ecMap.size() ; // среднее число повторений элементарного условия во всех системах аксиом
	for (iter = ecMap.begin(); iter != ecMap.end(); iter++) {
		if ((iter->second).size() < 2) continue; // то есть нет двух элементарных условий данного типа - некому скрещиваться
		tmpDiv = (int) ( 1.0 + (double) (iter->second).size() / average );
		for (int h = 0; h < tmpDiv; h++) {
			// Выбор ЭУ
			ec1 = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (iter->second).size() - 1.0) );
			// определяем - а будет ли скрещивание для выбранного ЭУ - используем параметр ecCrossProb 
			if (( (double) rand() / (double) RAND_MAX ) < population[((iter->second)[ec1]).first].ecCrossProb) {
				// выбор второго ЭУ того же типа для скрещивания				
				ec2 = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (iter->second).size() - 2.0) );
				if (ec2 == ec1) ec2++;
				// заполнение структуры tmpQuad
				tmpQuad.first = ((iter->second)[ec1]).first;
				tmpQuad.fax = ((iter->second)[ec1]).second;
				tmpQuad.second = ((iter->second)[ec2]).first;
				tmpQuad.sax = ((iter->second)[ec2]).second;
				tmpQuad.ecName = iter->first;
				// запись заполненной структуры в вектор входных значений
				ecQuads.push_back(tmpQuad);
				flag++;
			}
		}
	}
	
	return flag;
}


/****************************************************************************
*				CrossoverTpl::crossEcs
*
*	Description:	Скрещивание двух выбранных пар элементарных условий 
*				на уровне параметров элементаррных условий.
*				Скрещивание происходит путем усреднения параметров
*				одинаковых элементарных условий из разных аксиом.
*	Parameters:	as1 - номер первой скрещиваемой системы аксиом
*				ax1 - номер аксиомы в первой системе аксиом
*				as1 - номер второй скрещиваемой системы аксиом
*				ax1 - номер аксиомы во второй системе аксиом
*				ecName - имя элементарного условия, которое было выбранно для скрещивания
*				axiomSetPlus - особь популяции, которая может быть создана при скрещивании
*	Returns:		1 - если была создана новая особь
*					0 - если новая особь не была создана
*	Throws:		AxiomLibExeption - если для скрещивания были выбранны несовместимые элементарные условия (разного типа)
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::crossECs (unsigned short as1, unsigned short ax1, unsigned short as2, unsigned short ax2, std::string &ecName, AxiomSetPlus &axiomSetPlus) {
	// Производим копирование аксиомы для ее последующего восстановления
	Axiom saveAxiom;
	saveAxiom = *(*axioms[as1])[ax1];
	// подготовка внутренних переменных
	int ecNum1, ecNum2;
	int curRes = -1;
	double param = 0.0;
	double param1 = 0.0;
	double param2 = 0.0;
	int chgDone = 0;
	std::vector<std::string> ecParamNames;
	// получение числа элементарных условий в каждой из аксом - считываем чтобы по многу раз не обращаться к одной и той же функции в течение данной функции
	ecNum1 = (*axioms[as1])[ax1]->getECNumByName (ecName);
	ecNum2 = (*axioms[as2])[ax2]->getECNumByName (ecName);
	// проверка на корректность аксиом
	if ( (ecNum1 < 0) || (ecNum2 < 0) )
		throw AxiomLibException("CrossoverTpl::crossECs : there is an error in generating pairs of elementary conditions to cross. ");
	// Проводим скрещивание значений параметров выбранного элементарного условия
	(*axioms[as1])[ax1]->getECParamNames(ecNum1, ecParamNames);
	for (int j = 0; j < (int) ecParamNames.size(); j++) {
		curRes = (*axioms[as1])[ax1]->getParamValue (param1, ecNum1, ecParamNames[j]);
		if ( (curRes == 0) && ( ( (double) rand() / (double) RAND_MAX ) < ecAverageValue ) ) {
			// Если 0 - то все считалось правильно (в double) - проводим изменение параметров и записываем обратно
			// (для этого считываем второй параметр)
			curRes = (*axioms[as2])[ax2]->getParamValue (param2, ecNum2, ecParamNames[j]);
			if (curRes != 0)
				throw AxiomLibException("CrossoverTpl::crossEcs : there is an error in generating pairs of elementary conditions to cross, ECs are not of the same Class. ");
			param = (param1 + param2) / 2.0; // Проводим усреднение параметров - возможно заменить на произвольную функцию
			// Записываем обратно измененное значение парметра
			(*axioms[as1])[ax1]->setParamValue (param, ecNum1, ecParamNames[j]);
			chgDone++;
		}
		else{
			// Если параметр не double - и к нему не приводится взаимооднозначно - такой случай можно добавить - пока без надобности			
		}
	}

	// В случае, если скрещивание произошло - создаем новую особь согласно данному скрещиванию
	if (chgDone > 0) {
		// инициализация системы аксиом путем копирования аксиом из сформированного вектора
		if (axiomSetPlus.axiomSet == NULL)
			axiomSetPlus.axiomSet = new AxiomSet;
		axiomSetPlus.axiomSet->initAxiomSetByCopy (*axioms[as1], levelOfSub);
		// Восстановление измененных значений
		*(*axioms[as1])[ax1] = saveAxiom;
		saveAxiom.clear();
		return 1;
	}

	return 0;
}


/****************************************************************************
*					CrossoverTpl::initCrossover
*
*	Description:	Функция инициализации класса стратегии скрещивания. 
*					Получение вектора ссылок на наборы аксиом
*	Parameters:		population - популяция систем аксиом со статистикой
*	Returns:		-
*	Throws:			AxiomLibException - если хотя бы одна система аксиом 
*					в популяции не определена
*	Author:			dk
*	History:
*
****************************************************************************/ 
void CrossoverTpl::initCrossover (const std::vector <AxiomSetPlus> &population) {
	axioms.resize (population.size());
	axiomsProfit.resize (population.size());
	axiomsMask.resize (population.size());
	axiomsMaskProb.resize (population.size());
	for (unsigned int i = 0; i < population.size(); i++) {
		// Устанавливаем указатели на аксиомы
		if (population[i].axiomSet == NULL) 
			throw AxiomLibException("CrossoverTpl::initCrossover : population is not set or empry.");
		axioms[i] = &(population[i].axiomSet->axioms);
		// Устанавливаем статистику по каждой системе аксиом
		axiomsProfit[i] = population[i].axiomProfit;
		if (axioms[i]->size() != axiomsProfit[i].size())
			throw AxiomLibException("CrossoverTpl::initCrossover : axioms profit is not set.");
		// Устанавливаем значение маски, которая содержит номера аксиом в векторе в порядке уменьшения функции значимости
		sort (axiomsProfit[i], axiomsMask[i]);
		// Устанавливаем значения вектора накопленного значения профита аксиом
		axiomsMaskProb[i].resize(axiomsProfit[i].size());
		if (axiomsProfit[i].size() > 0) {
			axiomsMaskProb[i][0] = minProfit + axiomsProfit[i][axiomsMask[i][0]];
			for (unsigned short j = 1; j < axiomsProfit[i].size(); j++)
				axiomsMaskProb[i][j] = axiomsMaskProb[i][j-1] + minProfit + axiomsProfit[i][axiomsMask[i][j]];
		}
	}
}

 
/****************************************************************************
*					CrossoverTpl::sort
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
inline void CrossoverTpl::sort (std::vector <double> &vec, std::vector <unsigned short> &mask) {
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
*					CrossoverTpl::addSets
*
*	Description:	Функция добавления в популяцию созданных систем аксиом
*	Parameters:		population - популяция систем аксиом со статистикой
*					newSets - новые наборы особей
*					setsMask - маска на добавление новых особей
*	Returns:		-
*	Throws:			AxiomLibException - если входные параметры не согласованы
*	Author:			dk
*	History:
*
****************************************************************************/ 
inline void CrossoverTpl::addSets (std::vector <AxiomSetPlus> &newPop, const std::vector <AxiomSetPlus> &population, std::vector < std::vector <AxiomSetPlus> > &newSets, std::vector < std::vector <int> > &setsMask) const {
	// Проверка входных данных
	if (setsMask.size() != newSets.size())
		throw AxiomLibException("CrossoverTpl::addSets : internal error - vector and its mask of different size.");
	
	// Подсчет числа добавляемых элементов
	unsigned short toAdd = 0;
	for (unsigned int j = 0; j < setsMask.size(); j++) {
		if (setsMask[j].size() != newSets[j].size())
			throw AxiomLibException("CrossoverTpl::addSets : internal error - vector and its mask of different size.");
		for (unsigned short i = 0; i < setsMask[j].size(); i++) {
			if (setsMask[j][i] > 0)
				toAdd++;
		}
	}

	if (newPop.size() > 0)
		for (unsigned int i = 0; i < newPop.size(); i++)
			newPop[i].clear();

	unsigned short popSize = (unsigned short) population.size();
	newPop.resize(popSize + toAdd);
	for (unsigned short i = 0; i < popSize; i++)
		newPop[i] = population[i];
	toAdd = popSize;
	for (unsigned int j = 0; j < setsMask.size(); j++) {
		for (unsigned short i = 0; i < setsMask[j].size(); i++) {
			if (setsMask[j][i] > 0) {
				newPop[toAdd] = newSets[j][i];
				newPop[toAdd].clearStat();
				toAdd++;
			}
		}
	}
}

/****************************************************************************
*					CrossoverTpl::crossover
*
*	Description:	Функция скрещивания некоторых систем аксиом из вектора систем.
*					Выбор элементарных условий, аксиом и систем аксиом для 
*					скрещивания происходит в специально предназначенных 
*					для этого функциях данного класса.
*	Parameters:		axiomSetPop - популяция систем аксиом - набор систем аксиом, для которых требуется проводить скрещивание
*	Returns:		0 - если все произошло успешно
*					-1 - если скрещивание не прошло
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/ 
int CrossoverTpl::crossover (const std::vector <AxiomSetPlus> &population, std::vector <AxiomSetPlus> &toRet) {
	// Инициализация класса скрещивания - при этом создается вектор ссылок на аксиомы в системах аксиом
	initCrossover (population);

	// Скрещивание идет на нескольких уровнях:
	// 1) скрещивание на уросне систем аксиом
	// 2) скрещивание на уросне аксиом
	// 3) скрещивание на уросне элементарных условий

	// Создаем вектор для хранения новых особей популяциии маску для него
	std::vector < std::vector <AxiomSetPlus> > newSets;
	std::vector < std::vector <int> > setsMask;

	// так как 3 уровня скрещивания - для каждого свой вектор новых особей
	newSets.resize(3); 
	setsMask.resize(3);
	
	// 1) Скрещивание на уровне систем аксиом
	// Выбор пар систем аксиом для скрещивания
	std::vector <Pair> asPairs;
	//if (false) {
	if (choosePairs (asPairs, population) > 0) {
		// Скрещивание выбранных пар
		newSets[0].resize(asPairs.size());
		setsMask[0].resize(asPairs.size());
		for (unsigned int i = 0; i < asPairs.size(); i++) {
			setsMask[0][i] = crossAxiomSets (asPairs[i].first, asPairs[i].second, newSets[0][i]);
		}
	} else {
		// Если ни одна пара для секрещивания на уровне систем аксиом не выбрана
		newSets[0].clear();
		setsMask[0].clear();
	}
	
	// 2) Скрещивание на уровне аксиом
	// Выбор аксиом для скрещивания
	std::vector <Quad> axQuads;
	//if (false) {
	if (chooseAxQuads (axQuads, population) > 0) {
		newSets[1].resize(axQuads.size());
		setsMask[1].resize(axQuads.size());
		// Скрещивание выбранных аксиом
		for (unsigned int j = 0; j < axQuads.size(); j++) {
			setsMask[1][j] = crossAxioms ( axQuads[j].first, axQuads[j].fax, axQuads[j].second, axQuads[j].sax, newSets[1][j]);
		}
	}
	else {
		// Если ни одна пара для секрещивания на уровне аксиом не выбрана
		newSets[1].clear();
		setsMask[1].clear();
	}

	// 3) Скрещивание на уровне элементарных условий
	// Выбор аксиом, элементарные условия из которых будут скрещены
	std::vector <QuadPlus> ecQuads;
	//if (false) {
	if (chooseECQuads (ecQuads, population) > 0) {
		newSets[2].resize(ecQuads.size());
		setsMask[2].resize(ecQuads.size());
		// Скрещивание на уровне элементарых условий выбранных аксиом
		for (unsigned int k = 0; k < ecQuads.size(); k++) {
			setsMask[2][k] = crossECs ( (ecQuads[k]).first, (ecQuads[k]).fax, (ecQuads[k]).second, (ecQuads[k]).sax, (ecQuads[k]).ecName, newSets[2][k]);
		}
	}
	else {
		// Если ни одна пара для секрещивания на уровне элементарных условий не выбрана
		newSets[2].clear();
		setsMask[2].clear();
	}
	
	// Добавление всех созданных элементов в популяцию
	addSets (toRet, population, newSets, setsMask);
	
	/*toRet.resize (population.size() + 1);
	for (unsigned int i = 0; i < population.size(); i++)
		toRet[i] = population[i];
	toRet[population.size()] = population[0];
	*/
	return 0;
}
