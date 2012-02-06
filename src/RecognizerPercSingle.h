/****************************************************************************
*			RecognizerPercSingle.h
*
*	Description: Описание типа RecognizerPercSingle
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef RECOGNIZERPERCSINGLE_H_
#define RECOGNIZERPERCSINGLE_H_

#include "RecognizerNeuralNet.h"
#include "PercSingle.h"

namespace AxiomLib {

/****************************************************************************
*			RecognizerPercSingle
*
*	Description: Распознаватель, использующий один многослойный перцептрон
*				для всех длин входа (и подачу 0 на неиспользуемые нейроны
*				при обучении и распознавании)
*	Author:		wictor
*	History:	
*
****************************************************************************/
typedef RecognizerNeuralNet< PercSingle, PercSingleSettings > RecognizerPercSingle;

};

#endif
