/****************************************************************************
*			Net
*
*	Description:	Класс описывающий нейросеть. 
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_Net_HXX
#define __AXIOMS_LIB_Net_HXX

#include "AxiomLibException.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>
#include <set>

namespace AxiomLib {

class Net {

 private:

	 // Переменная хранит статус класса - если сеть обучалась, то стоит true, иначе false.
	 bool status; 

	 // Собственно параметр q-нарных сетей - q
	 unsigned int q;

	 // Число входных нейронов
	 unsigned int inNum;

	 // Число выходных нейронов
	 unsigned int outNum;

	 // Коэффициенты разложения числа по степеням q до outNum
	 std::vector <int> outCoeff;

	 // Максимальный индекс, который может быть закодирован в outNum числе нейронов
	 int maxOutNum;

	 // Матрицы коэффициентов для всех связей сети
	 std::vector <std::vector <std::vector <std::vector <char> > > > matrices;

	 std::map <int, int> distValues;
	 std::set <int> usedDistValues;

	 // Вспомогательная функция представления числа в виде бинарного вектора
	 inline void code (int input, std::vector <char> &res) const;

	 // Вспомогательная функция представления вектора чисел от 0 до q  в виде бинарной матрицы
	 inline void code (std::vector <int> &input, std::vector <std::vector <char> > &res) const;

	 // Функция кодирования номера класса в q-нарный вектор
	 inline void codeRes (int input, std::vector <int> &res);

	 // Функция декодирования номера класса в q-нарный вектор
	 inline void decodeRes (int &output, std::vector <int> &res) const;

	 // Вспомогательная функция переменожения столбца и строки, каждый из которых представлен в виде вектора
	 inline void mult (std::vector <char> &x, std::vector <char> &y, std::vector <std::vector <char> > &res) const;

 protected:
 
 public:
 
	// Стандартный конструктор - задание значений по умолчанию
	Net (unsigned int newQ);

	// Стандартный деструктор - пустой
	~Net ();

	// Функция обучения массива сетей по заданным разметкам для каждого класса аномального поведения
	int prepare (std::vector <int> &in, int out);

	// Функция проверки - есть ли в ряду markUp разметка из запомненных ранее, причем которая заканчивается в ряду на индексе i
	double check (int &resNum, std::vector <int> &row, long int &i) const;
  
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_Net_STRATEGY_HXX */
