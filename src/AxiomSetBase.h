/****************************************************************************
*				AxiomSetBase
*
*	Description:	Класс, отвечающий за чтение базы аксиом
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_READAXIOMSET_HXX
#define __AXIOMS_LIB_READAXIOMSET_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomBase.h"
#include "AxiomLibException.h"
#include "Environment.h"
#include "all_axioms.h"
#include "Axiom.h"
#include "AxiomSetStructure.h"
#include "AxiomStructure.h"
#include "AxiomExprSetStructure.h"
#include "AxiomExprStructure.h"
#include "ElemCondPlusStruture.h"
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

namespace AxiomLib {

class AxiomSetBase {

 private:
	
 protected:
	
	// Обозначние для лучшего понимания
	typedef boost::tokenizer<boost::char_separator<char> >      token_separator;
	
	// Функция поиска сподстроки в строке с заданными параметрами вхождения
	bool checkName (std::string name1,signed int from,signed int upTo, std::string name2) const;
	
	// Функция счравнения имени файла считанного из пути к фйала, и из самого файла
	bool compareStringAndPath (std::string fileName, std::string path, const char* n3) const;
	
 public:
	// Необходимо пренести в privat ! ! ! - чисто тестовые цели ! ! !
	// Все считанные в данной директории наборы систем аксиом
	AxiomSets axiomSets;
	
	// Пустой конструктор
	AxiomSetBase (void);
	
	// Функция считывания систем аксиом - откуда считывать берется из кинфигурационного файла
	signed int readSets (const char* axiomSetBaseDir);

	// Функция считывания параметров системы аксиом
	signed int readFromAS (std::string filePath, AxiomSetStructure &ass);

	// Функция считывания параметров системы аксиом 
	signed int readFromAS (std::string filePath, AxiomExprSetStructure &aess) const;

	// Функция считывания параметров системы аксиом (с указанием размернстей набора данных, на которых проверять элементарные условия)
	signed int readFromAS (std::string filePath, AxiomExprSetStructure &aess, const std::vector<std::string> &paramNames) const;

	// Функция считывания параметров системы аксиом из уже открытого файла (с указанием размернстей набора данных, на которых проверять элементарные условия)
	signed int readFromAS (boost::filesystem::ifstream &file, AxiomExprSetStructure &aess, const std::vector<std::string> &paramNames) const;

	// Функция записи системы аксиом в файл
	signed int saveToAS (std::string baseDir, AxiomSetStructure &ass, int first = -1, int second = -1) const;

	// Функция записи системы аксиом в файл
	signed int saveToAS (std::string baseDir, AxiomExprSetStructure &aess, int first = -1, int second = -1) const;

	// Функция записи системы аксиом в файл (с указанием размернстей набора данных, на которых проверять элементарные условия)
	signed int saveToAS (std::string baseDir, AxiomExprSetStructure &aess, std::vector <std::string> &paramNames, int first = -1, int second = -1) const;

	// Функция записи системы аксиом в уже открытый файл (с указанием размернстей набора данных, на которых проверять элементарные условия)
	signed int saveToAS (std::ofstream &file, AxiomExprSetStructure &aess, std::vector <std::string> &paramNames) const;
	
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_READAXIOMSET_HXX */
