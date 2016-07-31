/****************************************************************************
*				TreatmentFactory
*
*	Description:	Фабричный метод создания классов предобработки
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_TREATMENT_FACTORY_HXX
#define __AXIOMS_LIB_TREATMENT_FACTORY_HXX

#include "Treatment.h"
#include "AllTreatments.h"
#include <string>

namespace AxiomLib {

class TreatmentFactory {

 private:

 protected:
 
 public:

  TreatmentFactory(void) {}
  
  ~TreatmentFactory(void) {}
  
  Treatment* create (const std::string name);
  
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_TREATMENT_FACTORY_HXX */
