/****************************************************************************
*						PercWrapper.h
*
*       Description:	Объявление шаблона класса PercWrapper
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef PERCWRAPPER_H_
#define PERCWRAPPER_H_

#include "PercSettings.h"

#include "NeuralCommon.h"

namespace AxiomLib {

/****************************************************************************
*						PercWrapper<Holder>
*
*       Description:	Класс, инкапсулирующий многослойный перцептрон
*						Моделирует NetWrapper (см. NeuralConcepts.h)
*						Holder должен моделировать концепт Holder
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
class PercWrapper: public Holder {
	// Объект сети
	// Пришлось сделать его mutable, чтобы сохранить
	// константность вызова run()
	// т.к. вызов FANN::neural_net::run() не константный
	mutable FANN::neural_net net;
	// Данные для обучения
	FANN::training_data data;
	// Число входных и выходных нейронов
	int numInp, numOutp;
	// Макс. число эпох
	int maxIters;
	// Число эпох между сообщениями о состоянии обучения
	int itersBetweenReports;
	// Ошибка, при которой обучение прекращается
	double desiredError;
	
	// 0 - construct() не вызвана, 1 - construct() вызвана, train() не вызвана, 
	// 2 - construct() и train() вызваны
	int stage;
	
	// Построение обучающей выборки
	void constructTrainingData();
	
	// Преобразование значения компоненты строки
	// в значение, подаваемое на вход нейрону
	static double transform(const Qstring &str, int j) 
	{	
		if (j >= (int) str.size() || j < 0) {
			return 0;
		}
		return str[j] > 0.5 ? 1.0 : 0.0;
	}
	// Определение значение, ставимого в соответствие обучающей строке на
	// соответствующем выходном нейроне
	// (введено для того, чтобы доп. обучающим строкам давать значение,
	// меньшее 1)
	static double determineOutput(const Qstring &templ, const Qstring &current) 
	{ return 1.0-difference(templ, current); }

	// Заглушка для функции обратного вызова, она ставится, если комментарии не нужны
	static int defaultCallback(FANN::neural_net &net, FANN::training_data &train,
	        unsigned int max_epochs, unsigned int epochs_between_reports,
	        float desired_error, unsigned int epochs, void *user_data) { return 0; }
	
	// Подсчет "норамльного" числа нейронов в слое (в зависимости от номера слоя и
	// числа входных и выходных нейронов)
	static int calcAveNumNeurons(int layer_no, int num_lay, int num_inp, int num_outp) {
		return (layer_no*num_outp + (num_lay-layer_no)*num_inp)/num_lay;
	}

public:
	// Конструктор умолчания - инициализирует некоторые поля
	PercWrapper():numInp(0), numOutp(0), stage(0) { /*net.set_callback(defaultCallback, 0);*/ }
	
	// Конструктор копирования - нетривиален из-за того, что 
	// FANN::training_data не copy-constructible
	PercWrapper(const PercWrapper &);
	
	// Инициализация настроек
	template<class Settings>
	void construct(const Settings &settings);
	
	// Обучение
	void train();
	
	// Запуск нейросети на строке str
	int run(const Qstring &str, double &p) const;
	
	//void clear();
	// TODO: реализовать

};

};

#include "PercWrapper.cxx"

#endif
