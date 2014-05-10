#include "KMeansClusteringModel.h"

using namespace std;
using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;
using namespace shark;

void KMeansClusteringModel::addElement(const vector<double>& element){
	this->store.push_back(element);
}

void KMeansClusteringModel::addElements(const vector<vector<double> >& elements){
	for(auto i = 0; i < elements.size(); i++){
		addElement(elements[i]);
	}
}

void KMeansClusteringModel::makeClustering(){
	Normalizer<> normalizer;
	NormalizeComponentsUnitVariance<> normalizingTrainer;
	UnlabeledData<RealVector> data;

	vector<vector<double> > rows = this->store;
	vector<size_t> batchSizes = shark::detail::optimalBatchSizes(rows.size(), Data<RealVector>::DefaultBatchSize);
	data = shark::Data<RealVector>(batchSizes.size());	

	int dimension = rows[0].size();

	size_t currentRow = 0;
	for(size_t b = 0; b != batchSizes.size(); ++b) {
		RealMatrix& batch = data.batch(b);
		batch.resize(batchSizes[b], dimension);
		//copy the rows into the batch
		for(size_t i = 0; i != batchSizes[b]; ++i,++currentRow){
			if(rows[currentRow].size() != dimension)
				throw AxiomLibException("vectors are required to have same size");
			
			for(size_t j = 0; j != dimension; ++j){
				batch(i,j) = rows[currentRow][j];
			}
		}
	}

	normalizingTrainer.train(normalizer, data);
	data = normalizer(data);

	size_t iterations = kMeans(data, this->k, centroids, this->iters);

	model = new HardClusteringModel<shark::RealVector>(&centroids);
}

unsigned int KMeansClusteringModel::getClusterNumberToElement(const std::vector<double>& element) const{
	UnlabeledData<RealVector> data;

	vector<size_t> batchSizes = shark::detail::optimalBatchSizes(1, Data<RealVector>::DefaultBatchSize);
	int dimension = element.size();

	data = shark::Data<RealVector>(batchSizes.size());	

	for(size_t b = 0; b != batchSizes.size(); ++b) {
		RealMatrix& batch = data.batch(b);
		batch.resize(batchSizes[b], dimension);
		
		//copy the row into the batch
		for(size_t i = 0; i != batchSizes[b]; ++i){			
			for(size_t j = 0; j != dimension; ++j){
				batch(i,j) = element[j];
			}
		}
	}

	Data<unsigned> clusters = (*model)(data);	 
	return clusters.element(0);
}

void KMeansClusteringModel::reserve(int k){	
	this->store.reserve(k);
}

void KMeansClusteringModel::clearStore(){
	this->store.clear();
	this->store.resize(0);
}

void KMeansClusteringModel::setParam(std::string name, std::string value){
	if (strcmp(name.c_str(), "k") == 0){
		this->setClustersCount(atoi(value.c_str()));
	}

	if (strcmp(name.c_str(), "i") == 0){
		this->setItersCount(atoi(value.c_str()));
	}
}
