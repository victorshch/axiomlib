#include <sstream>
#include <iterator>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "Logger.h"

#include "optimization/MinimizerFactory.h"

#include "LabelingStrategySimple.h"



using namespace AxiomLib;

void LabelingStrategySimple::setupPrecisions(int numClasses)
{
	m_numClasses = numClasses;
	m_precisions.assign(numClasses, m_initialPrecision);
}

LabelingStrategySimple::LabelingStrategySimple()
{
	m_initialPrecision = 0.3;
	m_numClasses = -1;
}

void LabelingStrategySimple::initFromEnv(const Environment &env)
{
	double initialPrecisionTemp = m_initialPrecision;
	if(env.getParamValue(initialPrecisionTemp, "precision") < 0) {
		Logger::getInstance()->writeComment("LabelingStrategySimple::initFromEnv : warning : precision not specified, "
											"using default value " + boost::lexical_cast<std::string>(m_initialPrecision));
	} else {
		m_initialPrecision = initialPrecisionTemp;
	}
	
	std::string minimizerName;
	env.getMandatoryParamValue(minimizerName, "RecognizerGoalFunctionMinimizer");
	
	Optimization::MinimizerFactory minimizerFactory;
	m_minimizer = boost::shared_ptr<Optimization::Minimizer>(minimizerFactory.create(minimizerName));
	m_minimizer->initFromEnv(env);
}

void LabelingStrategySimple::performLabeling(const TrajectorySampleDistance &trajectorySampleDistance, std::vector<int> &labeling) const
{
	//Logger::getInstance()->writeDebug("Entering LabelingStrategySimple::performLabeling()");
	if(m_numClasses < 0) {
		int numClasses = trajectorySampleDistance.numClasses();
		std::vector<double> precisions(numClasses, m_initialPrecision);
		performLabeling(trajectorySampleDistance, labeling, precisions);
	} else {
		performLabeling(trajectorySampleDistance, labeling, m_precisions);
	}
	//Logger::getInstance()->writeDebug("Leaving LabelingStrategySimple::performLabeling()");
}

std::string toString(const std::vector<double>& v) {
	std::ostringstream o;
	o<<'['<<v.size()<<']';
	o<<'(';
	std::vector<double>::const_iterator i = v.begin();
	if(i != v.end()) {
		o<<*i;
		++i;
		for( ; i != v.end(); ++i) {
			o<<", "<<*i;
		}
	}
	o<<')';
	return o.str();
}

double LabelingStrategySimple::train(const std::vector<TrajectorySampleDistance> &trainTrajectorySampleDistance, 
									 const std::vector<std::vector<int> > &trainTrajectoryRightLabeling, 
									 Recognizer *recognizer) {	
	
	int numClasses = trainTrajectorySampleDistance[0].numClasses();
	
	if(m_numClasses < 0) {
		setupPrecisions(numClasses);
	}
	
	Function func(trainTrajectorySampleDistance, 
				  trainTrajectoryRightLabeling, 
				  recognizer,  
				  m_precisions);
	
	Logger::getInstance()->writeDebug("Training LabelingStrategySimple...");
	
	func.setCurrentClass(0);
	double initialGoal = func.f(m_precisions[0]);
	
	Logger::getInstance()->writeDebug("Initial goal value : " 
									  + boost::lexical_cast<std::string>(initialGoal));
	
	double bestF = initialGoal;
	double lastBestF = bestF;
	for (int i = 0; i < numClasses; ++i) {
		Logger::getInstance()->writeDebug("Current abnormal class : " 
										  + boost::lexical_cast<std::string>(i+1));
		
		double lastBestX = this->m_precisions[i];
		func.setCurrentClass(i);
		
		double bestX;
		//todo вынести значения верхней и нижней границы в конфиг
		m_minimizer->findMinimum(&func, 0.0, 1.0, bestX, bestF);
		
		if(bestF < lastBestF) {
			m_precisions[i] = bestX;
			lastBestF = bestF;
		} else {
			m_precisions[i] = lastBestX;
		}
		
		Logger::getInstance()->writeDebug("Current goal value : "
										  + boost::lexical_cast<std::string>(bestF));
	}
	
	Logger::getInstance()->writeDebug("Precisions: " + toString(m_precisions));
	
	return lastBestF;
}

