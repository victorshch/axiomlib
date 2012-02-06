#include "statistics.h"

template<class AxiomType>
void updateStatistics(AxiomType &axiomStatistics, 
					  const AxiomLib::FullFuzzyDataSet &fullFuzzyDataSet, int noClassToUse) {
	std::vector<std::vector<double> > multiTS;
	axiomStatistics.statAbnorm = 0.0;
	axiomStatistics.statNormal = 0.0;	
	{
		int numClasses;
		std::vector<int> numMultiTS;
		std::vector<std::vector<int> >numTS;
		
		fullFuzzyDataSet.getClassSize(numClasses, numMultiTS, numTS);		
		
		if(noClassToUse < 0) {
		
			for(int noClass = 0; noClass <= numClasses; noClass++) {
				for(int noMultiTS = 0; noMultiTS <= numMultiTS[noClass]; noMultiTS++) {
					multiTS.resize(numTS[noClass][noMultiTS]);
					for(int noTS = 0; noTS <= numTS[noClass][noMultiTS]; noTS++) {
						fullFuzzyDataSet.getTSByIndexFromClass(
									multiTS[noTS],
									noClass,
									noMultiTS,
									noTS
								);
					}
					updateStatistics(axiomStatistics, multiTS, true);
				}
			}		
		} else {
			for(int noMultiTS = 0; noMultiTS <= numMultiTS[noClassToUse]; noMultiTS++) {
				multiTS.resize(numTS[noClassToUse][noMultiTS]);
				for(int noTS = 0; noTS <= numTS[noClassToUse][noMultiTS]; noTS++) {
					fullFuzzyDataSet.getTSByIndexFromClass(
								multiTS[noTS],
								noClassToUse,
								noMultiTS,
								noTS
							);
				}
				updateStatistics(axiomStatistics, multiTS, true);
			}			
		}
	}
	{
		int numMultiTS;
		std::vector<int> numTS;
		
		fullFuzzyDataSet.getNormalClassSize(numMultiTS, numTS);
		for(int noMultiTS = 0; noMultiTS <= numMultiTS; noMultiTS++) {
			multiTS.resize(numTS[noMultiTS]);
			for(int noTS = 0; noTS <= numTS[noMultiTS]; noTS++) {
				fullFuzzyDataSet.getNormalTSFromClass(
							multiTS[noTS],
							noMultiTS,
							noTS
						);
			}
			updateStatistics(axiomStatistics, multiTS, false);
		}
	}
	axiomStatistics.goal = axiomStatistics.statAbnorm / axiomStatistics.statNormal;
}

template<class AxiomType>
void updateStatistics(AxiomType &axiomStatistics, 
					  const std::vector<std::vector<double> > &multiTS, 
					  bool abnormal) {
	int count = 0;
	for(int k = 0; k < multiTS.size(); k++) {
		int r = axiomStatistics.check(k, multiTS);
		if(r == 1) {
			++count;
		}
	}
	
	double value = (double) count / (double) multiTS.size();
	if(abnormal) {
		axiomStatistics.statAbnorm += value;
	} else {
		axiomStatistics.statNormal += value;
	}
}
