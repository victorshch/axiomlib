#include "TaggedDataSet.h"

#include <algorithm>

using namespace AxiomLib;

TaggedDataSet::TaggedDataSet()
{
}

TrajectoryVector TaggedDataSet::getTrajectoriesWithTags(TagSetSpec tagSet) const
{
	TrajectoryVector result;

	for(size_t i = 0; i < mTrajectories.size(); ++i) {
		if(tagSet.match(mTrajectories[i].tags)) {
			result.mTrajectories.push_back(mTrajectories[i].trajectory);
		}
	}

	return result;
}

void TaggedDataSet::addTrajectory(MultiTS *multiTS, const std::set<std::string> &tags)
{
	mTrajectories.push_back(TaggedTrajectory(multiTS, tags));
}


TagSetSpec::TagSetSpec(const std::string &tag)
{
	mPositiveSet = boost::shared_ptr<std::set<std::string> >(new std::set<std::string>());
	mPositiveSet->insert(tag);
	mNegativeSet = boost::shared_ptr<std::set<std::string> >(new std::set<std::string>());
}

bool TagSetSpec::match(const std::set<std::string> &target) const
{
	for(TagSet::const_iterator it = mNegativeSet->begin(); it != mNegativeSet->end(); ++it) {
		if(target.find(*it) != target.end()) {
			return false;
		}
	}

	for(TagSet::const_iterator it = mPositiveSet->begin(); it != mPositiveSet->end(); ++it) {
		if(target.find(*it) == target.end()) {
			return false;
		}
	}

	return true;
}

TagSetSpec TagSetSpec::operator +(const TagSetSpec &other)
{
	TagSetSpec result(*this);

	result.mPositiveSet->insert(other.mPositiveSet->begin(), other.mPositiveSet->end());
	result.mNegativeSet->insert(other.mNegativeSet->begin(), other.mNegativeSet->end());

	return result;
}

TagSetSpec TagSetSpec::operator -() const
{
	TagSetSpec result(*this);

	std::swap(result.mPositiveSet, result.mNegativeSet);

	return result;
}


TagSetSpec::TagSetSpec()
{
	mPositiveSet = boost::shared_ptr<std::set<std::string> >(new std::set<std::string>());
	mNegativeSet = boost::shared_ptr<std::set<std::string> >(new std::set<std::string>());
}
