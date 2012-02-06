/****************************************************************************
*						NeuralMultiplexer.cxx
*
*       Description:	Реализация членов NeuralMultiplexer
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef NEURALMULTIPLEXER_CXX_
#define NEURALMULTIPLEXER_CXX_

#include "NeuralMultiplexer.h"
#include <boost/foreach.hpp>

#include <iostream>

namespace AxiomLib {

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::supplyTrainingData
*
*       Description:	Функция добавляет одну строку для обучения
*       Parameters:		const Qstring &s - добавляемая строка
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
void NeuralMultiplexer<NetWrapper>::supplyTrainingData(const Qstring &s) {
	int len = s.size();
	
	// Получаем указатель на сеть, соответствующую длине len 
	// (если ее нет, создаем)
	/*NetWrapper *pNetWrapper; // Было так, НО - ни один компилятор в линуксе не смог создать iterator для определения map в котором используются названия из шаблона.
	MapType::const_iterator it = netArray.find(len);
	if(it!=netArray.end()) {
		// Сеть для данной длины уже есть - получаем указатель на нее
		pNetWrapper = it->second;
	} else {
		// Сети для данной длины еще нет - создаем ее
		pNetWrapper = new NetWrapper();
		netArray.insert(std::make_pair(len, pNetWrapper));
	}*/
	NetWrapper *pNetWrapper;
	if(netArray.find(len) != netArray.end()) {
		// Сеть для данной длины уже есть - получаем указатель на нее
		pNetWrapper = (netArray.find(len))->second;
	} else {
		// Сети для данной длины еще нет - создаем ее
		pNetWrapper = new NetWrapper();
		netArray.insert(std::make_pair(len, pNetWrapper));
	}
	// Получаем число уже добавленных в netWrapper строк
	// Это будет номером добавляемой строки в этой сети
	int num = pNetWrapper->getNumTrain();
	
	// Добавляем строку в сеть
	pNetWrapper->supplyTrainingData(s);
	
	//Локальный индекс данной строки
	IndexPair ip = std::make_pair(len, num);
	glNumToLocNum.push_back(ip);
	
	// Глобальный номер
	locNumToGlNum[ip] = globalTrainSize;
	
	// Увеличиваем счетчик строк
	globalTrainSize++;
	
	// Заполняем lastIndex
	lastIndex.len = ip.len;
	lastIndex.num = ip.num;
}

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::supplyCentralString
*
*       Description:	Функция добавляет одну primary-строку для обучения
*       Parameters:		const Qstring &s - добавляемая строка
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
void NeuralMultiplexer<NetWrapper>::supplyCentralString(const Qstring &s) {
	int len = s.size();
	
	// Получаем указатель на сеть, соответствующую длине len 
	// (если ее нет, создаем)
	/*NetWrapper *pNetWrapper;	
	MapType::const_iterator it = netArray.find(len);
	if(it!=netArray.end()) {
		// Сеть для данной длины уже есть - получаем указатель на нее
		pNetWrapper = it->second;
	} else {
		// Сети для данной длины еще нет - создаем ее
		pNetWrapper = new NetWrapper();
		netArray.insert(std::make_pair(len, pNetWrapper));
	}*/
	NetWrapper *pNetWrapper;
	if(netArray.find(len) != netArray.end()) {
		// Сеть для данной длины уже есть - получаем указатель на нее
		pNetWrapper = (netArray.find(len))->second;
	} else {
		// Сети для данной длины еще нет - создаем ее
		pNetWrapper = new NetWrapper();
		netArray.insert(std::make_pair(len, pNetWrapper));
	}
	// Получаем число уже добавленных в netWrapper строк
	// Это будет номером добавляемой строки в этой сети
	int num = pNetWrapper->getNumTrain();
	
	// Добавляем центральную строку в сеть
	pNetWrapper->supplyCentralString(s);
	
	//Локальный индекс данной строки
	IndexPair ip = std::make_pair(len, num);
	glNumToLocNum.push_back(ip);
	
	// Глобальный номер
	locNumToGlNum[ip] = globalTrainSize;
	
	// Увеличиваем счетчик строк
	globalTrainSize++;
	
	// Заполняем lastIndex
	lastIndex.len = ip.len;
	lastIndex.num = ip.num;
}

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::getTrainString
*
*       Description:	Возвращает обучающую строку с (глобальным) номером i
*       Parameters:		int i - номер строки
*       Returns:		const Qstring & - строка для обучения
*       Throws:			AxiomLibException, если номер строки некорректен
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
const Qstring &NeuralMultiplexer<NetWrapper>::getTrainString(int i) const {
	if (i < 0 || i >= globalTrainSize) {
		throw AxiomLibException("NeuralMultiplexer<NetWrapper>::getTrainString() : i out of bounds");
	}
	// Переводим глобальную нумерацию в локальную
	int len = glNumToLocNum[i].len;
	int num = glNumToLocNum[i].num;
	// Ищем сеть с нужной длиной с структуре netArray
	/*MapType::const_iterator it = netArray.find(len);
	
	// Этого не должно случаться, но проверка оставлена на всякий случай
	if(it == netArray.end()) {
		throw AxiomLibException("NeuralMultiplexer<NetWrapper>::getTrainString() : len not found (this shouldn't happen)");
	}
	
	// Возвращаем нужную строку
	return it->second->getTrainString(num);*/	
	// Этого не должно случаться, но проверка оставлена на всякий случай
	if(netArray.find(len) == netArray.end()) {
		throw AxiomLibException("NeuralMultiplexer<NetWrapper>::getTrainString() : len not found (this shouldn't happen)");
	}
	
	// Возвращаем нужную строку
	return (netArray.find(len))->second->getTrainString(num);
}

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::supplyAuxTrainingData
*
*       Description:	Функция добавляет одну дополнительную строку для обучения,
*						которая соответствует последней добавленной строке для обучения
*       Parameters:		const Qstring &s - добавляемая строка
*       Returns:		-
*       Throws:			AxiomLibException, если ни разу не было вызвано supplyTrainingData(),
*						либо размер inp не совпадает с размером последней добавленной строки для обучения
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
void NeuralMultiplexer<NetWrapper>::supplyAuxTrainingData(const Qstring &s) {
	int len = lastIndex.len;
	if(len <= 0) {
		throw AxiomLibException("NeuralMultiplexer<NetWrapper>::supplyAuxTrainingData : no train strings were supplied");
	}
	
	if(netArray.find(len) == netArray.end()) {
		throw AxiomLibException("NeuralMultiplexer<NetWrapper>::supplyAuxTrainingData : len not found (this shouldn't happen)");
	}
	
	// Добавление доп. строки для обучения в нужную сеть
	(netArray.find(len))->second->supplyAuxTrainingData(s);
}

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::construct
*
*       Description:	Установка параметров и создание сетей
*       Parameters:		const Settings &settings - параметры
*						тип Settings должен удовлетворять NetWrapper::construct()
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
template<class Settings>
void NeuralMultiplexer<NetWrapper>::construct(const Settings &settings) {
	// Задание параметров из настроек - для каждой сети
	MapTypePair val;
	BOOST_FOREACH(val, netArray) {
		val.second->construct(settings);
	}
}

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::train
*
*       Description:	Функция обучает сети на ранее предоставленных ей обучающих строках
*       Parameters:		-
*       Returns:		-
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
void NeuralMultiplexer<NetWrapper>::train() {
	// Итерация по массиву сетей и обучение каждой из них
	MapTypePair val;
	BOOST_FOREACH(val, netArray) {
		val.second->train();
	}
}

