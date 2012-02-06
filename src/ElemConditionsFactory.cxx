/****************************************************************************
*
*			Функции класса ElemConditionsFactory
*
****************************************************************************/

#include <iostream>

#include "ElemConditionsFactory.h"
// тут вставляются все includes элементарных аксиом из каталога axioms
#include "all_axioms.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

// Рекомендованное максимальное число элементарных условий в одной конъюнкции в составе системы аксиом
#define	recomendedMax		7

#define max(a,b)	(((a) >= (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define min3(a,b,c)	(min(min(a,b),c))

/****************************************************************************
*			ElemConditionsFactory::ElemConditionsFactory
*
*	Description:	Конструктор, заполняет список аксиом всеми аксиомами
*	Parameters:	-
*	Returns:	-
*	Throws:		-
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/
ElemConditionsFactory::ElemConditionsFactory (void)
{
  all_axioms.resize(N_EL_AXIOMS);
// BEGIN CREATE_ALL_AXIOMS
  all_axioms[3] = new DecreaseRightElemCondition();
  all_axioms[4] = new SecDerivativeDecreaseLeft();
  all_axioms[5] = new DerivativeDecreaseLeft();
  all_axioms[6] = new BetweenTwoLevels();
  all_axioms[7] = new Volatility();
  all_axioms[8] = new SecDerivativeIncreaseLeft();
  all_axioms[9] = new SecDerivativeDecreaseRight();
  all_axioms[10] = new DerivativeDecreaseRight();
  all_axioms[11] = new DerivativeIncreaseRight();
  all_axioms[12] = new DerivativeIncreaseLeft();
  all_axioms[13] = new SecDerivativeIncreaseRight();
  all_axioms[0] = new Integral();
  all_axioms[14] = new IncreaseRightElemCondition();
  all_axioms[1] = new DecreaseLeftElemCondition();
  all_axioms[2] = new IncreaseLeftElemCondition();
// END CREATE_ALL_AXIOMS

	// Установка значений матрицы смежности элементарных условий
	compatibilityMatrix.resize(N_EL_AXIOMS);
	for (int i = 0; i < N_EL_AXIOMS; i++) {
		compatibilityMatrix[i].resize(N_EL_AXIOMS);
	}
	for (int i = 0; i < N_EL_AXIOMS; i++) {
		for (int j = i+1; j < N_EL_AXIOMS; j++) {
			if ( ( all_axioms[i]->checkAndCompatibility( *(all_axioms[j])) ) > 0 )
				compatibilityMatrix[i][j] = true;
			else 
				compatibilityMatrix[i][j] = false;
			compatibilityMatrix[j][i] = compatibilityMatrix[i][j];
		}
		compatibilityMatrix[i][i] = false;
	}
}

/****************************************************************************
*			ElemConditionsFactory::~ElemConditionsFactory
*
*	Description:	Деструктор, освобождает все аксиомы из списка
*	Parameters:	-
*	Returns:	-
*	Throws:		-
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/
ElemConditionsFactory::~ElemConditionsFactory (void)
{
  for (int i = 0; i < N_EL_AXIOMS; i++) {
    delete all_axioms[i];
  }
}

/****************************************************************************
*			ElemConditionsFactory::createAxiomByNum
*
*	Description:	Создание аксиомы с заданным номером (в сугубо внутреннем для данного билда смысле, номер как индекс вектора all_axioms + 1)
*	Parameters:	signed int num - номер аксиомы
*	Returns:		ElemCondition* - указатель на аксиому (реально будет <тип соответствующей аксиомы>*)
*				или NULL, если аксиомы с номером num не существует
*	Throws:		AxiomLibException - если номер out of range
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/
ElemCondition* ElemConditionsFactory::createAxiomByNum(signed int num)
{
  // проверка выхода номера аксиомы за границы
  if (num < 1 || num > (int) all_axioms.size()) {
    throw AxiomLibException("ElemConditionsFactory::createAxiomByNum : argument out of range.");
    return 0;
  }

//  el_axiom* to_return = (all_axioms[num])->create();
  ElemCondition* toReturn;
  switch (num) {
// BEGIN CREATE_ALL_CREATE_AXIOM_BY_NUM
    case 4:
      toReturn = new DecreaseRightElemCondition();
      break;
    case 5:
      toReturn = new SecDerivativeDecreaseLeft();
      break;
    case 6:
      toReturn = new DerivativeDecreaseLeft();
      break;
    case 7:
      toReturn = new BetweenTwoLevels();
      break;
    case 8:
      toReturn = new Volatility();
      break;
    case 9:
      toReturn = new SecDerivativeIncreaseLeft();
      break;
    case 10:
      toReturn = new SecDerivativeDecreaseRight();
      break;
    case 11:
      toReturn = new DerivativeDecreaseRight();
      break;
    case 12:
      toReturn = new DerivativeIncreaseRight();
      break;
    case 13:
      toReturn = new DerivativeIncreaseLeft();
      break;
    case 14:
      toReturn = new SecDerivativeIncreaseRight();
      break;
    case 1:
      toReturn = new Integral();
      break;
    case 15:
      toReturn = new IncreaseRightElemCondition();
      break;
    case 2:
      toReturn = new DecreaseLeftElemCondition();
      break;
    case 3:
      toReturn = new IncreaseLeftElemCondition();
      break;
// END CREATE_ALL_CREATE_AXIOM_BY_NUM
    default:
      toReturn = NULL;
  }
  
  return toReturn;
}


/****************************************************************************
*				ElemConditionsFactory::findMaxNum
*
*	Description:	Вспомогательная функция. Ищет максимальное число среди вектора положительных чисел и возвращает его.
*	Parameters:	Вектор целых чисел
*	Returns:		-1 - Если что-то не так
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int ElemConditionsFactory::findMaxNum (std::vector <int> &vec) {
	if (vec.size() == 0) {
		return -1;
	}
	if (vec.size() == 1) {
		return vec[0];
	}
	else {
		int toReturn = vec[0];
		for (unsigned int i = 0; i < vec.size(); i++) {
			if (vec[i] > toReturn) {
				toReturn = vec[i];
			}
		}
		return toReturn;
	}
	return -1;
}


/****************************************************************************
*				ElemConditionsFactory::compatTable
*
*	Description:	Вспомогательная функция.
*					Функция создает массив одинаковых по длине векторов номеров элементраных условий, совместимых между собой.
*					При этом длина всех векторов задается как параметр функции. Функция организована на поиск в ширь таких векторов.
*	Parameters:		prev - формируемый вектор номеров совеместимых элементарных условий
*					numOfEC - требуемое число совместимых элементарных условий в каждом из векторов
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ElemConditionsFactory::compatTable (std::vector < std::vector<int> >& prev, const int numOfEC) {
	// Определение первого элемента вектора
	int f =  (int) (N_EL_AXIOMS * ((double) rand() / (double) ((unsigned int) RAND_MAX + 1)) );
	std::vector <int> cur;
	cur.resize(1);
	cur[0] = f;
	if (numOfEC < 2) {
		prev.resize(1);
		prev[0] = cur;
	}
	// Если в векторе более одного элемента - то запускаем вспомогательную функцию формирования вектора номеров совместимых элементарных условий
	prev.resize(0);
	int size = 0;
	std::vector <bool> curComp;
	curComp = compatibilityMatrix[f];
	subproc (prev, size, cur, curComp, numOfEC);
	prev.resize(size);

	return 0;
}


/****************************************************************************
*			ElemConditionsFactory::andVector
*
*	Description:	Функция поэлементной операции конъюнкции для двух векторов одинаковой длины
*	Parameters:		whereVec   - вектор, в который помещать результаты
*					whatFirst  - первый вектор для операции "и"
*					whatSecond - второй вектор для операции "и"
*	Returns:		0
*	Throws:			AxiomLibException - если вектора разной длины
*	Author:			dk
*	History:
*
****************************************************************************/
int ElemConditionsFactory::andVector (std::vector <bool>& whereVec, const std::vector <bool>& whatFirst, const std::vector <bool>& whatSecond) {
	int sizeFirst = (int) whatFirst.size();
	if (sizeFirst != whatSecond.size()) 
		throw AxiomLibException("Error in ElemConditionsFactory::andVector: given vectors are of different lengths.");
	whereVec.resize (sizeFirst);
	for (int i = 0; i < sizeFirst; i++) 
		whereVec[i] = whatFirst[i] & whatSecond[i];
	return 0;
}


