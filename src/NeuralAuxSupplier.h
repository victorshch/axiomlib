/****************************************************************************
*			NeuralAuxSupplier.h
*
*	Description: Описание класса NeuralAuxSupplier
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef NEURALAUXSUPPLIER_H_
#define NEURALAUXSUPPLIER_H_

#include "NeuralCommon.h"

namespace AxiomLib {

/****************************************************************************
*			NeuralAuxSupplier<NetWrapper>
*
*	Description: Класс - прослойка, осуществляющая автоматическое добавление
*				строк для дополнительного обучения (т.е вызывающая supplyAuxTrainingSata()
*				с автоматически сгенерированными строками)
*				NetWrapper - должен моделировать NetWrapper или Holder (см. NeuralConcepts.h)
*				Данный класс моделирует то же, что и NetWrapper
*	Author:		wictor
*	History:	
*
****************************************************************************/
template<class NetWrapper>
class NeuralAuxSupplier: public NetWrapper {
	public:
	
	// Генерация дополнительных строк
	template<class Settings>
	void construct(const Settings &settings);
};

};

#include "NeuralAuxSupplier.cxx"

#endif