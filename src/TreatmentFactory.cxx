/****************************************************************************
*
*			Функции класса TreatmentFactory
*
****************************************************************************/

#include "TreatmentFactory.h"
#include "AxiomLibException.h"

using namespace AxiomLib;

/****************************************************************************
*			TreatmentFactory::create
*
*	Description:	Создает класс предобработки
*	Parameters:	name - имя класса, экземпляр которого создаем
*	Returns:		Treatment* - указатель на созданный класс
*	Throws:		AxiomLibException - если класса нет
*	Author:		dk
*	History:
*
****************************************************************************/
Treatment* TreatmentFactory::create (const std::string name) {
  if (name == "TreatmentSample") {
    TreatmentSample* toReturn;
    toReturn = new TreatmentSample;
    return toReturn;
  }
  throw AxiomLibException ("TreatmentFactory::create : can not find the requested treatment strategy.");
}