/****************************************************************************
*			ElemConditionsFactory::subproc
*
*	Description:	Функция рекурсивного поиска совместимого набора элементарных условий
*	Parameters:		prev - заполняемый массив векторов номеров совместимых между собой условий
*					prevLast - номер последнего элемента в векторе prev (используется для того чтобы изменять длину вектора примерно в 10 раз реже)
*					cur  - набор номеров совместимых элементарных условий
*					curComp  - вектор совместимости для набора cur
*					numOfEC  - требуемое число элементов в создаваемом векторе совместимых элементарных условий
*	Returns:		0 если все ok
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ElemConditionsFactory::subproc (std::vector < std::vector<int> >& prev, int &prevLast, std::vector <int>& cur, std::vector <bool> &curComp, const int numOfEC) {
	// Обработка завершения рекурсии
	if (cur.size() == numOfEC) {
		if (prevLast == prev.size())
			prev.resize(prevLast + 10);
		prev[prevLast] = cur;
		prevLast++;
		return 1;
	}
	// Обработка продолжения рекурсии
	std::vector <int> temp;
	temp = cur;
	temp.resize(temp.size() + 1);
	std::vector <bool> tempComp;
	for (unsigned int i = 0; i < N_EL_AXIOMS; i++) {
		if (curComp[i]) {
			temp[temp.size() - 1] = i;
			andVector (tempComp, compatibilityMatrix[i], curComp);
			subproc (prev, prevLast, temp, tempComp, numOfEC);
		}
	}
	return 0;
}


/****************************************************************************
*			ElemConditionsFactory::createAxiomByRand
*
*	Description:	Создание аксиомы случайным образом с заданным числом элементарных условий
*	Parameters:	toReturn - возвращаемый вектор элементраных условий, совместимых между собой
*			numOfEC - число элементраных условий в создаваемой аксиоме
*	Returns:	0 если все ok
*	Throws:		AxiomLibException - если numOfEC превосходит общее число доступных элементарных условий
*	Author:		dk,gevor
*	History:
*
****************************************************************************/
int  ElemConditionsFactory::createAxiomByRand(std::vector<ElemCondition* >& toReturn, const signed int numOfEC) {
    // проверка выхода параметра за границы
    if ((numOfEC > (int) all_axioms.size()) || (numOfEC < 1)) {
		throw AxiomLibException("Error in ElemConditionsFactory::createAxiomByRand : argument out of range.");
    }
    std::vector < std::vector<int> > vec; // как я понимаю, вектор, содержащий наборы попарно совместимых по условию "И" элементарный условий - gevor
    compatTable (vec, numOfEC);
    if (vec.size() > 0) { 
	  int i;
	  i = (int)(((double) rand() / (double) RAND_MAX) * ((double) vec.size() - 1.0));
//	  cout << "Randomly selecting axiom " << i << " of " << vec.size() << ".\n";
	  toReturn.resize(vec[i].size());
	  for (int j = 0; j < (int) vec[i].size(); j++) {
		  toReturn[j] = createAxiomByNum(vec[i][j] + 1);
	  }
    }
    else {
//	  throw AxiomLibException("Error in ElemConditionsFactory::createAxiomByRand : vec.size() = 0.");
	  toReturn.resize(0);
    }

    return 0;
}


