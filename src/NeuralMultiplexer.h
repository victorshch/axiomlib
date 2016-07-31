/****************************************************************************
*						NeuralMultiplexer.h
*
*       Description:	Объявление шаблона класса NeuralMultiplexer и структуры IndexPair
*       Author:			wictor
*       History:
*
****************************************************************************/
#ifndef NEURALMULTIPLEXER_H_
#define NEURALMULTIPLEXER_H_

#include "NeuralCommon.h"
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/foreach.hpp>


namespace AxiomLib {

/****************************************************************************
*						IndexPair
*
*       Description:	Пара (len, num), для которой реализован оператор <
*       Author:			wictor
*       History:
*
****************************************************************************/
struct IndexPair: public std::pair<int, int> {
	// Алиас для first - длина строки
	int &len;
	// Алиас для second - номер обучающей строки этой длины
	int &num;

	//Конструктор преобразования из pair и умолчания
	IndexPair(const std::pair<int, int> &p):std::pair<int, int>(p), len(first), num(second) {}
	
	// Оператор < - он нужен для использования
	// данного класса в map в качестве key_type
	bool operator <(const IndexPair &p) {
		// Лексикографический порядок
		return len == p.len ? num < p.num : len < p.len;
	}
	
	// Конструктор копирования
	IndexPair(const IndexPair &ip): std::pair<int, int>(ip), len(first), num(second) {}
	
	// Оператор присваивания
	IndexPair &operator=(const IndexPair &ip) {
		len = ip.len;
		num = ip.num;
		return *this;
	}
};

/****************************************************************************
*						NeuralMultiplexer
*
*       Description:	Класс - прослойка, хранящий массив нейросетей
*						(по одной на каждую длину строки)
*						Моделирует NetWrapperMinimalWithCentralStrings
*						(см. NeuralConcepts.h)
*						NetWrapper должен моделировать NetWrapperMinimalWithCentralStrings
*       Author:			wictor
*       History:
*
****************************************************************************/
template<class NetWrapper>
class NeuralMultiplexer {
	// Тип массива сетей
	typedef std::map<int, NetWrapper *> MapType;
	typedef std::pair<int, NetWrapper *> MapTypePair;
	// Массив сетей
	MapType netArray;
	// Преобразование глобальной нумерации (номер строки) в локальную (длина, номер)
	std::vector<IndexPair> glNumToLocNum;
	// Преобразование локальной нумерации в глобальную
	std::map<IndexPair, int> locNumToGlNum;
	
	// Общее число обучающих строк
	int globalTrainSize;
	// Последний использованный при добавлении строки индекс
	IndexPair lastIndex;
	
	// Освобождение всех элементов netArray и ее очистка
	void clearNetArray()
	{
		MapTypePair val;
		BOOST_FOREACH(val, netArray)
		{
			delete val.second;
		}
		netArray.clear();
	}
	
public:
	// Конструктор умолчания, инициализирующий некоторые значения
	NeuralMultiplexer():lastIndex(std::make_pair(-1, 0)), globalTrainSize(0) {}
	
	// Добавляет строку для обучения
	void supplyTrainingData(const Qstring &s);
	// Возвращает обучающую строку с (глобальным) номером i
	const Qstring &getTrainString(int i) const;
	
	// Добавляет дополнительную строку для обучения
	// (относящуюся к последней добавленной строке для обучения)
	// Строка должна быть той же длины (иначе AxiomLibException)
	void supplyAuxTrainingData(const Qstring &s);
	
	void supplyCentralString(const Qstring &str);
	
	// Инициализация из настроек
	template<class Settings>
	void construct(const Settings &settings);
	
	// Обучение
	void train();
	
	// Запуск алгоритма
	int run(const Qstring &inp, double &p) const;	
	
	// Максимальная размерность (незакодированной) обучающей строки
	int getMaxTrainDim() const { 
		return netArray.size() > 0 ? netArray.rbegin()->first : 0;
	}
	
	// Минимальная размерность (незакодированной) обучающей строки
	int getMinTrainDim() const { 
		return netArray.size() > 0 ? netArray.begin()->first : 0;
	}
	
	// Возвращает число строк для обучения
	int getNumTrain() const { return globalTrainSize; }
	
	// Очищает объект для повторного использования
	void clear()
	{
		clearNetArray();
		glNumToLocNum.clear();
		locNumToGlNum.clear();
		globalTrainSize = 0;
		lastIndex = std::make_pair(-1, 0);
	}
	
	~NeuralMultiplexer()
	{
		clearNetArray();
	}
};

};

#include "NeuralMultiplexer.cxx"

#endif
