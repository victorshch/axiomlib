/****************************************************************************
*				PercWrapper.cxx
*
*	Description:	Файл, содержащий реализацию того, что описано в PercWrapper.h
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef PERCWRAPPER_CXX_
#define PERCWRAPPER_CXX_

#include "AxiomLibException.h"
#include "PercWrapper.h"

namespace AxiomLib {

// Значение входа нейрона, которое ставится в соответствие 0 в обучающей строке
const double zeroValue = 0.0;

/****************************************************************************
*						PercWrapper<Holder>::construct
*
*       Description:	Установка параметров и создание сети
*       Parameters:		const Settings &settings - параметры
*						тип Settings должен иметь поля 
*							CommonSettings commonSettings,
*							PercSettings percSettings,
*						а также удовлетворять требованиям Holder::construct()
*       Returns:		-
*       Throws:			AxiomLibException, если dstQ кодировщика не равно 2
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
template<class Settings>
//тип Settings должен иметь член PercSettings percSettings
void PercWrapper<Holder>::construct(const Settings & s) {
	// Инициализация настроек Holder
	Holder::construct(s);
	
	// Проверка
	if(Holder::getCoderDstQ()!=2) {
		throw AxiomLibException("PercWrapper<Holder>::construct(): dstQ value is not 2");
	}
	
	// Алиас для настроек перцептрона (для упрощения записи)
	const PercSettings &settings = s.percSettings;
	
	// Вычисление числа входных и выходных нейронов
	// Число входов - максимальная длина закодированной обучающей строки
	numInp = Holder::getCoderDstLen(Holder::getMaxTrainDim());
	// Число выходов - число обучающих строк
	numOutp = Holder::getNumTrain();
	
	// Заполнение массива чисел нейронов в слоях
	unsigned int *layers=new unsigned int[settings.numLay+1];
	layers[0]=numInp;
	for(int i=1; i<settings.numLay; i++)
		// Число нейронов во внутреннем слое - "нормальное" число,
		// умноженное на множитель numNeuronsF[i-1] из настроек
		layers[i]=static_cast<int>(std::floor(settings.numNeuronsF[i-1]*
		(calcAveNumNeurons(i, settings.numLay, numInp, numOutp))));
	layers[settings.numLay]=numOutp;
	
	// Создание сети
	net.create_standard_array(settings.numLay+1, layers);
	
	// Освобождение динамического массива
	delete [] layers;

	// Установка активационной функции и ее параметра для каждого слоя
	for(int i=1; i<=settings.numLay; i++)
	{
		net.set_activation_function_layer(settings.actFuncs[i-1], i);
		net.set_activation_steepness_layer(settings.actFuncsS[i-1], i);
	}

	// Установка желаемой ошибки
	desiredError=settings.desiredError;
	
	// Установка обучающего алгоритма
	net.set_training_algorithm(settings.trainingAlgorithm);
	// Мы устанавливаем learningRate и learningMomentum,
	// только если используется простое обратное распространение ошибки
	// (иначе алгоритмы сами их определяют, и устанавливать их вручную не
	// рекомендуется)
	if(settings.trainingAlgorithm==FANN::TRAIN_BATCH || settings.trainingAlgorithm==FANN::TRAIN_INCREMENTAL
		||settings.trainingAlgorithm==FANN::TRAIN_QUICKPROP)
	{
		net.set_learning_rate((float) settings.learningRate);
		net.set_learning_momentum((float) settings.learningMomentum);
	}
	
	// Установка остальных настроек
	maxIters = settings.maxIters;
	itersBetweenReports = settings.itersBetweenReports;
	
	// Если коментарии не нужны, мы ставим пустой defaultCallback
	// (иначе по умолчанию стоит некоторый callback, выводящий информацию)
	if(!s.commonSettings.comments) {
		net.set_callback(&(PercWrapper<Holder>::defaultCallback), 0);
	}
	
	// Формирование data
	constructTrainingData();
	
	// Установка состояния
	stage = 1;
}

/****************************************************************************
*						PercWrapper<Holder>::train
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
void PercWrapper<Holder>::train() {
	// TODO: выводить больше информации, сделать в Holder поле comments
	
	// Инициализация весов
	net.init_weights(data);
	// Обучение
	net.train_on_data(data, maxIters,
		  itersBetweenReports, (float) desiredError);
	// Установка состояния
	stage = 2;
}

/****************************************************************************
*						PercWrapper<Holder>::constructTrainingData
*
*       Description:	Построение обучающей выборки в том виде, в котором ее
*						принимает FANN
*       Parameters:		-
*       Returns:		-
*       Throws:			AxiomLibException, если обучающая выборка пуста
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
void PercWrapper<Holder>::constructTrainingData() {
	int sz=Holder::getNumTrain();
	
	numInp = Holder::getCoderDstLen(Holder::getMaxTrainDim());
	numOutp = Holder::getNumTrain();

	int aux_sz = 0;
	//int sz2=0;

	// Считаем число дополнительных строк для обучения
	for(int i = 0; i<Holder::getNumAuxTrain(); i++)
	{
		aux_sz+=Holder::getNumAuxTrain(i);
	}


	//sz2 = sz+aux_sz;
	
	// Вычисляем общий размер обучающей выборки (включая доп. строки)
	sz += aux_sz;
	
	//sz += inhibiting_data.size();
	
	// Проверка
	if(sz==0) {
		throw AxiomLibException("PercWrapper<Holder>::constructTrainingData() : no training data supplied");
	}
	
	// Массивы входов и выходов сети (fann_type - это double)
	fann_type **input=new fann_type *[sz];
	fann_type **output=new fann_type *[sz];

	// Сквозной счетчик элементов обучающей выборки
	// (как основных, так и дополнительных)
	int i=0;
	
	// Цикл по всем основным строкам для обучения
	for (int ii=0; ii<Holder::getNumTrain(); ii++)
	{
		// Вход и соответствующий ему выход
		input[i]=new fann_type[numInp];
		output[i]=new fann_type[numOutp];
		
		// Заполняем вход
		for(int j=0; j < numInp; j++) {
			input[i][j]=transform(Holder::getTranslated(ii), j);
		}
		
		// Заполняем выход
		for(int j=0; j < numOutp; j++)
		{
			output[i][j]= (ii==j) ? 1.0 : zeroValue;
		}
		
		// Увеличиваем счетчик
		i++;
		
		// Цикл по всем дополнительным строкам для обучения,
		// соответствующим данной основной строке
		// тут все аналогично
		for(int iii=0; iii<Holder::getNumAuxTrain(ii); iii++)
		{
			input[i]=new fann_type[numInp];
			output[i]=new fann_type[numOutp];
			
			for (int j = 0; i < numInp; j++) {
				input[i][j]=transform(Holder::getAuxTranslated(ii, iii), j);			
			}

			for (int j = 0; i < numOutp; j++) {
				// Здесь мы ставим не 1, а чтото меньшее,
				// т.к. это не основной паттерн, на который нужно реагировать
				output[i][j]= (ii==j) ? 
					determineOutput(Holder::getAuxTranslated(ii, iii), Holder::getTranslated(ii)) : 
					zeroValue;
			}
			
			i++;
		}
	}

	//for( ;i<sz; i++)
	//{
	//	input[i]=new fann_type[numInp];
	//	output[i]=new fann_type[numOutp];
	//	for(int j=0; j< max(numInp, numOutp); j++)
	//	{
	//		if(j<numInp) input[i][j]=inhibiting_data[i - sz2](j);
	//		if(j<numOutp) output[i][j]= zero_value;
	//	}
	//}

	// Заполняем data
	data.set_train_data(sz, numInp, input, numOutp, output);

	// Освобождаем память
	for(int i=0; i<sz; i++)
	{
		delete [] input[i];
		delete [] output[i];
	}
	delete [] input;
	delete [] output;
}

/****************************************************************************
*						PercWrapper<Holder>::run
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
int PercWrapper<Holder>::run(const Qstring &pattern, double &probability) const {
	// Вход в том виде, в котором понимает его FANN
	fann_type *input=new fann_type[numInp];
	
	// Кодирование входной строки
	Qstring patternCoded = Holder::coderTranslate(pattern);
	
	// Заполнение входной строки в виде массива
	for(int i=0; i< numInp; i++)
	{
		input[i]=transform(patternCoded,i);
	}

	// Запуск сети
	fann_type *res=net.run(input);

	// Освобождение памяти
	delete [] input;
	
	// Определение максимального индекса в выходе сети
	int max_index=0;
	for(int i=1; i<numOutp; i++) {
		if(res[i]>res[max_index]) max_index=i;
	}
	
	// Параметру присваиваем значение максимального выхода нейрона
	probability=res[max_index];
	
	return max_index;
}

/****************************************************************************
*						PercWrapper<Holder>::PercWrapper
*
*       Description:	Конструктор копирования - он нетривиален, т.к.
*						тип FANN::training_data не copy-constructible
*						В данном конструкторе data не копируется,
*						а создается заново, если нужно
*       Parameters:		const PercWrapper<Holder> &other
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class Holder>
PercWrapper<Holder>::PercWrapper(const PercWrapper<Holder> &other):
	Holder(other),
	net(other.net),
	numInp(other.numInp),
	numOutp(other.numOutp),
	maxIters(other.maxIters),
	itersBetweenReports(other.itersBetweenReports),
	desiredError(other.desiredError),
	stage(other.stage) {
	if(stage >= 1)  {
		constructTrainingData();
	}
}

};

#endif
