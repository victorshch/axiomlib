/****************************************************************************
*					Функции класса Environment.h
*
*	Description:	Функции класса, отвечающего за считывание параметров из файла - Реализация
*	Author:			dk
*	History:		
*
****************************************************************************/
#include "Environment.h"

/****************************************************************************
*					Environment::Environment
*
*	Description:	Конструктор класса, задание значений переменным класса по умолчанию
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:
*
****************************************************************************/
Environment::Environment (void) {
	params.clear();
}

/****************************************************************************
*					Environment::getStringParamValue
*
*	Description:	Функция возвращает первое значение параметра по name1 и name2 
*					в виде строки.
*	Parameters:		std::string& value - куда писать значение
*					std::string name1 - имя параметра
*					std::string name2 - имя параметра
*	Returns:		0 - если такой параметр нашелся
*					-1 - если такого параметра не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::getStringParamValue(std::string& value, const std::string& name1, const std::string& name2) const {
	std::multimap <std::pair <std::string, std::string>, std::string>::const_iterator i = params.find(std::make_pair (name1, name2));
	// проверка - есть ли такой элемент
	if (i == params.end())
		return -1;
	// если есть - возвращаем первое вхождение
	value = i->second;
	return 0;
}

/****************************************************************************
*					Environment::getDoubleParamValue
*
*	Description:	Функция возвращает первое значение параметра по name1 и name2 
*					в виде дробного числа.
*	Parameters:		double& value - куда писать значение
*					std::string name1 - имя параметра
*					std::string name2 - имя параметра
*	Returns:		0 - если такой параметр нашелся
*					-1 - если такого параметра не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::getDoubleParamValue(double& value, const std::string& name1, const std::string& name2) const {
	std::multimap<std::pair<std::string, std::string>, std::string>::const_iterator i = params.find(std::make_pair (name1, name2));
	// проверка - есть ли такой элемент
	if (i == params.end())
		return -1;
	// приводим к double
	value = atof(i->second.c_str());
	return 0;
}

/****************************************************************************
*					Environment::getIntParamValue
*
*	Description:	Функция возвращает первое значение параметра по name1 и name2 
*					в виде целого числа.
*	Parameters:		signed int& value - куда писать значение
*					std::string name1 - имя параметра
*					std::string name2 - имя параметра
*	Returns:		0 - если такой параметр нашелся
*					-1 - если такого параметра не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::getIntParamValue(int &value, const std::string &name1, const std::string &name2) const {
  	std::multimap<std::pair<std::string, std::string>, std::string>::const_iterator i = params.find(std::make_pair (name1, name2));
	// проверка - есть ли такой элемент
	if (i == params.end())
		return -1;
	// приводим к double
	value = atoi(i->second.c_str());
	return 0;
}

/****************************************************************************
*					Environment::getStringSetParamValue
*
*	Description:	Функция возвращает первое значение параметра по name1 и name2 
*					в виде набора строк.
*	Parameters:		std::set <std::string>& value - куда писать значение
*					std::string name1 - имя параметра
*					std::string name2 - имя параметра
*	Returns:		0 - если такой параметр нашелся
*					-1 - если такого параметра не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::getStringSetParamValue(std::set<std::string> &value, const std::string& name1, const std::string& name2) const {
	std::multimap<std::pair<std::string, std::string>, std::string>::const_iterator i = params.find(std::make_pair (name1, name2));
	// проверка - есть ли такой элемент
	if (i == params.end())
		return -1;
	// если есть - заполняем наше множество
	std::pair<mm_c_iter,mm_c_iter> runner = params.equal_range(std::make_pair (name1, name2));
	for (mm_c_iter i = runner.first; i != runner.second; ++i)
		value.insert(i->second);
	return 0;
}

/****************************************************************************
*					Environment::getDoubleSetParamValue
*
*	Description:	Функция возвращает все значения параметра по name1 и name2 
*					в виде набора чисел.
*	Parameters:		std::set<double>& value - куда писать значение
*					std::string name1 - имя параметра
*					std::string name2 - имя параметра
*	Returns:		0 - если такой параметр нашелся
*					-1 - если такого параметра не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::getDoubleSetParamValue (std::set<double>& value, const std::string& name1, const std::string& name2) const {
	std::multimap<std::pair<std::string, std::string>, std::string>::const_iterator i = params.find(std::make_pair (name1, name2));
	// проверка - есть ли такой элемент
	if (i == params.end())
		return -1;
	// если есть - заполняем наше множество
	std::pair<mm_c_iter,mm_c_iter> runner = params.equal_range(std::make_pair (name1, name2));
	for (mm_c_iter i = runner.first; i != runner.second; ++i)
		value.insert(atof(i->second.c_str()));
  return 0;
}

/****************************************************************************
*					Environment::moveSpaces
*
*	Description:	Функция преобразует строку - удаляет все пробелы и табуляции в начале и в конце
*	Parameters:		str - преобразуемая строка
*	Returns:		int - новый размер строки str
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::moveSpaces (std::string &str) {
	// проверка - путсая ли строка
	if (str.size() == 0) {
		return 0;
	}
	// преобраование
	if ((str[0] == '\t') || (str[0] == ' ') || (str[str.size() - 1] == '\t') || (str[str.size() - 1] == ' ')) {
		// помещаем в j - индекс первого символа отличного от табуляции и пробела
		std::string::size_type j = str.find_first_not_of ("\t ");
		if (j == std::string::npos) {
			// строка тока из пробелов и табуляции
			str.clear();
			return 0;
		}
		// помещаем в l - индекс последнего символа отличного от табуляции и пробела
		std::string::size_type l = str.find_last_not_of ("\t ");
		str = str.substr (j, l - j + 1);
	}
	return (int) str.size();
}


/****************************************************************************
*					Environment::readFile
*
*	Description:	Функция чтения файла в память данного класса
*	Parameters:		fileName - путь к считываемому файлу
*	Returns:		0 - если читывание произошло
*					-1 - иначе
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::readFile(const std::string &fileName) {
	// Проверяем - существует ли файл
	boost::filesystem::path fPath (fileName);
	if ( !boost::filesystem::exists( fPath ) ) {
		return -1;
	}

	// Открываем файл
	boost::filesystem::ifstream file( fileName );
	// задаем сепаратор
	boost::char_separator<char> sep("';:\t");

	// Цикл - считывается по одной строке файла и они переходит в вид набора токенов, 
	// которые после чего посылаются на обработку дополнительной функцие
	std::string str;
	while ( !file.eof() )  {
		std::getline(file, str);
		if (!str.empty()) {
			moveSpaces(str);
			token_separator tokens(str, sep);
			processTokens (tokens);
		}
	} 

	// закрываем файл, все ok
	file.close();
	return 0;
}


/****************************************************************************
*					Environment::processTokens
*
*	Description:	Парсит строчку токенов, заполняет multimap
*	Parameters:		tokens - Набор токенов для разбора
*	Returns:		0
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::processTokens (token_separator tokens) {
	for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
		std::string strTemp = *tok_iter;
		if ( (strTemp.size() < 1) || (strTemp[0] == ' ' ) ) { // либо пустой токен, либо начинается с пробела - а значит это не слово
			continue;
		}
		if (strTemp.size() >= 2 && strTemp[0] == '/' && strTemp[1] == '/')
			return 0;

		// Разбираем на param и value
		std::string name1 = *tok_iter;
		++tok_iter;
		if (tok_iter == tokens.end()) {
			return 0;
		}
		std::string name2 = *tok_iter;
		++tok_iter;
		if (tok_iter == tokens.end()) {
			return 0;
		}
		std::string value = *tok_iter;

		// Заносим в наш multimap
		params.insert(std::make_pair (std::make_pair (name1, name2), value));
	}
	return 0;
}