/****************************************************************************
*						NeuralMultiplexer<NetWrapper>::run
*
*       Description:	Функция запускает нейросетевой алгоритм
*						
*       Parameters:		const Qstring &inp - вход сети ()
*						double &p - число, которое может возвращать сеть при работе (пока не используется)
*       Returns:		Номер эталонной строки (в том порядке, в котором они были заданы функцией supplyTrainingData()), 
*						либо -1, если все объекты NetWrapper в массиве сетей вернули -1
*       Throws:			-
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
int NeuralMultiplexer<NetWrapper>::run(const Qstring &inp, double &p) const {
	// Для каждого элемента массива сетей
	MapTypePair val;
	BOOST_FOREACH(val, netArray) {
		
		Qstring actualInp;
		
		// Длина входа
		const int &len = val.first;
		// Указатель на нейросетевой объект
		const NetWrapper *pNetWrapper = val.second;
		// Копируем в actualInp содержимое inp от 0 до len
		copyVector(inp, actualInp, 0, len);
		
		// Запускаем текущую сеть на actualInp
		int res = pNetWrapper->run(actualInp, p);
		
		// Если она что-то обнаружила - возвращаем полученный результат
		if (res >= 0) {
			// Переводим результат в глобальную нумерацию
			std::map<IndexPair, int>::const_iterator it = locNumToGlNum.find(IndexPair(std::make_pair(len, res)));
			// Этого не должно случаться, но проверка оставлена на всякий случай
			if(it == locNumToGlNum.end()) {
				throw AxiomLibException("NeuralMultiplexer<NetWrapper>::run(): pair not found (this shouldn't happen)");
			}
			return it->second;
		}
	}
	// Если ни одна из сетей ничего не нашла, то возвращаем -1
	return -1;
}

};

#endif
