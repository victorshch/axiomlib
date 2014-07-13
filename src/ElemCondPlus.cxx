//****************************************************************************
//*
//*			Функции класса ElemCondPlus
//*
//****************************************************************************/

#include "ElemCondPlus.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"

#include "prettyprint.hpp"

#include <boost/lexical_cast.hpp>
		
using namespace AxiomLib;



/****************************************************************************
*					ElemCondPlus::ElemCondPlus
*
*	Description:	Конструктор, заполняет поля класса по умолчанию
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
ElemCondPlus::ElemCondPlus(void) {
	elemCondition = NULL;
	sign = true;
	dimension = -1;
}

/****************************************************************************
*					ElemCondPlus::ElemCondPlus
*
*	Description:	Конструктор копирования
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
ElemCondPlus::ElemCondPlus(const ElemCondPlus &second)
{
   this->dimension = second.dimension;
   this->sign = second.sign;
   this->name = second.name;
   this->index = second.index;
   if (second.elemCondition != NULL) {
	   this->elemCondition = second.elemCondition->clone();
   } else {
	   this->elemCondition = NULL;
   }
}

ElemCondPlus ElemCondPlus::operator !() const
{
	ElemCondPlus result(*this);
	result.sign = !result.sign;
	return result;
}

/****************************************************************************
*					ElemCondPlus::ElemCondPlus
*
*	Description:	Декструктор, удаляет созданные динамические объекты класса
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
ElemCondPlus::~ElemCondPlus(void) {
	if (elemCondition != NULL)
		delete (elemCondition);
}


/****************************************************************************
*					ElemCondPlus::ecTypeName
*
*	Description:	Функция возвращает имя элементарного условия
*	Parameters:		-
*	Returns:		std::string - имя данного условия
*	Throws:			-
*	Author:			dk
*	History:		22.02.10 - wictor - переименовал из name в ecTypeName
*
****************************************************************************/
std::string ElemCondPlus::ecTypeName (void) const {
	if (elemCondition != NULL) {
		if(this->sign) {
			return elemCondition->name();
		} else {
			return "not " + elemCondition->name();
		}
	}
	throw AxiomLibException("Error in ElemCondPlus::ecTypeName : elementary condition is not defined.");
}

void ElemCondPlus::getDimensions(std::vector<bool> &v) const
{
	std::fill(v.begin(), v.end(), false);

	if((int) v.size() > dimension) {
		v[dimension] = true;
	}
}


/****************************************************************************
*					ElemCondPlus::check
*
*	Description:	Функция проверки выполнимости ЭУ с учетом знака отрицания, если он есть
*	Parameters:		k - номер отсчета в ряду, на котором проверять условие
*					x - ряд, на котором проверять условие
*	Returns:		1 - если условие выполняется
*					0 - если не выполняется
*					-1 - если не применимо
*	Throws:			AxiomLibException - если элементарное условие не определено
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::check (const unsigned long k, const std::vector<double>& x) const {
	signed int res;
	if (elemCondition == NULL)
		throw AxiomLibException("Error in ElemCondPlus::check : elementary condition is not defined.");
	res = elemCondition->check (k, x);
	if (res < 0) return res;
	if (sign) return res;
	if (res == 0) return 1;
	return 0;
}

/****************************************************************************
*					ElemCondPlus::check
*
*	Description:	Функция проверки выполнимости ЭУ с учетом знака отрицания, если он есть
*					Данная функция имеет такую же сигнатуру, как AxiomExpr::check()
*					и написана для того, чтобы можно было писать
*					шаблоны,  униформно работающие с ЭУ и аксиомами
*	Parameters:		k - номер отсчета в ряду, на котором проверять условие
*					x - ряд, на котором проверять условие
*	Returns:		1 - если условие выполняется
*					0 - если не выполняется
*					-1 - если не применимо
*	Throws:			AxiomLibException - если элементарное условие не определено 
*					или индексы неверны
*	Author:			wictor
*	History:
*
****************************************************************************/
signed int ElemCondPlus::check(unsigned long k, const std::vector<std::vector<double> > &x) const {
	if(dimension < 0 || dimension >= (int) x.size())
	{
		throw AxiomLibException("ElemCondPlus::check():  dimension out of range: dimension = "
								+ boost::lexical_cast<std::string>(dimension)
								+ ", x.size() = " + boost::lexical_cast<std::string>(x.size())
								);
	}
	const std::vector<double> &v = x[dimension];
	if(k >= v.size())
	{
		throw AxiomLibException("ElemCondPlus::check(): invalid k index : k = " + boost::lexical_cast<std::string>(k)
								+ ", v.size() = " + boost::lexical_cast<std::string>(v.size()) );
	}
	return check(k, v);
}

