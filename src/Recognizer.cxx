#include "Recognizer.h"

#include "Logger.h"
#include "TemplateRecognizer.h"

using namespace AxiomLib;

double Recognizer::learn(TemplateRecognizer &templateRecognizer, int &resFirst, int &resSecond, vector<double> &stat, int asIndex)
{
	if(templateRecognizer.axiomSet == 0) {
		Logger::getInstance()->writeDebug("Recognizer::learn() : warning : templateRecognizer.axiomSet == 0");
	}
	
	double res = learn(templateRecognizer);
	
	run(*(templateRecognizer.axiomSet), templateRecognizer.prepDataSet, templateRecognizer.params, resFirst, resSecond, stat, asIndex);
	return res;
}

