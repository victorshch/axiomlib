/****************************************************************************
*						NeuralDistRejecter.cxx
*
*       Description:	Реализация шаблонов функций-членов шаблона класса NeuralDistRejecter
*       Author:			wictor
*       History:
*
****************************************************************************/

#ifndef NEURALDISTREJECTER_CXX_
#define NEURALDISTREJECTER_CXX_

#include "NeuralDistRejecter.h"
#include <boost/foreach.hpp>

namespace AxiomLib {

/****************************************************************************
*						NeuralDistRejecter<NetWrapper>::construct
*
*       Description:	Инициализация из настроек
*       Parameters:		const Settings &settings - настройки
*						тип Settings должен иметь член double commonSettings.minDist
*						и удовлетворять требованиям NetWrapper::construct()
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
template<class Settings>
void NeuralDistRejecter<NetWrapper>::construct(const Settings &settings) {
	NetWrapper::construct(settings);
	minDist = settings.commonSettings.minDist;
	useCentral = settings.commonSettings.useCentral;
}
/****************************************************************************
*						NeuralDistRejecter<NetWrapper>::run
*
*       Description:	Функция запускает нейросетевой алгоритм, затем проверяет 
*						результат на близость к входной строке
*       Parameters:		const Qstring &inp - вход сети
*						double &p - число, которое может возвращать сеть при работе (пока не используется)
*       Returns:		Номер эталонной строки (в том порядке, в котором они были заданы функцией supplyTrainingData()), 
*						либо -1, если сеть выдала ответ, выходящий за рамки нумерации эталонных строк,
*						либо если в результате получилась строка, расположенная на расстоянии от str большем,
*						чем minDist (это число задается в настройках)
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
int NeuralDistRejecter<NetWrapper>::run(const Qstring &str, double &p) const {
	// Работа нейросетевого алгоритма
	int res = NetWrapper::run(str, p);
		
	// Если он вернул -1, то возвращаем -1
	if(res<0) {
		return res;
	}
	
	int index;
	if(useCentral) {
		index = centralStrings[res] > -1 ? centralStrings[res] : res;
	} else {
		index = res;
	}
	// Вычисляем расстояние от str до строки, по которой мы меряем расстояние
	double d = difference(str, NetWrapper::getTrainString(index));
	// Если это расстояние больше minDist, то возвращаем -1
	if(d > minDist) { 
		return -1;
	}
	// Иначе возвращаем результат нейросетевого алгоритма
	return res;
}

template<class NetWrapper>
void NeuralDistRejecter<NetWrapper>::supplyCentralString(const Qstring &str) {
	lastCentral = NetWrapper::getNumTrain();
	this->supplyTrainingData(str);
}

template<class NetWrapper>
void NeuralDistRejecter<NetWrapper>::supplyTrainingData(const Qstring &str) {
	centralStrings.push_back(lastCentral);
	NetWrapper::supplyTrainingData(str);
}

};

#endif
