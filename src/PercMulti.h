/****************************************************************************
*						PercMulti.h
*
*       Description:	Объявление типов PercMulti и PercMultiSettings
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef PERCMULTI_H_
#define PERCMULTI_H_

#include <vector>
#include <string>

#include "NeuralDataHolder.h"
#include "NeuralAuxSupplier.h"
#include "PercWrapper.h"
#include "NeuralDistRejecter.h"
#include "NeuralMultiplexer.h"
#include "NeuralCoderGeneric.h"
#include "NeuralCommonSettings.h"
#include "PercSingle.h"

namespace AxiomLib {

// тип настроек для PercMulti - пока он совпадает с PercSingleSettings
typedef PercSingleSettings PercMultiSettings;

// тип, инкапсулирующий массив многослойных перцептронов,
// для каждого из которых делается отсечение по расстоянию,
// генерация дополнительных строк, хранение и кодирование данных
// (здесь применяется статический вариант паттерна Decorator)
class PercMulti: public
	// Слой, инкапсулирующий массив сетей и предоставляющий интерфейс как для одной сети
	// (т.е. моделирующий NetWrapperMinimal, см. NeuralConcepts.h)
	NeuralMultiplexer<
		// Слой отвергания строк по расстоянию
		// (для гарантии корректного распознавания он должен находиться 
		// ниже слоя NeuralMultiplexer, а не выше)
		NeuralDistRejecter<
			// Слой, инкапсулирующий многослойный перцептрон
			PercWrapper<
				// Слой, автоматически генерирующий дополнительные строки 
				// (искаженные эталонные строки для дополнительного обучения)
				NeuralAuxSupplier<
					// Слой, хранящий обучающие строки
					NeuralDataHolder<
						// Кодировщик - здесь использован обобщенный кодировщик, применяющий 
						// различные способы в зависимости от заданных параметров
						NeuralCoderGeneric
						> > > > > {
public:
	// Определение типа настроек для использования в шаблоне RecognizerNeuralNet
	typedef PercMultiSettings Settings;
};

};

#endif
