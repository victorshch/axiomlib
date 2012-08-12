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
#include "MetricsEqual.h"
#include "MetricsEuclidean.h"
#include "MetricsHamming.h"
#include "MetricsMatrix.h"
#include "Dice.h"
#include "Jaccard.h"
#include <iostream>

namespace AxiomLib {

namespace MultiMarking {

DTWMetric::DTWMetric(){}


double calculate(double a,double b,double c) {
	return ((a+b)/(c+1));
}
// t-рассматриваемая ref - эталонная
void DTWMetric::computeDTW (DTWMetric* m, const MultiMark& t,int i,int Nmin,
							int Nmax, const MultiMark& ref, std::vector<double>& result) {
	int len_t=Nmax; // Максимальный размер окна = размер матрицы
	int len_ref=ref.size(); /* Длина эталонной траектории*/

	// Инициализация матриц
	std::vector<std::vector<double>> D(len_ref+1),R(len_ref+1),S(len_ref+1);
	for (int u=0;u<len_ref+1;u++){
		D[u].resize(len_t+1);
		R[u].resize(len_t+1);
		S[u].resize(len_t+1);
	}

	// Заполнение матриц.
	for (int b=0 ; b < len_t ; b++ ){
		for (int a=0 ; a < len_ref ; a++) {
			D.at(a).at(b)=m->compute ( ref[a],t[i-len_t+b] );
			S.at(a).at(b)=0;
			R.at(a).at(b)=0;
		}
	}
	// Работа с матрицами R и S
	S.at(len_ref-1).at( len_t-1)=D[len_ref-1][len_t-1];
	R.at(len_ref-1).at( len_t-1)=1;
	// Заполнение дополнительных строки и столбца
	for(int z=0;z<len_t+1;z++) {
		S[len_ref][z]=D[len_ref-1][len_t-1];
		R[len_ref][z]=1;
	}
	for (int z=0;z<len_ref+1;z++) {
		S[z][len_t]=D[len_ref-1][len_t-1];
		R[z][len_t]=1;
	}
	// Заполнение матриц
	double diag,right,down;
	for (int a=(len_ref-1);a>-1;a--) {
		for (int b=(len_t-1);b>-1;b--) {
			if ((a!=(len_ref-1)) || (b!=len_t-1)){
				diag=calculate(D[a][b],S[a+1][b+1],R[a+1][b+1]);
				right=calculate(D[a][b],S[a][b+1],R[a][b+1]);
				down=calculate(D[a][b],S[a+1][b],R[a+1][b]);
				if ((down<right)&&(down<diag)) {
					S[a][b]=D[a][b]+S[a+1][b];
					R[a][b]=R[a+1][b]+1;
				}
				if ((diag<=right)&&(diag<=down)) {
					S[a][b]=D[a][b]+S[a+1][b+1];
					R[a][b]=R[a+1][b+1]+1;
				}
				if ((right<diag)&&(right<down)) {
					S[a][b]=D[a][b]+S[a][b+1];
					R[a][b]=R[a][b+1]+1;
				}
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
	throw AxiomLibException("Invalid DTW metric name: '" + name + "'");
}

void DTWMetric::computeDTWForMetric(const MultiMark &t, int i, int Nmin, int Nmax, const MultiMark &ref, std::vector<double> &result)
{
	DTWMetric::computeDTW (this,t,i,Nmin,Nmax, ref, result);
}

};//  end of namespace MultiMraking

};//  end of namespace AxiomLib
