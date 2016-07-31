/****************************************************************************
*						QnaryPercSingle.cxx
*
*       Description:	Определение функций-членов структуры QnaryPercSingleSettings
*       Author:			wictor
*       History:
*
****************************************************************************/
#include "QnaryPercSingle.h"

namespace AxiomLib {

/****************************************************************************
*					QnaryPercSingleSettings::initFromEnv()
*
*	Description:	Инициализация из окружения. Имя параметров окружения
*					 совпадают с именами полей структуры (для полей, не являющихся структурами).
*					Для qO умолчание 0, что означает, что число состояний будет равно числу обучающих строк
*	Parameters:	Environment& env - окружение
*	Returns:	-
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
void QnaryPercSingleSettings::initFromEnv(const Environment &env) {
	commonSettings.initFromEnv(env);
	coderSettings.initFromEnv(env);	

	// Считываем qO, по умолчанию - 0
	if(env.getIntParamValue(qO ,"qOutput") < 0) {
		qO = 0;
	}
}

};

