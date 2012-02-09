/****************************************************************************
*			DTWMetric
*
*	Description:	DTWMetric
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

   // create metric from mark
   DTWMetric();

   //
   ~DTWMetric() { }

 public:

   // choose metric type
   static DTWMetric* getMetric(std::string name);

   // calculate distance between 2 elements of mark
   virtual double compute(const std::vector<bool>& v1, const std::vector<bool>& v2);

   //
   //virtual double compute(MultiMark&  v1, MultiMark& v2);

   // compute distances for several windows
   static void computeDTW (DTWMetric* m, const MultiMark& t, int i,int Nmin,int Nmax, const MultiMark& ref, std::vector<double> result);

};// end of class

};//  end of namespace MultiMarking

};//  end of namespace AxiomLib

#endif // DTWMETRIC_H
