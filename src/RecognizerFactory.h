/****************************************************************************
*				RecognizerFactory
*
*	Description:	Фабричный метод создания классов распознавания
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_RECOGNIZER_FACTORY_HXX
#define __AXIOMS_LIB_RECOGNIZER_FACTORY_HXX

#include "AxiomLibException.h"
#include <string>

namespace AxiomLib {

class RecognizerFactory {

 private:

 protected:
 
 public:

  RecognizerFactory(void) {}
  
  ~RecognizerFactory(void) {}
  
  Recognizer* create (const std::string &name);
  
};

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_RECOGNIZER_FACTORY_HXX */
