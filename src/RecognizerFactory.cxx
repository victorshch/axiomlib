/****************************************************************************
*
*			Функции класса RecognizerFactory
*
****************************************************************************/

#include "AllRecognizers.h"
#include "RecognizerFactory.h"
#include <boost/algorithm/string/case_conv.hpp>

using namespace AxiomLib;

/****************************************************************************
*			RecognizerFactory::create
*
*	Description:	Создает класс распознавания
*	Parameters:	name - имя класса, экземпляр которого создаем
*	Returns:		Recognizer* - указатель на созданный класс
*	Throws:		AxiomLibException - если класса нет
*	Author:		dk
*	History:
*
****************************************************************************/
Recognizer* RecognizerFactory::create (const std::string &name) {
  if ( (name == "Common") || (name == "COMMON") || (name == "common") ) {
    RecognizerExtend* toReturn;
    toReturn = new RecognizerExtend;
    return toReturn;
  }
  if ( (name == "Metric") || (name == "METRIC") || (name == "metric") || (name == "DTW") || (name == "dtw") ) {
    RecognizerMetric* toReturn;
    toReturn = new RecognizerMetric;
    return toReturn;
  }
  if ( (name == "Net") || (name == "NET") || (name == "net") || (name == "Neuronet") || (name == "NEURONET") || (name == "neuronet") ) {
    RecognizerNet* toReturn;
    toReturn = new RecognizerNet;
    return toReturn;
  }
  if ( (name == "MultiMarking") || (name == "MULTIMARKING") || (name == "multimarking") || (name == "Multimarking") ) {
    RecognizerMultiMarkup* toReturn;
    toReturn = new RecognizerMultiMarkup;
    return toReturn;
  }
  // преобразование в строчные буквы
  std::string nameCopy = boost::algorithm::to_lower_copy(name);
  if(nameCopy == "neuralpercsingle") {
	RecognizerPercSingle* toReturn;
    toReturn = new RecognizerPercSingle;
    return toReturn;
  }
  if(nameCopy == "neuralpercmulti") {
	RecognizerPercMulti* toReturn;
    toReturn = new RecognizerPercMulti;
    return toReturn;
  }
  if(nameCopy == "neuralqnarypercsingle") {
	RecognizerQnaryPercSingle* toReturn;
    toReturn = new RecognizerQnaryPercSingle;
    return toReturn;
  }
  if(nameCopy == "neuralqnarypercmulti") {
	RecognizerQnaryPercMulti* toReturn;
    toReturn = new RecognizerQnaryPercMulti;
    return toReturn;
  }
  if (nameCopy == "reducedadapter") {
	  return new RecognizerReducedAdapter;
  }
  throw AxiomLibException ("RecognizerFactory::create : can not find the requested recognizer.");
}
