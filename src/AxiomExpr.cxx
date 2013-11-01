/***************************************************************************
*					Функции класса AxiomExpr.h
*
*	Description:	Аксиома в виде выражения от элементарных условий - Реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include <algorithm>

#include "AxiomExpr.h"

using namespace AxiomLib;

// Вероятность, что какому-то элементарному условию при случайном создании аксиомы будет преставлен знак отрицания
#define double_false_sign_prabability	0.2

// Пустой конструктор - с заданием имени аксиомы по умолчанию
AxiomExpr::AxiomExpr () {
	isVoting = false;
	votingThreshold = 2;
	setDefaultName();
	expression.clear();
	nameOfECBank.assign ("no_name_Elem_Cond_Bank");
}


// пустой конструктор с заданием имени аксиомы во входном параметре
AxiomExpr::AxiomExpr (const std::string sName) {
	votingThreshold = 2;
	isVoting = false;
	this->nameOfAxiomExpr.assign (sName);
	expression.clear();
	nameOfECBank.assign ("no_name_Elem_Cond_Bank");
}

AxiomExpr::AxiomExpr(const ElemCondPlus &ec)
{
	isVoting = false;
	votingThreshold = 2;
	expression.resize(1);
	expression[0].resize(1);
	expression[0][0] = ec;
}


// Деструктор класса - удаляет все динамические созданные объекты
AxiomExpr::~AxiomExpr(void) {
	for (unsigned int i = 0; i < expression.size(); i++) {
		for (unsigned int j = 0; j < expression[i].size(); j++)
			expression[i][j].clear();
	}
}


// Устанавливает дефолтное имя для экземпляра класса
signed int AxiomExpr::setDefaultName (void) {
	this->nameOfAxiomExpr.assign ("no_name");
	return 0;
}


// Название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
std::string AxiomExpr::name (void) const {
	return this->nameOfAxiomExpr;
}


// Функция возвращает текущеее число элемнтарных условий в данной аксиоме
signed int AxiomExpr::getNumOfEC (void) const {
	return size();
}


// Функция возвращает текущеее число элемнтарных условий в данной аксиоме
signed int AxiomExpr::size(void) const {
	int count = 0;
	for (unsigned int i = 0; i < expression.size(); i++) {
		count += expression[i].size();
	}
	return count;
}


/****************************************************************************
*					AxiomExpr::clear
*
*	Description:	Функция очищает содержимое аксиомы - удаляет все входящие
*					элементарные условия и обнуляет структуру аксиомы.
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::clear (void) {
	for (unsigned int i = 0; i < expression.size(); i++) {
		for (unsigned int j = 0; j < expression[i].size(); j++)
			expression[i][j].clear();
		expression[i].clear();
	}
	expression.clear();
	return 0;
}


/****************************************************************************
*					AxiomExpr::check 
*
*	Description:	Проверяет, размечается ли данной сложной аксиомой точка k ряда ts
*	Parameters:		k - точка в ряду для которой проверяется выполнимость аксиомы
*					ts - времененной ряд
*	Returns:		1  - если размечается
*					0  если не размечается
*					-1  если данная сложная аксиома не применима
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::check (const unsigned long k, const std::vector<double>& ts) const {
	if(!isVoting) {
		int res = 1;
		for (unsigned int i = 0; i < expression.size(); i++) {
			res = 1;
			for (unsigned int j = 0; j < expression[i].size(); j++){
				res *= expression[i][j].check(k,ts);
				if (res <= 0) {
					break;
				}
			}
			if (res > 0) {
				break;
			}
		}
		return res;
	} else {
		if(voters.empty()) {
			return -1;
		}

		unsigned voteCount = 0;

		for(unsigned i = 0; i < voters.size(); ++i) {
			int currentResult = voters[i].check(k, ts);
			if(currentResult == 1) {
				++voteCount;
			} else if (currentResult == -1) {
				return -1;
			}
		}

		return voteCount >= votingThreshold ? 1 : 0;
	}
}


/****************************************************************************
*					AxiomExpr::check 
*
*	Description:	Проверяет, размечается ли данной сложной аксиомой точка k ряда ts
*	Parameters:		k - точка в ряду для которой проверяется выполнимость аксиомы
*					ts - многомерный времененной ряд
*	Returns:		1  - если размечается
*					0  если не размечается
*					-1  если данная сложная аксиома не применима
*	Throws:			AxiomLibException - если входные параметры не согласованны
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::check (const unsigned long k, const std::vector < std::vector<double> >& ts) const {
	if(!isVoting) {
		int res = 1;
		for (unsigned int i = 0; i < expression.size(); i++) {
			res = 1;
			for (unsigned int j = 0; j < expression[i].size(); j++){
				if  ((expression[i][j].dimension < 0) || (expression[i][j].dimension >= (int) ts.size()))
					throw AxiomLibException("Error in AxiomExpr::check : wrong input parameters.");
				res *= expression[i][j].check(k, ts[expression[i][j].dimension]);
				if (res <= 0) {
					break;
				}
			}
			if (res > 0) {
				return res;
			}
		}
		return res;
	} else {
		if(voters.empty()) {
			return -1;
		}

		unsigned voteCount = 0;

		for(unsigned i = 0; i < voters.size(); ++i) {
			int currentResult = voters[i].check(k, ts);
			if(currentResult == 1) {
				++voteCount;
			} else if (currentResult == -1) {
				return -1;
			}
		}

		return voteCount >= votingThreshold ? 1 : 0;
	}
}


/****************************************************************************
*					AxiomExpr::operator<
*
*	Description:	Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой.
*					По дефолту, сравнение - четкое
*	Parameters:		second - Вторая сравниваемая аксиома
*	Returns:		Действительное число от -1 до 1:
*					Чем ближе к 1 - тем с большей уверенностью можно сказать,
*					что первая аксиома строго валожена во вторую (т.е. первая меньше второй)
*					Если -1 - то вторая меньше первой
*					Если 0 - то аксиомы либо совпадают, либо не сравнимы между собой.
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double AxiomExpr::operator< (const AxiomExpr& second) {
	double endres, res, curres, tmpres;
	// Проверка крайних случаев
	if (this->size() == 0)  {
		return -1;
	}
	if (second.size() == 0) {
		return 1;
	}

	endres = 0;
	for (unsigned int i1 = 0; i1 < this->expression.size(); i1++) {
		curres = -2;
		for (unsigned int j1 = 0; j1 < second.expression.size(); j1++) {
			//if (second.expression[j1].size() != this->expression[i1].size())
			//	continue;
			res = 0;
			curres = -2;
			for (unsigned int i2 = 0; i2 < this->expression[i1].size(); i2++) {
				curres = -2;
				for (unsigned int j2 = 0; j2 < second.expression[j1].size(); j2++) {
					if (this->expression[i1][i2].elemCondition->name() != second.expression[j1][j2].elemCondition->name())
						continue;
					tmpres = (*(this->expression[i1][i2].elemCondition)) < *(second.expression[j1][j2].elemCondition);
					if (tmpres > curres) {
						curres = tmpres;
					}
				}
				if (curres == -2)
					break;
				res += curres;
			}
			if (curres == -2)
				continue;
			endres += res;
			break;
		}
		if (curres == -2)
			return 0;
	}

	endres = endres / (double) this->size();
	return endres;
}


/****************************************************************************
*				AxiomExpr::operator=
*
*	Description:	Оператор присваивания
*	Parameters:	Присваиваемая аксиома
*	Returns:		*this
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
//AxiomExpr & AxiomExpr::operator= (const AxiomExpr& second) {
//	// Очищение содержимого данной аксиомы
//	this->clear();
//	// копирование названия аксиомы и используемого банка элементарных условий
//	nameOfAxiomExpr = second.name();
//	this->nameOfECBank = second.nameOfECBank;
//	// копирвание содержимых элементарных условий
//	this->expression.resize (second.expression.size());
//	for (unsigned int i = 0; i < second.expression.size(); i++) {
//		this->expression[i].resize (second.expression[i].size());
//		for (unsigned int j = 0; j < second.expression[i].size(); j++)
//			this->expression[i][j] = second.expression[i][j];
//	}
//	return *this;
//}


/****************************************************************************
*				AxiomExpr::checkWeakEquivalence
*
*	Description:	Проверка (слабой) структурной идентичности двух аксиом:
*					множества элементарных условий совпадают.
*	Parameters:		const AxiomExpr& second - аксиома, с которой надо проводить сравнение
*	Returns:		0 если структурной эквивалентности нет
*					1 если структурная эквивалентность есть
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::checkWeakEquivalence(const AxiomExpr& second) const {
	bool flag; 
	// Проверка граничных случаев
	if ((this->expression.size() == 0) && (second.expression.size() == 0))
		return 1;
	if ((this->expression.size() == 0) || (second.expression.size() == 0))
		return 0;
	// Сравнение структуры в общем случае
	// Для каждой конъюнкции ЭУ в первой аксиоме ищем соответствие во второй аксиоме
	for (unsigned int i1 = 0; i1 < this->expression.size(); i1++) {
		flag = false;
		for (unsigned int j1 = 0; j1 < second.expression.size(); j1++) {
			flag = false;
			for (unsigned int i2 = 0; i2 < this->expression[i1].size(); i2++) {
				flag = false;
				for (unsigned int j2 = 0; j2 < second.expression[j1].size(); j2++) {
					if (this->expression[i1][i2].elemCondition->name() == second.expression[j1][j2].elemCondition->name()) {
						flag = true;
						break;
					}
				}
				if (!flag)
					break;
			}
			if (flag)
				break;
		}
		if (!flag)
			return 0;
	}
	// Поиск соответствия в обратную сторону:
	// Для каждой конъюнкции ЭУ во второй аксиоме ищем соответствие в первой аксиоме
	for (unsigned int j1 = 0; j1 < second.expression.size(); j1++) {
		flag = false;
		for (unsigned int i1 = 0; i1 < this->expression.size(); i1++) {
			flag = false;
			for (unsigned int j2 = 0; j2 < second.expression[j1].size(); j2++) {
				flag = false;
				for (unsigned int i2 = 0; i2 < this->expression[i1].size(); i2++) {
					if (this->expression[i1][i2].elemCondition->name() == second.expression[j1][j2].elemCondition->name()) {
						flag = true;
						break;
					}
				}
				if (!flag)
					break;
			}
			if (flag)
				break;
		}
		if (!flag)
			return 0;
	}
	return 1;
}


/****************************************************************************
*					AxiomExpr::operator&
*
*	Description:	Слитие аксиом по оператору И
*	Parameters:		Вторая аксиома, учавствующая в операции объединения аксиом
*	Returns:		AxiomExpr - новая аксиома, как объединение *this и second
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
AxiomExpr AxiomExpr::operator& (const AxiomExpr& second) const {
	AxiomExpr result;
	result.andExpr (*this, second);
	return result;
}


/****************************************************************************
*					AxiomExpr::operator|
*
*	Description:	Слитие аксиом по оператору ИЛИ
*	Parameters:		Вторая аксиома, учавствующая в операции объединения аксиом
*	Returns:		AxiomExpr - новая аксиома, как объединение *this и second
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
AxiomExpr AxiomExpr::operator| (const AxiomExpr& second) const {
	AxiomExpr result;
	result.orExpr (*this, second);
	return result;
}

AxiomExpr &AxiomExpr::operator &=(const AxiomExpr &second)
{
	AxiomExpr first(*this);
	andExpr(first, second);
	return *this;
}

AxiomExpr &AxiomExpr::operator |=(const AxiomExpr &second)
{
	AxiomExpr first(*this);
	orExpr(first, second);
	return *this;
}


/****************************************************************************
*					AxiomExpr::andExpr
*
*	Description:	Слитие аксиом по оператору И в данную аксиому
*	Parameters:		first - первая аксиома, учавствующая в операции объединения аксиом
*					second - вторая аксиома, учавствующая в операции объединения аксиом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::andExpr (const AxiomExpr& first, const AxiomExpr& second) {
	int numDisjunts, numDisjunts1, numDisjunts2;
	this->clear();
	numDisjunts1 = first.expression.size();
	numDisjunts2 = second.expression.size();
	numDisjunts = numDisjunts1*numDisjunts2;
	if (numDisjunts < 1) {
		return orExpr(first, second);
	}
	this->expression.resize(numDisjunts);
	unsigned int curLen;
	unsigned int k = 0;
	for (int i = 0; i < numDisjunts1; i++) {
		curLen = first.expression[i].size();
		for (int j = 0; j < numDisjunts2; j++, k++) {
			this->expression[k].resize (curLen + second.expression[j].size());
			for (unsigned int l = 0; l < curLen; l++)
				this->expression[k][l] = first.expression[i][l];
			for (unsigned int l = 0; l < second.expression[j].size(); l++)
				this->expression[k][curLen + l] = second.expression[j][l];
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomExpr::orExpr
*
*	Description:	Слитие аксиом по оператору ИЛИ в данную аксиому
*	Parameters:		first - первая аксиома, учавствующая в операции объединения аксиом
*					second - вторая аксиома, учавствующая в операции объединения аксиом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::orExpr (const AxiomExpr& first, const AxiomExpr& second) {
	int numDisjunts, numDisjunts1, numDisjunts2;
	this->clear();
	numDisjunts1 = first.expression.size();
	numDisjunts2 = second.expression.size();
	numDisjunts = numDisjunts1 + numDisjunts2;
	if (numDisjunts < 1)
		return 0;
	this->expression.resize(numDisjunts);
	for (int i = 0; i < numDisjunts1; i++) {
		this->expression[i].resize (first.expression[i].size());
		for (unsigned int l = 0; l < first.expression[i].size(); l++)
			this->expression[i][l] = first.expression[i][l];
	}
	for (int j = 0; j < numDisjunts2; j++) {
		this->expression[numDisjunts1 + j].resize (second.expression[j].size());
		for (unsigned int l = 0; l < second.expression[j].size(); l++)
			this->expression[numDisjunts1 + j][l] = second.expression[j][l];
	}
	return 0;
}

/****************************************************************************
*					AxiomExpr::getDimensions
*
*	Description:	Вычисление размерностей датасета, необходимых для данной
*					аксиомы
*	Parameters:		v - сюда записывается true на место размерностей, которые
*					используются. если размер вектора недостаточен, то он не
*					увеличивается (т. е. нужно задавать правильный размер заранее)
*	Returns:		-
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
void AxiomExpr::getDimensions(std::vector<bool> &v) const {
	std::fill(v.begin(), v.end(), false);
	
	for(unsigned int i = 0; i < expression.size(); i++) {
		for(unsigned int j = 0; j < expression[i].size(); j++) {
			const ElemCondPlus& ecPlus = expression[i][j];			
			
			if((int) v.size() > ecPlus.dimension) {
				v[ecPlus.dimension] = true;
			}
		}
	}
}

/****************************************************************************
*					AxiomExpr::initAxiomFromStructure
*
*	Description:	Функция задающая аксиому по специализированной структуре с описанием акисомы
*	Parameters:		aes - структура с описанием аксиомы
*	Returns:		0
*	Throws:			AxiomLibException - если имя одного из элементарных условий указано неверно
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomFromStructure (AxiomExprStructure &aes) {
	this->clear();
	ElemConditionsFactory ecf;
	if (aes.axiomName.size() > 0)
		this->nameOfAxiomExpr = aes.axiomName;
	if (aes.elemCondBankName.size() > 0)
		this->nameOfECBank = aes.elemCondBankName;
	this->expression.resize (aes.elemConds.size());
	for (unsigned int i = 0; i < aes.elemConds.size(); i++) {
		this->expression[i].resize (aes.elemConds[i].size());
		for (unsigned int j = 0; j < aes.elemConds[i].size(); j++) {
			this->expression[i][j].initFromStructure (aes.elemConds[i][j]);
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomExpr::initAxiomByRand
*
*	Description:	Функция задает данную аксиому как выражение над элементарными условиями, выбранными случайным образом.
*					Реализауия предполагает обращение к более низкому уровню - уровню создания элементарных условий -
*					и получение случайно выбранного набора совместимых элементарных условий,
*					число элементарных условий при этом задается как параметр
*	Parameters:		numOfEC - число элементарных условий, которые будут входить в данную аксиому
*	Returns:		-1 - Если  создать не удалось
*					 0 - Если все прошло нормально
*	Throws:			AxiomLibExeption - если невозможно подобрать numOfEC совместимых элементарных условий
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomByRand (const int numOfEC) {
  	return initAxiomByRand(numOfEC, -1, -1);
}


/****************************************************************************
*					AxiomExpr::initAxiomByRand
*
*	Description:	Функция задает данную аксиому как выражение над элементарными условиями, выбранными случайным образом.
*					Для каждого условие случайно определяется размерность набора данных, на которой его проверять, из заданного диапазна.
*					Реализауия предполагает обращение к более низкому уровню - уровню создания элементарных условий -
*					и получение случайно выбранного набора совместимых элементарных условий,
*					число элементарных условий при этом задается как параметр
*	Parameters:		numOfEC - число элементарных условий, которые будут входить в данную аксиому
*					dimensionMin - минимальное значение для размерности исходных данных
*					dimensionMax - максимальное значение для размерности исходных данных
*	Returns:		-1 - Если  создать не удалось
*					 0 - Если все прошло нормально
*	Throws:			AxiomLibExeption - если невозможно подобрать numOfEC совместимых элементарных условий
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomByRand (const int numOfEC, const int dimensionMin, const int dimensionMax) {
	// Проверка входных параметров - устанавливаем ли значение для dimension
	bool defineDimension = true;
	if ((dimensionMax < dimensionMin) || (dimensionMin < 0))
		defineDimension = false;
	// Очищаем содержимое данной аксиомы
	this->clear();
	ElemConditionsFactory ecf;
	std::vector <std::vector <int> > numECs;
	// Определение - какие жлементарные условия и в каком порядке включать в состав аксиомы
	ecf.createAxiomByRand(numECs, numOfEC);
	this->expression.resize (numECs.size());
	for (unsigned int i = 0; i < numECs.size(); i++) {
		this->expression[i].resize (numECs[i].size());
		for (unsigned int j = 0; j < numECs[i].size(); j++) {
			// Задание выражения над элементарными условиями
			this->expression[i][j].elemCondition = ecf.createAxiomByNum (numECs[i][j] + 1);
			// Случайное определение знака с заданием по умолчанию значения true
			this->expression[i][j].sign = true;
			if ( ( (double) rand() / (double) RAND_MAX ) < double_false_sign_prabability )
				this->expression[i][j].sign = false;
			// устанавливаем случайное значение размерности исходных данных, на которой проверять элементарное условие
			if (defineDimension)
				this->expression[i][j].dimension = dimensionMin + (int) (((double) rand() / ((double) RAND_MAX + 1.0) ) * (double) (dimensionMax - dimensionMin + 1));
		}
	}
  	return 0;
}


/****************************************************************************
*					AxiomExpr::createAxiomExprStructure
*
*	Description:	Функция сохраняет структуру выбранной аксиомы в AxiomExprStructures
*	Parameters:		aes - заполняемое поле структуры аксиомы
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::createAxiomExprStructure (AxiomExprStructure &aes) const {
	// очистка заполняемой структуры аксиомы
	aes.clear();
	// заполение полей структуры
	aes.axiomName = name();
	aes.elemCondBankName = nameOfECBank;
	aes.elemConds.resize (expression.size());
	for (unsigned int i = 0; i < expression.size(); i++) {
		aes.elemConds[i].resize(expression[i].size());
		for (unsigned int j = 0; j < expression[i].size(); j++) {
			expression[i][j].createStructure (aes.elemConds[i][j]);
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomExpr::initAxiomFromFile
*
*	Description:	Функция задания аксиомы из файла
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания аксиом
*					AxiomSetName - имя файла с описанием аксиомы
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomFromFile (const std::string baseDir, const std::string axiomName) {
	AxiomExprStructure aes;
	AxiomBase axiomBase;
	std::string fileName;
	// создание полного пути к файлу
	fileName = baseDir;
	fileName.append ("/");
	fileName.append (axiomName);
	// считывание описания в структуру
	axiomBase.readFromAX(fileName, aes);
	// инициализация из структуры
	initAxiomFromStructure(aes);
	return 0;
}


/****************************************************************************
*					AxiomExpr::initAxiomFromFile
*
*	Description:	Функция задания аксиомы из файла
*					(с указанием размерности набора данных, на которой проверяется каждое элементарное условие из данной аксиомы)
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания аксиом
*					AxiomSetName - имя файла с описанием аксиомы
*					paramNames - вектор парамтеров набора данных - для указания размерности,
*						на которой проверять каждое элементарное условие из аксиомы
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomFromFile (const std::string baseDir, const std::string axiomName, const std::vector <std::string> &paramNames) {
	AxiomExprStructure aes;
	AxiomBase axiomBase;
	std::string fileName;
	// создание полного пути к файлу
	fileName = baseDir;
	fileName.append ("/");
	fileName.append (axiomName);
	// считывание описания в структуру
	axiomBase.readFromAX(fileName, aes, paramNames);
	// инициализация из структуры
	initAxiomFromStructure(aes);
	return 0;
}


/****************************************************************************
*					AxiomExpr::saveAxiomToFile
*
*	Description:	Функция сохранения аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания аксиом
*					AxiomSetName - имя, под которым сохрянять данную аксиом
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::saveAxiomToFile (const std::string baseDir, const std::string axiomName) const {
	AxiomExprStructure aes;
	createAxiomExprStructure (aes);
	aes.axiomName = axiomName;
	AxiomBase axiomBase;
	axiomBase.saveToAX (baseDir, aes);
	return 0;
}


/****************************************************************************
*					AxiomExpr::saveAxiomToFile
*
*	Description:	Функция сохранения аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания аксиом
*					AxiomSetName - имя, под которым сохрянять данную аксиом
*					paramNames - вектор парамтеров набора данных - для указания размерности,
*						на которой проверять каждое элементарное условие из аксиомы
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::saveAxiomToFile (const std::string baseDir, const std::string axiomName, std::vector <std::string> &paramNames) const {
	AxiomExprStructure aes;
	createAxiomExprStructure (aes);
	aes.axiomName = axiomName;
	AxiomBase axiomBase;
	axiomBase.saveToAX (baseDir, aes, paramNames);
	return 0;
}

bool AxiomExpr::setVoting(bool value)
{
	isVoting = value;
}

void AxiomExpr::setVotingThreshold(unsigned value)
{
	votingThreshold = value;
}

void AxiomExpr::addVoter(const AxiomExpr &a)
{
	voters.push_back(a);
}

void AxiomExpr::clearVoters()
{
	voters.clear();
}

void AxiomExpr::print(std::ostream &ostr) const
{
	if(!isVoting) {
		ostr << "(";
		for(int i = 0; i < expression.size(); ++i) {
			for(int j = 0; j  < expression[i].size(); ++j) {
				if(!expression[i][j].sign) {
					ostr << "!";
				}
				ostr << expression[i][j].ecTypeName();
				ostr << "&";
			}
			ostr << "|";
		}
		ostr << ")";
	} else {
		ostr << "vote " << votingThreshold << " (";
		for(unsigned i = 0; i < voters.size(); ++i) {
			voters[i].print(ostr);
			ostr << ", ";
		}
		ostr << ")";
	}
}
