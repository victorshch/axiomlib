#ifndef ROULETTEWHEELSELECTION_H
#define ROULETTEWHEELSELECTION_H

#include <vector>
#include <algorithm>
#include <shark/Rng/GlobalRng.h>

struct RouletteWheelSelection {

	/**
	* \brief Selects an individual from the range of individuals with prob. proportional to given number.
	* \tparam Extractor Type for mapping elements from the underlying range to non-negative real numbers.
	* \param begin Iterator pointing to the first valid element.
	* \param end Iterator pointing to the first invalid element.
	* \param out OutIterator where to put selected individuals
	* \param e Object of type Extractor for extracting the values according which to select
	* \param size the number of individuals to select
	*/
	template<class Iterator, class OutIterator, class Extractor>
	void operator() (Iterator begin, Iterator end, OutIterator out, Extractor& e, int size) {
		std::size_t n = std::distance(begin, end);

		std::vector< double > rouletteWheel(n, 0.);

		std::transform(begin,
				end,
				rouletteWheel.begin(),
				e
		);

		double sum = std::accumulate(rouletteWheel.begin(), rouletteWheel.end(), 0.);

		for(int i = 0; i < size; ++i) {
			double rouletteSpin = shark::Rng::uni(0.0, sum);
			std::vector< double >::const_iterator rouletteIt = rouletteWheel.begin();
			Iterator it = begin;
			for(; it != end; ++it) {
				if(rouletteSpin <= *rouletteIt) {
					break;
				}

				rouletteSpin -= *rouletteIt;

				++rouletteIt;
			}

			if (it != end) {
				*out = *it;
			} else {
				// This shouldn't happen
				*out = *(begin + n - 1);
			}
			++out;
		}
	}
};

#endif // ROULETTEWHEELSELECTION_H