/****************************************************************************
*					ElemConditionsFactory::createAxiomByRand
*
*	Description:	Создание аксиомы случайным образом с заданным числом элементарных условий
*	Parameters:		toReturn - возвращаемый вектор векторов элементраных условий, который описывает выражение над элементарными условиями
*					numOfEC - число элементраных условий в создаваемой аксиоме
*	Returns:		0 - если все ok
*	Throws:			AxiomLibException - если numOfEC превосходит общее число доступных элементарных условий
*	Author:			dk
*	History:
*
****************************************************************************/
int  ElemConditionsFactory::createAxiomByRand(std::vector< std::vector <int> > &toReturn, const signed int numOfEC) {
	// проверка выхода параметра за границы
    if (numOfEC < 1) {
		throw AxiomLibException("Error in ElemConditionsFactory::createAxiomByRand(II) : input argument out of range.");
    }
	// Определение необхоидмых переменных
    std::vector < std::vector<int> > vec; // вектор для хранения наборов попарно совместимых по условию "И" элементарный условий
	toReturn.clear();
	int curNumToInclude, curMaxToInclude, i;
	int toInclude = numOfEC;
	// Цикл - пока не включим в состав аксиомы заданное количество элементарных условий
	while (toInclude > 0) {
		//andECs = new std::vector <ElemCondPlus>;
		// В состав одной конъюнкции включаем не больше all_axioms.size() условий, так как по умолчанию у них параметры одинаковые
		curMaxToInclude = min3 (recomendedMax, toInclude, (int) all_axioms.size());
		// Случайно определяем - сколько условий разместить в данной конъюнкции
		curNumToInclude = (int)(((double) rand() / (double) RAND_MAX) * ((double) curMaxToInclude - 1.0) + 1.0);
		// Определяем номера совместимых элементарных условий
		vec.clear();
		compatTable (vec, curNumToInclude);
		// Пока не получим хотя бы один вектор совместимых условий - ослабляем требования
		while (vec.size() < 1) {
			curMaxToInclude--;
			if (curMaxToInclude < 1)
				throw AxiomLibException("Error in ElemConditionsFactory::createAxiomByRand(II) : internal error - not enough elementary conditions.");
			curNumToInclude = (int)(((double) rand() / (double) RAND_MAX) * ((double) curMaxToInclude - 1.0) + 1.0);
			compatTable (vec, curNumToInclude);
		}
		// Определяем какой из векторов совместимых условий использовать
		i = (int)(((double) rand() / (double) RAND_MAX) * ((double) vec.size() - 1.0));
		// Заполняем вектор, описывающий одну конъюнкцию
		toReturn.push_back (vec[i]);
		toInclude -= vec[i].size();
	}
    return 0;
}


/****************************************************************************
*			ElemConditionsFactory::create_axiom_by_name
*
*	Description:	Создание аксиомы с заданным именем
*	Parameters:	const char* name - имя аксиомы
*	Returns:	el_axiom* - указатель на аксиому (реально будет <тип соответствующей аксиомы>*)
*			или NULL, если аксиомы с именем name не существует
*	Throws:		AxiomLibException - если имя не существует
*	Author:		Eugene A. Vasin
*	History:
*
****************************************************************************/
ElemCondition* ElemConditionsFactory::createAxiomByName(const char* name) {
	ElemCondition* toReturn = NULL;
	// ищем
	//  for(std::vector<el_axiom*>::iterator i = all_axioms.begin(); i != all_axioms.end(); i++)
	for(int i = 0; i < N_EL_AXIOMS; i++)
	{
		if (all_axioms[i]->name() == name) {
	//      to_return = (*i)->create();
		toReturn = createAxiomByNum(i+1);
		}
	}
	
	if (toReturn == NULL) {
		//имя которого найти не можем
		throw AxiomLibException("ElemConditionsFactory::createAxiomByName : invalid name.");
	}
	
	return toReturn;
}


