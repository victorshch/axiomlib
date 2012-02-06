/****************************************************************************
*			Environment
*
*	Description:	Класс, отвечающий за работу с конфигурацией и параметрами окружения,
*				в том числе с библиотеками аксиом и систем аксиом
*	Author:		gevor
*	History:
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ENVIRONMENT_HXX
#define __AXIOMS_LIB_ENVIRONMENT_HXX

#include <iostream>      
#include <cctype>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include "AxiomLibException.h"

namespace AxiomLib {

class Environment {

 private:

	// взято отсюда: http://stackoverflow.com/questions/1801892/making-mapfind-operation-case-insensitive
	/************************************************************************/
	/* Comparator for case-insensitive comparison in STL assos. containers  */
	/************************************************************************/
	struct ci_less : std::binary_function<std::string, std::string, bool>
	{
	  // case-independent (ci) compare_less binary function
	  struct nocase_compare : public std::binary_function<unsigned char,unsigned char,bool> 
	  {
		bool operator() (const unsigned char& c1, const unsigned char& c2) const {
			return std::tolower (c1) < std::tolower (c2); 
		}
	  };
	  bool operator() (const std::string & s1, const std::string & s2) const {
		return std::lexicographical_compare 
		  (s1.begin (), s1.end (),   // source range
		  s2.begin (), s2.end (),   // dest range
		  nocase_compare ());  // comparison
	  }
	};
	
	typedef std::multimap<std::string, std::string, ci_less> MapType;
  std::multimap<std::string, std::string, ci_less> configFileParams; // параметры из конфигурационного файла

 protected:
  
  // определение типа - введено для сокращения и понимания кода
  typedef boost::tokenizer<boost::char_separator<char> >      		token_separator;
  typedef MapType::const_iterator		mm_c_iter;
  
  // внутренняя вспомогательная функция - получает набор токенов для одной строки конфигурационного файла
  // и в соответствие с внутренней структурой этого файла записывает внутренние переменные класса
  signed int processTokens (token_separator tokens);
  // Тестовый вывод считанных параметров в стандартный поток вывода
  int testOut (void);
  // Функция преобразует строку - удаляет все пробелы и табуляции в начале и в конце файла
  int moveSpaces (std::string &str);
  // Создает директорию с указанным адресом
  int createDir (std::string dir);
 public:
	
  // Конструктор с заданием некоторых параметров по умолчанию
  Environment (void) {};	

  // Читает дефолтный конфигурационный файл
  signed int readConfigFile(void);
  // Читает конфигурационный файл с заданным именем
  signed int readConfigFile(const char* filename);

  // Читает параметры поданные на вход при запуске программы
  signed int readConfigParams (int argc, char** argv);

  // Читает параметры из списка
  signed int readConfigParams(std::list<const char*>);

  // Читает параметры
  template<class ForwardIterator, class ConvertFunctor>
  int readConfigParams(ForwardIterator begin, ForwardIterator end, ConvertFunctor convert);

  // Сохраняет указанный файл в результаты (копирует файл с указанным именем в результаты)
  signed int saveFile (const std::string fileName);
  
  void writeToStream(std::ostream& ostream) const;
  std::string getStringRepresentation() const;
  
  void getParamNames(std::vector<std::string>& result) const;
  
  // Получить параметр с именем name
  int getStringParamValue(std::string& value, const std::string& name) const;
  int getDoubleParamValue(double& value, const std::string& name) const;
  int getIntParamValue(signed int& value, const std::string& name) const;
  int getBoolParamValue(bool& value, const std::string& name) const;
  int getStringSetParamValue(std::set<std::string>& value, const std::string& name) const;
  int getDoubleSetParamValue(std::set<double>& value, const std::string& name) const;
  int getIntSetParamValue(std::set<int>& value, const std::string& name) const;
  
  // Общая функция взятия значения параметра
  template<class T>
  int getParamValue(T& value, const std::string& name) const;
  
  // Установка (перезапись) параметра
  template<class T>
  void setParamValue(const T& value, const std::string& name);
  void setParamValues(const std::set<std::string>& value, const std::string& name);
  
  // В случае отсутствия значения бросает исключение
  template<class T>
  void getMandatoryParamValue(T& value, const std::string& name) const;
  
  // Удаляет параметр из окружения
  int undefineParam(const std::string& name);
  
  bool paramDefined(const std::string& paramName) const;
  
  void clear();

  // Оператор объединения содержимого классов окружения
  Environment& operator += (const Environment& second);
  
  // Выставляет все параметры, содержащиеся в обоих окружениях,
  // в значения из other
  Environment& operator %= (const Environment& other);

}; // end of class

/****************************************************************************
*					Environment::readConfigParams
*
*	Description:	Читает параметры, поданные в виде последовательности, заданной
*					начальным и конечным итератором
*	Parameters:	-
*	Returns:		0
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
template<class ForwardIterator, class ConvertFunctor>
int Environment::readConfigParams(ForwardIterator begin,
		ForwardIterator end,
		ConvertFunctor convert) {
	signed int toRet = 0;
	std::string name, value;
	ForwardIterator current = begin;
	while(current != end) {
		name = convert(*current);
		current++;
		if(current == end) break;
		value = convert(*current);
		configFileParams.insert(std::make_pair(name, value));
		current++;
	}
	return toRet;
}

template<class T>
int Environment::getParamValue(T& value, const std::string& name) const {
	std::string buffer;
	if(getStringParamValue(buffer, name) != 0) {
		return -1;
	}
	try {
		value = boost::lexical_cast<T>(buffer);
		return 0;
	} catch (boost::bad_lexical_cast) {
		return -1;
	}
}

template<class T>
void Environment::getMandatoryParamValue(T &value, const std::string &name) const {
	if(getParamValue<T>(value, name) != 0) {
		throw AxiomLibException("Mandatory parameter '" + name + "' not found in the environment");
	}
}


/****************************************************************************
*				Environment::setParamValue
*
*	Description:	Функция устанавливает новое значение параметра
*					и удаляет все его старые вхождения
*	Parameters:	T& value - значение
*				const std::string& name - имя параметра
*	Returns:	-
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
template<class T>
void Environment::setParamValue(const T &value, const std::string &name) {
	configFileParams.erase(name);
	
	configFileParams.insert(std::make_pair(name, boost::lexical_cast<std::string>(value)));
}


}; //  end of namespace

#endif /* __AXIOMS_LIB_ENVIRONMENT_HXX */
