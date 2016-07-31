#ifndef DATASETBASE_H
#define DATASETBASE_H

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

#include "EnvDataSet.h"
#include "MultiTS.h"

namespace AxiomLib {

class DataSetBase
{
public:
    DataSetBase();
	
	// Функция чтения параметров
	signed int readParams(std::string path);
	
	// Читает датасет из папки baseDataSetDir
	signed int readDataSet(const std::string &baseDataSetDir);

	// Получаем информацию о положении параметра в каноническом представлении по его имени
	int placeOfParam(std::string parameter);

	// Получаем информацию об имения параметра по его номеру в каноническом представлении
	std::string paramName(int parameter) const;

	// Получаем информацию об имения аномального поведения по его номеру в каноническом представлении
	std::string className(int parameter) const;
	
	// Имена классов нештатного поведения в каноническом порядке
	std::vector<std::string> getClassNames() const;

	// Получаем информацию обо всех названиях параметров набора данных в их каноническом представлении
	const std::vector <std::string>& getParamNames (void) const;
	
	// Функция по классам конфигураций создает вектор номеров параметров в каноническом представлении по которым ведется подбор системы аксиом
	int getParamNums(std::vector<int>& toReturn, const Environment &env, EnvDataSet &envDataSet);
	
	int getParamNums(std::vector<int>& toReturn) const;
	
	std::vector<int> getParamNums() const;
	
	// Получение информации о числе элементов в векторе названий параметров
	int paramNamesSize (void) const;
	
	// Функция устанавливает новое обозначение для отсутсвия какой либо траектории во временном ряду
	int setNullStr (const std::string newNullStr);
	
	// Функция устанавливает новое обозначение для отсутсвия какой либо траектории во временном ряду
	int setNullStr (EnvDataSet &envDataSet);
	
	// Функция устанавливает новое обозначение для имени столбца с разметкой траекторий
	int setNameStr (const std::string newNameStr);	
	
protected:
	// Названия классов нештатных ситуаций (читаются из файла class_names)
	std::vector<std::string> classNames;
  
	// Названия параметров (колонок таблиц)
	// В этом векторе они идут в каноническом порядке
	// Все таблицы должны при загрузке приводиться к каноническому порядку
	std::vector<std::string> paramNames;
	
	// Номера используемых параметров (размерностей)
	std::vector<int> paramNums;

	// Строка - кторой обозначается колонка имен классов неисправностей в *-ref.csv файлах
	std::string name;

	// Строка - кторой обозначается отсутствие на данном отсчете неисправности любого типа на некотором отсчете в *-ref.csv файлах
	std::string null;
  
	// Обозначние для лучшего понимания
   typedef boost::tokenizer<boost::char_separator<char> >      token_separator;
	
	void setupParamNums(const Environment& env, const EnvDataSet& envDataSet);
   
   // Функция возвращает индекс первого вхождения элемента whatToFind или -1, если элемент не найден
	// Функция вынесена в Common.h
//   template<class T> 
//   static typename std::vector<T>::size_type isIn(const T& whatToFind, const std::vector<T> &whereToFind);
 
   // Функция возвращает число - записанное в подаваемой строке - составленное из всех цифр входящих в файл
   signed int getIntByName (std::string str);
 
   // Функция чтения данных из csv-файла
   signed int readFromCSV (std::string fileName, MultiTS &multiTS);
   
   // Функция чтения данных из файла с информацией о вхождении в ряды образов, траекторий соответствующих нештатным ситуациям
   signed int readFromRefCSV (std::string fileName, MultiTS &multiTS);
   
   // Функция чтения набора траекторий класса
   signed int readClassTS(const std::string& dirName, ClassTS &classTS);
   
   // функция браузит по файлам и читает этолонные ряды для каждого из классов в указанную переменную
   signed int readReferenceTS (const std::string &path, ReferenceClassesTS &refClassesTS);

   // Функция сравнения двух строк (имен файлов), вторая строка может содержать знаки * и ? как оозначения произвольно символа или набора символов
   static bool checkName (std::string name1, int from, int upTo, std::string name2);   

   // Функция проверяет соответствие имени файла шаблону
   static bool checkRefCSVName(const std::string &name);
private:
};

//template<class T>
//typename std::vector<T>::size_type DataSetBase::isIn(const T &whatToFind, const std::vector<T> &whereToFind) {
//	auto iter = std::find(whereToFind.begin(), whereToFind.end(), whatToFind);
//	return iter != whereToFind.end() ? std::distance(whereToFind.begin(), iter) : -1;
//}

};

#endif // DATASETBASE_H