/****************************************************************************
*					ElemCondPlus::markUp
*
*	Description:	Функция разметки ряда x данным ЭУ
*	Parameters:		x - размечаемый рад
*					markup - получаемая разметка (вектор из 0 и 1 того же размера)
*	Returns:		число отсчетов, на которых условие выполняется (т. е. число 1 в markup)
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
unsigned long ElemCondPlus::markUp(const std::vector<double>& x, std::vector<int> &markup) const {
	markup.resize(x.size());
	unsigned long result = 0;
	for(unsigned long i = 0; i < x.size(); i++) {
		int checkResult = check(i, x);
		if(checkResult == 1) {
			markup[i] = 1;
			result++;
		} else {
			markup[i] = 0;
		}
	}
	return result;
}

/****************************************************************************
*					ElemCondPlus::clear
*
*	Description:	Фнукция обнуляет содержимое данной структуры
*	Parameters:		-
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::clear (void) {
	if (elemCondition != NULL) {
		delete (elemCondition);
		elemCondition = NULL;
	}
	sign = true;
	dimension = -1;
	return 0;
}


/****************************************************************************
*					ElemCondPlus::operator<
*
*	Description:	Сравнение данной аксиомы с ее внутренними параметрами с другой аксиомой того же типа.
*	Parameters:		second - второе сравниваемое условие
*	Returns:		1 - если первое условие является частным случаем второго
*					0 - если условия не сравнимы или тождественны
*					-1 - если второе является частным случаем первого
*	Throws:			AxiomLibException - если одно из условий не задано
*	Author:			dk
*	History:
*
****************************************************************************/
double ElemCondPlus::operator< (const ElemCondPlus& second) const {
	double res;
	if ((this->elemCondition == NULL) || (second.elemCondition == NULL))
		throw AxiomLibException("Error in ElemCondPlus::operator< : elementary conditions are not defined.");
	if (this->ecTypeName() == second.ecTypeName()) {
		res = *(this->elemCondition) < *(second.elemCondition);
		if ((res == -1) && (this->sign == true) && (second.sign == true) || 
			(res == 1) && (this->sign == false) && (second.sign == false))
			return -1;
		if ((res == -1) && (this->sign == false) && (second.sign == false) || 
			(res == 1) && (this->sign == true) && (second.sign == true))
			return 1;
	}
	return 0;
}


