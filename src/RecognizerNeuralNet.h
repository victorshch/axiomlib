/****************************************************************************
*			RecognizerNeuralNet.h
*
*	Description: Описание шаблона класса RecognizerNeuralNet - распознавателя,
*				использующего нейронные сети
*	Author:		wictor
*	History:	
*
****************************************************************************/

//#define RECOGNIZERNEURALNET_H_

#ifndef RECOGNIZERNEURALNET_H_
#define RECOGNIZERNEURALNET_H_

#include "Environment.h"
#include "AxiomSetPop.h"
#include "Recognizer.h"
#include "DataSet.h"
#include "NeuralCommon.h"
#include <vector>
#include <string>

namespace AxiomLib {

/****************************************************************************
*			RecognizerNeuralNet.h
*
*	Description: Класс для распознавания нештатных ситуаций с использованием
*				нейросетевого алгоритма.
*				NetObject должен моделировать концепт NetWrapperMinimal (см. NeuralConcepts.h)
*				Settings должен удовлетворять требованиям NetObject::construct(),
*				иметь член bool commonSettings.comments
*				и моделировать концепт Settings
*	Author:		wictor
*	History:	
*
****************************************************************************/
template<class NetObject, class Settings>
class RecognizerNeuralNet: public Recognizer {
private:
	// Флаг того, нужно ли выводить коментарии в cout
	bool comments;
	
	// флаг того, нужно ли использовать центральные строки
	bool useCentral;
	
	// Настройки для объекта NetObject
	Settings settings;
		
	// Параметр дельта
	int delta;
	
	// Нейросетевой объект
	NetObject netObject;
	
	// Датасет
	DataSet dataSet;
	
	// Номера тех рядов датасета, которые нужно использовать
	std::vector<int> dataSetParams;
	
	// Отображение (номер строки в обучающей выборке сети -> номер класса нештатной ситуации)
	std::vector<int> classNumbers;
	
	// Подсчет числа ошибок первого и второго рода
	void countErrors(const std::vector<int> &res, const std::vector<int> &ref, 
		int &first, int &second);
	
	//  Распознавание нештатных ситуаций, затем сравнение с 
	//	"правильным ответом" и подсчет числа ошибок 
	//	первого и второго рода
	int runInternal(DataSet& dataSet, std::vector<int>& params, 
		int& resFirst, int& resSecond);
	
protected:
	
public:

	// Конструктор умолчания	
	RecognizerNeuralNet():comments(false) {}
	
	virtual void setOutputToFile(const bool newValue) {}
	
	// Установка флага коментариев
	void setComments(const bool newValue) { comments = newValue; }
	
	//    Инициализировать параметры данными из env
	int setParamsFromEnv(const Environment& env);

	// Установка главных параметров из окружения (в данном случае то же самое, что setParamsFromEnv(env))
	int setNominalParamsFromEnv(const Environment& env) { return setParamsFromEnv(env); }
	
	// Установка датасета
	int setDataSet(const DataSet &newDataSet) {
		dataSet = newDataSet;
		return 0;
	}
	
	// Установка параметров датасета
	int setParamsOfDataSet(std::vector<int> &newParams) { 
		dataSetParams = newParams; 
		return 0; 
	}
	
	// Создание копии на куче
	Recognizer* copy(void) {
		// Конструктора копирования по умолчанию здесь достаточно 
		return new RecognizerNeuralNet<NetObject, Settings>(*this);
	}
	
	// Обучение сети на датасете, ранее установленом setDataSet()
	double learn(TemplateRecognizer &templateRecognizer);
	
	// Распознавание, этот вариант функции - заглушка
	signed int run(AxiomSetPop& pop) { return 0; }

	// Распознавание, этот вариант функции - заглушка
	signed int run(AxiomSetPop& pop, std::vector <double> &precs) { return 0; }

	//       Распознавание нештатных ситуаций, затем сравнение с "правильным ответом" и подсчет числа ошибок первого и второго рода
	//				Эта функция вызывает runInternal
	signed int run(AxiomSet& axiomSet, DataSet& dataSet, 
		std::vector<int>& params, int& resFirst, int& resSecond, const int asIndex = -1);

	//   Распознавание нештатных ситуаций, затем сравнение с "правильным ответом" и подсчет числа ошибок первого и второго рода
	//						Эта функция вызывает runInternal
	signed int run(AxiomSet& axiomSet, DataSet& dataSet, 
		std::vector<int>& params, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex = -1);
};

};

#include "RecognizerNeuralNet.cxx"

#endif
