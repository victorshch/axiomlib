/****************************************************************************
*			NeuralConcepts.h
*
*	Description: Описание концептов NetWrapper, Holder и Coder.
*				Данный файл не включен в сборку, он нужен только для понимания кода. 
*				В нем, фактически, описываются
*				интерфейсы, которые должны предоставлять некоторые классы
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef NEURALCONCEPTS_H_
#define NEURALCONCEPTS_H_

#include "NeuralCommon.h"
#include "Environment.h"
#include <string>

namespace AxiomLib {

// NetWrapperMinimal + возможность добавлять доп. строку для обучения
class NetWrapper: public NetWrapperMinimal {
public:
	NetWrapper();
	
	// Добавляет дополнительную строку для обучения 
	// (относящуюся к последней добавленной строке для обучения)
	void supplyAuxTrainingData(const Qstring &str);
	
};

class NetWrapperMinimalWithCentralStrings: public NetWrapperMinimal {
public:
	NetWrapperMinimalWithCentralStrings();
	
	// Добавляет центральную строку.
	// Все строки для обучения, поданные после вызова этой функции,
	// относятся к данной центральной строке.
	void supplyCentralString(const Qstring &str);
	// При функционировании нейросети на некоторой поданной строке
	// она выдает номер некоторой эталонной строки.
	// Затем при отсечении по расстоянию
	// вычисляется расстояние между поданной строкой и
	// центральной строкой, соответствующей полученной
	// эталонной строке
};

// Это концепт нейросети, которая может обучаться и функционировать
class NetWrapperMinimal {
public:
	NetWrapperMinimal();
	
	// Добавляет строку для обучения
	void supplyTrainingData(const Qstring &str);

	// Возвращает строку для обучения с номером i
	const Qstring &getTrainString(int i) const;
			
	// Возвращает число строк для обучения
	int getNumTrain() const;
	
	// Максимальная размерность (незакодированной) обучающей строки
	int getMaxTrainDim() const;
	// Минимальная размерность (незакодированной) обучающей строки
	int getMinTrainDim() const;
	
	// Инициализирует объект настройкамим из settings
	// (вызывается после добавления всех обучающих строк)
	template<class Settings>
	void construct(const Settings &settings);
	
	// Обучает сеть на представленных ей ранее обучающих строках
	void train();
	
	// Запускает сеть на строке inp, возвращает номер обучающей строки либо -1
	int run(const Qstring &inp, double &p) const;
	
	// Очищает объект
	void clear();
};

// Это концепт сущности, осуществляющей хранение и кодирование обучающей выборки
class Holder {
public:
	Holder();
	
	// Добавляет строку для обучения
	void supplyTrainingData(const Qstring &str);
	
	// Добавляет дополнительную строку для обучения 
	// (относящуюся к последней добавленной строке для обучения)
	void supplyAuxTrainingData(const Qstring &str);
	
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
	int getCoderSrcQ() const;
	
	// Возвращает dstQ кодировщика
	int getCoderDstQ() const;
	
	// Возвращает длину закодированной строки с исходной длиной srcLen
	int getCoderDstLen(int srcLen) const;
	
	// Максимальная размерность (незакодированной) обучающей строки
	int getMaxTrainDim() const;
	// Минимальная размерность (незакодированной) обучающей строки
	int getMinTrainDim() const;
	
	// Возвращает число строк для обучения
	int getNumTrain() const;
	
	// Возвращает число наборов дополнительных строк для обучения
	// Фактически, возвращает то же, что getNumTrain()
	int getNumAuxTrain() const;
	
	// Возвращает число дополнительных строк для обучения, 
	// соответствующих строке для обучения с номером i
	int getNumAuxTrain(int i) const;
	
	// Инициализирует объект настройкамим из settings
	template<class Settings>
	void construct(const Settings &settings);
	
	// Очищает объект
	void clear();
};

// Это концепт сущности, осуществляющей кодирование строки
class Coder {
public:
	Coder();
	
	// Инициализирует объект настройкамим из settings
	template<class Settings>
	void construct(const Settings &settings);
	
	// Кодирует src, результат помещает в dst
	void translate(const Qstring &src, Qstring &dst) const;
	// Возвращает srcQ
	int getSrcQ() const;
	// Возвращает dstQ
	int getDstQ() const;
	
	// Длина выхода при длине входа srcLen	
	int getDstLen(int srcLen) const;
};

class Settings {
public:
	// Инициализирует значение настроек из окружения
	void initFromEnv(const Environment &env);
};

};

#endif