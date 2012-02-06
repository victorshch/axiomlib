#include <map>
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>
#include <deque>
#include <iterator>

#include "../FuzzyMultiDataExtAlgorithm.h"

#include "../AxiomLibException.h"

#include "../Logger.h"

#include "ECOccFilter.h"

using namespace AxiomLib;
using namespace FuzzyMultiDataExt;

ECOccFilter::ECOccFilter()
{
	// Значения по умолчанию
	this->setOrder(2);
	m_maxECs = 50;
	m_offset = 0.2;
	m_requiredSaturation = 10;
	m_maxSaturation = 20;
}

std::string ECOccFilter::name() const {
	return "ECOccFilter";
}

int ECOccFilter::stage() const {
	return 1;
}

ECOccFilter* ECOccFilter::clone() const {
	return new ECOccFilter(*this);
}

//struct compare {
//	bool operator()(const std::pair<double, double>& p1, const std::pair<double, double>& p2) {
//		if(p1.first < p2.first) {
//			return true;
//		} else if (p1.first - p2.first < epsilon) {
//			return p1.second < p2.second;
//		} else {
//			return false;
//		}
//	}
//};

bool ecCompare(const ElemCondPlusStat &ec1, const ElemCondPlusStat &ec2);

struct ecComparer
{
	ecComparer(FuzzyMultiDataExtAlgorithm& algorithm) : m_algorithm(algorithm) {}

	bool operator()(FuzzyMultiDataExtAlgorithm::ECIndex i1, FuzzyMultiDataExtAlgorithm::ECIndex i2)
	{
		return ecCompare(m_algorithm.getEC(i1), m_algorithm.getEC(i2));
	}

private:
	FuzzyMultiDataExtAlgorithm& m_algorithm;
};

template<class VectorType>
struct emptyChecker
{
	emptyChecker(std::vector<VectorType> &ecs) : m_ecs(ecs) {}

	bool operator()(int i)
	{
		return m_ecs[i].empty();
	}

	private:
		std::vector<VectorType>& m_ecs;
};

struct indexComparer
{
	indexComparer(const std::vector<int>& v) : m_v(v) {}

	bool operator()(int i1, int i2)
	{
		return m_v[i1] < m_v[i2];
	}

private:
	const std::vector<int>& m_v;
};

