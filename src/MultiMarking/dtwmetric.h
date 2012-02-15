/****************************************************************************
*			DTWMetric
*
*	Description:	DTWMetric - Класс-родитель для всех метрик,
*                       используемых для работы DTW алгоритма.
*
*	Author:		armkor
*	History:
*
****************************************************************************/

#ifndef DTWMETRIC_H
#define DTWMETRIC_H

#include "multimarks.h"

namespace AxiomLib {

namespace MultiMarking {

class DTWMetric {

 private:

   // Создание метрики
   DTWMetric();

   //
   ~DTWMetric() { }

 public:

   // Выбор типа метрики. Возвращается указатель на класс-метрику, которая используется
   static DTWMetric* getMetric(std::string name);

   // Вычисление расстояния между двумя разметками
   virtual double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);

   // Вычисление расстояния для одного типа окна
   static void computeDTW (DTWMetric* m, const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double> result);

};// end of class

};//  end of namespace MultiMarking

};//  end of namespace AxiomLib

#endif // DTWMETRIC_H
