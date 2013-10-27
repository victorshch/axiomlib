/****************************************************************************
*				AxiomBase
*
*	Description:	Класс, отвечающий за чтение базы аксиом
*	Author:		dk
*	History:
*
****************************************************************************/
/*!
  @file AxiomBase.h
  @author dk
  Class for reading base of axioms.
 */

#ifndef __AXIOMS_LIB_READAXIOMBASE_HXX
#define __AXIOMS_LIB_READAXIOMBASE_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "AxiomExprStructure.h"
#include "Environment.h"
#include "all_axioms.h"
#include "Axiom.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
//includes for using boost
#include "boost/tokenizer.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

/*!
  @brief
  Checking for weak structure equivalence for two axioms.
  Sets of elementary conditions are equal.
  @author    dk
  @param     second - second comparing axiom
  @retval    0 no weak structure equivalence
  @retval    1 weak structure equivalence
  @return
 */


namespace AxiomLib {

class AxiomBase {

 private:

    // Набор всех аксиом в базе аксиом
    std::map <std::string, std::vector<std::string> > axioms;

    // Имя считанного банка аксиом
    std::string name;

 protected:

    // Обозначние для лучшего понимания
    typedef boost::tokenizer<boost::char_separator<char> >      token_separator;

    // Функция чтения данных из ax-файла
    signed int readFromAX (std::string fileName, std::vector<std::string> &vec);

    // Функция чтения данных из x-файла
    signed int readFromX (std::string fileName, std::string &strName);

    // Функция сравнения имен с шаблоном
    bool checkName (std::string name1, int from, int upTo, std::string name2) const;

    // Функция сравнения строки с именем  файла и строки с путем файла - сравнение просиходит только названий файлов
    bool compareStringAndPath (std::string a, std::string b, const char* n3) const;

 public:


    // Функция возвращает число  аксиом в считанном множестве
    int size (void) const;

    // Не забыть перенести в private ! ! !
    // Имя считанного банка аксиом
    //std::string name;

    // Пустой конструктор
    AxiomBase (void);

    // Функция читает все аксиомы из каталога, путь которго указан в файле конфигураций
    signed int readAxioms (const char* axiomBaseDir);

    // Фунция выдает вектор элементарных условий соответсвующих запрошенной аксиоме, либо пустой вектор если не нашлось аксиомы с заданным именем
    signed int getVectorOfEC (std::vector<std::string>& toReturn, std::string axiomName);

    // Функция считывания описания аксиомы и ее параметров в структуру AxiomExprStructure
    signed int readFromAX (const std::string& filePath, AxiomExprStructure &aes) const;

    // Функция считывания описания аксиомы и ее параметров в структуру AxiomExprStructure
    signed int readFromAX (const std::string &filePath, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Чтение из уже открытого файла одного описания аксиомы
    signed int readFromAX (boost::filesystem::ifstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Функция записи аксиомы в файл
    signed int saveToAX (const std::string& baseDir, AxiomExprStructure &aes) const;

    // Функция записи аксиомы в файл
    signed int saveToAX (const std::string& baseDir, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Функция записи аксиомы в уже открытый файл
    signed int saveToAX (std::ofstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Функция считывания параметров элементарного условия из файла
    signed int readFromEC (const std::string& filePath, ElemCondPlusStruture &ecps) const;

    // Функция считывания параметров элементарного условия из файла
    signed int readFromEC (const std::string &filePath, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const;

    // Чтение из уже открытого файла одного элементарного условия
    signed int readFromEC (boost::filesystem::ifstream &file, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const;

    // Функция записи параметров элементарного условия в файл
    signed int saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps) const;

    // Функция записи параметров элементарного условия в файл
    signed int saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps, const std::string &paramName) const;

    // Функция записи параметров элементарного условия в файл
    signed int saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps, const std::vector <std::string> &paramNames) const;

    // Функция записи параметров элементарного условия в уже открытый файл
    signed int saveToEC (std::ofstream &file, ElemCondPlusStruture &ecps, const std::string &paramName) const;

    // Чтение из файла со списком названий файлов
    signed int readFromList (const std::string &filePath, std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const;

    // Чтение из файла со списком названий файлов
    signed int readFromList (const std::string &filePath, std::vector <std::vector <std::string> >  &strList) const;

    // Функция записи в файл списка с названиями файлов
    signed int saveToList (const std::string &baseDir, const std::string &fileName, const std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const;

    // Функция записи в файл списка с названиями файлов
    signed int saveToList (const std::string& baseDir, const std::string& fileName, const std::vector <std::vector <std::string> > &strList) const;

    // Проверяет, имеет ли имя файла заданное расширение
    static bool checkExtension(const std::string& fileName, const std::string& extension);

    // Добавляет имени файла заданное расширение, если оно его уже не имеет
    static void checkedAddExtension(std::string& fileName, const std::string& extension);

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_READAXIOMBASE_HXX */
