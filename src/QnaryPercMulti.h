#ifndef QNARYPERCMULTI_H_
#define QNARYPERCMULTI_H_

#include "NeuralDataHolder.h"
#include "QnaryPercWrapper.h"
#include "NeuralDistRejecter.h"
#include "NeuralMultiplexer.h"
#include "NeuralCoderGeneric.h"
#include "NeuralCommonSettings.h"
#include "QnaryPercSingle.h"

namespace AxiomLib {

// тип настроек для QnaryPercMulti - пока он совпадает с QnaryPercSingleSettings
typedef QnaryPercSingleSettings QnaryPercMultiSettings;

// Тип объекта q-нарного перцептрона 
// со своей сетью для каждого значения длины входа
// и отсечением по расстоянию
// (здесь применяется статический вариант паттерна Decorator)
class QnaryPercMulti: public
	// Слой, инкапсулирующий массив сетей и предоставляющий интерфейс как для одной сети
	// (т.е. моделирующий NetWrapperMinimal, см. NeuralConcepts.h)
	NeuralMultiplexer<
		// Слой отвергания строк по расстоянию
		// (для корректного распознавания он должен находиться ниже слоя NeuralMultiplexer, а не выше)
		NeuralDistRejecter<
			// Слой, инкапсулирующий q-нарную сеть
			QnaryPercWrapper<
				// Слой, хранящий обучающие строки
				NeuralDataHolder<
					// Кодировщик - здесь использован обобщенный кодировщик, применяющий 
					// различные способы в зависимости от заданных параметров
					NeuralCoderGeneric
					> > > > {
public:
	// Тип настроек - он определен тут для использования в шаблоне RecognizerNeuralNet
	typedef QnaryPercMultiSettings Settings;
};

};

#endif
