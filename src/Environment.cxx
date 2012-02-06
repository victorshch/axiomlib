/****************************************************************************
*				Environment.cxx
*
*	Description:	Функции класса Environment - Реализация
*	Author:		gevor,dk
*	History:		
*
****************************************************************************/

#include <algorithm>
#include <iterator>
#include <list>
#include <iostream>
#include <sstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "AxiomLibException.h"
#include "Environment.h"

using namespace AxiomLib;
using namespace std;

#define str_default_config_file_name	"./dataset/axiomlib.conf"

/****************************************************************************
*				Environment::getStringParamValue
*
*	Description:	Функция возвращает первое значение параметра с именем name в виде строки
*	Parameters:	string& value - куда писать значение
*			string name - имя параметра
*	Returns:	0 - если такой параметр нашелся
*			-1 - если такого параметра не нашлось
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int Environment::getStringParamValue(string& value, const string& name) const
{
  MapType::const_iterator i = configFileParams.find(name);
  
  // проверка - есть ли такой элемент
  if (i == configFileParams.end())
    return -1;

  // если есть - возвращаем первое вхождение
  value = (*i).second;
  return 0;
}

/****************************************************************************
*				Environment::getDoubleParamValue
*
*	Description:	Функция возвращает первое значение параметра с именем name в виде дроного числа
*	Parameters:	double& value - куда писать значение
*				string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось, или он не привелся к double
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int Environment::getDoubleParamValue(double& value, const string& name) const
{
  MapType::const_iterator i = configFileParams.find(name);
  
  // проверка - есть ли такой элемент
  if (i == configFileParams.end())
    return -1;

  // приводим к double
  value = atof((*i).second.c_str());
  return 0;
}

/****************************************************************************
*				Environment::getIntParamValue
*
*	Description:	Функция возвращает первое значение параметра с именем name в виде целого
*	Parameters:	signed int& value - куда писать значение
*				string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось, или он не привелся к int
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int Environment::getIntParamValue(signed int& value, const string& name) const
{
  MapType::const_iterator i = configFileParams.find(name);
  
  // проверка - есть ли такой элемент
  if (i == configFileParams.end())
    return -1;

  // приводим к double
  value = atoi((*i).second.c_str());
  return 0;
}

/****************************************************************************
*					Environment::getBoolParamValue
*
*	Description:	Функция возвращает первое значение параметра с именем name как булевскую переменную
*					(правило преобразования - case-insensitive, "true" или "false")
*	Parameters:	bool& value - куда писать значение
*				string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось, или он не привелся к bool (по указанному правилу)
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
int Environment::getBoolParamValue(bool &value, const std::string &name) const {
	std::string str;
	if(getStringParamValue(str, name) < 0) {
		return -1;
	} else {
		boost::algorithm::to_lower(str);
		if (str == "true") {
			value = true;
		} else if (str == "false") {
			value = false;
		} else {
			return -1;	
		}
		return 0;	
	}	
}

/****************************************************************************
*				Environment::getStringSetParamValue
*
*	Description:	Функция возвращает все значения параметра с именем name в виде строки
*	Parameters:	string& value - куда писать значение
*			string name - имя параметра
*	Returns:	0 - если такой параметр нашелся
*			-1 - если такого параметра не нашлось
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int Environment::getStringSetParamValue(set<string>& value, const string& name) const
{
  MapType::const_iterator i = configFileParams.find(name);
  
  // проверка - есть ли такой элемент вообще, хоть один
  if (i == configFileParams.end())
    return -1;

  // если есть - заполняем наше множество
  pair<mm_c_iter,mm_c_iter> runner = configFileParams.equal_range(name);
  for (mm_c_iter i = runner.first; i != runner.second; ++i)
    value.insert((*i).second);
  return 0;
}

/****************************************************************************
*				Environment::getDoubleSetParamValue
*
*	Description:	Функция возвращает все значения параметра с именем name в виде набора чисел
*	Parameters:	string& value - куда писать значение
*				string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
int Environment::getDoubleSetParamValue (std::set<double>& value, const std::string& name) const 
{
  MapType::const_iterator i = configFileParams.find(name);
  
  // проверка - есть ли такой элемент вообще, хоть один
  if (i == configFileParams.end())
    return -1;

  // если есть - заполняем наше множество
  pair<mm_c_iter,mm_c_iter> runner = configFileParams.equal_range(name);
  for (mm_c_iter i = runner.first; i != runner.second; ++i)
    value.insert(atof((*i).second.c_str()));
  return 0;
}

/****************************************************************************
*				Environment::getIntSetParamValue
*
*	Description:	Функция возвращает все значения параметра с именем name в виде набора чисел
*	Parameters:	std::set<int>& value - куда писать значения 
*				(значения добавляются, то, что уже там лежит, не трогается)
*				string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
int Environment::getIntSetParamValue(std::set<int>& value, const std::string& name) const {
	std::set<std::string> stringSet;
	if(getStringSetParamValue(stringSet, name) < 0) {
		return -1;
	}
	// Отдельная переменная - чтобы в случае ошибки ничего в value не писать
	std::list<int> result;
	// Переводим строки в числа
	BOOST_FOREACH(const std::string &str, stringSet) {
		try {
			result.push_back(boost::lexical_cast<int>(str));
		} catch(boost::bad_lexical_cast &) {
			return -1;
		}
	}
	// Копируем result в value
	value.insert(result.begin(), result.end());
	return 0;
}

void Environment::setParamValues(const std::set<std::string> &value, const std::string &name) {
	configFileParams.erase(name);
	for(auto it = value.begin(); it != value.end(); ++it) {
		configFileParams.insert(std::make_pair(name, *it));
	}
}

/****************************************************************************
*				Environment::undefineParam
*
*	Description:	Функция удаляет параметр из окружения
*	Parameters:	string name - имя параметра
*	Returns:		0 - если такой параметр нашелся
*				-1 - если такого параметра не нашлось
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
int Environment::undefineParam(const std::string &name) {
	int erasedCount = configFileParams.erase(name);
	return erasedCount > 0 ? 0 : -1;
}

bool Environment::paramDefined(const std::string &paramName) const {
	return configFileParams.find(paramName) != configFileParams.end();
}

void Environment::clear() {
	configFileParams.clear();
}

/****************************************************************************
*					Environment::moveSpaces
*
*	Description:	Функция преобразует строку - удаляет все пробелы и табуляции в начале и в конце файла
*	Parameters:	str - преобразуемая строка
*	Returns:		0
*	Throws:		-
*	Author:		dk
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
		int j = 0;
		while (j < (int) str.size()) {
			if ((str[j] != '\t') && (str[j] != ' ')) {
				break;
			}
			j++;
		}
		if (j == str.size()) {
			// строка тока из пробелов и табуляции
			str.resize(0);
			return 0;
		}
		// теперь j - индекс первого символа отличного от табуляции и пробела
		int l = (int) str.size() - 1;
		while (l >= j) {
			if ((str[l] != '\t') && (str[l] != ' ')) {
				break;
			}
			l--;
		}
		// теперь в l - индекс последнего символа отличного от табуляции и пробела
		std::string newStr;
		newStr.resize(l - j + 1);
		int h = 0;
		for (int i = j; i <= l; i++) {
			newStr[h] = str[i];
			h++;
		}
		str= newStr;
	}
	return 0;
}


/****************************************************************************
*					Environment::testOut
*
*	Description:	Тестовый вывод всех считанных параметров в stdout
*	Parameters:	-
*	Returns:		0
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
int Environment::testOut (void) {
	return 0;
}


/****************************************************************************
*					Environment::createDir
*
*	Description:	Создает директорию по указанному адресу
*	Parameters:		dir - строка с адресом для создания директории
*	Returns:		0 - если директория создана или существует
*					-1 - если директорию создать не удалось
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int Environment::createDir (std::string dir) {
	// Проверка входного параметра
	if (dir.size() < 1)
		return -1;

	boost::filesystem::path path (dir);
	// Проверка - существует ли указанная директория
	if ( boost::filesystem::exists(path) ) {
		if ( !boost::filesystem::is_directory(path) ) {
			boost::filesystem::create_directory(path);
		}
		return 0;
	}

	// Если директория с указанным именем не существует
	int index;
	index = (int) dir.find_last_of ("/\\");
	if (index > 0) {
		if (index == (dir.size() - 1)) {
			dir.erase(index, 1);
			return createDir (dir);
		}
		if (createDir (dir.substr(0, index)) < 0)
			return -1;
		boost::filesystem::create_directory(path);
		return 0;
	} else {
		// значит слешей в пути не осталось - такая директория, считаем, должна всегда существовать
		return -1;
	}

	// Сюда дойти не должны - поэтому возвращаем ошибку
	return -1;
}


/****************************************************************************
*					Environment::saveFile
*
*	Description:	Сохранение указанного файла в папку с результатами
*	Parameters:		fileName - адрес файла, который сохранять в результаты
*	Returns:		0 - если все нормально
*					-1 - если в данном классе Environment не задан путь для сохранения резултатов
*					-2 - если указанный путь для сохранения результатов не является директорией
*					-3 - если не существует указанный для копирования файл
*					-4 - если не удается заменить уже существующий файл по указанному пути
*					-5 - если не удается создать указанный путь, по которому сохранить файл
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int Environment::saveFile (const std::string fileName) {
	std::string whereToSave;
	// Если нет параметра - куда сохранять файл - то выходим из функции
	if (getStringParamValue (whereToSave, "PathToSaveResults") < 0)
		return -1;
	boost::filesystem::path path (whereToSave);
	if ( !boost::filesystem::exists( path ) ) {
		if (createDir (whereToSave) < 0)
			return -5;
	} else {
		if ( !boost::filesystem::is_directory( path ) )
			return -2;
	}
	int indexName, indexSlash;
	indexName = (int) fileName.find_last_of("/\\");
	indexSlash = (int) whereToSave.find_last_of("/\\");
	if (indexSlash != (whereToSave.size() - 1))
		whereToSave.append ("/");
	whereToSave.append (fileName.substr (indexName + 1, fileName.size() - indexName - 1));
	boost::filesystem::path fPath (fileName);
	boost::filesystem::path fPathNew (whereToSave);
	if ( !boost::filesystem::exists( fPath ) ) {
		return -3;
	}
	if ( boost::filesystem::exists( fPathNew ) ) {
		if (!boost::filesystem::remove( fPathNew ))
			return -4;
	}
	boost::filesystem::copy_file (fPath, fPathNew);
	return 0;
}

void Environment::writeToStream(std::ostream &ostr) const {
	// TODO подумать о escape'е спецсимволов
	ostr<<"{ ";
	auto it = configFileParams.begin();
	ostr<<"\""<<it->first<<"\""<<":"<<"\""<<it->second<<"\"";
	++it;
	for(; it != configFileParams.end(); ++it) {
		ostr<<", \""<<it->first<<"\""<<":"<<"\""<<it->second<<"\"";
	}
	ostr<<" }";
}

std::string Environment::getStringRepresentation() const {
	std::ostringstream ostr;
	writeToStream(ostr);
	return ostr.str();
}

void Environment::getParamNames(std::vector<std::string> &result) const {
	if(configFileParams.size() == 0) {
		return;
	}
	
	result.reserve(configFileParams.size());
	auto it = configFileParams.begin();
	result.push_back(it->first);
	
	for(++it; it != configFileParams.end(); ++it) {
		if(it->first != result.back()) {
			result.push_back(it->first);
		}
	}
}

/****************************************************************************
*					Environment::readConfigParams
*
*	Description:	Читает параметры поданные на вход при запуске программы
*	Parameters:	-
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int Environment::readConfigParams (int argc, char** argv) {
	signed int toRet = 0;
	std::string name, value;
	int p = 2;
	while (p < argc) {
		name.assign (argv[p-1]);
		value.assign (argv[p]);
		configFileParams.insert(make_pair(name, value));
		p += 2;
	}
	return toRet;
}


/****************************************************************************
*					Environment::readConfigFile
*
*	Description:	Функция чтения конфигурационного файла по умолчанию
*	Parameters:	-
*	Returns:	0
*	Throws:		-
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int Environment::readConfigFile(void) {
  readConfigFile(str_default_config_file_name);
  return 0;
}


/****************************************************************************
*					Environment::readConfigFile
*
*	Description:	Функция чтения данного конфигурационного файла
*	Parameters:	Параметром является тектовая строка содержащая путь к конфигурационному файлу
*	Returns:	0
*	Throws:		AxiomLibException - если не нашлось конфигурационного файла с заданным путем
*	Author:		gevor
*	History:
*
****************************************************************************/
signed int Environment::readConfigFile(const char* filename) {
  std::string fileName(filename);
  // Проверяем - существует ли файл
  boost::filesystem::path fPath (filename);
  bool tempBool = boost::filesystem::exists( fPath );
  if ( !boost::filesystem::exists( fileName ) ) {
    throw AxiomLibException("Error in Environment::ReadConfigFile: cannot find the config file.");
  }

  // Открываем файл
  boost::filesystem::ifstream file( fileName );
  // задаем сепаратор
  boost::char_separator<char> sep("';:\t ");

  // Цикл - считывается по одной строке файла и они переходит в вид набора токенов, 
  // которые после чего посылаются на обработку дополнительной функции
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
*	Parameters:	Набор токенов для разбора
*	Returns:	0
*	Throws:		AxiomLibException - если формат данного конфигурационного не совпадает с форматом описанном в ../format.x
*	Author:		dk,gevor
*	History:
*
****************************************************************************/
signed int Environment::processTokens (token_separator tokens) {
  for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
    string strTemp = *tok_iter;
    if ( (strTemp.size() < 1) || (strTemp[0] == ' ' ) ) {
		// либо пустой токен, либо начинается с пробела - а значит это не слово
		continue;
    }
	if (strTemp.size() >= 2 && strTemp[0] == '/' && strTemp[1] == '/') {
		// данная строка - это строка с комментами
		return 0;
	}

    // Разбираем на param и value
    // (!!!) - не работает с пробелами
    string name = *tok_iter;
    //cerr << "Name token: " << *tok_iter << "\n";
	
    ++tok_iter;
    if (tok_iter == tokens.end()) {
      throw AxiomLibException ("Error in Environment::processTokens : error in config file - value not set. name = '" + name + "'");
    }
	//cerr << "Value token: " << *tok_iter << "\n";
	
    string value = *tok_iter;

    // Заносим в наш multimap
    configFileParams.insert(make_pair(name, value));
  }    
  return 0;
}


/****************************************************************************
*				Environment::operator +=
*
*	Description:	Оператор объединения содержимого классов окружения
*	Parameters:		second - добавляемый класс окружения
*	Returns:		*this
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
Environment& Environment::operator += (const Environment& second) {
	if(this == &second) {
		return *this;
	}
	
	MapType::const_iterator iter;
	// удаляем значения для обновляемых параметрах
	for (iter = second.configFileParams.begin(); iter != second.configFileParams.end(); ++iter ) {
			this->configFileParams.erase (iter->first);		
	}
	// добавляем новые значения для обновляемых параметров
	for (iter = second.configFileParams.begin(); iter != second.configFileParams.end(); ++iter ) {
		this->configFileParams.insert(make_pair(iter->first, iter->second));	
	}
	return *this;
}

Environment& Environment::operator %= (const Environment& other) {
	if(this == &other) {
		return *this;
	}
	
	std::set<std::string> stringSet;
	std::vector<std::string> paramNames;
	getParamNames(paramNames);
	for(auto it = paramNames.begin(); it != paramNames.end(); ++it) {
		stringSet.clear();
		if(!other.getStringSetParamValue(stringSet, *it)) {
			this->setParamValues(stringSet, *it);
		}
	}
	
	return *this;
}
