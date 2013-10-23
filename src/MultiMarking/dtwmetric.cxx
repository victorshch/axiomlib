/****************************************************************************
*			dtwmetric.сxx
*
*	Description: Реализация методов класса
*	Author:		armkor
*	History:
*
****************************************************************************/

#include "dtwmetric.h"
#include <boost/algorithm/string/case_conv.hpp>


#include "../AxiomLibException.h"
#include "MetricsBetweenSets1.h"
#include "MetricsBetweenSets2.h"
#include "AsymmBetweenSets.h"
#include "MetricsEqual.h"
#include "MetricsEuclidean.h"
#include "MetricsHamming.h"
#include "AsymmHamming.h"
#include "MetricsMatrix.h"
#include "Dice.h"
#include "Jaccard.h"
#include "WeakEqual.h"
#include "Priority.h"
#include "WeightedHamming.h"
#include <iostream>

namespace AxiomLib {

namespace MultiMarking {

DTWMetric::DTWMetric(){}


double calculate(double a,double b,double c) {
	//return ((a + b)/(c + 1));
	return ((a + b)*(c + 1));
}
// t-рассматриваемая ref - эталонная
void DTWMetric::computeDTW (DTWMetric* m, const MultiMark& t,int i,int Nmin,
							int Nmax, const MultiMark& ref, std::vector<double>& result) {
	int len_t=Nmax; // Максимальный размер окна = размер матрицы
	int len_ref=ref.size(); /* Длина эталонной траектории*/

	// Инициализация матриц
	std::vector<std::vector<double> > D(len_ref),R(len_ref),S(len_ref);
	for (int u=0;u<len_ref;u++){
		D[u].resize(len_t);
		R[u].resize(len_t);
		S[u].resize(len_t);
	}

	// Заполнение матриц.
	for (int b=0 ; b < len_t ; b++ ){
		for (int a=0 ; a < len_ref ; a++) {
			D[a][b]=m->compute ( ref[a],t[i-len_t+b + 1] );
			S[a][b]=0;
			R[a][b]=0;
		}
	}
	// Работа с матрицами R и S
	S[len_ref-1][len_t-1]=D[len_ref-1][len_t-1];
	R[len_ref-1][len_t-1]=1;
	// Заполнение последних строки и столбца
	for(int z = len_t - 2;z >= 0;--z) {
		S[len_ref - 1][z]=D[len_ref - 1][z] + S[len_ref-1][z + 1];
		R[len_ref - 1][z]=R[len_ref - 1][z + 1] + 1;
	}
	for (int z = len_ref - 2;z >= 0;--z) {
		S[z][len_t - 1]=D[z][len_t - 1] + S[z + 1][len_t - 1];
		R[z][len_t - 1]=R[z + 1][len_t] + 1;
	}
	// Заполнение матриц
	double diag,right,down;
	for (int a=(len_ref-2);a>=0;--a) {
		for (int b=(len_t-2);b>=0;--b) {
			diag=calculate(D[a][b],S[a+1][b+1],R[a+1][b+1]);
			right=calculate(D[a][b],S[a][b+1],R[a][b+1]);
			down=calculate(D[a][b],S[a+1][b],R[a+1][b]);
			if ((down<right)&&(down<diag)) {
				S[a][b]=D[a][b]+S[a+1][b];
				R[a][b]=R[a+1][b]+1;
			} else
			if ((diag<=right)&&(diag<=down)) {
				S[a][b]=D[a][b]+S[a+1][b+1];
				R[a][b]=R[a+1][b+1]+1;
			} else
			if ((right<diag)&&(right<down)) {
				S[a][b]=D[a][b]+S[a][b+1];
				R[a][b]=R[a][b+1]+1;
			}
		}
	};
	result.resize(Nmax-Nmin+1);
	for (int j=0;j<(Nmax-Nmin+1);j++) {
		result[j]=(S[0][j]/R[0][j]);
	};
}


DTWMetric* DTWMetric::getMetric(std::string name){
	// преобразование в строчные буквы
	std::string nameCopy = boost::algorithm::to_lower_copy(name);
	if(nameCopy == "betweensets1") {
		BetweenSets1* result;
		result=new BetweenSets1;
		return result;
	}
	if(nameCopy == "betweensets2") {
		BetweenSets2* result;
		result=new BetweenSets2;
		return result;
	}
	if(nameCopy == "equal") {
		Equal* result;
		result=new Equal;
		return result;
	}
	if(nameCopy == "euclidean") {
		Euclidean* result;
		result=new Euclidean;
		return result;
	}
	if(nameCopy == "hamming") {
		Hamming* result;
		result=new Hamming;
		return result;
	}
	if(nameCopy == "matrix") {
		Matrix* result;
		result=new Matrix;
		return result;
	}
	if(nameCopy == "dice") {
		return new Dice();
	}
	if(nameCopy == "jaccard") {
		return new Jaccard();
	}
	if(nameCopy == "asymmhamming") {
		return new AsymmHamming();
	}
	if (nameCopy == "asymmbetweensets") {
		return new AsymmBetweenSets();
	}
	if (nameCopy == "weakequal") {
		return new WeakEqual();
	}
	if (nameCopy == "priority") {
		return new Priority();
	}
	if (nameCopy == "weightedhamming") {
		return new WeightedHamming();
	}
	throw AxiomLibException("Invalid DTW metric name: '" + name + "'");
}

void DTWMetric::computeDTWForMetric(const MultiMark &t, int i, int Nmin, int Nmax, const MultiMark &ref, std::vector<double> &result)
{
	DTWMetric::computeDTW (this,t,i,Nmin,Nmax, ref, result);
}

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
