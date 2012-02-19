/****************************************************************************
*			DTWMetric
*
*	Description:	DTWMetric - � ласс-родитель для всех метрик,
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

   //
   DTWMetric();

   //
   ~DTWMetric() { }

 public:

   //
   static DTWMetric* getMetric(std::string name);

   //
   virtual double compute(const std::vector<bool>& v1, const std::vector<bool>& v2)=0;

   //
   static void computeDTW (DTWMetric* m, const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double> result);

};// end of class

};//  end of namespace MultiMarking

};//  end of namespace AxiomLib

#endif // DTWMETRIC_H
