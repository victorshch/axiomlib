/****************************************************************************
*			RecognizerPercMulti.h
*
*	Description: Описание типа RecognizerPercMulti
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef RECOGNIZERPERCMULTI_H_
#define RECOGNIZERPERCMULTI_H_

#include "RecognizerNeuralNet.h"
#include "PercMulti.h"

namespace AxiomLib {

/****************************************************************************
*			RecognizerPercMulti
*
*	Description: Распознаватель, использующий свой многослойный перцептрон
*				для каждого значения длины входа
*	Author:		wictor
*	History:	
*
****************************************************************************/
typedef RecognizerNeuralNet< PercMulti, PercMultiSettings > RecognizerPercMulti;

};

#endif
