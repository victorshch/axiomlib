#ifndef PROBABILITYMAPPING_H
#define PROBABILITYMAPPING_H

#include <algorithm>
#include <shark/Algorithms/DirectSearch/TypedIndividual.h>

namespace AxiomLib {

struct ProbabilityMapping {
	ProbabilityMapping(double minFitnessDifference = 0.5): mMinFitnessDifference(minFitnessDifference) {}

	template<class Iterator, class Extractor, class Mapper, class Tag>
	void operator()(Iterator begin, Iterator end, Extractor e, Mapper mapper, Tag tag) {
		unsigned n = std::distance(begin, end);

		std::vector<double> fitnessVector(n, 0.);

		std::transform(begin, end, fitnessVector.begin(), e);

		double minFitness = *std::min_element(fitnessVector.begin(), fitnessVector.end());
		double maxFitness = *std::max_element(fitnessVector.begin(), fitnessVector.end());
		if(std::abs(maxFitness - minFitness) < mMinFitnessDifference) {
			for(Iterator it = begin; it != end; ++it) {
				it->probability(tag) = 1.0 / (double) n;
			}

			return;
		}

		std::vector<double>::const_iterator fitnessIt = fitnessVector.begin();
		for(Iterator it = begin; it != end; ++it) {
			it->probability(tag) = mapper(*fitnessIt, minFitness, maxFitness);
			++fitnessIt;
		}

	}

private:
	double mMinFitnessDifference;
};

struct LinearMapper {
	double operator()(double value, double min, double /*max*/) const {
		double t = value - min;

		return t;
	}
};

struct ExpMapper {
	ExpMapper(double beta, bool relative = true) : mBeta(beta), mRelative(relative) {}

	double operator()(double value, double min, double /*max*/) const {
		double t = value;

		if(mRelative) {
			t -= min;
		}

		return (double) std::exp((long double) ((double) t * beta));
	}

private:
	double mBeta;
	bool mRelative;
};

template<class Mapper>
struct InverseMapper {
	InverseMapper(Mapper innerMapper): mInnerMapper(innerMapper) {}


	double operator()(double value, double min, double max) {
		double t = max - (value - min);
		return mInnerMapper(t, min, max);
	}

private:
	Mapper mInnerMapper;
};

template<class Mapper>
struct ScaledMapper {
	ScaledMapper(Mapper innerMapper): mInnerMapper(innerMapper) {}

	double operator()(double value, double min, double max) {
		double t = (value - min) / (max - min);

		return mInnerMapper(t, 0., 1.);
	}

private:
	Mapper mInnerMapper;
};

}

#endif // PROBABILITYMAPPING_H