void LabelingStrategySimple::performLabeling(const TrajectorySampleDistance &trajectorySampleDistance, std::vector<int> &labeling, const std::vector<double> &precs)
{
	//Logger::getInstance()->writeDebug("Entering LabelingStrategySimple::performLabeling() (inner)");
	int numClasses = trajectorySampleDistance.numClasses();
	
	int length = trajectorySampleDistance.length();
	
	if(numClasses != precs.size()) {
		throw AxiomLibException("LabelingStrategySimple::performLabeling : wrong size of precs");
	}
	
	labeling.assign(length, 0);
	
	std::vector<double> bulletin(numClasses);
	
	//Logger::getInstance()->writeDebug("LabelingStrategySimple::performLabeling() (inner) : filling bulletin");
	
	for(int i = 0; i < length; ++i) {
		if (trajectorySampleDistance.distance(0, i) < 0) {
			continue;
		}
		bool flag = false;
		for(int cl = 0; cl < numClasses; ++cl) {
			double currD = trajectorySampleDistance.distance(cl, i);
			
			if(currD <= precs[cl] && currD >= 0) 
			{
				bulletin[cl] = (precs[cl] - currD) / precs[cl];
				flag = true;
			} 
			else
			{
				bulletin[cl] = -1;
			}
		}
		
		if (flag) {
			std::vector<double>::iterator it = std::max_element(bulletin.begin(), bulletin.end());
			int index = std::distance(bulletin.begin(), it);
			labeling[i] = index + 1;
		}
	}
	//Logger::getInstance()->writeDebug("Leaving LabelingStrategySimple::performLabeling() (inner)");
}

LabelingStrategySimple *LabelingStrategySimple::copy() const
{
	return new LabelingStrategySimple(*this);
}

LabelingStrategySimple::Function::Function(const std::vector<TrajectorySampleDistance> &trainTrajectorySampleDistance, 
										   const std::vector<std::vector<int> > &trainTrajectoryRightLabeling, 
										   Recognizer *recognizer, std::vector<double>& precs)
	: trainTrajectorySampleDistance(trainTrajectorySampleDistance),
	  trainTrajectoryRightLabeling(trainTrajectoryRightLabeling),
	  recognizer(recognizer),
	  m_precisions(precs),
	  currentClass(0)
{
	if(trainTrajectoryRightLabeling.size() != trainTrajectorySampleDistance.size()) {
		boost::format msg = boost::format("LabelingStrategySimple::Function::Function() : trainTrajectoryRightLabeling.size() = %d != %d = trainTrajectorySampleDistance.size()")
				% trainTrajectoryRightLabeling.size() % trainTrajectorySampleDistance.size();
		throw AxiomLibException(msg.str());
	}
	
	for(int i = 0; i < (int) trainTrajectoryRightLabeling.size(); ++i) {
		if((int)trainTrajectoryRightLabeling[i].size() != trainTrajectorySampleDistance[i].length()) {
			boost::format msg = boost::format("LabelingStrategySimple::Function::Function() : trainTrajectoryRightLabeling[%d].size() = %d != %d = trainTrajectorySampleDistance[%d].length())")
					% i % trainTrajectoryRightLabeling[i].size() % trainTrajectorySampleDistance[i].length() % i;
			throw AxiomLibException(msg.str());
		}
	}
}

void LabelingStrategySimple::Function::setCurrentClass(int classNo)
{
	currentClass = classNo;
}

double LabelingStrategySimple::Function::f(double x) const
{
	m_precisions[currentClass] = x;
	
	int first;
	int second;
	
	return recognizer->countErrors(trainTrajectorySampleDistance, trainTrajectoryRightLabeling, first, second);
}
