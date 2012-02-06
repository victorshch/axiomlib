/****************************************************************************
*			RecognizerQnaryPercSingle.h
*
*	Description: Описание типа RecognizerQnaryPercSingle
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef RECOGNIZERQNARYPERCSINGLE_H_
#define RECOGNIZERQNARYPERCSINGLE_H_

#include "RecognizerNeuralNet.h"
#include "QnaryPercSingle.h"

namespace AxiomLib {

/****************************************************************************
*			RecognizerQnaryPercSingle
*
*	Description: Распознаватель, использующий один q-нарный перцептрон
*				для всех длин входа (и обрубание неиспользуемых нейронов)
*	Author:		wictor
*	History:	
*
****************************************************************************/
typedef RecognizerNeuralNet< QnaryPercSingle, QnaryPercSingleSettings > RecognizerQnaryPercSingle;

};

#endif
