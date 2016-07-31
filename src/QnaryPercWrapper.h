/****************************************************************************
*						QnaryPercWrapper.h
*
*       Description:	Объявление шаблона класса QnaryPercWrapper
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef QNARYPERCWRAPPER_H_
#define QNARYPERCWRAPPER_H_

#include "NeuralCommon.h"
#include "QnaryPerc.h"
#include <vector>
#include <boost/foreach.hpp>

namespace AxiomLib {

/****************************************************************************
*						QnaryPercWrapper<Holder>
*
*       Description:	Класс, инкапсулирующий функциональность q-нарного перцептрона
*						и моделирующий NetWrapper (см. NeuralConcepts.h)
*						Holder должен моделировать концепт Holder
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
class QnaryPercWrapper : public Holder {
	// Объект нейросети
	QnaryPerc qp;
	public:
	//Конструктор умолчания
	//QnaryPercWrapper() {}
	
	// Установка параметров сети
	// Settings должен иметь поле int qO
	template<class Settings>
	void construct(const Settings &settings);
	
	// Обучение сети (на ранее заданных строках)
	void train();
	
	// Запуск сетевого алгоритма
	int run(const Qstring &str, double &p) const;
	
	// Очистка (перед повторным использованием)
	void clear();
};

};

#include "QnaryPercWrapper.cxx"

#endif
