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

namespace AxiomLib {

class RecognizerMultiMarkup : public RecognizerImplBase {

private:

    int stretch; // используется при указании размеров окна


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

    // Задание параметров класса-потомка считываю имя создаю метрику
    int initFromEnv (const Environment& env) ;

    // Функция возвращает копию распознавателя
    // Рекомендуется реализовывать вызовом конструктора копирования
    Recognizer* copy (void) ;

};// end of class

}; //  end of namespace

#endif // RECOGNIZERMULTIMARKUP_H
