/****************************************************************************
*						NeuralDataHolder.cxx
*
*       Description:	Реализация шаблонов функций-членов шаблона класса NeuralDataHolder
*						Об ограничениях на Coder см. NeuralDataHolder.h
*       Author:			wictor
*       History:
*
****************************************************************************/

#ifndef NEURALDATAHOLDER_CXX_
#define NEURALDATAHOLDER_CXX_

#include "NeuralDataHolder.h"
#include "QnaryPercWrapper.h"

namespace AxiomLib {

/****************************************************************************
*						NeuralDataHolder<Coder>::supplyTrainingData
*
*       Description:	Функция добавляет одну строку в вектор строк для обучения
*       Parameters:		const Qstring &s - добавляемая строка
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
inline void NeuralDataHolder<Coder>::supplyTrainingData(const Qstring &s) {
	trainData.push_back(s);
	auxTrainData.push_back(std::vector<Qstring>());
	if((unsigned) maxDim < s.size()) {
		if(maxDim == 0) {
			minDim = (int) s.size();
		}
		maxDim = (int) s.size();
	} else if((unsigned) minDim > s.size()) {
		minDim = (int) s.size();
	}
}

/****************************************************************************
*						NeuralDataHolder<Coder>::supplyAuxTrainingData
*
*       Description:	Функция добавляет одну дополнительную строку для обучения,
*						которая соответствует последней добавленной строке для обучения
*       Parameters:		const Qstring &s - добавляемая строка
*						i - индекс основной строки, с которой надо ассоциировать 
*						добавляемую строку или -1, если надо ассоциировать с последней
*						добавленной основной строкой
*       Returns:		-
*       Throws:			AxiomLibException, если ни разу не было вызвано supplyTrainingData(),
*						либо размер inp не совпадает с размером последней добавленной строки для обучения
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
void NeuralDataHolder<Coder>::supplyAuxTrainingData(const Qstring &inp, int i)
{
	if(auxTrainData.empty() || trainData.empty()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::supplyAuxTrainingData():"
		" no train data supplied");	
	}
	if(i >= (int) auxTrainData.size()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::supplyAuxTrainingData():"
		" index out of range");	
	}
	if(i <= -1) {
		i = auxTrainData.size() - 1;
	}
	if(trainData[i].size() != inp.size()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::supplyAuxTrainingData():"
		" aux data does not match train data in size");	
	}
	
	auxTrainData[i].push_back(inp);
}

/****************************************************************************
*						NeuralDataHolder<Coder>::getNumAuxTrain
*
*       Description:	Возвращает число дополнительных строк для обучения, соответствующих
*						строке для обучения с номером i
*       Parameters:		const Qstring &s - добавляемая строка
*       Returns:		-
*       Throws:			AxiomLibException, если номер i некорректен
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
int NeuralDataHolder<Coder>::getNumAuxTrain(int i) const {
	// Проверка на выход за границы массива
	if(i < 0 || i >= (int) trainData.size()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::getNumAuxTrain() : i out of range");
	}
	
	return auxTrainData[i].size();
}

/****************************************************************************
*						NeuralDataHolder<Coder>::construct
*
*       Description:	Функция строит сеть, основываясь на полученных данных.
*       Parameters:		тип Settings - должен удовлетворять ограничениям Coder::construct()
*						const Settings &settings - настройки
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
template<class Settings>
void NeuralDataHolder<Coder>::construct(const Settings &settings) {
	coder.construct(settings);
}

/****************************************************************************
*						NeuralDataHolder<Coder>::clear
*
*       Description:	Функция очищает объект для повторного использования
*       Parameters:		-
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
inline void NeuralDataHolder<Coder>::clear()
{
	trainData.clear();
	auxTrainData.clear();
	minDim = 0;
	maxDim = 0;
}

/****************************************************************************
*						NeuralDataHolder<Coder>::getTrainString
*
*       Description:	Возвращает обучающую строку с номером i
*       Parameters:		int i - номер строки
*       Returns:		const Qstring & - строка для обучения
*       Throws:			AxiomLibException, если номер строки некорректен
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
const Qstring & NeuralDataHolder<Coder>::getTrainString(int i) const { 
	// Проверка на выход за границы массива
	if(i < 0 || i >= (int) trainData.size()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::getTrainString() : i out of range");
	}
	return trainData.at(i); 
}

/****************************************************************************
*						NeuralDataHolder<Coder>::getAuxTrainString
*
*       Description:	Возвращает строку для доп. обучения с номером j,
*						соответствующую обучающей строке с номером i
*       Parameters:		int i - номер обучающей строки
*						int j - номер строки для доп. обучения
*       Returns:		const Qstring & - строка для доп. обучения
*       Throws:			AxiomLibException, если номер i или j некорректен
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
const Qstring & NeuralDataHolder<Coder>::getAuxTrainString(int i, int j) const { 
	// Проверки на выход за границы массивов
	if(i < 0 || i >= (int) auxTrainData.size()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::getAuxTrainString() : i out of range");
	}
	if(j < 0 || j >= (int) auxTrainData[i].size()) {
		throw AxiomLibException("NeuralDataHolder<Coder>::getAuxTrainString() : j out of range");
	}

	return auxTrainData[i][j]; 
}

/****************************************************************************
*						NeuralDataHolder<Coder>::getTranslated
*
*       Description:	Возвращает обучающую строку с номером i в закодированном виде
*       Parameters:		int i - номер строки
*       Returns:		const Qstring & - закодированная строка для обучения 
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
const Qstring NeuralDataHolder<Coder>::getTranslated(int i) const {
	Qstring translated;
	coder.translate(getTrainString(i), translated);
	return translated;
}

/****************************************************************************
*						NeuralDataHolder<Coder>::getAuxTranslated
*
*       Description:	Возвращает (в закодированном виде) строку для доп. 
*						обучения с номером j,
*						соответствующую обучающей строке с номером i
*       Parameters:		int i - номер обучающей строки
*						int j - номер строки для доп. обучения
*       Returns:		const Qstring & - закодированная строка для доп. обучения
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
const Qstring NeuralDataHolder<Coder>::getAuxTranslated(int i, int j) const {
	Qstring translated;
	coder.translate(getAuxTrainString(i, j), translated);
	return translated;
}

/****************************************************************************
*						NeuralDataHolder<Coder>::coderTranslate
*
*       Description:	Кодирует строку str, возвращает результат
*       Parameters:		const Qstring &str
*       Returns:		const Qstring & - закодированная str
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Coder>
const Qstring NeuralDataHolder<Coder>::coderTranslate(const Qstring &str) const {
	Qstring res;
	coder.translate(str, res);
	return res;
}

};

#endif