/****************************************************************************
*					ElemCondPlus::operator=
*
*	Description:	Оператор присваивания для элементарного условия
*	Parameters:		second - присваиваемое элементарное услови
*	Returns:		ElemCondPlus - заполняемое элементарное условие
*	Throws:			-
*	Author:			dk
*	History:		wictor, 26.11.2009 : переписано с использованием ElemCondition.clone()
*
****************************************************************************/
ElemCondPlus& ElemCondPlus::operator= (const ElemCondPlus& second) {
	if(this != &second) {
		this->dimension = second.dimension;
		this->sign = second.sign;
		this->name = second.name;
		this->index = second.index;
		
//		// Пробуем присвоить, не создавая нового экземпляра ElemCondition
//		if(this->elemCondition && second.elemCondition && this->elemCondition->assign(second.elemCondition)) {
//			return *this;
//		}
		
		// Если не получилось, то действуем стандартным образом
		if(this->elemCondition != NULL) {
			delete this->elemCondition;
		}
		if (second.elemCondition != NULL) {
			this->elemCondition = second.elemCondition->clone();
		} else {
			this->elemCondition = NULL;
		}
	}
	return *this;
}


 /****************************************************************************
*					ElemCondPlus::initFromStructure
*
*	Description:	Функция задающая элементарное условие и его параметры по
*					специализированной структуре с описанием условия
*	Parameters:		ecps - структура с описанием параметров элементарного условия
*	Returns:		0
*	Throws:			AxiomLibException - если имя элементарного условия указано неверно
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromStructure (ElemCondPlusStruture &ecps) {
	this->clear();
	ElemConditionsFactory ecf;
	// Установка знака элементарного условия
	this->sign = ecps.sign;
	this->dimension = ecps.dimension;
	this->elemCondition = ecf.createAxiomByName(ecps.ecName.c_str());
	// Проверка что условие с указанным именем нашлось и было создано
	if (this->elemCondition == NULL) {
		throw AxiomLibException("Error in ElemCondPlus::initFromStructure : cannot creat elementary condition with given name.");
	}
	// Установка параметров элементарного условия
	this->elemCondition->setParams (ecps.ecParams);
	return 0;
}


/****************************************************************************
*					ElemCondPlus::createStructure
*
*	Description:	Функция сохраняет параметры данного элементарного условия
*					в специальную структуру ElemCondPlusStruture
*	Parameters:		ecps - заполняемое поле структуры элементарного условия
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::createStructure (ElemCondPlusStruture &ecps) const {
	// очистка заполняемой структуры аксиомы
	ecps.clear();
	// заполение полей структуры
	ecps.ecName = this->ecTypeName();
	ecps.dimension = this->dimension;
	ecps.sign = this->sign;
	this->elemCondition->getParams(ecps.ecParams);
	return 0;
}


/****************************************************************************
*					ElemCondPlus::saveECToFile
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		baseDir - директория, где будет находится записываемый файл
*					ecName - имя, под которым сохрянять файл
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::saveECToFile (const std::string baseDir, const std::string ecName) const {
	ElemCondPlusStruture ecps;
	createStructure (ecps);
	AxiomBase axiomBase;
	axiomBase.saveToEC (baseDir, ecName, ecps);
	return 0;
}


/****************************************************************************
*					ElemCondPlus::saveECToFile
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		baseDir - директория, где будет находится записываемый файл
*					ecName - имя, под которым сохрянять файл
*					paramNames - вектор набора названий параметров исходных данных
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::saveECToFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames) const {
	if ((this->dimension < 0) || (this->dimension >= (int) paramNames.size()))
		throw AxiomLibException("Error in ElemCondPlus::saveECToFile : num of dimensions in elem condition is out of range: "
								+ boost::lexical_cast<std::string>(this->dimension));
	ElemCondPlusStruture ecps;
	createStructure (ecps);
	AxiomBase axiomBase;
	axiomBase.saveToEC (baseDir, ecName, ecps, paramNames[this->dimension]);
	return 0;
}

/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	Функция загрузки из файлв
*	Parameters:		fullName - полное имя файлв
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string &fullName) {
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (fullName, ecps);
	// Инициализация данного класса из считанной структуры
	initFromStructure(ecps);
	
	return 0;
}

/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		baseDir - директория, где будет находится записываемый файл
*					ecName - имя, под которым сохрянять файл
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string baseDir, const std::string ecName) {
	// Создание полного пути к файлу
	std::string filePath;
	filePath = baseDir;
	filePath.append("/");
	filePath.append(ecName);
	//filePath.append(str_ec_file_ending);
	// Считывание структуры из файла
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (filePath, ecps);
	// Инициализация данного класса из считанной структуры
	initFromStructure(ecps);
	this->name = ecName;
	return 0;
}


/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		baseDir - директория, где будет находится записываемый файл
*					ecName - имя, под которым сохрянять файл
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames) {
	// Создание полного пути к файлу
	std::string filePath;
	filePath = baseDir;
	filePath.append("/");
	filePath.append(ecName);
	//filePath.append(str_ec_file_ending);
	// Считывание структуры из файла
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (filePath, ecps, paramNames);
	// Инициализация данного класса из считанной структуры
	initFromStructure(ecps);
	this->name = ecName;
	return 0;
}

/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		filePath - полный путь к файлу .ec (БЕЗ .ec)
*					ecName - имя, под которым сохрянять файл
*	Returns:		0
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string &filePath, const std::vector <std::string> &paramNames) {
	// Считывание структуры из файла
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (filePath, ecps, paramNames);
	// Инициализация данного класса из считанной структуры
	initFromStructure(ecps);
	return 0;
}

std::ostream& operator<<(std::ostream& ostr, const ElemCondPlus& ecPlus) {
	ostr << ecPlus.dimension << " : ";
	ostr << (ecPlus.sign ? "+" : "-");
	ostr << ecPlus.elemCondition->name();
	std::map<std::string, std::string> ecParams;
	ecPlus.elemCondition->getParams(ecParams);
	ostr << " {" << ecParams << "}";

	return ostr;
}
