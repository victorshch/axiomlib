/****************************************************************************
*			Функции класса Axiom.h
*
*	Description:	Конъюнкция элементарных аксиом - Реализация
*	Author:		dk
*	History:	
*
****************************************************************************/
#include "Axiom.h"

using namespace AxiomLib;
using namespace std;

// Пустой конструктор - с заданием имени аксиоы как no_name
Axiom::Axiom () {
	//this->nameOfAxiom.resize(7);
	this->nameOfAxiom = std::string("no_name");
	elemConditions.resize(0);
	// Установка значений атрибутов класса по умолчанию
	minNumOfEC = 1;
	maxNumOfEC = 7;
}

// пустой конструктор с заданием имени аксиомы во входном параметре
Axiom::Axiom (const std::string sName) {
	this->nameOfAxiom.resize(sName.size());
	this->nameOfAxiom = sName;
	elemConditions.resize(0);
	// Установка значений атрибутов класса по умолчанию
	minNumOfEC = 1;
	maxNumOfEC = 7;
}

// Деструктор класса - удаляет все динамические созданные объекты
Axiom::~Axiom(void) {
	for (unsigned int i = 0; i < this->elemConditions.size(); i++) {
		delete(this->elemConditions[i]);
	}
}
  
// Название аксиомы (одинаковое независимо от параметров, используется для задания библиотек аксиом)
std::string Axiom::name (void) const {
	return this->nameOfAxiom;
}

// Функция устанавливает значения атрибутов класса
signed int Axiom::setAttrValue (const unsigned int min, const unsigned int max) {
	// Проверка корректности параметров
	if ( max < min ) 
		throw AxiomLibException("Error in Axiom::initAxiomByNums : cannot creat elementary condition with given number.");
	minNumOfEC = min;
	maxNumOfEC = max;
	return 0;
}

// Функция возвращает значения атрибутов класса
signed int Axiom::getAttrValue (unsigned int &min, unsigned int &max) {
	// Проверка корректности параметров
	min = minNumOfEC;
	max = maxNumOfEC;
	return 0;
}

// Функция возвращает число элементарных условий в данной аксиоме
//int Axiom::getNumOfEC (void) const {
//	return elemConditions.size();
//}
  
// Функция устанавливает значение определенного параметра определенного элементарного условия
signed int Axiom::setParamValue (const double param, const int ecNum, const std::string &paramName) {
	// Проверка корректности параметров
	if ( (ecNum >= 0) && (ecNum < (int) elemConditions.size()) ) {
		return ( (elemConditions[ecNum])->setParamValue(param, paramName) );
	}
	return -1;
}
     
// Функция возвращает значение определенного параметра определенного элементарного условия
signed int Axiom::getParamValue (double &param, const int ecNum, const std::string &paramName) const {
	// Проверка корректности параметров
	if ( (ecNum >= 0) && (ecNum < (int) elemConditions.size()) ) {
		return ( (elemConditions[ecNum])->getParamValue(param, paramName) );
	}
	return -1;
}


