/****************************************************************************
*			NeuralDistRejecter.h
*
*	Description: Описание шаблона класса NeuralDistRejecter<NetWrapper>
*	Author:		wictor
*	History:	
*
****************************************************************************/

#include "NeuralCommon.h"

#ifndef NEURALREJECTER_H_
#define NEURALREJECTER_H_

namespace AxiomLib {

/****************************************************************************
*			NeuralDistRejecter
*
*	Description: Класс - прослойка для обработки результатов нейросетевого
*				алгоритма с учетом параметров minDist и useCentral
*				NetWrapper должен моделировать концепт
*				NetWrapperMinimalWithCentralStrings (см. NeuralConcepts.h)
*				Этот класс моделирует то же, что NetWrapper
*	Author:		wictor
*	History:	
*
****************************************************************************/
template<class NetWrapper>
class NeuralDistRejecter: public NetWrapper {
	// Минимальное расстояние до нормального поведения
	double minDist;
	
	// Вектор содержит индексы строк, с которыми нужно сравнивать расстояние
	std::vector<int> centralStrings;
	
	int lastCentral;
	
	bool useCentral;
	public:
	
	NeuralDistRejecter(): lastCentral(-1) {}
	
	// Инициализация из настроек
	template<class Settings>	
	void construct(const Settings &settings);
	
	// Добавление строки для обучения - этот класс перегружает данную функцию для
	// поддержки центральных строк
	void supplyTrainingData(const Qstring &str);
	
	// Добавление центральных строки - после этого вызова все
	// следующие строки, добавленные вызовом supplyTrainingData(),
	// будут относиться
	// к данной центральной строке (до следующего вызова этой функции)
	void supplyCentralString(const Qstring &str);
	
	// Запуск нейросетевого алгоритма (с дальнейшим отсеиванием по расстоянию)
	int run(const Qstring &str, double &d) const;
};

};

#include "NeuralDistRejecter.cxx"

#endif
