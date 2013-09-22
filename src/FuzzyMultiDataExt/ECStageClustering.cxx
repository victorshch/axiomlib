#include "ECStageClustering.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

ECStageClustering::ECStageClustering(FuzzyDataSet* fuzzyDataSet, ECTypeStage* stage0){
	this->stage0 = stage0;
	this->fuzzyDataSet = fuzzyDataSet;
	this->logger = Logger::getInstance();
}