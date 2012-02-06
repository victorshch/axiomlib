/****************************************************************************
*				EnvDataSet
*
*	Description:	Файл чтения конфигурации обработки набора данных
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ENV_DATA_SET_HXX
#define __AXIOMS_LIB_ENV_DATA_SET_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "all_axioms.h"
#include "Axiom.h"
#include "Environment.h"
#include <iostream>      
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
//includes for using boost
#include <boost/tokenizer.hpp>

namespace AxiomLib {

class EnvDataSet {

 private:

 protected:
	// DataSet Parameters
	// Строка, которая обозначет отсутсвие траектории в разметке файла
	std::string nullStr;

	// Строка - кторой обозначается класс неисправнойстей, которому соответстует нормальное поведение системы
	std::string normalStr;
	
	// Вектор с именами обрабатываемых параметров
	std::vector<std::string> analizeNames;
	
	//AxiomSet Parameters
	std::string saveTo;
	
	// this class functions
	// определение типа - введено для сокращения и понимания кода
	typedef boost::tokenizer<boost::char_separator<char> > token_separator;
  
	// внутренняя вспомогательная функция - получает набор токенов для одной строки конфигурационного файла 
	// и в соответствие с внутренней структурой этого файла записывает внутренние переменные класса
	signed int processTokens (token_separator tokens);

	// Вспомогательыне переменные для корректного распознавания конфигурационного файла
	int tmpNull;
	int tmpNormal;
	int tmpSaveTo;
	
	// Тестовый вывод считанных параметров в стандартный поток вывода 
	int testOut (void);
 
 public:
	
	// Конструктор с заданием некоторых параметров по умолчанию
	EnvDataSet (void);
	
	// Конструктор с заданием некоторых параметров по умолчанию
	~EnvDataSet (void);
	
	// Возвращает строку с обозначением отсутсвия траектории в строке разметки
	int getNullStr (std::string &retNull);
	
	// Возвращает строку, которой обозначается нормальное поведение среди классов нештатного поведения
	int getNormalStr (std::string &retNormal);
	
	// Возвращает строку с названием параметра с заданным индексом
	std::string getAnalizeName (const int i) const;
	
	// Возвращает число элементов в векторе строк с названиями параметров
	int getAnalizeNameSize (void) const;
	
	// Функция возвращает имя записываемой лучшой системы аксиом
	int getWhereToSave (std::string &whereToSave);
	
	// Функция преобразует строку - удаляет все пробелы и табуляции в начале и в конце файла
	int moveSpaces (std::string &str);
	
	// Читает конфигурационный файл с заданным именем
	signed int readConfigFile(std::string confDir, std::string confName);
	
	// Читает конфигурационный файл с заданным именем
	signed int readConfigFile(Environment &env);

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_ENV_DATA_SET_HXX */
