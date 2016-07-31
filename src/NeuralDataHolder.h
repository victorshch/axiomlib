/****************************************************************************
*			NeuralDataHolder.h
*
*	Description: Класс для хранения и кодирования обучающей выборки
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef NEURALDATAHOLDER_H_
#define NEURALDATAHOLDER_H_

#include "NeuralCommon.h"
#include "QnaryPerc.h"
#include <vector>
#include <boost/foreach.hpp>

namespace AxiomLib {

/****************************************************************************
*			NeuralDataHolder<Coder>
*
*	Description: Шаблон класса, осуществляющего хранение и
*				кодирование данных. Моделирует Holder
*				Параметр шаблона: Coder - должен быть моделью Coder (см. NeuralConcepts.h)
*	Author:		wictor
*	History:	
*
****************************************************************************/
template<class Coder>
class NeuralDataHolder {

	// Данные для обучения
	std::vector<Qstring> trainData;
	// Дополнительные данные для обучения
	std::vector<std::vector<Qstring> > auxTrainData;
	// Кодировщик	
	Coder coder;
	
	// Минимальные и максимальные длины строк
	int maxDim, minDim;
public:

	NeuralDataHolder():maxDim(0), minDim(0) {}
	
	// Максимальная размерность (незакодированной) обучающей строки
	int getMaxTrainDim() const { return maxDim; }
	
	// Минимальная размерность (незакодированной) обучающей строки
	int getMinTrainDim() const { return minDim; }
	
	// Возвращает число строк для обучения
	int getNumTrain() const { return trainData.size(); }
	
	// Возвращает число наборов дополнительных строк для обучения
	// Фактически, возвращает то же, что getNumTrain()
	int getNumAuxTrain() const { return auxTrainData.size(); }
	
	// Возвращает число дополнительных строк для обучения, 
	// соответствующих строке для обучения с номером i
	int getNumAuxTrain(int i) const;
	
	// Возвращает строку для обучения с номером i
	const Qstring &getTrainString(int i) const;
	
	// Возвращает дополнительную строку для обучения с номером j, 
	// соответствующую строке для обучения с номером i
	const Qstring &getAuxTrainString(int i, int j) const;
	
	// Возвращает строку для обучения с номером i, предварительно кодируя ее
	const Qstring getTranslated(int i) const;
	// Возвращает дополнительную строку для обучения с номером j, 
	// соответствующую строке для обучения с номером i, предварительно кодируя ее	
	const Qstring getAuxTranslated(int i, int j) const;
	
	// Кодирует строку str
	const Qstring coderTranslate(const Qstring &str) const;
	
	// Возвращает srcQ кодировщика	
	int getCoderSrcQ() const { return coder.getSrcQ(); }
	
	// Возвращает dstQ кодировщика
	int getCoderDstQ() const { return coder.getDstQ(); }
	
	// Возвращает длину закодированной строки с исходной длиной srcLen
	int getCoderDstLen(int srcLen) const { return coder.getDstLen(srcLen); }
	
	// Добавляет строку для обучения
	void supplyTrainingData(const Qstring &s);
	
	// Добавляет дополнительную строку для обучения 
	// (относящуюся к последней добавленной строке для обучения)
	// если i=-1, то добавленная строка ассоциируется с последней
	// добавленной строкой для обучения
	// иначе - со строкой с номером i
	void supplyAuxTrainingData(const Qstring &s, int i = -1);
	
	// Инициализирует объект
	template<class Settings>
	void construct(const Settings &settings);

	// Очищает объект (удаляет все обучающие строки)
	void clear();	
};

};

#include "NeuralDataHolder.cxx"

#endif
