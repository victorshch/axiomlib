/****************************************************************************
*			QnaryPerc.h
*
*	Description: Описание класса QnaryPerc, реализующего q-нарный перцептрон
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef QNARYPERC_H_
#define QNARYPERC_H_

#include "QnaryBase.h"


namespace AxiomLib {

class QnaryPerc: public QnaryBase
{
	// Число состояний выходного нейрона (число состояний входного нейрона хранится в базовом класса)
	int qo;
	
	// Число входных нейронов
	int N;
	
	// Число выходных нейронов
	int n;
	
	// Множество всех весов сети (матрица матриц)
	NetWeights netWeights;
	
	// Множество всех порогов сети (вектор векторов)
	NetThresholds netThresholds;

	// Вычисление выхода одного выходного нейрона
	void calcOutputNeuron(int neuronNo, double &p, const Qstring &inp, Qstring &outp) const;
	
	// Вычисление выхода нейросети
	void calcOutput(double &p, const Qstring &inp, Qstring &outp) const;
	
	// Флаг того, что сеть инициализирована
	bool valid;

public:

	// Конструктор умолчания - инициализирует некоторые значения
	QnaryPerc(): valid(false) {}

	// Инициализация сети
	void construct(int qi_, int qo_, int N_, int n_);
	
	// Обучение сети
	void train(const Qstring &inp, const Qstring &outp);
	
	// Работа сети
	Qstring run(const Qstring &inp, double &p) const;
	
	// Возвращает число состояний выходного нейрона
	int getQo() const { return qo; }

};

};

#endif /* QNARYPERC_H_ */
