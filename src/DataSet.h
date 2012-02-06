/****************************************************************************
*			DataSet
*
*	Description:	Класс --- набор данных (см. файл ../formats.x)
*				Служит для чтения данных из каталога в программу,
*				хранения данных, их выдачи по запросу
*	Author:		eav
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_DataSet_HXX
#define __AXIOMS_LIB_DataSet_HXX

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>

#include "AxiomLibException.h"
#include "Environment.h"
#include "EnvDataSet.h"
#include "TestTS.h"
#include "DataSetBase.h"

namespace AxiomLib {

class DataSet : public DataSetBase {

  private:
  
  protected:
	// Считанные эталонные ряды для классов неисправностей
	ReferenceClassesTS referenceClassesTS;
  
	// Считанные тестовые ряды
	TestSets testSets;

	// функция браузит по файлам и читает этолонные ряды для каждого из классов
	signed int readClassesTS(const std::string &path);
  
	// функция браузит по файлам  и читает тестовую выборку
	signed int readTestesTS (const std::string &path);
    
  public:
    
	// Дефолтный конструктор
	DataSet(void);
	
	// Читает набор данных из каталога path (формат данных в каталоге описан в файле ../formats.x
	signed int readDataSet(const std::string &dataSetDir, const std::string &dataSetName);	
	
	signed int readDataSet(const std::string &baseDataSetDir);
 
	// Функция возвращает временной ряд из переменных класса по определенным - номеру класса, номеру мультиряда в классе, номеру ряда в мультиряде
	bool getTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const;

	// Функция устанавливает новое значение ряду по определенным - номеру класса, номеру мультиряда в классе, номеру ряда в мультиряде
	bool setTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS);
	
	// Функция возвращает число классов нештатного поведения
	int getNumberOfClasses() const;
	
	// Функция возвращает число элементов в струтуре referenceClassesTS
	bool getClassSize (int &numOfClasses, std::vector<int> &numOfMultiTS, std::vector<std::vector<int> > &numOfTS) const;
	
	// Функция возвращает число элементов в струтуре testSets
	bool getTestSize (int &numOfTests, std::vector<int> &numOfTS) const;

	// Функция возвращает временной ряд по номеру теста, по номеру ряда в мультиряде в тесте
	bool getTSByIndexFromTests (std::vector<double> &vec, int indexTest, int indexTS) const;

	// Функция устанавливает новый ряд по номеру теста, по номеру ряда в мультиряде в тесте
	bool setTSByIndexFromTests (std::vector<double> &vec, int indexTest, int indexTS);
	
	// Функция возвращает временной ряд по номеру теста, по номеру ряда в мультиряде в тесте
	bool getTSByIndexFromTests (std::vector<int> &vec, int indexTest, int indexTS) const;
	
	// Функция устанавливает новый ряд по номеру теста, по номеру ряда в мультиряде в тесте
	bool setTSByIndexFromTests (std::vector<int> &vec, int indexTest, int indexTS);
	
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_DataSet_HXX */
