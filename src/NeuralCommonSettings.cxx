/****************************************************************************
*				NeuralCommonSettings.cxx
*
*	Description:	Файл, содержащий реализацию того, что описано в NeuralCommonSettings.h
*					(структура NeuralCommonSettings - общие настройки для класса RecognizerNeuralNet)
*	Author:		wictor
*	History:	
*
****************************************************************************/
#include "NeuralCommonSettings.h"
#include "NeuralCommon.h"
#include "AxiomLibException.h"

namespace AxiomLib {

/****************************************************************************
*					NeuralCommonSettings::initFromEnv()
*
*	Description:	Инициализация из окружения. Имя параметра окружения
*					для srcQ - "numSymbols", для остальных параметров
*					имена совпадают с именами полей структуры.
*					Для comments умолчание - false
*	Parameters:	Environment& env - окружение
*	Returns:	-
*	Throws:		AxiomLibException, если параметр, для которого нет умолчания, не установлен
*	Author:		wictor
*	History:
*
****************************************************************************/
void NeuralCommonSettings::initFromEnv(const Environment &env) {
	// Считываем число символов
	if(env.getIntParamValue(srcQ ,"numSymbols") < 0) {
		throw AxiomLibException("PercSingleSettings::initFromEnv(): numSymbols not set");
	};
		
	// считываем normalDist
	if(env.getDoubleParamValue(minDist ,"normalDist") < 0) {
		throw AxiomLibException("PercSingleSettings::initFromEnv(): normalDist not set");
	};
	
	// Считываем значение comments, по умолчанию false
	if(env.getBoolParamValue(comments, "comments") < 0) {
		comments = false;
	}

	// Считываем значение useCentral, по умолчанию false
	if(getBoolParamValueFromEnv(env, useCentral, "useCentral") < 0) {
		useCentral = false;
	}
}

/****************************************************************************
*					NeuralCoderSettings::initFromEnv()
*
*	Description:	Инициализация из окружения. Имя параметров окружения
*					 совпадают с именами полей структуры.
*					Для dstQ умолчание 2,
*					для numVar 0
*	Parameters:	Environment& env - окружение
*	Returns:	-
*	Throws:		AxiomLibException, если параметр, для которого нет умолчания, не установлен
*	Author:		wictor
*	History:
*
****************************************************************************/
void NeuralCoderSettings::initFromEnv(const Environment &env) {
	// Считываем dstQ
	if(env.getIntParamValue(dstQ ,"dstQ") < 0) {
		// По умолчанию - 2
		dstQ = 2;
	}
	
	// Считываем numVar, по умолчанию - 0
	if(env.getIntParamValue(numVar ,"numVar") < 0) {
		numVar = 0;
	}
	
	// Считываем windowSize, по умолчанию - 0
	if(env.getIntParamValue(windowSize ,"windowSize") < 0) {
		windowSize = 1;
	}
}

};
