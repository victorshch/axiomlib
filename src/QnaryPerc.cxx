
#include "QnaryPerc.h"
#include "AxiomLibException.h"
using namespace AxiomLib;

#include <cmath>

/****************************************************************************
*					QnaryPerc::calcOutputNeuron
*
*	Description:	Вычисляет выход одного выходного нейрона с номером neuronNo
*					 и записывает результат в outp[neuronNo]
*	Parameters:	int neuronNo - номер нейрона
*				 double &p - туда записывается максимальная компонента поля нейрона
*				const Qstring &inp - вход сети
*				Qstring &outp - выход сети, компонента которого с номером neuronNo вычисляется в данной функции
*	Returns:	-
*	Throws:		AxiomLibException, если neuronNo имеет некорректное значение (больше макс. номера нейрона или меньше 0)
*	Author:		wictor
*	History:
*
****************************************************************************/
void QnaryPerc::calcOutputNeuron(int neuronNo, double &p, const Qstring &inp, Qstring &outp) const
{
	//cout<<"calcOutputNeuron()"<<endl;
	// Проверка того, что номер выходного нейрона корректен
	if(neuronNo >= n || neuronNo < 0) throw AxiomLibException("QnaryPerc: bad parameters in calcOutputNeuron()");
	
	// h - локальное поле, действующее на данный нейрон. Пока инициализируем его значением порога данного нейрона
	LocalField h(netThresholds(neuronNo));
	// актуальный размер входа - он может быть меньше числа входных нейронов
	int actualN = inp.size();
	// итерация по входу сети
	for(int j=0; j<actualN; j++)
	{
		// netWeights(j, j) - нулевая матрица, поэтому идем на следующую итерацию
		if(j==neuronNo) continue;
		
		// Прибавляем к h значение произведения netWeights(neuronNo, j) на орт, соответствующий j-му входному нейрону
		// Менее оптимальный (но более читаемый) способ: h+=prod(netWeights(neuronNo, j), input(j));
		// Более оптимальный способ:
		for (int i=0; i<qo; i++) {
			h(i)+=netWeights(neuronNo, j)(i, inp(j));
		}
	}
	
	int index_max=0;
	
	//Находим индекс максимального элемента h
	p = h(index_max);
	for(int i = 1; i < qo; i++)
	{
		double v = h(i);
		if(v > h(index_max))
		{
			index_max=i;
			p = v;
		}
	}
	
	// Присваиваем значение найденного индекса состоянию данного нейрона
	outp(neuronNo) = index_max;

	//cout<<"calcOutputNeuron() end"<<endl;
}

/****************************************************************************
*					QnaryPerc::calcOutput
*
*	Description:	Вычисляет выходы всех нейронов и записывает результат в outp
*	Parameters:	double &p - туда записывается максимальная компонента поля последнего нейрона (TODO: сделать что-нибудь еще)
*				const Qstring &inp - вход сети
*				Qstring &outp - вычисляемый в данной функции выход сети
*	Returns:	-
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
void QnaryPerc::calcOutput(double &p, const Qstring &inp, Qstring &outp) const
{
	//Итерация по всем выходным нейронам
	for(int i=0; i<n; i++)
	{
		calcOutputNeuron(i, p, inp, outp);
	}
}

/****************************************************************************
*					QnaryPerc::construct
*
*	Description:	Строит (инициализирует) сеть (в т.ч. выделяет память под веса и пороги) - эту функцию нужно вызывать перед работой с сетью
*	Parameters:	int qi_ - число состояний входного нейрона
*				int qo_ - число состояний выходного нейрона
*				int N_ - число входных нейронов
*				int n_ - число выходных нейронов
*	Returns:	-
*	Throws:		AxiomLibException, если qi_<2 || qo_<2 || n_<1 || N_<1
*	Author:		wictor
*	History:
*
****************************************************************************/
void QnaryPerc::construct(int qi_, int qo_, int N_, int n_) {
	// Инициализация полей класса
	q = qi_;
	qo = qo_;
	n = n_;
	N = N_;
	
	// Проверка
	if(q<2 || qo<2 || n<1 || N<1) {
		throw AxiomLibException("QnaryPerc: bad parameters in construct()");
	}
	
	// Инициализация полей класса
	netWeights.resize(n, N);
	netThresholds.resize(n);
	valid = true;
	
	// Инициализации матриц весов и векторов порогов
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<N; j++)
		{
			// qi==q
			netWeights(i, j).resize(qo, q);
			netWeights(i, j)=boost::numeric::ublas::zero_matrix<Elem>(qo,q);
		}
		//netThresholds(i).resize(q);
		netThresholds(i)=ZeroThreshold(qo);
	}	
}

/****************************************************************************
*					QnaryPerc::train
*
*	Description:	Обучает сеть (по правилу Хебба) сопоставлению входного вектора выходному
*				Можно подавать входной вектор не большего чем максимальный размера
*	Parameters:	const Qstring &inp - входной обучающий вектор
*				const Qstring &outp - выходной обучающий вектор
*	Returns:	-
*	Throws:		AxiomLibException, если размер входного вектора больше максимального размера входного вектора,
*				либо размер выходного вектора не равен заявленному в construct()
*	Author:		wictor
*	History:
*
****************************************************************************/
void QnaryPerc::train(const Qstring &inp, const Qstring &outp)
{
	
	NetThresholds thres(n);
	
	// Актуальный размер входа
	int actualN = inp.size();
	
	// Проверки
	if(actualN > N) {
		throw AxiomLibException("QnaryPerc::train(): incorrect inp size");
	}
	if(outp.size() != n) {
		throw AxiomLibException("QnaryPerc::train(): incorrect outp size");
	}
	// Множитель матрицы весов
	const double factor = double(N)/double(actualN);
	// Цикл по выходным нейронам
	for(int i=0; i<n; i++)
	{
		// Цикл по входным нейронам
		for(int j=0; j < actualN; j++)
		{
			if(i!=j)
			// Добавление к весу матрицы, равной произведению двух ортов,
			// умноженному на множитель faactor 
			// (он должен быть равным 1 на входах максимальной длины,
			// а вообще я подобрал экспериментально)
			netWeights(i,j)(outp(i), inp(j)) += factor;
		}
		thres(i).resize(qo);
		// Порог - орт с 1 в позиции outp(i)
		thres(i)=boost::numeric::ublas::unit_vector<Elem>(qo, outp(i));
	}
	// Порог домножается на множитель
	thres*=(-double(actualN)/double(qo));
	// Прибавление полученных весов к общим весам сети
	netThresholds+=thres;
}

/****************************************************************************
*					QnaryPerc::run
*
*	Description:	Запускает сеть на строке inp
*	Parameters:	const Qstring &inp - входная строка
*				double &p - туда записывается некоторое значение, полученное в результате запуска
*	Returns:	Qstring - результат запуска (то, что получилось на выходных нейронах)
*	Throws:		AxiomLibException, если размер входного вектора больше максимального размера входного вектора
*	Author:		wictor
*	History:
*
****************************************************************************/
Qstring QnaryPerc::run(const Qstring &inp, double &p) const
{
	// Проверка размера входа
	if((int) inp.size() > N) {
		throw AxiomLibException("QnaryPerc::run() : inp size too large");
	}
	
	Qstring outp(n);
	
	calcOutput(p, inp, outp);

	return outp;
}