/****************************************************************************
*				Axiom::clear
*
*	Description:	Функция удаляет все динамически созданные элементарные условия
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::clear (void) {
	for (unsigned int i = 0; i < elemConditions.size(); i++) {
		delete (elemConditions[i]);
	}
	elemConditions.resize(0);
	return 0;
}


/****************************************************************************
*				Axiom::check 
*
*	Description:	Проверяет, размечается ли данной сложной аксиомой точка k ряда ts
*	Parameters:	k - точка в ряду для которой проверяется выполнимость аксиомы
*				ts - времененной ряд
*	Returns:		1  - если размечается
*				0  если не размечается
*				-1  если данная сложная аксиома не применима
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::check (const unsigned long k, const std::vector<double>& ts) {
	int res = 1;
	for (unsigned int i = 0; i < this->elemConditions.size(); i++) {
		res = this->elemConditions[i]->check(k,ts);
		if (res != 1) {
			break;
		}
	}  
	return res;
}
  

/****************************************************************************
*				Axiom::initAxiomByNums
*
*	Description:	Задает данную аксиому как конъюнкцию элементарных аксиом с номерами, заданными в векторе axiom_nums. 
*				Номера соответствуют позиции элемнтарного условия в массие всех используемых элементарных условий.
*				Эта штука используется для создания "случайных" аксиом. 
*				При создании используются конструкторы элементарных условий без параметров
*	Parameters:	axiomNums - вектор номеров соответсвующих элементарным условиям, которые будут добавлены в аксиому.
*	Returns:		0 - если все выполнено успешно, в противном случае - выход из функции по throw
*	Throws:		AxiomLibException -  1)если не удалось создать некоторое из элементарных условий, перечисленных в списке
*								2)если перчисленные во входном параметре аксиомы не совместимы
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::initAxiomByNums (const std::vector<signed int>& axiomNums) {
	ElemConditionsFactory ecf;
	
	elemConditions.resize(axiomNums.size());
	for (unsigned int i = 0; i < axiomNums.size(); i++) {
		// Создание элементарного условия
		elemConditions[i] = ecf.createAxiomByNum(axiomNums[i]);
		if (elemConditions[i] == NULL) {
			//std::cerr << "Wrong number of elementary condition in Axiom::init AxiomByNums\n";
			throw AxiomLibException("Error in Axiom::initAxiomByNums : cannot creat elementary condition with given number.");
			//exit(1);
		}
		// Проверка совместимости созданного элементарного условия с уже присутствующими в аксиоме
		for (unsigned int j=0; j < i; j++) {
			if (elemConditions[i]->checkAndCompatibility(*(elemConditions[j])) != 1) {
				//std::cerr << "Incompatible axioms in Axiom::init AxiomByNums\n";
				throw AxiomLibException("Error in Axiom::initAxiomByNums : incompatible elementary conditions.");
				return 1;
			}
		}
	}
	
	return 0;
}

/****************************************************************************
*				Axiom::initAxiomByNames
*
*	Description:	Задает данную аксиому как конъюнкцию элементарных аксиом с именами, перечисленными в векторе axiom_nums. 
*				Эта штука используется для создания экспертом аксиом. При создании используются конструкторы элементарных условий  без параметров
*	Parameters:	axiomNums - вектор номеров соответсвующих элементарным условиям, которые будут добавлены в аксиому.
*	Returns:		0 - если все выполнено успешно, в противном случае - выход из функции по throw
*	Throws:		AxiomLibException -  1)если не удалось создать некоторое из элементарных условий, перечисленных в списке
*								2)если перчисленные во входном параметре аксиомы не совместимы
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::initAxiomByNames (const std::vector<std::string>& axiomNames) {
	ElemConditionsFactory ecf;
	
	elemConditions.resize(axiomNames.size());
	for (unsigned int i = 0; i < axiomNames.size(); i++) {
		//Создание элементарного условия
		elemConditions[i] = ecf.createAxiomByName(axiomNames[i].c_str());
		if (elemConditions[i] == NULL) {
			//std::cerr << "Wrong number of elementary condition in Axiom::init AxiomByNames\n";
			throw AxiomLibException("Error in Axiom::initAxiomByNames : cannot creat elementary condition with given name.");
			//exit(1);
		}
		//Проверка совместимости созданного элементарного условия с уже существующими в данной аксиоме
		for (unsigned int j=0; j < i; j++) {
			if (elemConditions[i]->checkAndCompatibility(*(elemConditions[j])) != 1) {
				//std::cerr << "Incompatible axioms in Axiom::init AxiomByNames\n";
				throw AxiomLibException("Error in Axiom::initAxiomByNames : incompatible elementary conditions.");
				return 1;
			}
		}
	}
	
	return 0;
}


/****************************************************************************
*				Axiom::initAxiomByRand
*
*	Description:	Функция задает данную аксиому как конъюнкцию элементарных условий выбранных случайным образом
*				Реализауия предполагает обращение к более низкому уровню - уровню создания элементарных условий - 
*				и получение случайно выбранного набора совместимых элементарных условий, 
*				число элементарных условий при этом задается как параметр
*	Parameters:	numOfEC - число элементарных условий, которые будут входить в данную аксиому
*	Returns:		-1 - Если  создать не удалось
*				 0 - Если все прошло нормально
*	Throws:		AxiomLibExeption - если невозможно подобрать numOfEC совместимых элементарных условий 
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::initAxiomByRand (const int numOfEC) {
	ElemConditionsFactory ecf;

	ecf.createAxiomByRand(elemConditions, numOfEC);

  	return 0;
}


/****************************************************************************
*				Axiom::setECParameters
*
*	Description:	Инициализация параметров элементарных условий, входящих в аксиому
*	Parameters:	setECParameters - задает отображение из имен элементарныз условий в "низкое отображение"
*				"низкое отображение" - это отобрадение из имен параметров элементарных условий в значения этих параметров
*				Значения параметров задаются в виде строк, для удобства ( если это числа то они переводятся функцией atof() )
*	Returns:		0  - если хотя бы один парамтр был изменен
*				-2 - в ecParams не нашлось ни одного подхдяшего отображения
*	Throws:		AxiomLibExeption - если отображение ecParams пусто
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::setECParameters (const std::map<std::string, std::map<std::string, std::string> >& ecParams) {
	signed int toReturn=-2;

	// проверяем, пустая ли карта?
	if (ecParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in Axiom::setECParameters : zero parameter map.");
	}

	// общий метод: ходим в цикле, выставляя параметры
	// при этом, не проверяется, был ли уже выставлен такой параметр в этом цикле, или нет
	for (std::map<std::string, std::map<std::string, std::string> >::const_iterator i = ecParams.begin(); i != ecParams.end(); i++) {
		std::string ecName = i->first;
		std::map<std::string, std::string> paramMap = i->second;

		for (unsigned int j = 0; j < this->elemConditions.size(); j++) {
			// Ищем, какому параметру соответствует i
			if (ecName == this->elemConditions[j]->name()) {
				// ok, нашли, находим value
				toReturn = 0;
				this->elemConditions[j]->setParams(paramMap);
			}
		}
	}

  	return toReturn;
}
  

/****************************************************************************
*				Axiom::getECParameters
*
*	Description:	Функция возвращает параметры акиомы
*	Parameters:	ecParams - заполняется - задает отображение из имен элементарныз условий в "низкое отображение"
*				"низкое отображение" - это отобрадение из имен параметров элементарных условий в значения этих параметров
*				Значения параметров задаются в виде строк, для удобства ( если это числа то они переводятся функцией atof() )
*	Returns:		0 
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::getECParameters (std::map <std::string, std::map <std::string, std::string> > &ecParams) const {
	ecParams.clear();
	std::string curEC;
	for (unsigned int j = 0; j < this->elemConditions.size(); j++) {
		curEC = this->elemConditions[j]->name();
		std::map <std::string, std::string> newMap;
		(this->elemConditions[j])->getParams(newMap);
		ecParams[curEC] = newMap;
	}
	return 0;    
}


/****************************************************************************
*				Axiom::getECParamNames
*
*	Description:	Функция заполняет входной вектор именами параметров элементарного 
*				условия с заданным номером в векторе элементарных условий
*	Parameters:	ecNum - номер элементарного условия, о котором запрашивается информация
*				ecParamNames - заполняемый вектор строк с названиями параметров
*	Returns:		0
*	Throws:		AxiomLibException - если номер указанного элементарного условия 
*				выходит за границы числа элементарных условий данной аксиомы
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::getECParamNames (int ecNum, std::vector<std::string> &ecParamNames) const {
	if ((ecNum >= 0) && (ecNum < (int) elemConditions.size())) {
		return ( (elemConditions[ecNum])->getECParamNames (ecParamNames) );
	}
	else {
		throw AxiomLibException("Error in Axiom::getECParamNames : given ecNum out of range.");
	}
	return -1;
}


/****************************************************************************
*				Axiom::getECNumByName
*
*	Description:	Функция возвращает номер элементарного условия в векторе условий по его имени
*	Parameters:	ecName - имя элементарного условия
*	Returns:		>= 0 - если эелементарное условие с заданным именем нашлось
*				< 0 - если элементарного условия с заданным именем не нашлось
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::getECNumByName (std::string &ecName) const {
	for (unsigned int i = 0; i < elemConditions.size(); i++) {
		if ( ecName == (elemConditions[i])->name() ) {
			return i;
		}
	}
	return -1;
}


/****************************************************************************
*				Axiom::getECNames
*
*	Description:	Функция заполняет входной вектор именами элементарных условий данной аксиомы
*	Parameters:	ecParamNames - заполняемый вектор строк с названиями элементарных условий
*	Returns:		положительное целое - число элементов вектора
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
 signed int Axiom::getECNames(std::vector<std::string> &ecNames) const {
 	signed int toReturn;
	toReturn = (int) elemConditions.size();
 	ecNames.resize(toReturn);
	for (int i = 0; i < toReturn; i++) {
		ecNames[i] = (elemConditions[i])->name();
	}
 	return 0;
 }


 /****************************************************************************
*					Axiom::createAxiomStructure
*
*	Description:	Функция сохраняет структуру данной системы аксиом в AxiomStructure
*	Parameters:		as - структура, в которую записывает описание акисомы
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int Axiom::createAxiomStructure (AxiomStructure &as) const {
	// Заполняем поля стуктуры
	as.axiomName = this->nameOfAxiom;
	this->getECParameters (as.axiomParameters);	
	return 0;
}


 /****************************************************************************
*				Axiom::transmute
*
*	Description:	Мутация на уровне параметров элементраных условий, входящих в состав аксиомы (ectp)
*				и на уровне структуры самой аксиомы (параметр atp)
*				Мутация на уровне элементарного условия определяет (как правило) некоторое изменение параметров
*				Мутация на уровне аксиомы пердполагает замену, удаление или добавлние некоторого случано выбранного элементарного условия
*	Parameters:	ectp - параметр определяет степень мутации на уровне элементарных условий - значения параметра обязаны варьироваться то 0 до 1
*				atp  - параметр определяет степень мутации на уровне аксиомы - значения параметра обязаны варьироваться то 0 до 1
*	Returns:		0 - если мутация прошла успешно, в противном случае - если что-то идет не так - мутации просто не происходит
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Axiom::transmute (const double ectp, const double atp) { 
	ElemConditionsFactory ecf;
	int flagOfConset = 0; //используется как флаг совместимости

	//Пробегаем по каждому элементарному условию данной аксиомы и случайно выбираем - мутировать или нет
	for (unsigned int i = 0; i < this->elemConditions.size(); i++) {
		if ((double) rand() / (double) RAND_MAX  <  atp) {
			//данное элементарное условие выбрано для мутации на уровне аксиомы - то есть происходит его замена
			std::vector <ElemCondition* > compatibleEC;
			compatibleEC.resize(0);
			ElemCondition* tempUrl;
			// Создание массива совместимых элементарных условий - то есть условий, на которые моно заменить выбранное для мутации
			for (int k = 1; k <= N_EL_AXIOMS; k++) {
				tempUrl = ecf.createAxiomByNum(k);
				int flag = 0; //флаг - выбора совместимых элеменарных условий
				for (unsigned int j = 0; j < this->elemConditions.size(); j++) {
					if (i!=j) {
						if (this->elemConditions[j]->checkAndCompatibility(*tempUrl) != 1) {
							flag = 1;
							break;
						}
					}
				}
				if ((flag == 0) && (this->elemConditions.size() != 1)) {
					compatibleEC.push_back(tempUrl);
				}
			}
			// теперь в compatibleEC хранятся все условия, на которые может быть заменено выбранное для мутации элментарное условие
			int doNotDel = (int) compatibleEC.size();//здесь будет  хранится элементарное условие на которе произойдет замена выбранного для мутации
			//замена - если возможно - выбранного для мутации элементарного условия на случайно выбранное из списка совместимых
			if (compatibleEC.size() != 0) {
				int cngEC = round(((double) rand() / (double) RAND_MAX ) * ((double) compatibleEC.size() - 1.0));
				doNotDel = cngEC;
				this->elemConditions[i] = compatibleEC[cngEC];
			}
			else {
				flagOfConset = 1;
				if (this->elemConditions.size() > minNumOfEC) {
					delete (this->elemConditions[i]);
					for (unsigned int temp = (i+1); temp < this->elemConditions.size(); temp++) {
						this->elemConditions[temp-1] = this->elemConditions[temp];
					}
					this->elemConditions.pop_back();
				}
				else {
					//заменить выбранное элементарное условие  не удалось - поэтому /*лишний раз*/ запускаем для него мутацию
					this->elemConditions[i]->transmute(ectp);
				}
			}
			//удаление не использованных элементарных условий
			for (unsigned int del = 0; del < compatibleEC.size(); del++) {
				if (del != doNotDel) {
					delete(compatibleEC[del]);
				}
			}			
		}
	}
	
	//теперь пробуем добавить еще одно элементарое условие к аксиоме (часть мутации)
	if (((double) rand() / (double) RAND_MAX  <  atp) && ((flagOfConset == 0) || (this->elemConditions.size() == 1))) {
		if (this->elemConditions.size() < maxNumOfEC) {
			std::vector <ElemCondition* > compatibleEC;
			compatibleEC.resize(0);
			ElemCondition* tempUrl;
			//создадим список элементарных условий, которые возможно было бы добавить к уже присутствующим в аксиоме
			for (int k = 1; k <= N_EL_AXIOMS; k++) {
				tempUrl = ecf.createAxiomByNum(k);
				int flag = 0;
				for (unsigned int j = 0; j < this->elemConditions.size(); j++) {
					if (this->elemConditions[j]->checkAndCompatibility(*tempUrl) != 1) {
						flag = 1;
						break;
					}
				}
				if (flag == 0) {
					compatibleEC.push_back(tempUrl);
				}
			}
			//список совместимых элем условие создан - добавляем случайное из них
			int doNotDel = (int) compatibleEC.size();
			if (compatibleEC.size() > 0) {
				int addEC = round(((double) rand() / (double) RAND_MAX ) * ( (double) compatibleEC.size() - 1.0));
				this->elemConditions.push_back(compatibleEC[addEC]);
				doNotDel = addEC;
			} 
			for (unsigned int del = 0; del < compatibleEC.size(); del++) {
				if (del != doNotDel) {
					delete(compatibleEC[del]);
				}
			}
		}
	}
  
	//запуск мутации на уровне элементарных условий
	for (unsigned int i=0; i < this->elemConditions.size(); i++ ){
		this->elemConditions[i]->transmute(ectp);
	}
	
  	return 0;
}


