#include <sstream>
#include <iterator>
#include <algorithm>
#include <boost/lexical_cast.hpp>

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
									 CompareStatistic *compareStatistic,
									 GoalStrategy* goalStrategy) {	
	
	int numClasses = trainTrajectorySampleDistance[0].numClasses();
	
	if(m_numClasses < 0) {
		setupPrecisions(numClasses);
	}
	
	Function func(trainTrajectorySampleDistance, 
				  trainTrajectoryRightLabeling, 
				  compareStatistic, 
				  goalStrategy, 
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
		
		func.setCurrentClass(i);
		
		double bestX;
		//todo вынести значения верхней и нижней границы в конфиг
		m_minimizer->findMinimum(&func, 0.0, 1.0, bestX, bestF);
		
		if(bestF < lastBestF) {
			m_precisions[i] = bestX;
			lastBestF = bestF;
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
										   CompareStatistic *compareStatistic, GoalStrategy* goalStrategy,
										   std::vector<double> &precs)
	: trainTrajectorySampleDistance(trainTrajectorySampleDistance),
	  trainTrajectoryRightLabeling(trainTrajectoryRightLabeling),
	  compareStatistic(compareStatistic), goalStrategy(goalStrategy),
	  m_precisions(precs),
	  currentClass(0)
{ }

void LabelingStrategySimple::Function::setCurrentClass(int classNo)
{
	currentClass = classNo;
}

double LabelingStrategySimple::Function::f(double x) const
{
	std::vector<double> precisions = m_precisions;
	precisions[currentClass] = x;
	
	std::vector<int> labeling;
	
	double e1 = 0, e2 = 0;
	
	for(std::vector<TrajectorySampleDistance>::size_type i = 0; 
		i < trainTrajectorySampleDistance.size(); ++i) {
		double e1temp = 0, e2temp = 0;
		
		LabelingStrategySimple::performLabeling(trainTrajectorySampleDistance[i],
												labeling, precisions);
		compareStatistic->getStatistic(labeling, trainTrajectoryRightLabeling[i], 
									   e1temp, e2temp, false);
		e1 += e1temp;
		e2 += e2temp;
	}
	
	return goalStrategy->compute((int) e1, (int) e2);
}
