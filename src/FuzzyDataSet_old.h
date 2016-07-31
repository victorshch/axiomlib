/****************************************************************************
*					FuzzyDataSet
*
*	Description:	Класс - расширение набора хранимых данных за счет траекторий 
*					нормального поведения в обучающей выборке
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_FuzzyDataSet_HXX
#define __AXIOMS_LIB_FuzzyDataSet_HXX

#include "AxiomLibException.h"
#include "Environment.h"
#include "EnvDataSet.h"
#include "DataSet.h"
#include "MultiTS.h"
#include "TestTS.h"
#include <iostream>
#include <string>
#include <vector>

namespace AxiomLib {

class FuzzyDataSet_old : public DataSet {

  private:
 
	// Строка - кторой обозначается класс неисправнойстей, которому соответстует нормальное поведение системы
	std::string normal;
	
	// Считанные эталонные ряды для классов неисправностей
	ClassTS normalTS;
  
  protected:
 
  public:
    
	// Дефолтный конструктор
	FuzzyDataSet_old(void);
 
	// Читает набор данных из указанного каталога
	signed int readDataSet(std::string dataSetDir, std::string dataSetName);
	
	// Читает траектории относящиеся к классу нормального поведения из указанного пути
	signed int readNormalClassTS (std::string &path);
	
	// Функция возвращает временной ряд, соответствующий нормальному поведению, из переменных класса по определенным - номеру мультиряда, номеру ряда в мультиряде
	bool getNormalTS (std::vector<double> &vec, int indexMultiTS, int indexTS) const;

	// Функция устанавливает новое значение ряду обучающей выборки, соответстующему нормальному поведению
	bool setNormalTS (std::vector<double> &vec, int indexMultiTS, int indexTS);

	// Функция выдает информацию о числе временных рядов соответстующих нормальному поведению
	bool getNormalClassSize (int &numOfMultiTS, std::vector<int> &numOfTS) const;

	// Функция устанавливает новое обозначение под именем котороого в обучающей выборке находятся траектории нормального поведения
	signed int setNormalStr (EnvDataSet &envDataSet);
	
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_FuzzyDataSet_HXX */
