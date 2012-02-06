/****************************************************************************
*					ReducedRecognizerFactory
*
*	Description:	Фабричный метод создания сокращенных классов распознавания
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_RECOGNIZER_FACTORY_HXX
#define __AXIOMS_LIB_RECOGNIZER_FACTORY_HXX

#include "AllReducedRecognizers.h"
#include "AxiomLibException.h"
#include <string>

namespace AxiomLib {

class ReducedRecognizerFactory {

 private:

 protected:
 
 public:

	ReducedRecognizerFactory (void) {}
  
	~ReducedRecognizerFactory (void) {}
  
	ReducedRecognizer* create (const std::string &name) const;
  
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_RECOGNIZER_FACTORY_HXX */