void ECOccFilter::run(FuzzyMultiDataExtAlgorithm &algorithm, int abType) const {
	// TODO сделать нормальный комментарий
	
	typedef std::deque<FuzzyMultiDataExtAlgorithm::ECIndex> EcVectorType;
	
	Logger* logger = Logger::getInstance();

	
	std::vector<EcVectorType> ecs;
	std::vector<int> saturation;
	std::list<int> indices;
	
	int multiTSCount = algorithm.getDataSet().getMutiTSCount(ClippedFullFuzzyDataSet::Reference, abType); 
	
	double maxOccurrence = 0.0;
	for(int dimension = 0; dimension < algorithm.getECSize(abType); dimension++) {
		for(int ecType = 0; ecType < algorithm.getECSize(abType, dimension); ecType++) {	
			for(int n = 0; n < algorithm.getECSize(abType, dimension, ecType); n++) {
				auto ecIndex = FuzzyMultiDataExtAlgorithm::makeECIndex(abType, dimension, ecType, n);
				if(algorithm.isECSelected(ecIndex)) {
					double currentOccurrence = algorithm.getEC(ecIndex).statOccurence;
					if(maxOccurrence < currentOccurrence) {
						maxOccurrence = currentOccurrence;
					}
				}
			}
		}
	}
	
	// Порог -- макс. встречаемость минус отступ
	double threshold = maxOccurrence - m_offset;
	
	ecs.clear();
	ecs.resize(multiTSCount);
	for(int dimension = 0; dimension < algorithm.getECSize(abType); dimension++) {
		for(int ecType = 0; ecType < algorithm.getECSize(abType, dimension); ecType++) {	
			for(int n = 0; n < algorithm.getECSize(abType, dimension, ecType); n++) {
				auto ecIndex = FuzzyMultiDataExtAlgorithm::makeECIndex(abType, dimension, ecType, n);
				const ElemCondPlusStat& currentEC = algorithm.getEC(ecIndex);
				if(algorithm.isECSelected(ecIndex)) {
					if(currentEC.statOccurence >= threshold
							&& currentEC.statOccVector.size() > 0) {
						for(int i = 0; i < ecs.size(); i++) {
							if(currentEC.statOccVector[i]) {
								// Кладем в ecs[i] индекс ЭУ, встречающегося на i-й траектории
								ecs[i].push_back(ecIndex);
							}
						}
					}
					algorithm.setECSelected(ecIndex, false);
				}
			}
		}
	}
	
	//logger->writeComment("ECs after threshold: "+boost::lexical_cast<std::string>(std::accumulate()));
	
	// Сортируем каждый дек по значению целевой функции и встречаемости
	for(auto it = ecs.begin(); it != ecs.end(); it++) {
		std::sort(it->begin(), it->end(), 
				  ecComparer(algorithm));
	}

	saturation.resize(multiTSCount);
	std::fill(saturation.begin(), saturation.end(), 0);

	{
		// Формируем в indices индексы непустых векторов из ecs
		for(int i = 0; i < multiTSCount; ++i)
		{
			indices.push_back(i);
		}
		//std::generate_n(std::back_inserter(indices), multiTSCount, [&i]() { return i++; });
		indices.remove_if(emptyChecker<EcVectorType>(ecs));
	}
	
	if(indices.empty()) {
		logger->writeComment("No ecs selected for abnormal type "+boost::lexical_cast<std::string>(abType));
		return;
	}

	int selectedCount = 0;
	auto currentIndexIter = indices.begin();
	int currentIndex = indices.front();
	auto ecIndex = ecs[currentIndex].back();
	
	while(true) {
		const std::vector<bool>& currentOccVector = algorithm.getEC(ecIndex).statOccVector;
		bool select = false;
		for(int i = 0; i < multiTSCount; i++) {
			if(currentOccVector[i] && saturation[i] < m_maxSaturation) {
				++saturation[i];
				select = true;
			}
		}
		
		if(select) {
			++selectedCount;
			algorithm.setECSelected(ecIndex, true);
		}
		
		if(selectedCount >= m_maxECs) {
			logger->writeComment("Stopping because selected maximum number of ECs");
			break;				
		}
					
		//Находим индекс траектории с минимальным насыщением и индекс еще не выбранного ЭУ
		while(!indices.empty()) {
			currentIndexIter = std::min_element(indices.begin(), indices.end(), 
												indexComparer(saturation));
			currentIndex = *currentIndexIter;
			
			do {
				ecIndex = ecs[currentIndex].back();
				ecs[currentIndex].pop_back();				
			} while (algorithm.isECSelected(ecIndex) && ecs[currentIndex].size() != 0);
			
			if(ecs[currentIndex].size() == 0) {
				indices.erase(currentIndexIter);
				if(algorithm.isECSelected(ecIndex)) {
					continue;
				}
			}
			break;
		}
		
		if(indices.empty()) {
			logger->writeComment("Stopping because used all available ECs");
			break;								
		}
				
		if(saturation[currentIndex] >= m_requiredSaturation) {
			logger->writeComment("Stopping because reached required saturation");
			break;
		}					
	}
				
	// TODO функция слишком большая, сделать декомпозицию
}

const double epsilon = 1e-6;

bool ecCompare(const ElemCondPlusStat &ec1, const ElemCondPlusStat &ec2) {
	if(ec1.goal < ec2.goal) {
		return true;
	} else if (ec1.goal - ec2.goal < epsilon) {
		return ec1.statOccurence < ec2.statOccurence; 
	} else {
		return false;
	}
}

void ECOccFilter::initFromEnvImpl(const Environment &env) {
	env.getParamValue(m_maxECs, getParamName("maxToSelect"));
	env.getParamValue(m_maxSaturation, getParamName("maxSaturation"));
	env.getParamValue(m_requiredSaturation, getParamName("requiredSaturation"));
	env.getParamValue(m_offset, getParamName("offset"));
}
