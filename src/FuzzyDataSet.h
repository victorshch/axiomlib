/****************************************************************************
*				ClippedFullFuzzyDataSet
*
*	Description: Набор данных для алгоритма FuzzyMultiData, поддерживающий
*				отсечение временных рядов
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef CLIPPEDFULLFUZZYDATASET_H
#define CLIPPEDFULLFUZZYDATASET_H

#include <vector>
#include <boost/lexical_cast.hpp>

#include "DataSetDivision.h"
#include "EnvDataSet.h"
#include "DataSetBase.h"
#include "IntInterval.h"

namespace AxiomLib {
	
class FuzzyDataSet : public DataSetBase
{
	
public:
	// Разделы датасета
	enum DataSetDivisionType { First = 0, Reference = 0, Testing = 1, Validation = 2, Last = Validation };
	
	// Имена разделов датасета (т.е. соответствующих папок)
	// Определение см. в .cxx
	static const char *divisionNames[];
	
	// Конструктор умолчания
    FuzzyDataSet();
	
	// Чтение датасета
	signed int readDataSet(std::string dataSetDir, std::string dataSetName);
	
	int readDataSet(const std::string &dataSetPath);
	
	std::string getMultiTSName(DataSetDivisionType division, int indexClass, int indexMultiTS) const;

	// Возвращает одномерный ряд по разделу, номеру класса, траектории и размерности
	bool getTSByIndex(DataSetDivisionType division, std::vector<double>& vec, int indexClass, int indexMultiTS, int indexTS) const;
	
	// Функция возвращает временной ряд из переменных класса по определенным - номеру класса, номеру мультиряда в классе, номеру ряда в мультиряде
	bool getTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const;	
	
	// Функция возвращает временной ряд из обучающей выборки по определенным - номеру класса, номеру мультиряда в классе, номеру ряда в мультиряде
	bool getTSByIndexFromTest (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const;
	
	// Функция возвращает временной ряд нормального поведения из обучающей выборки по определенным - номеру мультиряда в наборе, номеру ряда в мультиряде
	bool getNormalTSFromClass (std::vector<double> &vec, int indexMultiTS, int indexTS) const;

	// Функция возвращает временной ряд нормального поведения из контрольной выборки по определенным - номеру мультиряда в наборе, номеру ряда в мультиряде
	bool getNormalTSFromTest (std::vector<double> &vec, int indexMultiTS, int indexTS) const;
	
//	// Функция устанавливает сразу все интервалы отсечения
//	void setClippingIntervals(const IntervalSetPair &intervals);
	
//	// Возвращает набор интервалов отсечения для обучающей выборки
//	const IntervalSetPair &getClippingIntervals() const;
	
	// Устанавливает интервал отсечения для заданного раздела, класса и траектории
	void setClippingInterval(DataSetDivisionType division, const IntInterval &interval, int classNo, int multiTSNo);
	
	// Функция устанавливает интервал отсечения для заданной обучающей траектории 
	// (classNo = -1 - нормальное поведение)
	void setClassClippingInterval(const IntInterval &interval, int classNo, int multiTSNo);
	
	// Функция устанавливает интервал отсечения для заданной тестовой траектории
	// (classNo = -1 - нормальное поведение)
	void setTestClippingInterval(const IntInterval &interval, int classNo, int multiTSNo);
	
	IntInterval getClippingInterval(DataSetDivisionType division, int classNo, int multiTSNo) const;
	
	// Функция возвращает интервал отсечения для заданной обучающей траектории 
	IntInterval getClassClippingInterval(int classNo, int multiTSNo) const;
		
	// Функция возвращает интервал отсечения для заданной тестовой траектории 
	IntInterval getTestClippingInterval(int classNo, int multiTSNo) const;
	
	// Возвращает длину траектории
	int getMultiTSLength(DataSetDivisionType division, int classNo, int multiTSNo) const;
	
	// Функция возвращает длину мультиряда
	int getClassMultiTSLength(int classNo, int multiTSNo) const;
	
	// Функция возвращает длину мультиряда
	int getTestMultiTSLength(int classNo, int multiTSNo) const;
	
	// Число классов нештатного поведения
	int getClassCount() const;

	// Общее число размерностей (включая время)
	int getDimensionCount() const;
	
	// Число мультирядов в классе
	int getMutiTSCount(DataSetDivisionType division, int classNo) const;
	
	// Получение размеров классов нештатного поведения для заданного раздела
	bool getSize(DataSetDivisionType division, int &v1, std::vector<int> &v2, std::vector<std::vector<int> > &v3) const;

	// Получение размеров классов нештатного поведения reference
	bool getClassSize(int &v1, std::vector<int> &v2, std::vector<std::vector<int> > &v3) const;
	
	// Получение размеров классов нештатного поведения testing
	bool getTestSize(int &v1, std::vector<int> &v2, std::vector<std::vector<int> > &v3) const;
	
	// Получение размера класса нормального поведения заданного раздела
	bool getNormalSize(DataSetDivisionType division, int &v1, std::vector<int> &v2) const;
	
	// Получение размера класса нормального поведения reference
	bool getNormalClassSize(int &v1, std::vector<int> &v2) const;
	
	// Получение размера класса нормального поведения testing
	bool getNormalTestSize(int &v1, std::vector<int> &v2) const;
	
	// Получение размера заданного класса из заданного раздела (classNo = -1 -- нормальное поведение)
	bool getSizeForClass(DataSetDivisionType division, int classNo, std::vector<int> &v) const;
		
	void setNormalStr(EnvDataSet &env);
	
	void setNormalStr(const std::string& str);

	void bootstrapNormalBehavior(double p);
private:
	
	std::vector<DataSetDivision> divisions;
	
	void readNormalTS(const std::string &divisionPath, ClassTS &classTS);
	
	std::string normal;
	
};

};

#endif // CLIPPEDFULLFUZZYDATASET_H
