#ifndef QNARYPERCSINGLE_H_
#define QNARYPERCSINGLE_H_

#include "NeuralDataHolder.h"
#include "QnaryPercWrapper.h"
#include "NeuralDistRejecter.h"
#include "NeuralCoderGeneric.h"
#include "NeuralCommonSettings.h"

namespace AxiomLib {

struct QnaryPercSingleSettings {
	// Общие настройки
	NeuralCommonSettings commonSettings;
	// Настройки кодировщика
	NeuralCoderSettings coderSettings;
	// Число состояний выходных нейронов
	int qO;
	// Инициализация из окружения
	void initFromEnv(const Environment &env);
};

// Тип объекта q-нарного перцептрона 
// с одной сетью для разных длин входа
// и отсечением по расстоянию
// (здесь применяется статический вариант паттерна Decorator)
class QnaryPercSingle: public
	// Слой отвергания строк по расстоянию
	NeuralDistRejecter<
		// Слой, инкапсулирующий q-нарную сеть
		QnaryPercWrapper<
			// Слой, хранящий обучающие строки
			NeuralDataHolder<
				// Кодировщик - здесь использован обобщенный кодировщик, применяющий 
				// различные способы в зависимости от заданных параметров
				NeuralCoderGeneric
				> > > {
public:
// Тип настроек - он определен тут для использования в шаблоне RecognizerNeuralNet
	typedef QnaryPercSingleSettings Settings;
};

};

#endif
