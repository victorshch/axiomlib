/****************************************************************************
*					Environment
*
*	Description:	Класс, отвечающий за работу с конфигурацией и параметрами
*	Author:			dk
*	History:
*
****************************************************************************/
#ifndef __qtGui_ENVIRONMENT_HXX
#define __qtGui_ENVIRONMENT_HXX

#include <iostream>      
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <boost/tokenizer.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

// Обозначение, используемое по умолчанию для названия второго параметра, по которому выводить результат
#define str_def_map_value	"__def_second_param_name_"

class Environment {

 private:
	
  // параметры из считанного файла
  std::multimap < std::pair <std::string, std::string> , std::string> params;

 protected:
  
  // определение типа - введено для сокращения и понимания кода
  typedef boost::tokenizer<boost::char_separator<char> >	token_separator;
  typedef std::multimap <std::pair <std::string, std::string>, std::string>::const_iterator	mm_c_iter;
  
  // внутренняя вспомогательная функция - получает набор токенов для одной строки конфигурационного файла 
  // и в соответствие с внутренней структурой этого файла записывает внутренние переменные класса
  int processTokens (token_separator tokens);
  
  // Функция преобразует строку - удаляет все пробелы и табуляции в начале и в конце файла
  int moveSpaces (std::string &str);
  
 public:
	
  // Конструктор класса
  Environment (void);
	
  // Читает конфигурационный файл с заданным именем
  int readFile (const std::string &filename);
	
  // Получить параметр по name1 и name2
  int getStringParamValue (std::string& value, const std::string& name1, const std::string& name2 = str_def_map_value) const;
  int getDoubleParamValue (double& value, const std::string& name1, const std::string& name2 = str_def_map_value) const;
  int getIntParamValue    (int& value, const std::string& name1, const std::string& name2 = str_def_map_value) const;
  int getStringSetParamValue(std::set<std::string>& value, const std::string& name1, const std::string& name2 = str_def_map_value) const;
  int getDoubleSetParamValue(std::set<double>& value, const std::string& name1, const std::string& name2 = str_def_map_value) const;

}; // end of class

#endif /* __qtGui_ENVIRONMENT_HXX */
