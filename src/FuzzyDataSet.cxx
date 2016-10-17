#include <iostream>
#include <boost/filesystem.hpp>

#include "FuzzyDataSet.h"

namespace AxiomLib {

// TODO перенести в конфигурацию
const char *FuzzyDataSet::divisionNames[] = { "reference", "testing", "validation" };
	
/****************************************************************************
*				ClippedFullFuzzyDataSet()
*
*	Description:  Конструктор умолчания
*	Parameters:	
*	Returns:	
*	Throws:		
*	Author:		wictor
*	History:	-
*
****************************************************************************/
FuzzyDataSet::FuzzyDataSet() : DataSetBase()
{
    setNormalStr("normal");
}

bool FuzzyDataSet::isEmpty()
{
    return divisions.empty();
}

void FuzzyDataSet::setDivisions(std::vector<DataSetDivision> &divisions)
{
    this->divisions.swap(divisions);
    this->divisions.resize(Last - First);

    paramNames.clear();
    paramNums.clear();

    int dimensionCount = this->divisions.front().getDimensionCount();

    if (dimensionCount == 0) {
        throw AxiomLibException("FuzzyDataSet::setDivisions(): ill-formed division vector: dimension count is 0");
    }

    for (int i  = 0; i < dimensionCount; ++i) {
        paramNames.push_back(boost::lexical_cast<std::string>(i));
        paramNums.push_back(i);
    }
}

/****************************************************************************
*				readDataSet
*
*	Description:	Чтение датасета из файла и установка интервалов
*			по границам рядов
*	Parameters:	dataSetDir - папка датасетов
*			dataSetName - имя датасета
*	Returns:	0
*	Throws:		-
*	Author:		wictor
*	History:	-
*
****************************************************************************/
signed int FuzzyDataSet::readDataSet(std::string dataSetDir, std::string dataSetName) {	
	readDataSet(dataSetDir + "/" + dataSetName);
	return 0;
}

int FuzzyDataSet::readDataSet(const std::string &dataSetPathString) {
	
	DataSetBase::readDataSet(dataSetPathString);
	
	divisions.clear();
	divisions.reserve(FuzzyDataSet::Last - FuzzyDataSet::First);
	
	boost::filesystem::path dataSetPath = boost::filesystem::path(dataSetPathString);
	for(int i = FuzzyDataSet::First; i < FuzzyDataSet::Last; i++) {
		boost::filesystem::path divisionPath = dataSetPath / std::string(divisionNames[i]);
		
		ReferenceClassesTS refClasses;
		ClassTS normalClass;
		
		if(!boost::filesystem::exists(divisionPath)) {
			//throw AxiomLibException("Division "+std::string(divisionNames[i])+" not found in dataset in path "+dataSetPathString);
			divisions.push_back(
					DataSetDivision(refClasses, normalClass)
					);
			continue;
		}
		
		this->readReferenceTS(divisionPath.string(), refClasses);
		this->readNormalTS(divisionPath.string(), normalClass);
		divisions.push_back(
				DataSetDivision(refClasses, normalClass)
				);
		
//		std::vector<int> v;
//		divisions[i].getNormalSize(v);
		
//		std::cout<<"Division :"<<this->divisionNames[i]<<", class count :"<<divisions[i].getClassCount()
//				<<", normal TS count :"<<v.size()<<std::endl;
//		for(int i = 0; i < v.size(); i++) {
//			std::cout<<"dimension count : "<<v[i]<<std::endl;
//		}
		
	}
	
	return 0;
}

std::string FuzzyDataSet::getMultiTSName(FuzzyDataSet::DataSetDivisionType division, int indexClass, int indexMultiTS) const
{
	return divisions[division].getMultiTSName(indexClass, indexMultiTS);
}

bool FuzzyDataSet::getTSByIndex(DataSetDivisionType division, std::vector<double>& vec, int indexClass, int indexMultiTS, int indexTS) const {
	return divisions[division].getTSByIndex(vec, indexClass, indexMultiTS, indexTS);
}

bool FuzzyDataSet::getMultiTSByIndex(FuzzyDataSet::DataSetDivisionType division,
                                     std::vector<std::vector<double> > &v,
                                     int indexClass,
                                     int indexMultiTS, bool omitFirstDimension) const
{
    unsigned dimCount = (unsigned)getDimensionCount();
    if (omitFirstDimension) --dimCount;
    v.resize(dimCount);
    for(unsigned i = (omitFirstDimension ? 1 : 0); i < (unsigned)getDimensionCount(); ++i) {
        int dstIndex = i;
        if (omitFirstDimension) dstIndex = i - 1;
        bool ok =  divisions[division].getTSByIndex(v[dstIndex], indexClass, indexMultiTS, i);
		if(!ok) {
			return false;
		}
	}

	return true;
}

/****************************************************************************
*				getTSByIndexFromClass
*
*	Description:	Возвращает ряд из обучающей выборки 
*					по индексам класса, траектории и размерности
*	Parameters:	vec - заполняемый вектор
*				indexClass - индекс класса
*				indexMultiTS - индекс мультиряда
*				indexTS - индекс ряда
*	Returns:	
*	Throws:	
*	Author:		wictor
*	History:	-
*
****************************************************************************/
bool FuzzyDataSet::getTSByIndexFromClass(std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const {
	return getTSByIndex(FuzzyDataSet::Reference, vec, indexClass, indexMultiTS, indexTS);
}

/****************************************************************************
*					getTSByIndexFromTest
*
*	Description:	Функция возвращает временной ряд из обучающей выборки по 
*					определенным - номеру класса, номеру мультиряда в классе, 
*					номеру ряда в мультиряде
*	Parameters:		vec - заполняемый временной ряд
*					indexClass - номер класса неисправностей во внутреннем 
*						представлении данного класса, откуда считывать ряд
*					indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*					indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*					false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:			-
*	Author:			wictor
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet::getTSByIndexFromTest(std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const {
	return getTSByIndex(FuzzyDataSet::Testing, vec, indexClass, indexMultiTS, indexTS);}

/****************************************************************************
*				getNormalTSFromClass
*
*	Description:	Функция возвращает временной ряд, соответствующий нормальному 
*					поведению, из обучающей выборки по определенным - номеру 
*					мультиряда, номеру ряда в мультиряде.
*	Parameters:		vec - заполняемый временной ряд
*					indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*					indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*					false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:			-
*	Author:			wictor
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet::getNormalTSFromClass(std::vector<double> &vec, int indexMultiTS, int indexTS) const {
	return getTSByIndex(FuzzyDataSet::Reference, vec, -1, indexMultiTS, indexTS);
}

/****************************************************************************
*					FullFuzzyDataSet::getNormalTSFromTest
*
*	Description:	Функция возвращает временной ряд, соответствующий нормальному 
*					поведению, из контрольной выборки по определенным - номеру 
*					мультиряда, номеру ряда в мультиряде.
*	Parameters:		vec - заполняемый временной ряд
*					indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*					indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*					false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:			-
*	Author:			wictor
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet::getNormalTSFromTest(std::vector<double> &vec, int indexMultiTS, int indexTS) const {
	return getTSByIndex(FuzzyDataSet::Testing, vec, -1, indexMultiTS, indexTS);
}

///****************************************************************************
//*				 getClippingIntervals
//*
//*	Description: Возвращает набор всех интервалов отсечения
//*	Parameters:	 -
//*	Returns:	 const IntervalSetPair & - ссылка на набор всех интервалов отсечения
//*	Throws:		 -
//*	Author:		wictor
//*	History:	-
//*
//****************************************************************************/
//const IntervalSetPair &ClippedFullFuzzyDataSet::getClippingIntervals() const {
//	return clippingIntervals;
//}

///****************************************************************************
//*				 setClippingIntervals
//*
//*	Description: Устанавливает набор всех интервалов отсечения
//*	Parameters:	 intervals - новый набор интервалов отсечения
//*	Returns:	 -
//*	Throws:		 -
//*	Author:		wictor
//*	History:	-
//*
//****************************************************************************/
//void ClippedFullFuzzyDataSet::setClippingIntervals(const IntervalSetPair &intervals) {
//	// TODO: сделать проверку того, что новый набор совпадает по размеру со старым
//	clippingIntervals = intervals;
//}

void FuzzyDataSet::setClippingInterval(DataSetDivisionType division, const IntInterval &interval, int classNo, int multiTSNo) {
	divisions[division].setClippingInterval(interval, classNo, multiTSNo);
}

/****************************************************************************
*				 setClassClippingInterval
*
*	Description: Устанавливает интервал отсечения для заданной траектории 
*					обучающей выборки
*	Parameters:	 interval - новый интервал отсечения
*				classNo, multiTSNo - индексы, задающие траекторию
*				(classNo < 0 означает нормальное поведение)
*	Returns:	 -
*	Throws:		 -
*	Author:		wictor
*	History:	-
*
****************************************************************************/
void FuzzyDataSet::setClassClippingInterval(const IntInterval &interval, int classNo, int multiTSNo) {
	setClippingInterval(FuzzyDataSet::Reference, interval, classNo, multiTSNo);
}

/****************************************************************************
*				 setTestClippingInterval
*
*	Description: Устанавливает интервал отсечения для заданной траектории 
*					контрольной выборки
*	Parameters:	 interval - новый интервал отсечения
*				classNo, multiTSNo - индексы, задающие траекторию
*				(classNo < 0 означает нормальное поведение)
*	Returns:	 -
*	Throws:		 -
*	Author:		wictor
*	History:	-
*
****************************************************************************/
void FuzzyDataSet::setTestClippingInterval(const IntInterval &interval, int classNo, int multiTSNo) {
	setClippingInterval(FuzzyDataSet::Testing, interval, classNo, multiTSNo);
}

IntInterval FuzzyDataSet::getClippingInterval(DataSetDivisionType division, int classNo, int multiTSNo) const {
	return divisions[division].getClippingInterval(classNo, multiTSNo);
}

/****************************************************************************
*				 getClassClippingInterval
*
*	Description: Возвращает интервал отсечения для заданной траектории 
*					обучающей выборки
*	Parameters:	 
*				classNo, multiTSNo - индексы, задающие траекторию
*				(classNo < 0 означает нормальное поведение)
*	Returns:	 IntInterval
*	Throws:		 -
*	Author:		wictor
*	History:	-
*
****************************************************************************/
IntInterval FuzzyDataSet::getClassClippingInterval(int classNo, int multiTSNo) const {
	return getClippingInterval(FuzzyDataSet::Reference, classNo, multiTSNo);
}

/****************************************************************************
*				 getTestClippingInterval
*
*	Description: Возвращает интервал отсечения для заданной траектории 
*					контрольной выборки
*	Parameters:	 
*				classNo, multiTSNo - индексы, задающие траекторию
*				(classNo < 0 означает нормальное поведение)
*	Returns:	 IntInterval
*	Throws:		 -
*	Author:		wictor
*	History:	-
*
****************************************************************************/
IntInterval FuzzyDataSet::getTestClippingInterval(int classNo, int multiTSNo) const {
	return getClippingInterval(FuzzyDataSet::Testing, classNo, multiTSNo);
}

int FuzzyDataSet::getClassCount() const {
	// TODO: инвариант: число классов нештатного поведения должно быть одинаково
	// во всех папках
	return divisions[FuzzyDataSet::Reference].getClassCount();
}

int FuzzyDataSet::getDimensionCount() const {
	// TODO инвариант: число размерностей должно быть везде одинаковым
	
	// TODO проверить, что это то
	return this->paramNamesSize();
}

int FuzzyDataSet::getMutiTSCount(DataSetDivisionType division, int classNo) const {
	return divisions[division].getMultiTSCount(classNo);
}

int FuzzyDataSet::getMultiTSLength(DataSetDivisionType division, int classNo, int multiTSNo) const {
	return divisions[division].getMultiTSLength(classNo, multiTSNo);
}

/****************************************************************************
*				 getClassMultiTSSize
*
*	Description: Возвращает длину заданной траектории обучающей выборки
*	Parameters:	 
*				classNo, multiTSNo - индексы, задающие траекторию
*				(classNo < 0 означает нормальное поведение)
*	Returns:	 int
*	Throws:		 -
*	Author:		wictor
*	History:	-
*
****************************************************************************/
int FuzzyDataSet::getClassMultiTSLength(int classNo, int multiTSNo) const {
	return getMultiTSLength(FuzzyDataSet::Reference, classNo, multiTSNo);
}

/****************************************************************************
*				 getTestMultiTSSize
*
*	Description: Возвращает длину заданной траектории контрольной выборки
*	Parameters:	 
*				classNo, multiTSNo - индексы, задающие траекторию
*				(classNo < 0 означает нормальное поведение)
*	Returns:	 int
*	Throws:		 -
*	Author:		wictor
*	History:	-
*
****************************************************************************/
int FuzzyDataSet::getTestMultiTSLength(int classNo, int multiTSNo) const {
	return getMultiTSLength(FuzzyDataSet::Testing, classNo, multiTSNo);
}

bool FuzzyDataSet::getSize(DataSetDivisionType division, int &v1, std::vector<int> &v2, std::vector<std::vector<int> > &v3) const {
	if(!divisions[division].getAbnormalSize(v3)) {
		return false;
	}
        v1 = v3.size();
	v2.clear();
	v2.reserve(v3.size());
        for(int i = 0; i < v3.size(); ++i) {
            v2.push_back(v3[i].size());
        }
	return true;
}

bool FuzzyDataSet::getClassSize(int &v1, std::vector<int> &v2, std::vector<std::vector<int> > &v3) const {
	return getSize(FuzzyDataSet::Reference, v1, v2, v3);
}

bool FuzzyDataSet::getTestSize(int &v1, std::vector<int> &v2, std::vector<std::vector<int> > &v3) const {
	return getSize(FuzzyDataSet::Testing, v1, v2, v3);
}

bool FuzzyDataSet::getNormalSize(DataSetDivisionType division, int &v1, std::vector<int> &v2) const {
	if(!divisions[division].getNormalSize(v2))
	{
            return false;
	}
	v1 = v2.size();
	
	return true;
}

bool FuzzyDataSet::getNormalClassSize(int &v1, std::vector<int> &v2) const {
	return getNormalSize(FuzzyDataSet::Reference, v1, v2);
}

bool FuzzyDataSet::getNormalTestSize(int &v1, std::vector<int> &v2) const {
	return getNormalSize(FuzzyDataSet::Testing, v1, v2);
}

bool FuzzyDataSet::getSizeForClass(DataSetDivisionType division, int classNo, std::vector<int> &v) const {
	return divisions[(int)division].getClassSize(classNo, v);
}

void FuzzyDataSet::readNormalTS(const std::string &divisionPath, ClassTS &classTS) {
	boost::filesystem::path normalClassPath(divisionPath);
	normalClassPath /= this->normal;
	
	if(!boost::filesystem::exists(normalClassPath)) {
		classTS.resize(0);
	} else {	
		this->readClassTS(normalClassPath.string(), classTS);
	}
}

void FuzzyDataSet::setNormalStr(EnvDataSet &env) {
	std::string s;
	int r = env.getNormalStr(s);
	if(r == 0) {
		setNormalStr(s);
	}
}

void FuzzyDataSet::setNormalStr(const std::string &str) {
	this->normal = str;
}

}
