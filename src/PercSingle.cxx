/****************************************************************************
*						PercSingle.cxx
*
*       Description:	Определение функций-членов структуры PercSingle
*       Author:			wictor
*       History:
*
****************************************************************************/

#include "PercSingle.h"

namespace AxiomLib {

/****************************************************************************
*					PercSingleSettings::initFromEnv()
*
*	Description:	Инициализация из окружения. Имя параметров окружения
*					 совпадают с именами полей структуры (для полей, не являющихся структурами).
*					Для numAux умолчание 0,
*					для auxDiff 0.1
*	Parameters:	Environment& env - окружение
*	Returns:	-
*	Throws:		AxiomLibException, если параметр, для которого нет умолчания, не установлен
*	Author:		wictor
*	History:
*
****************************************************************************/
void PercSingleSettings::initFromEnv(const Environment &env) {
	commonSettings.initFromEnv(env);
	coderSettings.initFromEnv(env);
	percSettings.initFromEnv(env);	

	// Считываем numAux, по умолчанию - 0
	if(env.getIntParamValue(numAux ,"numAux") < 0) {
		numAux = 0;
	}

	// Считываем auxDiff, по умолчанию - 0.1
	if(env.getDoubleParamValue(auxDiff ,"auxDiff") < 0) {
		auxDiff = 0.1;
	};
}

};
