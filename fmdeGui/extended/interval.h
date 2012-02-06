#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>
#include <iostream>
#include <string>
#include <sstream>

class DoubleInterval
{
	double min_, max_, minmin_, maxmax_;
public:
	
    DoubleInterval() 
	{
		reset();
	}
	
	void reset()
	{
		min_ = maxmax_ = std::numeric_limits<double>::max();
		max_ = minmin_ = std::numeric_limits<double>::min();		
	}
	
	void setMinMin(double newMinMin) {
		minmin_ = newMinMin;
	}
	
	void setMaxMax(double newMaxMax) {
		maxmax_ = newMaxMax;
	}
	
	void fitLimits(double value)
	{
		if(value < min_ && value > minmin_)
		{
			min_ = value;
		}
		if(value > max_ && value < maxmax_)
		{
			max_ = value;
		}
	}
	
	void operator()(double value)
	{
		fitLimits(value);
	}
	
	double min() const { return min_; }
	double max() const { return max_; }
	
	std::string toString() const {
		std::ostringstream ostr;
		ostr<<"("<<min()<<", "<<max()<<")";
		return ostr.str();
	}
};

#endif // INTERVAL_H
