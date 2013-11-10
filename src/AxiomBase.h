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
    //!
    //! \brief axioms
    //!
    std::map <std::string, std::vector<std::string> > axioms;

    // Имя считанного банка аксиом
    //!
    //! \brief name
    //!
    std::string name;

 protected:

    // Обозначние для лучшего понимания
    //!
    //! \brief token_separator
    //!
    typedef boost::tokenizer<boost::char_separator<char> >      token_separator;

    // Функция чтения данных из ax-файла
    //!
    //! \brief readFromAX
    //! \param fileName
    //! \param vec
    //! \return
    //!

    /****************************************************************************
    *					AxiomBase::readFromAX
    *
    *	Description:	Функция обработки .ax файлов
    *				(Вспомогательная функция к readAxioms)
    *				Функция считывает информацию из файла описания аксиомы
    *				и записывает полчученную информацию в специально
    *				определенную структуру - отображение по имени аксиомы в
    *				вектор элементарных условий. составляющих данную аксиому
    *	Parameters:	filePath - путь к обрабатываему файлу описания аксиомы
    *				vec - заполняемый вектор элементарных условий составляющих
    *					данную аксиому
    *	Returns:	0 - если файл был считан и корректно разобран
    *				-1 - если файл не был считан
    *	Throws:		AxiomLibException  - если файл с заданным путем не найден
    *								если по этому пути находится каталог
    *								если формат файла не соответствует
    *								описанию форматов (см. formats.x)
    *	Author:		dk
    *	History:
    *
    ****************************************************************************/
    signed int readFromAX (std::string fileName, std::vector<std::string> &vec);

    // Функция чтения данных из x-файла
    //!
    //! \brief readFromX
    //! \param fileName
    //! \param strName
    //! \return
    //!


    /****************************************************************************
    *					AxiomBase::readFromX
    *
    *	Description:	Функция обработки .x файла
    *				Функция считывает имя банка аксиом и сравнивает с именем данного файла
    *				(Вспомогательная функция к readAxioms)
    *	Parameters:	filePath - путь к обрабатываему .x файлу
    *				strName - заполняемый параметр - именем банка аксиом - считывается из файла
    *	Returns:		0
    *	Throws:		AxiomLibException  - если обрабатываемго файла нет - то есть путь не верный
    *								если файл пуст
    *								если имя банка аксиом, записанное в файле отчличается
    *								от имени этого файла
    *	Author:		dk
    *	History:
    *
    ****************************************************************************/
    signed int readFromX (std::string fileName, std::string &strName);

    /****************************************************************************
    *					AxiomBase::checkName
    *
    *	Description:	Функция сравнения строк.
    *				Кроме того отдельно указывается с какого символа в каждой
    *				строке начинать сравнение и до какого символа его вести.
    *				Используется для сравнения с шаблоном имен файлов.
    *	Parameters:	name1 - строка для сравнения
    *				from -номер символа в строке name1 с которого начинать сравнение
    *				upTo - номер символа в строке name1 до которого вести сравнение
    *				name2 - строка для сравнения
    *	Returns:		true  - если первый файл подходит под второй как под шаблон
    *				false - в противном случае
    *	Throws:		-
    *	Author:		dk
    *	History:
    *
    ****************************************************************************/

    // Функция сравнения имен с шаблоном
    //!
    //! \brief checkName
    //! \param name1
    //! \param from
    //! \param upTo
    //! \param name2
    //! \return
    //!
    bool checkName (std::string name1, int from, int upTo, std::string name2) const;

    // Функция сравнения строки с именем  файла и строки с путем файла - сравнение просиходит только названий файлов
    //!
    //! \brief compareStringAndPath
    //! \param a
    //! \param b
    //! \param n3
    //! \return
    //!

    /****************************************************************************
    *					AxiomBase::compareStringAndPath
    *
    *	Description:	Функция сравнения имени файла с путем к файлу.
    *				Сравнение происходит по этапам:
    *				- к имени файла дописывается расширение - указываемое в параметрах
    *				- в начало имени дописывается "/"
    *				- измененное имя файла ищется в конце пути к файлу
    *	Parameters:	fileName - имя файла
    *				path - путь к файлу(не факт что к этому)
    *				n3 - расширение файла, с которым предполагается найти файл в пути к файлу
    *	Returns:		bool
    *	Throws:		-
    *	Author:		dk
    *	History:
    *
    ****************************************************************************/

    bool compareStringAndPath (std::string a, std::string b, const char* n3) const;

 public:


    // Функция возвращает число  аксиом в считанном множестве
    //!
    //! \brief size
    //! \return
    //!
    int size (void) const;

    // Не забыть перенести в private ! ! !
    // Имя считанного банка аксиом
    //std::string name;

    // Пустой конструктор
    //!
    //! \brief AxiomBase
    //!
    AxiomBase (void);

    // Функция читает все аксиомы из каталога, путь которго указан в файле конфигураций

    /****************************************************************************
    *					AxiomBase::readAxioms
    *
    *	Description:	Функция чтения базы аксиом.
    *				Функция получает из входного параметра информацию
    *				о месте (пути к каталогу) хранения базы аксиом. Далее функция
    *				в цикле ходит файлам каталога - и сравнивает имя файлов
    *				на удолвлетворение шаблонам. Псоле отбора файлов - считыватся
    *				аксиомы, описанные в файлах.
    *	Parameters:	axiomBaseDir - строка с путем к каталогу базы аксиом
    *	Returns:		0
    *	Throws:		AxiomLiException  - 	если не нашлось каталога, указанного в
    *								конфигурационном файле как банк аксиом
    *								если заданный путь к этому каталогу - путь к файлу
    *								если в каталоге не содержится файла .x с указанием
    *								имени банка аксиом (либо таких файлов более одного)
    *	Author:		dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief readAxioms
    //! \param axiomBaseDir
    //! \return
    //!
    signed int readAxioms (const char* axiomBaseDir);

    // Фунция выдает вектор элементарных условий соответсвующих запрошенной аксиоме, либо пустой вектор если не нашлось аксиомы с заданным именем

    /****************************************************************************
    *					AxiomBase::getVectorOfEC
    *
    *	Description:	Фунция выдает вектор элементарных условий соответсвующих
    *				запрошенной аксиоме, либо пустой вектор если не нашлось аксиомы
    *				с заданным именем
    *	Parameters:	toReturn - возвращаемый вектор
    *			axiomName - имя аксиомы, информацию о которой ищем
    *	Returns:		0
    *	Throws:		AxiomLiException  - 	если не нашлось каталога, указанного в
    *								конфигурационном файле как банк аксиом
    *								если заданный путь к этому каталогу - путь к файлу
    *								если в каталоге не содержится файла .x с указанием
    *								имени банка аксиом (либо таких файлов более одного)
    *	Author:		dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief getVectorOfEC
    //! \param toReturn
    //! \param axiomName
    //! \return
    //!
    signed int getVectorOfEC (std::vector<std::string>& toReturn, std::string axiomName);

    // Функция считывания описания аксиомы и ее параметров в структуру AxiomExprStructure

    /****************************************************************************
    *					AxiomBase::readFromAX
    *
    *	Description:	Функция чтения информации об аксиом из одного .ax файла
    *					Формат файла описан в .formats.x
    *					В данной функцие считывается и сравнивается с инменем файла,
    *					имя записанное в первой строке файла; Считывается и сохраняется
    *					имя используемой базы элементарных условий;
    *					(Имена не должны содержать разделяющих символов " ,';:[]" )
    *					Далее в функцие считываются входящие в аксиом условия и их параметры.
    *					Все считанный сущности записываются в специально определенную
    *					структуру, см. AxiomExprStrucutre.h.
    *	Parameters:		filePath - имя файла для обработки (путь к фйлу)
    *					aes - заполняемая по файлу струтура типа AxiomExprStrucutre
    *	Returns:		0
    *	Throws:			AxiomLibException  - если файл с заданным путем не существует
    *							если формат файла не соглаосван описанием формата (см. formats.x)
    *							если имя файла не согласовано с именем в первой строке файла
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief readFromAX
    //! \param filePath
    //! \param aes
    //! \return
    //!
    signed int readFromAX (const std::string& filePath, AxiomExprStructure &aes) const;

    // Функция считывания описания аксиомы и ее параметров в структуру AxiomExprStructure

    /****************************************************************************
    *					AxiomBase::readFromAX
    *
    *	Description:	Функция чтения информации об аксиом из одного .ax файла
    *					Формат файла описан в .formats.x
    *					В данной функцие считывается и сравнивается с инменем файла,
    *					имя записанное в первой строке файла; Считывается и сохраняется
    *					имя используемой базы элементарных условий;
    *					(Имена не должны содержать разделяющих символов " ,';:[]" )
    *					Далее в функцие считываются входящие в аксиом условия и их параметры.
    *					Все считанный сущности записываются в специально определенную
    *					структуру, см. AxiomExprStrucutre.h.
    *	Parameters:		filePath - имя файла для обработки (путь к фйлу)
    *					aes - заполняемая по файлу струтура типа AxiomExprStrucutre
    *					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
    *	Returns:		0
    *	Throws:			AxiomLibException  - если файл с заданным путем не существует
    *							если формат файла не соглаосван описанием формата (см. formats.x)
    *							если имя файла не согласовано с именем в первой строке файла
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief readFromAX
    //! \param filePath
    //! \param aes
    //! \param paramNames
    //! \return
    //!
    signed int readFromAX (const std::string &filePath, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Чтение из уже открытого файла одного описания аксиомы

    /****************************************************************************
    *					AxiomBase::readFromAX
    *
    *	Description:	Функция чтения информации об аксиом из одного .ax файла
    *					Формат файла описан в .formats.x
    *					В данной функцие считывается и сравнивается с инменем файла,
    *					имя записанное в первой строке файла; Считывается и сохраняется
    *					имя используемой базы элементарных условий;
    *					(Имена не должны содержать разделяющих символов " ,';:[]" )
    *					Далее в функцие считываются входящие в аксиом условия и их параметры.
    *					Все считанный сущности записываются в специально определенную
    *					структуру, см. AxiomExprStrucutre.h.
    *	Parameters:		filePath - имя файла для обработки (путь к фйлу)
    *					aes - заполняемая по файлу струтура типа AxiomExprStrucutre
    *					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
    *					file - открытый файл из которого будет производиться считывание аксиомы
    *	Returns:		0
    *	Throws:			AxiomLibException  - если файл с заданным путем не существует
    *							если формат файла не соглаосван описанием формата (см. formats.x)
    *							если имя файла не согласовано с именем в первой строке файла
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief readFromAX
    //! \param file
    //! \param aes
    //! \param paramNames
    //! \return
    //!
    signed int readFromAX (boost::filesystem::ifstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Функция записи аксиомы в файл

    /****************************************************************************
    *					AxiomBase::saveToAX
    *
    *	Description:	Функция сохранения аксиомы в файл
    *	Parameters:		baseDir - директория, где находится записываемый файл описания аксиомы
    *					AxiomName - имя аксиомы
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файла не нашлось
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToAX
    //! \param baseDir
    //! \param aes
    //! \return
    //!
    signed int saveToAX (const std::string& baseDir, AxiomExprStructure &aes) const;

    // Функция записи аксиомы в файл

    /****************************************************************************
    *					AxiomBase::saveToAX
    *
    *	Description:	Функция сохранения аксиомы в файл
    *	Parameters:		baseDir - директория, где находится записываемый файл описания аксиомы
    *					AxiomName - имя аксиомы
    *					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файла не нашлось
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToAX
    //! \param baseDir
    //! \param aes
    //! \param paramNames
    //! \return
    //!
    signed int saveToAX (const std::string& baseDir, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    // Функция записи аксиомы в уже открытый файл

    /****************************************************************************
    *					AxiomBase::saveToAX
    *
    *	Description:	Функция сохранения аксиомы в файл
    *	Parameters:		baseDir - директория, где находится записываемый файл описания аксиомы
    *					AxiomName - имя аксиомы
    *					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
    *					file - открытый файл, в который дописывается описание аксиомы
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файла не нашлось
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToAX
    //! \param file
    //! \param aes
    //! \param paramNames
    //! \return
    //!
    signed int saveToAX (std::ofstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const;

    /****************************************************************************
    *					AxiomBase::readFromEC
    *
    *	Description:	Функция чтения параметров элементарного условия из одного .ec файла
    *					Формат файла описан в .formats.x
    *					Все считанный сущности записываются в специально определенную
    *					структуру, см. ElemCondPlusStruture.h.
    *	Parameters:		filePath - имя файла для обработки (путь к фйлу)
    *					ecps - заполняемая по файлу струтура типа ElemCondPlusStruture
    *	Returns:		0
    *	Throws:			AxiomLibException  - если файл с заданным путем не существует
    *							если формат файла не соглаосван описанием формата (см. formats.x)
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/


    // Функция считывания параметров элементарного условия из файла
    //!
    //! \brief readFromEC
    //! \param filePath
    //! \param ecps
    //! \return
    //!
    signed int readFromEC (const std::string& filePath, ElemCondPlusStruture &ecps) const;

    // Функция считывания параметров элементарного условия из файла

    /****************************************************************************
    *					AxiomBase::readFromEC
    *
    *	Description:	Функция чтения параметров элементарного условия из одного .ec файла
    *					Формат файла описан в .formats.x
    *					Все считанный сущности записываются в специально определенную
    *					структуру, см. ElemCondPlusStruture.h.
    *	Parameters:		filePath - имя файла для обработки (путь к фйлу)
    *					ecps - заполняемая по файлу струтура типа ElemCondDataDimensionStruture
    *					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
    *	Returns:		0
    *	Throws:			AxiomLibException  - если файл с заданным путем не существует
    *							если формат файла не соглаосван описанием формата (см. formats.x)
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief readFromEC
    //! \param filePath
    //! \param ecps
    //! \param paramNames
    //! \return
    //!
    signed int readFromEC (const std::string &filePath, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const;


    /****************************************************************************
    *					AxiomBase::readFromEC
    *
    *	Description:	Функция чтения параметров элементарного условия из одного .ec файла
    *					Формат файла описан в .formats.x
    *					Все считанный сущности записываются в специально определенную
    *					структуру, см. ElemCondPlusStruture.h.
    *	Parameters:		filePath - имя файла для обработки (путь к фйлу)
    *					ecps - заполняемая по файлу струтура типа ElemCondDataDimensionStruture
    *					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
    *					file - открытый файл из которого будет производиться считывание описания элементарного условия
    *	Returns:		0
    *	Throws:			AxiomLibException  - если файл с заданным путем не существует
    *							если формат файла не соглаосван описанием формата (см. formats.x)
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    // Чтение из уже открытого файла одного элементарного условия
    //!
    //! \brief readFromEC
    //! \param file
    //! \param ecps
    //! \param paramNames
    //! \return
    //!
    signed int readFromEC (boost::filesystem::ifstream &file, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const;

    // Функция записи параметров элементарного условия в файл

    /****************************************************************************
    *					AxiomBase::saveToEC
    *
    *	Description:	Функция сохранения параметров элементарного условия в файл
    *	Parameters:		filePath - полный путь к записываемому файлу с параметрами
    *					ecps - структура типа ElemCondPlusStruture с содержанием
    *						   параметров элементарного условия
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файл не удалось записать
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToEC
    //! \param baseDir
    //! \param fileName
    //! \param ecps
    //! \return
    //!
    signed int saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps) const;

    // Функция записи параметров элементарного условия в файл

    /****************************************************************************
    *					AxiomBase::saveToEC
    *
    *	Description:	Функция сохранения параметров элементарного условия в файл
    *	Parameters:		filePath - полный путь к записываемому файлу с параметрами
    *					ecps - структура типа ElemCondPlusStruture с содержанием
    *						   параметров элементарного условия
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файл не удалось записать
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToEC
    //! \param baseDir
    //! \param fileName
    //! \param ecps
    //! \param paramName
    //! \return
    //!
    signed int saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps, const std::string &paramName) const;

    // Функция записи параметров элементарного условия в файл

    /****************************************************************************
    *					AxiomBase::saveToEC
    *
    *	Description:	Функция сохранения параметров элементарного условия в файл
    *	Parameters:		filePath - полный путь к записываемому файлу с параметрами
    *					ecps - структура типа ElemCondPlusStruture с содержанием
    *						   параметров элементарного условия
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файл не удалось записать
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToEC
    //! \param baseDir
    //! \param fileName
    //! \param ecps
    //! \param paramNames
    //! \return
    //!
    signed int saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps, const std::vector <std::string> &paramNames) const;

    // Функция записи параметров элементарного условия в уже открытый файл

    /****************************************************************************
    *					AxiomBase::saveToEC
    *
    *	Description:	Функция сохранения параметров элементарного условия в файл
    *	Parameters:		filePath - полный путь к записываемому файлу с параметрами
    *					ecps - структура типа ElemCondPlusStruture с содержанием
    *						   параметров элементарного условия
    *					file - открытый файл, в который дописывается описание элементарного условия
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			AxiomLibExeption - если файл не удалось записать
    *	Author:			dk
    *	History:
    *
    ****************************************************************************/

    //!
    //! \brief saveToEC
    //! \param file
    //! \param ecps
    //! \param paramName
    //! \return
    //!
    signed int saveToEC (std::ofstream &file, ElemCondPlusStruture &ecps, const std::string &paramName) const;

    // Чтение из файла со списком названий файлов


    /****************************************************************************
    *					AxiomBase::readFromList
    *
    *	Description:	Чтение из файла со списком названий файлов
    *	Parameters:		baseDir - директория, где находится записываемый файл
    *					fileName - имя файла
    *					strList - структура, в которую считываются данные
    *	Returns:		0 - если все нормально, в противном случае выход по throw
    *	Throws:			-
    *	Author:			dk
    *	History:
    *

    ****************************************************************************/

    //!
    //! \brief readFromList
    //! \param filePath
    //! \param strList
    //! \return
    //!
    signed int readFromList (const std::string &filePath, std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const;

    // Чтение из файла со списком названий файлов
    //! Read from a file with a list of file names
    //! \brief Read from a file with a list of file names
    //! \param filePath - the file name
    //! \param StrList - a structure in which the data is readed
    //! \retval 0 - if all goes well
    //! \retval throw - otherwise
    //!
    signed int readFromList (const std::string &filePath, std::vector <std::vector <std::string> >  &strList) const;

    // Функция записи в файл списка с названиями файлов

    //! Function for writing to file a list of file names
    //! \brief Function for writing to file a list of file names
    //! \param baseDir - the directory where the writable file is
    //! \param fileName - the file name
    //! \param strList - structure with saving data
    //! \retval 0 - if all goes well
    //! \retval throw - otherwise
    //!
    signed int saveToList (const std::string &baseDir, const std::string &fileName, const std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const;

    // Функция записи в файл списка с названиями файлов
    //! Function to write to a file a list of file names
    //! \brief Function to write to a file a list of file names
    //! \param baseDir - the directory where the writable file is
    //! \param fileName - the file name
    //! \param strList - structure with saving data
    //! \retval 0 - if all goes well
    //! \retval throw - otherwise
    //!
    signed int saveToList (const std::string& baseDir, const std::string& fileName, const std::vector <std::vector <std::string> > &strList) const;

    // Проверяет, имеет ли имя файла заданное расширение
    //!
    //! \brief checkExtension
    //! \param fileName
    //! \param extension
    //! \return
    //!
    static bool checkExtension(const std::string& fileName, const std::string& extension);

    // Добавляет имени файла заданное расширение, если оно его уже не имеет
    //!
    //! \brief checkedAddExtension
    //! \param fileName
    //! \param extension
    //!
    static void checkedAddExtension(std::string& fileName, const std::string& extension);

}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_READAXIOMBASE_HXX */
