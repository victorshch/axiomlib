/****************************************************************************
*					FullFuzzyDataSet
*
*	Description:	Класс - расширение набора хранимых данных за счет траекторий 
*					нормального поведения в обучающей выборке. Кроме того, 
*					в данном способе задания выборки нет точно размеченных траекторий. 
*					Есть только траектории, содержащие нештатное поведение, и
*					траектории нормального поведения.
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_FullFuzzyDataSet_HXX
#define __AXIOMS_LIB_FullFuzzyDataSet_HXX

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

class FullFuzzyDataSet : public DataSet {

  private:
 
	// Строка - кторой обозначается класс неисправнойстей, которому соответстует нормальное поведение системы
	std::string normal;
	  
  protected:
	
	// Дополнение траекторий обучающей выборки - траекториями нормального поведения
	// Считанные эталонные ряды траекторий нормального поведения из Reference
	ClassTS referenceNormalTS;

	// Траектории контрольной выборки задаются так же как и траектории из обучающей выборки
	// Считанные эталонные ряды для классов неисправностей
	ReferenceClassesTS testingClassesTS;

	// Считанные эталонные ряды траекторий нормального поведения из Testing
	ClassTS testingNormalTS;	

	// Читает траектории относящиеся к классу нормального поведения из указанного пути
	signed int readNormalClassTS (std::string &path, ClassTS &normalTS);
 
  public:
    
	// Дефолтный конструктор
	FullFuzzyDataSet(void);
 
	// Читает набор данных из указанного каталога
	signed int readDataSet(std::string dataSetDir, std::string dataSetName);

	// Функция возвращает временной ряд нормального поведения из обучающей выборки по определенным - номеру мультиряда в наборе, номеру ряда в мультиряде
	bool getNormalTSFromClass (std::vector<double> &vec, int indexMultiTS, int indexTS) const;

	// Функция возвращает временной ряд нормального поведения из контрольной выборки по определенным - номеру мультиряда в наборе, номеру ряда в мультиряде
	bool getNormalTSFromTest (std::vector<double> &vec, int indexMultiTS, int indexTS) const;

	// Функция выдает информацию о числе временных рядов соответстующих нормальному поведению в обучающей выборке
	bool getNormalClassSize (int &numOfMultiTS, std::vector<int> &numOfTS) const;

	// Функция выдает информацию о числе временных рядов соответстующих нормальному поведению в контрольной выборке
	bool getNormalTestSize (int &numOfMultiTS, std::vector<int> &numOfTS) const;

	// Функция возвращает временной ряд из обучающей выборки по определенным - номеру класса, номеру мультиряда в классе, номеру ряда в мультиряде
	bool getTSByIndexFromTest (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const;

	// Функция возвращает число элементов в струтуре testingClassesTS
	bool getTestSize (int &numOfClasses, std::vector<int> &numOfMultiTS, std::vector<std::vector<int> > &numOfTS) const;

	// Функция устанавливает новое обозначение под именем котороого в обучающей выборке находятся траектории нормального поведения
	signed int setNormalStr (EnvDataSet &envDataSet);
	
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_FullFuzzyDataSet_HXX */
