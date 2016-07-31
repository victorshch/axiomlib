/****************************************************************************
*						NeuralFANNHeader.h
*
*       Description:	Файл, который нужно включать для использования FANN
*       Author:			wictor
*       History:
*
****************************************************************************/

#ifndef NEURALFANNHEADER_H_
#define NEURALFANNHEADER_H_

extern "C" {
// Для использования float вместо double здесь нужно 
// вместо doublefann.h написать floatfann.h,
// а также выключить из сборки doublefann.c
// и включить floatfann.c
#include "FANN/doublefann.h"
}

#define FANN_NO_DLL
#include "FANN/fann_cpp.h"

#endif
