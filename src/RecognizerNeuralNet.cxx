/****************************************************************************
*				RecognizerNeuralNet.cxx
*
*	Description:	Функции шаблона класса 
*					RecognizerNeuralNet<NetObject, Settings> - Реализация
*	Author:		wictor
*	History:		
*
****************************************************************************/

#ifndef RECOGNIZERNEURALNET_CXX_
#define RECOGNIZERNEURALNET_CXX_

#include "RecognizerNeuralNet.h"
#include <iostream>
#include <cstdlib>

namespace AxiomLib {

/****************************************************************************
*                       RecognizerNeuralNet<NetObject, Settings>::countErrors
*
*       Description:    Подсчет числа ошибок первого и второго рода
*       Parameters: const std::vector<int> &res - вектор полученных результатов
*					const std::vector<int> &ref - вектор "правильных ответов"
*					int &first
*					int &second
*       Returns:        -
*       Throws:         -
*       Author:         wictor
*       History:
*
****************************************************************************/
template<class NetObject, class Settings>
void RecognizerNeuralNet<NetObject, Settings>::countErrors(const std::vector<int> &res, const std::vector<int> &ref, 
	int &first, int &second) {
	// Проверка на совпадение размеров res и ref
	if(res.size() != ref.size()) {
		throw AxiomLibException("countErrors() : res and ref sizes don't match");
	}
	
	first = 0;
	second = 0;
	
	// Ошибки первого рода
	for(unsigned int i = 0; i < ref.size(); i++) {
		if(res[i] != 0 && ref[i] == 0) {
			if(!scanDelta(ref, i, delta, res[i])) {
				first++;
			}
		}
	}
	
	// Ошибки второго рода
	int curr;
	int prev = 0;
	for (unsigned int i = 0; i < ref.size(); i++) {
		curr = ref[i];
		bool isCurrNormal = (curr == 0);
		if (!isCurrNormal && (curr!=prev)) {
			if(!scanDelta(res, i, delta, curr)) {
				second++;
			}
		}
		prev = curr;
	}
}

/****************************************************************************
*                     RecognizerNeuralNet<NetObject, Settings>::run
*
*       Description:    Распознавание нештатных ситуаций, затем сравнение с "правильным ответом" и подсчет числа ошибок первого и второго рода
*						Эта функция вызывает runInternal
*       Parameters: AxiomSet& axiomSet - игнорируется   
*					DataSet& dataSet - датасет 
*					std::vector<int>& params - номера используемых рядов датасета
*					int& resFirst - число ошибок первого рода
*					int& resSecond - число ошибок второго рода
*       Returns:        0
*       Throws:         -
*       Author:         wictor
*       History:
*
****************************************************************************/
template<class NetObject, class Settings>
signed int RecognizerNeuralNet<NetObject, Settings>::run(AxiomSet& axiomSet, DataSet& dataSet, 
	std::vector<int>& params, int& resFirst, int& resSecond, const int asIndex) {
	return runInternal(dataSet, params, resFirst, resSecond);
}

/****************************************************************************
*                       RecognizerNeuralNet<NetObject, Settings>::run
*
*       Description:    Распознавание нештатных ситуаций, затем сравнение с "правильным ответом" и подсчет числа ошибок первого и второго рода
*						Эта функция вызывает runInternal
*       Parameters: AxiomSet& axiomSet - игнорируется   
*					DataSet& dataSet - датасет 
*					std::vector<int>& params - номера используемых рядов датасета
*					int& resFirst - число ошибок первого рода
*					int& resSecond - число ошибок второго рода
*					std::vector<double> &stat - игнорируется
*       Returns:        0
*       Throws:         -
*       Author:         wictor
*       History:
*
****************************************************************************/
template<class NetObject, class Settings>
signed int RecognizerNeuralNet<NetObject, Settings>::run(AxiomSet& axiomSet, DataSet& dataSet, 
	std::vector<int>& params, int& resFirst, int& resSecond, std::vector<double> &stat, const int asIndex) {
	return runInternal(dataSet, params, resFirst, resSecond);
}

/****************************************************************************
*                       RecognizerNeuralNet<NetObject, Settings>::learn
*
*       Description:    Обучение сети на датасете, ранее установленом setDataSet()
*       Parameters:     templateRecognizer - игнорируется
*       Returns:        0
*       Throws:         AxiomLibException - если размер dataSetParams равен 0
*       Author:         wictor
*       History:
*
****************************************************************************/
template<class NetObject, class Settings>
double RecognizerNeuralNet<NetObject, Settings>::learn(TemplateRecognizer &templateRecognizer) {
	if(dataSetParams.size() == 0) {
		throw AxiomLibException("learn() : no parameters to analyze (dataSetParams is empty)");
	}

	classNumbers.clear();
	
	// Получение размерности эталонных строк
	int numOfClasses;
	std::vector<int> numOfMultiTS;
	std::vector<std::vector<int> > numOfTS;
	dataSet.getClassSize(numOfClasses, numOfMultiTS, numOfTS);	
	
	Qstring str;
	// TODO: reserve some memory for classNumbers
	// Итерация по номерам параметров
	for(unsigned int paramNumber = 0; paramNumber < dataSetParams.size(); paramNumber++) {		
		// Итерация по классам
		for(int classNo = 0; classNo < numOfClasses; classNo++) {
			// Итерация по мультирядам	
			for(int multiTSNo = 0; multiTSNo < numOfMultiTS[classNo]; multiTSNo++) {							
				// Получаем разметку эталонной траектории
				std::vector<double> currTS;
				dataSet.getTSByIndexFromClass(currTS, classNo, multiTSNo, dataSetParams[paramNumber]);
				
				// Переводим из std::vector в Qstring
				copyVector(currTS, str);
				// Передаем обучающую строку
				if(multiTSNo == 0 && useCentral) {
					netObject.supplyCentralString(str);					
				} else {
					netObject.supplyTrainingData(str);
				}
				
				// Записываем в classNumbers номер текущего класса плюс один (так, чтобы он считался от 1, т.к. 0 - нормальное поведение)
				classNumbers.push_back(classNo+1);
			}
		}
	}	
	
	// Вызов construct - строит сеть (для этого ей (в частности) нужно 
	// знать размер входа, так что мы передавали обучающие данные до вызова этой функции)
	netObject.construct(settings);

	// Вызов train - обучает сеть (на данных, которые уже переданы)
	netObject.train();
	
	return 0;
}

/****************************************************************************
*		RecognizerNeuralNet<NetObject, Settings>::setParamsFromEnv
*
*       Description:    Инициализировать параметры данными из env
*       Parameters:     const Environment& env - среда
*       Returns:        0
*       Throws:         AxiomLibException - если нет какого-то параметра
*       Author:         wictor
*       History:
*
****************************************************************************/
template<class NetObject, class Settings>
int RecognizerNeuralNet<NetObject, Settings>::setParamsFromEnv(const Environment& env) {
	// Инициализация настроек нейросетевого объекта
	settings.initFromEnv(env);
	
	// Инициализация флага коментариев
	comments = settings.commonSettings.comments;
	
	// Инициализация флага использования primary-строк
	useCentral = settings.commonSettings.useCentral;
	
	// инициализация delta
	if(env.getIntParamValue(delta ,"delta") < 0) {
		throw AxiomLibException("setParamsFromEnv(): delta not set");
	};

	return 0;
}

/****************************************************************************
*                       RecognizerNeuralNet<NetObject, Settings>::runInternal
*
*       Description:    Распознавание нештатных ситуаций, затем сравнение с 
*						"правильным ответом" и подсчет числа ошибок 
*						первого и второго рода
*       Parameters:     DataSet& dataSet - датасет 
*					std::vector<int>& params - номера используемых рядов датасета
*					int& resFirst - число ошибок первого рода
*					int& resSecond - число ошибок второго рода
*       Returns:        0
*       Throws:         AxiomLibException - если размер params равен 0
*       Author:         wictor
*       History:
*
****************************************************************************/
template<class NetObject, class Settings>
int RecognizerNeuralNet<NetObject, Settings>::runInternal(DataSet& dataSet, std::vector<int>& params, 
	int& resFirst, int& resSecond) {
	
	if(params.size() == 0) {
		throw AxiomLibException("RecognizerNeuralNet<NetObject, Settings>::runInternal() : no parameters to analyze (params is empty)");
	}

	//Число ошибок, допущенных при данном значении номера ряда из params (обнуляется на каждой итерации по params)
	int first=0, second=0;
	
	// Получаем размеронсти тестов
	int numOfTests;
	std::vector<int> numOfTS;
	dataSet.getTestSize(numOfTests, numOfTS);
	
	// Итерация по тестам
	for(int testNo = 0; testNo < numOfTests; testNo++) {			
		std::vector<int> currTS, currRefTS;


		// Получаем "правильный ответ"
		dataSet.getTSByIndexFromTests(currRefTS, testNo, -1);
			//Итерация по значениям из params
		for(unsigned int paramNo=0; paramNo < params.size(); paramNo++) {
			// Получаем разметку фазовой траектории
			dataSet.getTSByIndexFromTests(currTS, testNo, params[paramNo]);

			// Разметка, которую получает наш алгоритм
			std::vector<int> resTS(currTS.size(), 0);
						
			// Значение позиции в фазовой строке, до которой окно будет доходить
			int maxI = currTS.size() - netObject.getMinTrainDim();
			
			// Итерация по размеченной фазовой траектории
			for (int i = 0; i < maxI; i++) {
				Qstring str;
				// Берем окно длины, равной максимальной длине строки и копируем его в str
				copyVector(currTS, str, i, netObject.getMaxTrainDim());
				double p;
				// Запускаем нейросетевой алгоритм
				int result = netObject.run(str, p);
				
				// Анализируем результат
				if(result>=0) {
					// Если нештатная ситуация, то ставим номер ее класса на позицию i
					resTS[i] = classNumbers[result];
					// и прибавляем размер найденной строки к позиции
					i+=netObject.getTrainString(result).size();
				} // Иначе - 0 уже стоит
			}
			// Анализируем полученный ответ и правильный ответ, делаем выводы о количестве ошибок
			countErrors(resTS, currRefTS, first, second);
			if(comments) {
				std::cout<<"Test number: "<<testNo<<"\tTS number: "<<
				dataSetParams[paramNo]<<"\nfirst kind errors: "<<
				first<<"\nsecond kind errors: "<<second<<"\n";
			}
			
			// Увеличиваем общее число ошибок
			resFirst += first;
			resSecond += second;
		}
	}
	return 0;
}

};

#endif
