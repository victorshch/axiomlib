/****************************************************************************
*				CrossoverTry.cxx
*
*	Description:	Функции класса CrossoverTry - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "CrossoverTry.h"

using namespace AxiomLib;


/****************************************************************************
*					CrossoverTry::CrossoverTry
*
*	Description:	Стандартный конструкор
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
CrossoverTry::CrossoverTry (void) {
	// Задание значений параметров скрещивания по умолчанию (отсутствие скрещивания)
	elemCondCrossLevel = 0.0;
	axiomCrossLevel = 0.0;
	axiomSetCrossLevel = 0.0;
}


/****************************************************************************
*					CrossoverTry::name
*
*	Description:	Возвращает имя данного класса
*	Parameters:	-
*	Returns:		std::string - имя данного класса
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string CrossoverTry::name (void) const {
	std::string toReturn("CrossoverTry");
	return toReturn;
}


/****************************************************************************
*                       CrossovercTry::setParamsFromEnv
*
*       Description:	Инициализировать параметры данными из env
*       Parameters:	const Environment& env - среда
*       Returns:		0
*       Throws:		AxiomLibException - если нет какого-топараметра
*       Author:		dk
*       History:
*
****************************************************************************/
int CrossoverTry::setParamsFromEnv (const Environment& env) {
	double allCrossLevel;
	if (env.getDoubleParamValue(allCrossLevel, "crossLevel") < 0.0) {
		env.getDoubleParamValue(elemCondCrossLevel, "elemCondCrossLevel");
		if ( (elemCondCrossLevel < 0.0) || (elemCondCrossLevel > 1.0) ) 
			throw AxiomLibException("CrossoverTry::setParamsFromEnv : elemCondCrossLevel isn't set in config file or elemCondCrossLevel is out of range.");
			
		env.getDoubleParamValue(axiomCrossLevel, "axiomCrossLevel");
		if ( (axiomCrossLevel < 0.0) || (axiomCrossLevel > 1.0) ) 
			throw AxiomLibException("CrossoverTry::setParamsFromEnv : axiomCrossLevel isn't set in config file or axiomCrossLevel is out of range.");
			
		env.getDoubleParamValue(axiomSetCrossLevel, "axiomSetCrossLevel");
		if ( (axiomSetCrossLevel < 0.0) || (axiomSetCrossLevel > 1.0) ) 
			throw AxiomLibException("CrossoverTry::setParamsFromEnv : axiomSetCrossLevel isn't set in config file or axiomSetCrossLevel is out of range.");
	}
	else {
		if ( (allCrossLevel < 0.0) || (allCrossLevel > 1.0) ) 
			throw AxiomLibException("CrossoverTry::setParamsFromEnv : crossLevel isn't set in config file or crossLevel is out of range.");
		// Если не заданы степени мутации по отдельности для каждого уровня мутации, а задан единый уровень - то его и присваиваем каждому из уровней
		elemCondCrossLevel = allCrossLevel;
		axiomCrossLevel = allCrossLevel;
		axiomSetCrossLevel = allCrossLevel;
	}
	return 0;
}


