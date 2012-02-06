/****************************************************************************
*						PercSettings.cxx
*
*       Description:	Реализация того, что объявлено в PercSettings.h
*       Author:			wictor
*       History:
*
****************************************************************************/
#include "PercSettings.h"
#include "NeuralActFuncStr.h"
#include "NeuralCommon.h"

namespace AxiomLib {

/****************************************************************************
*					PercSettings::initFromEnv()
*
*	Description:	Инициализация из окружения.
*					Имена параметров окружения совпадают с именами полей структуры.
*	Parameters:	Environment& env - окружение
*	Returns:	-
*	Throws:		AxiomLibException, если параметр не установлен
*	Author:		wictor
*	History:
*
****************************************************************************/
void PercSettings::initFromEnv(const Environment &env) {
	// считываем настройки перцептрона
	// считываем число слоев
	if(env.getIntParamValue(numLay ,"numLay") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): numLay not set");
	}
	
	// считываем строку с алгоритмом
	std::string algorithmStr;
	if(env.getStringParamValue(algorithmStr ,"trainingAlgorithm") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): trainingAlgorithm not set");
	}
	
	// переводим строку с алгоритмом в FANN::training_algorithm_enum
	int algorithmNum = trainFromStr(algorithmStr);
	if (algorithmNum < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): incorrect trainingAlgorithm");
	}
	trainingAlgorithm = (FANN::training_algorithm_enum) algorithmNum;
	
	// считываем строку со списком активационных функций
	std::string actFuncsStr;
	if(env.getStringParamValue(actFuncsStr ,"actFuncs") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): actFuncs not set");
	}
	
	// переводим строку со списком активационных в вектор строк активационных функций
	std::vector<std::string> actFuncsStrs;
	getVectorFromString(actFuncsStrs, actFuncsStr);
	// переводим вектор строк активационных функций в вектор из FANN::activation_function_enum
	actFuncs.clear();
	actFuncs.reserve(actFuncsStrs.size());
	for(unsigned int i = 0; i < actFuncsStrs.size(); i++) {
		int actFuncNum = actFuncFromStr(actFuncsStrs[i]);
		if(actFuncNum < 0) {
			throw AxiomLibException("PercSettings::initFromEnv(): incorrect actFuncs");
		}
		actFuncs.push_back((FANN::activation_function_enum) actFuncNum);
	}
	
	// считываем список факторов (множителей) числа нейронов слоев
	std::string numNeuronsFStr;
	if(env.getStringParamValue(numNeuronsFStr ,"numNeuronsF") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): numNeuronsF not set");
	}
	// переводим в вектор из double
	getVectorFromString(numNeuronsF, numNeuronsFStr);
	
	// считываем список параметров активационных функций
	std::string actFuncsSStr;
	if(env.getStringParamValue(actFuncsSStr ,"actFuncsS") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): actFuncsS not set");
	}
	// переводим в вектор из double
	getVectorFromString(actFuncsS, actFuncsSStr);
	
	// считываем коэффициент обучения
	if(env.getDoubleParamValue(learningRate ,"learningRate") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): learningRate not set");
	};

	// считываем момент обучения
	if(env.getDoubleParamValue(learningMomentum ,"learningMomentum") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): learningMomentum not set");
	};
	
	// считываем желаемую ошибку
	if(env.getDoubleParamValue(desiredError ,"desiredError") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): desiredError not set");
	};

	// считываем максимальное число эпох
	if(env.getIntParamValue(maxIters ,"maxIters") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): maxIters not set");
	}

	// считываем максимальное число эпох
	if(env.getIntParamValue(itersBetweenReports ,"itersBetweenReports") < 0) {
		throw AxiomLibException("PercSettings::initFromEnv(): itersBetweenReports not set");
	}
	
	// проверяем настройки на консистентность
	if(!valid()) {
		throw AxiomLibException("PercSettings::initFromEnv(): invalid perceptron settings");
	}	
}

};