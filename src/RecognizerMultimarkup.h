/****************************************************************************
*			RecognizerMultiMarkup
*
*	Description:  RecognizerMultiMarkup
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef RECOGNIZERMULTIMARKUP_H
#define RECOGNIZERMULTIMARKUP_H

#include "RecognizerImplBase.h"
#include "MultiMarking/dtwmetric.h"
#include "MultiMarking/AxiomUsageWrapper.h"
#include "Environment.h"
#include "AxiomSet.h"
#include "DataSet.h"

namespace AxiomLib {

typedef double (*ResultStrategy)(const std::vector<double>& v);

class RecognizerMultiMarkup : public RecognizerImplBase {

private:
	double mStretch; // используется при указании размеров окна
	MultiMarking::DTWMetric* mMetric;
	std::string mMetricName; // name of metric
	ResultStrategy mResultStrategy;
	bool mCancelAxioms;

    ~RecognizerMultiMarkup();

protected:

    // Вычисление расстояний от участков разметок тестовых траекторий до разметок эталонных траекторий
    void computeDistances(AxiomSet& axiomSet, DataSet& dataSet, std::vector<int>& params, // номера столбцов кторые надо использовать
                                                              // расстояния от участков разметок тестовых траекторий до разметок эталонных траекторий
                                                              std::vector<TrajectorySampleDistance>& result,
                                                              // какие аксиомы срабатывали при разметке эталонных траекторий
                                                              std::vector<bool>& refAxiomUsage,
                                                              // какие аксиомы срабатывали при разметке тестовых траекторий
                                                              std::vector<bool>& testAxiomUsage) ;
public:

	RecognizerMultiMarkup() : mMetric(0) {}
    // Задание параметров класса-потомка считываю имя создаю метрику
    int initFromEnv (const Environment& env) ;

    // Функция возвращает копию распознавателя
    Recognizer* copy (void) ;

    RecognizerMultiMarkup(const RecognizerMultiMarkup& other);

};// end of class

}; //  end of namespace

#endif // RECOGNIZERMULTIMARKUP_H