/****************************************************************************
*				CrossoverTry::setParams
*
*	Description:	Функция задания значений параметров 
*	Parameters:	params - отображение из строки с названием параметра класса в устанавливаемое значение этого параметра в виде строки
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int CrossoverTry::setParams (std::map<std::string, std::string> &params) {
	std::map<std::string, std::string>::iterator iter;
	double value;
	for (iter = params.begin(); iter != params.end(); ++iter) {
		if ((*iter).first == "crossLevel") {
			value = atof ( (*iter).second.c_str() );
			if ( (value < 0.0) || (value > 1.0) ) 
				throw AxiomLibException("CrossoverTry::setParams : crossLevel is out of range.");
			// Если не заданы степени мутации по отдельности для каждого уровня мутации, а задан единый уровень - то его и присваиваем каждому из уровней
			elemCondCrossLevel = value;
			axiomCrossLevel = value;
			axiomSetCrossLevel = value;
			break;
		}
		
		if ((*iter).first == "elemCondCrossLevel") {
			value = atof ( (*iter).second.c_str() );
			if ( (value < 0.0) || (value > 1.0) ) 
				throw AxiomLibException("CrossoverTry::setParams : elemCondCrossLevel is out of range.");
			elemCondCrossLevel = value;
			continue;
		}
		
		if ((*iter).first == "axiomCrossLevel") {
			value = atof ( (*iter).second.c_str() );
			if ( (value < 0.0) || (value > 1.0) ) 
				throw AxiomLibException("CrossoverTry::setParams : axiomCrossLevel is out of range.");
			axiomCrossLevel = value;
			continue;
		}
		
		if ((*iter).first == "axiomSetCrossLevel") {
			value = atof ( (*iter).second.c_str() );
			if ( (value < 0.0) || (value > 1.0) ) 
				throw AxiomLibException("CrossoverTry::setParams : axiomSetCrossLevel is out of range.");
			axiomSetCrossLevel = value;
			continue;
		}
			
		// значит в отобразении указан какой - то неизвестный параметр - выдаем исключение так как это может быть важно
		throw AxiomLibException ("Error in CrossoverTry::setParams: in config - too many params for CrossoverTry Class.");
	}
	return 0;
}


/****************************************************************************
*				CrossoverTry::setParams
*
*	Description:	Функция задания значений параметров 
*	Parameters:	новые параметры скрещивания:
*				newElemCondCrossLevel - велечина скрещивания на уровне элементарных условий 
*				newAxiomCrossLevel - велечина скрещивания на уровне аксиом
*				newAxiomSetCrossLevel - велечина скрещивания на уровне систем аксиом
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int CrossoverTry::setParams (const double newElemCondCrossLevel, const double newAxiomCrossLevel, const double newAxiomSetCrossLevel) {
	// Проверка на соответствие граничным значениям параметров
	if (	(newElemCondCrossLevel < 0.0) || (newElemCondCrossLevel > 1.0) ||
		(newAxiomCrossLevel < 0.0) || (newAxiomCrossLevel > 1.0) ||
		(newAxiomSetCrossLevel < 0.0) || (newAxiomSetCrossLevel > 1.0) 	) 
		throw AxiomLibException ("Error in CrossoverTry::setParams: some of crossLevel Parameters are out of range.");
	// Установка новых значений параметров
	elemCondCrossLevel = newElemCondCrossLevel;
	axiomCrossLevel = newAxiomCrossLevel;
	axiomSetCrossLevel = newAxiomSetCrossLevel;	
	return 0;
}


/****************************************************************************
*				CrossoverTry::setParams
*
*	Description:	Функция задания значений параметров 
*	Parameters:	новые параметры скрещивания:
*				newCrossLevel - велечина скрещивания на всех уровнях
*	Returns:		0
*	Throws:		AxiomLibException - если входные значения параметров некорректное
*	Author:		dk
*	History:
*
****************************************************************************/
int CrossoverTry::setParams (const double newCrossLevel) {
	// Проверка на соответствие граничным значениям параметров
	if ( (newCrossLevel < 0.0) || (newCrossLevel > 1.0) ) 
		throw AxiomLibException ("Error in CrossoverTry::setParams: given crossLevel is out of range.");
	// Установка новых значений параметров
	elemCondCrossLevel = newCrossLevel;
	axiomCrossLevel = newCrossLevel;
	axiomSetCrossLevel = newCrossLevel;
	return 0;
}


