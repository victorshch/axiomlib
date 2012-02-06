#include <cmath>

#include "GoldenSectionSearch.h"

using namespace AxiomLib;
using namespace AxiomLib::Optimization;

double GoldenSectionSearch::goldenSectionSearch(const FunctionOneVariable *function,
												double a, double b, double fb, double c, double tau) {
	double x;
    if (c - b > b - a)
      x = b + resphi * (c - b);
    else
      x = b - resphi * (b - a);
    //if (std::abs(c - a) < tau * (std::abs(b) + std::abs(x))) 
	// В оригинале странный критерий завершения, сделал более простой
	if (std::abs(c - a) < tau)
      return (c + a) / 2;
	double fx = function->f(x);
    if (fx < fb) {
      if (c - b > b - a) return goldenSectionSearch(function, b, x, fx, c, tau);
      else return goldenSectionSearch(function, a, x, fx, b, tau);
    }
    else {
      if (c - b > b - a) return goldenSectionSearch(function, a, b, fb, x, tau);
      else return goldenSectionSearch(function, x, b, fb, c, tau);
    }
}

GoldenSectionSearch::GoldenSectionSearch()
{
	phi = (1 + std::sqrt(5.0)) / 2.0;
	
	resphi = 2.0 - phi;
	
	tau = 0.01;
}

void GoldenSectionSearch::initFromEnv(const Environment &env)
{
	env.getMandatoryParamValue(tau, "minimizerTau");
}

void GoldenSectionSearch::findMinimum(const FunctionOneVariable *function, 
									  double lowerBound, double upperBound, 
									  double &bestX, double &bestF)
{
	double a = lowerBound;
	double c = upperBound;
	double b = a + (c-a) / (phi + 1);
	//double fa = f(a);
	double fb = function->f(b);
	//double fc = f(c);
	
	double x = goldenSectionSearch(function, a, b, fb, c, tau);
	
	bestX = x;
	bestF = function->f(x);
}