/****************************************************************************
*				Axiom::operator<
*
*	Description:	Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой.
*				По дефолту, сравнение - четкое
*	Parameters:	-
*	Returns:		Действительное число от 0 до 1 - Чем ближе к 1 - тем с большей уверенностью можно сказать, 
*				что первая аксиома строго валожена во вторую
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double Axiom::operator< (const Axiom& second) {
	double res, curres, tmpres;
	if (this->elemConditions.size() == 0)  {
		return 0;
	}
	if ((this->elemConditions.size() != 0) && (second.elemConditions.size() == 0)) {
		return 1;
	}
	res = 0;
	for (unsigned int i = 0; i < this->elemConditions.size(); i++) {
		curres = 0.0;
		tmpres = 0.0;
		for (unsigned int j = 0; j < second.elemConditions.size(); j++) {
			if (this->elemConditions[i]->name() == second.elemConditions[j]->name()) {
				tmpres = (*(this->elemConditions[i])) < *(second.elemConditions[j]);
				if (tmpres > curres) {
					curres = tmpres;
				}
			}
		}
		if (curres == 0.0) {
			return 0;
		}
		else {
			res += curres;
		}
	}
	res = res / (double) this->elemConditions.size();	
	return res;
};


/****************************************************************************
*				Axiom::operator=
*
*	Description:	Оператор присваивания
*	Parameters:	Присваиваемая аксиома
*	Returns:		*this
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
Axiom & Axiom::operator= (const Axiom& second) {
	ElemConditionsFactory ecf;
	// копирование названия аксиомы
	nameOfAxiom = second.name();
	// копирвание элементарных условий путем создания таких же с такими же параметрами
	std::map <std::string, std::map <std::string, std::string> > ecParams;
	second.getECParameters (ecParams);
	int num;
	std::vector<std::string> ecNames;
	second.getECNames(ecNames);
	num = (int) ecNames.size();
	elemConditions.resize (num);
	for (int i = 0; i < num; i++) {
		elemConditions[i] = ecf.createAxiomByName( (ecNames[i]).c_str() );
	}
	setECParameters (ecParams);
	return *this;
}

Axiom::Axiom(const Axiom &other)
{
	*this = other;
}


/****************************************************************************
*				Axiom::checkWeakEquivalence
*
*	Description:	Проверка (слабой) структурной идентичности двух аксиом:
*			множества элементарных условий совпадают. В силу того,
*			что пока допустима только связка "И", проверку
*			связок не делаем.
*	Parameters:	onst Axiom& second - аксиома, с которой надо проводить сравнение
*	Returns:	0 если структурной эквивалентности нет
*			1 если структурная эквивалентность есть
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int Axiom::checkWeakEquivalence(const Axiom& second) const
{
  // проверка на количество элементарных условий
  if (elemConditions.size() != second.elemConditions.size()) {
    return 0;
  }
  
  // получаем списки названий элементарных условий
  vector<string> thisNames;
  vector<string> secondNames;
  getECNames(thisNames);
  second.getECNames(secondNames);

  // сортируем имена
  //sort(thisNames.begin(), thisNames.end());
  //sort(secondNames.begin(), secondNames.end());
  
  // сравниваем
  if (thisNames == secondNames)
    return 1;
  else
    return 0;
}
