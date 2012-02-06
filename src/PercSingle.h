/****************************************************************************
*						PercSingle.h
*
*       Description:	Объявление типов PercSingle и PercSingleSettings
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef PERCSINGLE_H_
#define PERCSINGLE_H_

#include <vector>
#include <string>

#include "NeuralDataHolder.h"
#include "NeuralAuxSupplier.h"
#include "PercWrapper.h"
#include "NeuralDistRejecter.h"
#include "Environment.h"
#include "NeuralCoderGeneric.h"
#include "NeuralCommonSettings.h"

namespace AxiomLib {

/****************************************************************************
*						PercSingleSettings
*
*       Description:	Структура, содержащая настройки для PercSingle
*       Author:			wictor
*       History:
*
****************************************************************************/
struct PercSingleSettings {
	// Общие настройки
	NeuralCommonSettings commonSettings;
	// Настройки кодировщика
	NeuralCoderSettings coderSettings;
	// Настройки перцептрона
	PercSettings percSettings;
	// Число доп. строк для обучения
	int numAux;
	// Расстояние от доп. строк для обучения до основных обучающих строк
	double auxDiff;
	// Инициализация из окружения
	void initFromEnv(const Environment &env);
};

// тип, инкапсулирующий многослойный перцептрон,
// для которого делается отсечение по расстоянию,
// генерация дополнительных строк, хранение и кодирование данных
// (здесь применяется статический вариант паттерна Decorator)
class PercSingle: public
	// Слой отвергания строк по расстоянию
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
					> > > > {
public:
	// Тип настроек
	typedef PercSingleSettings Settings;
};

};

#endif