/****************************************************************************
*				CrossoverTry::choosePairs
*
*	Description:	Функция выбора пар систем аксиом для скрещивания на 
*				уровне систем аксиом. Выбор происходит по рандому 
*				из всего набора систем аксиом. Число выбранных пар 
*				пропорционально параметру скрещивания axiomSetCrossLevel.
*	Parameters:	asPairs - вектор пар систем аксиом для скрещивания (заполняется в данной функцие)
*	Returns:		0 - если ни одной пары систем аксиом не было выбранно
*				1 - если была выбранна хотя бы одна пара систем аксиом для скрещивания
*	Throws:		- 
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::choosePairs (std::vector <Pair> &asPairs) {

	// проверка на достаочное число систем аксиом
	if (asPop->size() < 2) return 0;
	
	// выбор пар для скрещивания
	// Подготовка внутренних вспомогательных переменных
	Pair tmpPair;
	int sec;
	int flag = 0;
	// пробегаем по всему набору систем аксиом и определяем какие из систем аксиом будут чавствовать в скрещвании
	for (int i = 0; i < (int) asPop->size(); i++) {
		// для каждой системы аксиом случайным образом с заданным параметром отсечения определяем будет ли она учавствовать в скрещивании
		if (( (double) rand() / (double) RAND_MAX ) < axiomSetCrossLevel) {
			sec = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) asPop->size() - 2.0) );
			// выбрали системы аксиом - заполняем структуру пары и дополняем ее в asPairs
			if (sec >= i) sec++;
			tmpPair.first = i;
			tmpPair.second = sec;
			asPairs.push_back(tmpPair);
			flag = 1;
		}
	}
	return flag;
}


/****************************************************************************
*				CrossoverTry::crossAxiomSets
*
*	Description:	Функция скрещивания двух выбранных систем аксиом на 
*				уровне систем аксиом. Скрещивание происходит путем 
*				создания новой системы аксиом из аксиом составляющих
*				две системы, выбранные для скрещивания.
*	Parameters:	axSet1 - указатель на вектор аксиом из первой системы аксиом, выбранной для скрещивания
*				axSet2 - указатель на вектор аксиом из второй системы аксиом, выбранной для скрещивания
*				axiomSet - система аксиом , которая может быть создана при скрещивании
*	Returns:		1 - если все произошле успешно, была создана новая система аксиом
*				0 - если все произошло успешно, но новой системы аксиом не было создано
*				-1 - если скрещивание не прошло
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::crossAxiomSets (std::vector<Axiom *>* axSet1, std::vector<Axiom *>* axSet2, AxiomSetWithStats &axiomSet) {
	// ВАЖНО - Перенести задание данного параметра в environment - уровень - по которому 
	// определяетя - последоватьльность аксиом в системе:
	// то есть когда выбрали аксиомы которые будут составлять новую систему аксиом их сортировка ведется по оператрору "<" с уровнем отсечения в levelOfSub
	double  levelOfSub = 0.55;
	// Подготовка внутренних переменных функции
	int newSet1, newSet2; 
	std::vector<Axiom *> toInclude;
	//AxiomSetWithStats axiomSet;
	std::vector<int> mask;
	int curNum;
	// определение числа аксиом из каждой системы, которые будут входить во вновь созданную систему аксиом
	// (в новую систему будут входить от 1/3 до 2/3 аксиом из каждой из систем)
	newSet1 = (int) ( ( ( (double) rand() / (double) RAND_MAX ) * ( (double) axSet1->size() - 1.0) / 3 ) + ( (double) axSet1->size() - 1.0) / 3);
	newSet2 = (int) ( ( ( (double) rand() / (double) RAND_MAX ) * ( (double) axSet2->size() - 1.0) / 3 ) + ( (double) axSet2->size() - 1.0) / 3);
	if ( (newSet1 <= 0) && (newSet2 <= 0) ) return 0;
	// Набор аксиом, которые будут использоваться
	toInclude.resize( newSet1 + newSet2 );
	// Определение - какие аксиомы из первой системы аксиом будут входить во вновь создаваемую систему аксиом
	// Обнуление вектора маски - используется для препятствия вхождения в систему аксиом - одинаковых аксиом
	mask.resize (axSet1->size());
	for (unsigned int t = 0; t < mask.size(); t++) {
		mask[t] = 0;
	}
	for (int i = 0; i < newSet1; i++) {
		curNum = (int) ( ( (double) rand() / (double) RAND_MAX ) * (double) (axSet1->size() - i - 1) );
		while (mask[curNum] == 1) {
			curNum++;
		}
		mask[curNum] = 1;
		toInclude[i] = (*axSet1)[curNum];
	}
	// Определение - какие аксиомы из второй системы аксиом будут входить во вновь создаваемую систему аксиом
	// Обнуление вектора маски - используется для препятствия вхождения в систему аксиом - одинаковых аксиом
	mask.resize ( axSet2->size() );
	for (unsigned int t = 0; t < mask.size(); t++) {
		mask[t] = 0;
	}
	for (int i = 0; i < newSet2; i++) {
		curNum = (int) ( ( (double) rand() / (double) RAND_MAX ) * (double) (axSet2->size() - i - 1) );
		while (mask[curNum] == 1) {
			curNum++;
		}
		mask[curNum] = 1;
		toInclude[newSet1 + i] = (*axSet2)[curNum];
	}
	// инициализация системы аксиом путем копирования выбранных аксиом
	axiomSet.initAxiomSetByCopy (toInclude, levelOfSub);
	return 1;
}


/****************************************************************************
*				CrossoverTry::chooseAxQuads
*
*	Description:	Функция выбора пар аксиом для скрещивания на уровне аксиом.
*				Выбор происходит по рандому из всего набора аксиом. Число 
*				выбранных пар пропорционально параметру скрещивания axiomCrossLevel.
*	Parameters:	axQuads - вектор из описаний Quad - какие аксиомы скрещивать
*	Returns:		0 - если не выбрали какие аксиомы скрещивать
*				1 - если выбрали
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::chooseAxQuads (std::vector <Quad> &axQuads) {
	// Подготовка внутрених переменных функции
	int tmpDiv;
	int fax, second, sax;
	double average;
	Quad tmpQuad;
	int flag = 0;
	// Подсчет среднего числа аксиом в системе аксиом в популяции
	average = 0.0;
	for (unsigned int j = 0; j < axiomSets.size(); j++) {
		average += (double) (axiomSets[j])->size();
	}
	if (axiomSets.size() < 1) return 0;
	average = average / (double) axiomSets.size();
	// пробегаем по всем аксиомам в популяции и случайно определяем будет ли использоваться аксиома в скрещивании
	for (int i = 0; i < (int) axiomSets.size(); i++) {
		tmpDiv = (int) ( 1.0 + (double) (axiomSets[i])->size() / average );
		for (int y = 0; y < tmpDiv; y++) {
			if (( (double) rand() / (double) RAND_MAX ) < axiomCrossLevel) {
				fax = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (axiomSets[i])->size() - 1.0) );
				second = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) axiomSets.size() - 1.0) );
				sax = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (axiomSets[second])->size() - 1.0) );
				// Определили тех, кто используется в скрещивании - заполняем соответствующуюс структуру и дополняем вектор axQuads
				tmpQuad.first = i;
				tmpQuad.fax = fax;
				tmpQuad.second = second;
				tmpQuad.sax = sax;
				axQuads.push_back(tmpQuad);
				flag = 1;
			}
		}
	}
	
	return flag;
}


/****************************************************************************
*				CrossoverTry::crossAxioms
*
*	Description:	Функция скрещивания двух выбранных аксиом 
*				на уровне аксиом. В данной реализации скрещивание 
*				предполагает обмен элементарными условиями из аксиом.
*				Выбор элементарных условий для обмена основывается на 
*				совместимости с элементарными условиями во вновь 
*				полученных аксиомах.
*	Parameters:	ax1 - указатель на первую выбранную для скрещивания систему аксиом
*				ax2 - указатель на вторую выбранную для скрещивания систему аксиом
*				newAxiomSet - система аксиом , которая может быть создана при скрещивании
*	Returns:		1 - если все произошле успешно, была создана новая система аксиом
*				0 - если все произошло успешно, но не было создано новой системы аксиом
*				-1 - если скрещивание не прошло
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::crossAxioms (Axiom* ax1, Axiom* ax2, AxiomSetWithStats &newAxiomSet) {
	// Подготовка внутренних переменных класса
	Check checker;
	int numOfEc1, numOfEc2;
	int ch;
	Axiom* tmp;
	bool flag;
	int t;
	// Определение числа элементарных условий в каждой из аксиом
	numOfEc1 = ax1->getNumOfEC();
	numOfEc2 = ax2->getNumOfEC();
	// Сортировка и возможно переобозначение - необхожимо для того, чтобы упорядочить цикл - необязательный элемент, но введен в структурных целях
	if (numOfEc1 <= numOfEc2) {
		t = numOfEc2; 
		numOfEc2 = numOfEc1;
		numOfEc1 = t;
		tmp = ax2;
		ax2 = ax1;
		ax1 = tmp;
	}
	// поиск совместимых по обмену эелементарных условий - то есть таких, при обмене которыми получаются совместимые аксиомы (аксиомы из совместимых по and элементарных условий)
	for (int i = 0; i < numOfEc1; i++) {
		flag = true;
		ch = (int) (( (double) rand() / (double) RAND_MAX ) * ( (double) numOfEc2 - 1.0) );
		for (int j = 0; j < numOfEc2; j++) {
			if (j == ch) continue;
			if (checker.checkAndCompatibility (ax1, i, ax2, j) == 0 ) {
				// значит элемнтарные условия не совместимы 
				flag = false;
				break;
			}
		}
		if (flag) {
			for (int k = 0; k < numOfEc1; k++) {
				if (k == i) continue;
				if (checker.checkAndCompatibility (ax1, k, ax2, ch) == 0 ) {
					// значит элемнтарные условия не совместимы по обмену
					flag = false;
					break;
				}
			}
		}
		if (flag) {
			// обмен аксиомами возможен - производим его при одном дополнительном условии
			if ( ( (double) rand() / (double) RAND_MAX ) < axiomCrossLevel) {
				checker.change (ax1, i, ax2, ch);
			}
		}
	}
	return 0;
}


/****************************************************************************
*				CrossoverTry::chooseEcQuads
*
*	Description:	Функция выбора пар элементарных условий для скрещивания 
*				на уровне параметров элементарных усовий.
*				Выбор происходит по рандому из всего набора элементарных условий. Число 
*				выбранных пар пропорционально параметру скрещивания elemCondCrossLevel.
*	Parameters:	ecQuads - вектор описаний QuadPlus - описаний какие элементарные условия были выбранны для скрещивания
*	Returns:		0 - если не выбрали какие элементарные условия скрещивать
*				1 - если выбрали
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::chooseEcQuads (std::vector <QuadPlus> &ecQuads) {
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
	//int first;
	//int fax, second, sax;
	int ec1, ec2;
	int flag;
	QuadPlus tmpQuad;
	// предварительный этап - сбор информации о наличии элементарных условий в аксиомах - заполнение структуры ecMap
	for (int i = 0; i < (int) axiomSets.size(); i++) {
		for (int j = 0; j < (int) (axiomSets[i])->size(); j++) {
			ecNames.clear();
			(*(axiomSets[i]))[j]->getECNames(ecNames);
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
			// определяем - а будет ли скрещивание - используем параметр elemCondCrossLevel 
			if (( (double) rand() / (double) RAND_MAX ) < elemCondCrossLevel) {
				// выбор элементарных условий одного типа из разных аксиом
				ec1 = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (iter->second).size() - 1.0) );
				ec2 = (int) ( ( (double) rand() / (double) RAND_MAX ) * ((double) (iter->second).size() - 2.0) );
				if (ec2 >= ec1) ec2++;
				// заполнение структуры tmpQuad
				tmpQuad.first = ((iter->second)[ec1]).first;
				tmpQuad.fax = ((iter->second)[ec1]).second;
				tmpQuad.second = ((iter->second)[ec2]).first;
				tmpQuad.sax = ((iter->second)[ec2]).second;
				tmpQuad.ecName = iter->first;
				// запись заполненной структуры в вектор входных значений
				ecQuads.push_back(tmpQuad);
				flag = 1;
			}
		}
	}
	
	return flag;
}


/****************************************************************************
*				CrossoverTry::crossEcs
*
*	Description:	Скрещивание двух выбранных пар элементарных условий 
*				на уровне параметров элементаррных условий.
*				Скрещивание происходит путем усреднения параметров
*				одинаковых элементарных условий из разных аксиом.
*	Parameters:	ax1 - указатель на первую аксиому, из которой было выбранно элементарное условие для скрещивания
*				ax2 - указатель на вторую аксиому, из которой было выбранно элементарное условие для скрещивания
*				ecName - имя элементарного условия, которое было выбранно для скрещивания
*				newAxiomSet - система аксиом , которая может быть создана при скрещивании
*	Returns:		1 - если все произошле успешно, была создана новая система аксиом
*				0 - если все произошло успешно, никаких новых систем аксиом не было создано
*				-1 - если скрещивание не прошло
*	Throws:		AxiomLibExeption - если для скрещивания были выбранны несовместимые элементарные условия (разного типа)
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::crossEcs (Axiom* ax1, Axiom* ax2, std::string &ecName, AxiomSetWithStats &newAxiomSet) {
	// подготовка внутренних переменных
	int ecNum1, ecNum2;
	int curRes = -1;
	double param = 0.0;
	double param1 = 0.0;
	double param2 = 0.0;
	std::vector<std::string> ecParamNames;
	// получение числа элементарных условий в каждой из аксом - считываем чтобы по многу раз не обращаться к одной и той же функции в течение данной функции
	ecNum1 = ax1->getECNumByName (ecName);
	ecNum2 = ax2->getECNumByName (ecName);
	// проверка на корректность аксиом
	if ( (ecNum1 < 0) || (ecNum2 <0) )
		throw AxiomLibException("CrossoverTry::crossEcs : there is an error in generating pairs of elementary conditions to cross. ");
	// Проводим скрещивание значений параметров выбранного элементарного условия
	ax1->getECParamNames(ecNum1, ecParamNames);
	for (int j = 0; j < (int) ecParamNames.size(); j++) {
		curRes = ax1->getParamValue (param1, ecNum1, ecParamNames[j]);
		if ( (curRes == 0) && ( ( (double) rand() / (double) RAND_MAX ) < elemCondCrossLevel ) ) {
			// Если 0 - то все считалось правильно (в double) - проводим изменение параметров и записываем обратно
			// (для этого считываем второй параметр)
			curRes = ax2->getParamValue (param2, ecNum2, ecParamNames[j]);
			if (curRes != 0)
				throw AxiomLibException("CrossoverTry::crossEcs : there is an error in generating pairs of elementary conditions to cross, ECs are not of the same Class. ");
			param = (param1 + param2) / 2.0; // Проводим усреднение параметров - возможно заменить на произвольную функцию
			// Записываем обратно измененное значение парметра
			ax1->setParamValue (param, ecNum1, ecParamNames[j]);
			ax2->setParamValue (param, ecNum2, ecParamNames[j]);
		}
		else{
			// Если параметр не double - и к нему не приводится взаимооднозначно - такой случай можно добавить - пока без надобности
			
		}
	}
	return 0;
}


/****************************************************************************
*				CrossoverTry::crossover
*
*	Description:	Функция скрещивания некоторых систем аксиом из вектора систем.
*				Выбор элементарных условий, аксиом и систем аксиом для 
*				скрещивания происходит в специально предназначенных 
*				для этого функциях данного класса.
*	Parameters:	axiomSetPop - популяция систем аксиом - набор систем аксиом, для которых требуется проводить скрещивание
*	Returns:		0 - если все произошло успешно
*				-1 - если скрещивание не прошло
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
int CrossoverTry::crossover (AxiomSetPop &axiomSetPop) {
	// Инициализация класса скрещивания - при этом создается вектор ссылок на аксиомы в системах аксиом
	initCrossover (axiomSetPop);
	// Скрещивание идет на нескольких уровнях:
	// 1) скрещивание на уросне элементарных условий
	// 2) скрещивание на уросне аксиом
	// 3) скрещивание на уросне систем аксиом

	std::vector < std::vector <AxiomSetWithStats> > newSets;
	std::vector < std::vector <int> > setsMask;
	newSets.clear();
	setsMask.clear();
	newSets.resize(3);
	setsMask.resize(3);
	// 1) Скрещивание на уровне систем аксиом
	if (axiomSetCrossLevel > 0) {
		// Выбор пар систем аксиом для скрещивания
		std::vector <Pair> asPairs;
		if (choosePairs (asPairs)) {
			// Скрещивание выбранных пар
			newSets[0].resize(asPairs.size());
			setsMask[0].resize(asPairs.size());
			for (unsigned int i = 0; i < asPairs.size(); i++) {
				setsMask[0][i] = crossAxiomSets (axiomSets[asPairs[i].first], axiomSets[asPairs[i].second], newSets[0][i]);
			}
		}
		else {
			newSets[0].clear();
			setsMask[0].clear();
		}
	}
	
	// 2) Скрещивание на уровне аксиом
	if (axiomCrossLevel > 0) {
		// Выбор аксиом для скрещивания
		std::vector <Quad> axQuads;
		if (chooseAxQuads (axQuads)) {
			newSets[1].resize(axQuads.size());
			setsMask[1].resize(axQuads.size());
			// Скрещивание выбранных аксиом
			for (unsigned int j = 0; j < axQuads.size(); j++) {
				setsMask[1][j] = crossAxioms ( (*(axiomSets [ (axQuads[j]).first ] ))[(axQuads[j]).fax], (*(axiomSets[(axQuads[j]).second]))[(axQuads[j]).sax], newSets[1][j]);
			}
		}
		else {
			newSets[1].clear();
			setsMask[1].clear();
		}
	}
	// 3) Скрещивание на уровне элементарных условий
	if (elemCondCrossLevel > 0) {
		// Выбор аксиом элементарные условия из которых будут скрещены
		std::vector <QuadPlus> ecQuads;
		if (chooseEcQuads (ecQuads)) {
			newSets[2].resize(ecQuads.size());
			setsMask[2].resize(ecQuads.size());
			// Скрещивание на уровне элементарых условий выбранных аксиом
			for (unsigned int k = 0; k < ecQuads.size(); k++) {
				setsMask[2][k] = crossEcs ( (*(axiomSets[(ecQuads[k]).first]))[(ecQuads[k]).fax], (*(axiomSets[(ecQuads[k]).second]))[(ecQuads[k]).sax], (ecQuads[k]).ecName, newSets[2][k]);
			}
		}
		else {
			newSets[2].clear();
			setsMask[2].clear();
		}
	}
	
	axiomSetPop.addSets(newSets, setsMask);
	//std::cout << "\n 	NewSets were succesfully added. Press Enter, please.";
	//std::cin.get();
	
	return 0;
}
