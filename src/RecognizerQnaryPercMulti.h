/****************************************************************************
*			RecognizerQnaryPercMulti.h
*
*	Description: Описание типа RecognizerQnaryPercMulti
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef RECOGNIZERQNARYPERCMULTI_H_
#define RECOGNIZERQNARYPERCMULTI_H_

#include "RecognizerNeuralNet.h"
#include "QnaryPercMulti.h"

namespace AxiomLib {

/****************************************************************************
*			RecognizerQnaryPercMulti
*
*	Description: Распознаватель, использующий свой q-нарный перцептрон
*				для каждого значения длины входа
*	Author:		wictor
*	History:	
*
****************************************************************************/
typedef RecognizerNeuralNet< QnaryPercMulti, QnaryPercMultiSettings > RecognizerQnaryPercMulti;

};

#endif
