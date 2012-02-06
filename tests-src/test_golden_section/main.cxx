#include <algorithm>
#include <cctype>
#include <boost/lexical_cast.hpp>

#include "Logger.h"
#include "Environment.h"

#include "optimization/Minimizer.h"
#include "optimization/MinimizerFactory.h"

using namespace AxiomLib;
using namespace AxiomLib::Optimization;

class TestFunc : public FunctionOneVariable {
public:
	virtual double f(double x) const {
		double v = x - 0.02;
		return v*v;
	}
};

bool compareLess(char c1, char c2) {
	//return std::tolower(c1) < std::tolower(c2);
	return (c1) < (c2);
}

int main(int argc, char *argv[])
{
//	std::string s = "minimizerNumPartitions";
//	std::string s2 = "minimizernumpartitions";
//	bool b = std::lexicographical_compare(s.begin(), s.end(), s2.begin(), s2.end(), &compareLess);
//	bool b2 = std::lexicographical_compare(s2.begin(), s2.end(), s.begin(), s.end(), &compareLess);
//	std::cout<<b<<" "<<b2<<std::endl;
//	return 0;
	try {
	Logger::getInstance()->setComments(true);
	
	Environment env;
	env.setParamValue(0.001, "minimizerTau");
	env.setParamValue(10, "minimizerNumPartitions");
	env.setParamValue("goldensectionsearch", "hybridLocalMinimizer");
	MinimizerFactory factory;
	Minimizer* m = factory.create("hybridgridsearch");
	//Minimizer* m = factory.create("goldensectionsearch");
	m->initFromEnv(env);
	
	TestFunc testFunc;
	
	double bestX, bestF, bestX2, bestF2;
	
	Logger::getInstance()->writeComment("Optimizing...");
	m->findMinimum(&testFunc, 0, 1, bestX, bestF);
	//m2->findMinimum(&testFunc, 0, 1, bestX2, bestF2);
	Logger::getInstance()->writeComment("Optimization finished.");
	Logger::getInstance()->writeComment("bestX = "+boost::lexical_cast<std::string>(bestX) + ", bestF = "+boost::lexical_cast<std::string>(bestF));
	//Logger::getInstance()->writeComment("bestX2 = "+boost::lexical_cast<std::string>(bestX2) + ", bestF2 = "+boost::lexical_cast<std::string>(bestF2));
	} catch (AxiomLibException e) {
		std::cerr<<e.error()<<std::endl;
		return -1;
	} catch (std::exception e) {
		std::cerr<<e.what()<<std::endl;
		return -1;
	}
	return 0;
}
