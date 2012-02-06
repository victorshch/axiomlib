/****************************************************************************
*						QnaryPercWrapper.cxx
*
*       Description:	Реализация того, что объявлено в QnaryPercWrapper.h
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef QNARYPERCWRAPPER_CXX_
#define QNARYPERCWRAPPER_CXX_

#include "QnaryPercWrapper.h"

namespace AxiomLib {

/****************************************************************************
*						QnaryPercWrapper<Holder>::construct
*
*       Description:	Установка параметров и создание сети
*       Parameters:		const Settings &settings - параметры
*						тип Settings должен иметь поле int qO,
*						поле CommonSettings commonSettings
*						и удовлетворять требованиям Holder::construct()
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
template<class Settings>
void QnaryPercWrapper<Holder>::construct(const Settings &settings) {
	// инициализация верхнего уровня - держателя данных
	Holder::construct(settings);
	// далее созаем сеть, только если число обучающих строк больше 1
	// иначе в run всегда возвращаем 0
	if(Holder::getNumTrain() >= 2) {
		int actualQo;
		if (settings.qO < 1) {
			// Если qO < 1, то считаем, что оно равно числу обучающих строк
			// т.е. у нас сеть с одним выходным нейроном с числом состояний,
			// равным числу обучающих строк
			actualQo = Holder::getNumTrain();
		} else if (settings.qO == 1) {
			// если qO = 1, то считаем, что оно равно числу состояний входных нейронов 
			// т.е. в этом случае число состояний вых. нейронов равно числу состояний вх. нейронов
			actualQo = Holder::getCoderDstQ();
		} else {
			actualQo = settings.qO;
		}
		// инициализация q-нарного перцептрона
		qp.construct(
			// число состояний входных нейронов - определяется кодировщиком
			Holder::getCoderDstQ(),
			// число состояний выходных нейронов
			actualQo,
			// число входных нейронов - максимальная длина 
			// закодированной обучающей строки
			getCoderDstLen(Holder::getMaxTrainDim()),
			// число выходных нейронов - достаточное для того, чтобы представить
			// самый большой номер обучающей строки в qO-ичной системе счисления
			numDigits(actualQo, (Holder::getNumTrain() - 1)
			)
		);
	}
}

/****************************************************************************
*						QnaryPercWrapper<Holder>::clear
*
*       Description:	Функция очищает объект (и его можно использовать повторно)
*       Parameters:		-
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
void QnaryPercWrapper<Holder>::clear() {
	Holder::clear();
}

/****************************************************************************
*						QnaryPercWrapper<Holder>::train
*
*       Description:	Функция обучает сеть на ранее предоставленных ей обучающих строках
*       Parameters:		-
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
void QnaryPercWrapper<Holder>::train()
{
	// Если число обучающий строк меньше 2 - выходим
	if(Holder::getNumTrain() < 2) {
		return;
	}
	
	// номер обучающей строки (он нумерует только основные строки, не дополнительные)
	int outputIndex = 0;
	// строка, соответствующая выходу сети
	Qstring output(numDigits(qp.getQo(), Holder::getNumTrain() - 1));
	
	// цикл по всем обучающим строкам
	for (int i = 0; i < Holder::getNumTrain(); i++)
	{
		// Закодированная i-я обучающая строка
		const Qstring inpTranslated = Holder::getTranslated(i);
		
		makeZero(output);
		// Записываем в output номер обучающей строки в виде
		// числа в qO-ичной СС
		numToQnaryVector(qp.getQo(), outputIndex, output);
		
		// Обучаем сеть на i-м входе и соотв. выходе
		qp.train(inpTranslated, output);
		
		// Цикл по доп. обучающим строкам (соответствующим i-й обучающей строке)
		for (int j = 0; j < Holder::getNumAuxTrain(i); j++)
		{
			// Закодированная j-я доп. обучающая строка, соответствующая 
			// i-й обучающей строке
			const Qstring inpAuxTranslated = Holder::getAuxTranslated(i, j);
			
			// Обучаем на ней сеть, выход должен быть тот же, что и 
			// у i-й обучающей строки
			qp.train(inpAuxTranslated, output);
		}
		
		outputIndex++;
	}
}

/****************************************************************************
*						QnaryPercWrapper<Holder>::run
*
*       Description:	Функция запускает нейросетевой алгоритм
*						
*       Parameters:		const Qstring &inp - вход сети ()
*						double &p - число, которое может возвращать сеть при работе (пока не используется)
*       Returns:		Номер эталонной строки (в том порядке, в котором они были заданы функцией supplyTrainingData()), 
*						либо -1, если сеть выдала ответ, выходящий за рамки нумерации эталонных строк
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
int QnaryPercWrapper<Holder>::run(const Qstring &inp, double &p) const {
	// Если обучающая строка только одна, то возвращаем 0
	// (т.е. мы ее как бы всегда определяем)
	if (Holder::getNumTrain() == 1) {
		return 0;
	// Если их 0, то возвращаем -1 (т.е. мы ничего не определили)
	} else if (Holder::getNumTrain() == 0) {
		return -1;
	}
	
	// Кодируем вход
	const Qstring inpCoded = Holder::coderTranslate(inp);
	// Вычисляем выход сети
	int index = qnaryVectorToNum(qp.getQo(), qp.run(inpCoded, p));
	// Если выход некорректен, возвращаем -1
	if(index >= Holder::getNumTrain() || index < 0) return -1;
	return index;
}

};

#endif
