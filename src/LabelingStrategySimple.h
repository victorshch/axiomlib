/****************************************************************************
*					LabelingStrategySimple
*
*	Description:	Алгоритм построения маркировки 
*					по данным о расстояниях
*					от участков траектории до эталонных траекторий.
*					В данной реализации каждому классу нештатного поведения 
*					соответствует свое значение precision.
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef LABELINGSTRATEGYSIMPLE_H
#define LABELINGSTRATEGYSIMPLE_H

#include "LabelingStrategy.h"
#include "optimization/FunctionOneVariable.h"
#include "optimization/Minimizer.h"

namespace AxiomLib {

class LabelingStrategySimple : public LabelingStrategy
{
private:
	double m_initialPrecision;
	std::vector<double> m_precisions;
	int m_numClasses;
	
	boost::shared_ptr<Optimization::Minimizer> m_minimizer;
	
	void setupPrecisions(int numClasses);
	
	class Function : public Optimization::FunctionOneVariable {
		const std::vector<TrajectorySampleDistance> &trainTrajectorySampleDistance;
		const std::vector<std::vector<int> >& trainTrajectoryRightLabeling;
		CompareStatistic *compareStatistic;
		GoalStrategy* goalStrategy;
		std::vector<double> &m_precisions;
		
		int currentClass;
		
	public:
		Function(const std::vector<TrajectorySampleDistance> &trainTrajectorySampleDistance, 
				 const std::vector<std::vector<int> >& trainTrajectoryRightLabeling,
				 CompareStatistic *compareStatistic, GoalStrategy* goalStrategy,
				 std::vector<double> &m_precisions);
		
		void setCurrentClass(int classNo);
		
		double f(double x) const;
	};
	
public:
	LabelingStrategySimple();
	
	void initFromEnv(const Environment &env);
	
	void performLabeling(const TrajectorySampleDistance &trajectorySampleDistance, std::vector<int> &labeling) const;
	
	double train(const std::vector<TrajectorySampleDistance> &trainTrajectorySampleDistance, 
				 const std::vector<std::vector<int> >& trainTrajectoryRightLabeling,
				 CompareStatistic *compareStatistic, GoalStrategy *goalStrategy);
	
	static void performLabeling(const TrajectorySampleDistance &trajectorySampleDistance, 
								std::vector<int> &labeling, 
								const std::vector<double>& precs);
	
	LabelingStrategySimple* copy() const;
};

}

#endif // LABELINGSTRATEGYSIMPLE_H
