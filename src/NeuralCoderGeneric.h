/****************************************************************************
*			NeuralCoderGeneric.h
*
*	Description:	Описание кодировщика NeuralCoderGeneric, делающего кодирование различного типа
*					в зависимости от своих параметров
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef NEURALCODERGENERIC_H_
#define NEURALCODERGENERIC_H_

#include <vector>
#include <ctime>

#include "NeuralCommon.h"
#include "Round.h"

namespace AxiomLib {

/****************************************************************************
*			NeuralCoderGeneric
*
*	Description:	Кодировщик, делающй кодирование различного типа
*					в зависимости от своих параметров
*					Моделирует Coder (см. NeuralConcepts.h)
*	Author:		wictor
*	History:	
*
****************************************************************************/
class NeuralCoderGeneric {
	// Тип структуры данных, осуществляющей отображение (для 1-го типа кодирования)
	typedef std::vector<Qstring> map_t;
	
	// Структура данных, осуществляющая отображение
	map_t dict;
	
	// Параметры
	int srcQ, dstQ, numVar, windowSize, 
	
	// Число символов конечного алфавита, которым кодируется один символ исходного
	codedSymbolSize;
	
	// Тип способа кодирования
	enum CodingWay {Way1, Way2, None};
	// Способ кодирования
	CodingWay codingWay;
	
	// Кодирование по первому способу
	void translateWay1(const Qstring &src, Qstring &dst) const;
	// Кодирование по второму способу
	void translateWay2(const Qstring &src, Qstring &dst) const;
public:
	// Конструктор умолчания
	NeuralCoderGeneric():codingWay(None) {}
		
	// Инициализация
	// Settings должен иметь члены int srcQ, dstQ, numVar
	template<class Settings>
	void construct(const Settings &settings);
	
	// Перевод
	void translate(const Qstring &src, Qstring &dst) const;
	
	// Возвращает srcQ
	int getSrcQ() const { return srcQ; }
	// Возвращает dstQ
	int getDstQ() const { return dstQ; }
	
	// Длина выхода при длине входа srcLen	
	int getDstLen(int srcLen) const;
};

/****************************************************************************
*					NeuralCoderGeneric::construct
*
*	Description:	Инициализация объекта, в т.ч. построение отображения символов 
*					исходного алфавита (словаря) по правилу для способа 1.
*					тип Settings должен иметь члены int srcQ, dstQ, numVar
*					Если NumVar < 1, то в дальнейшем будет использован способ 2
*					Иначе будет использован способ 1
*	Parameters:	const Settings &settings - настройки
*	Returns:	-
*	Throws:		AxiomLibException, если в settings srcQ <= 1 || dstQ <= 1
*	Author:		wictor
*	History:
*
****************************************************************************/
template<class Settings>
void NeuralCoderGeneric::construct(const Settings &settings)
{	
	srcQ = settings.commonSettings.srcQ;
	dstQ = settings.coderSettings.dstQ;
	numVar = settings.coderSettings.numVar;
	windowSize = settings.coderSettings.windowSize;
	
	// Cчитаем, что если numVar < 1, то надо использовать способ 2
	if(numVar < 1) {
		if(dstQ != 2) {
			throw AxiomLibException("NeuralCoderWayGeneric::construct() : bad settings value");
		}
		codingWay = Way2;
		codedSymbolSize = srcQ;
		return;
	} else {
		if(dstQ <= 1) {
			throw AxiomLibException("NeuralCoderWay1::construct() : bad settings value");
		}
		codingWay = Way1;
	}
	
	// Случай, когда нет кодировки
	if(srcQ == dstQ && numVar == 1) {
		codedSymbolSize = 1;
		return;
	}
	
	if(srcQ <= 1 || dstQ <= 1) {
		throw AxiomLibException("NeuralCoderWay1::construct() : bad settings value");
	}
	
	int numWords = round(std::pow((double)srcQ, windowSize));
	
	dict.resize(numWords);
	
	codedSymbolSize = numDigits(dstQ, numVar*numWords-1);
	for(int i=0; i<numWords; i++)
	{
		Qstring str(codedSymbolSize);
		makeZero(str);
		int number=i*numVar +(rand()%numVar);
		numToQnaryVector(dstQ, number, str);
		dict[i].resize(codedSymbolSize);
		dict[i]=str;
	}
}

};

#endif /* NEURALCODERGENERIC_H_ */
