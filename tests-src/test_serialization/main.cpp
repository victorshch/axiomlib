#include <iostream>

#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"

#include "boost/serialization/vector.hpp"

#include <shark/Algorithms/Trainers/NormalizeComponentsUnitVariance.h> //normalize
#include <shark/Algorithms/KMeans.h> //k-means algorithm
#include <shark/Models/Clustering/HardClusteringModel.h>//model performing hard clustering of points
#include <shark/Models/Normalizer.h>

#include "prettyprint.hpp"

using namespace std;
using namespace shark;

int main(int argc, char *argv[])
{
	// Инициализируем генератор случайных чисел
	shark::Rng::seed(time(0));

	// Генерируем случайный набор данных
	std::vector<size_t> batchSizes = shark::detail::optimalBatchSizes(500, shark::Data<RealVector>::DefaultBatchSize);
	shark::Data<RealVector> inputVectorsData = shark::Data<RealVector>(batchSizes.size());

	int dimension = 5;

	size_t currentRow = 0;
	for(size_t b = 0; b != batchSizes.size(); ++b) {
		RealMatrix& batch = inputVectorsData.batch(b);
		batch.resize(batchSizes[b], dimension);
		//copy the rows into the batch
		for(size_t i = 0; i != batchSizes[b]; ++i,++currentRow){

			for(size_t j = 0; j != dimension; ++j){
				batch(i,j) = shark::Rng::gauss(shark::Rng::discrete(1, 10), 1);
			}
		}
	}

	// Проводим кластеризацию
	shark::Centroids centroids;
	kMeans(inputVectorsData, 10, centroids);

	// Загоняем центроиды в std::vector<std::vector<double> >
	shark::Data<shark::RealVector> centroidsData = centroids.centroids();
	std::cout << "Centroids: " << centroidsData << std::endl;

	std::vector<std::vector<double> > serializable;

	for(shark::Data<shark::RealVector>::const_element_range::iterator it = centroidsData.elements().begin();
		it != centroidsData.elements().end(); ++it) {
		shark::RealVector currentCentroid = *it;

		serializable.push_back(std::vector<double>());
		for(unsigned i = 0; i < currentCentroid.size(); ++i) {
			serializable.back().push_back(currentCentroid[i]);
		}
	}

	std::cout << "Centroids in std::vector form: " << serializable << std::endl;

	// Теперь достаем центроиды из serializable и загоняем их в Centroids
	batchSizes = shark::detail::optimalBatchSizes(serializable.size(), shark::Data<RealVector>::DefaultBatchSize);
	shark::Data<RealVector> restoredCentroidsData = shark::Data<RealVector>(batchSizes.size());

	currentRow = 0;
	for(size_t b = 0; b != batchSizes.size(); ++b) {
		RealMatrix& batch = restoredCentroidsData.batch(b);
		batch.resize(batchSizes[b], dimension);
		// copy the rows into the batch
		for(size_t i = 0; i != batchSizes[b]; ++i,++currentRow){
			// TODO check row size

			for(size_t j = 0; j != dimension; ++j){
				batch(i,j) = serializable[currentRow][j];
			}
		}
	}

	// Если здесь использовать не конструктор, а метод Centroids::initFromData(),
	// то центроиды будут случайно переупорядочены, что не очень хорошо.
	// Поэтому нужно использовать именно конструктор.
	Centroids restoredCentroids(restoredCentroidsData);

	std::cout << "Restored centroids: " << restoredCentroids.centroids() << std::endl;

	return 0;
}
