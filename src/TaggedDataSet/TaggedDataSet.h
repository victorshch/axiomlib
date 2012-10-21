#ifndef TAGGEDDATASET_H
#define TAGGEDDATASET_H

#include <string>
#include <deque>
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>

#include "DataSetBase.h"

namespace AxiomLib {

// Спецификация запроса к набору данных.
// Содержит набор тегов, каждый из которых должен присутствовать
// у возвращаемых траекторий, и набор тегов, каждый из которых не должен
// присутствовать у возвращаемых траекторий
class TagSetSpec {
public:
	TagSetSpec();

	TagSetSpec(const std::string& tag);

	// Проверка того, подходит ли данная спецификация ко множеству тегов target
	bool match (const std::set<std::string>& target) const;

	// Объединение запросов -- объединяются множества тегов, которые
	// должны присутствовать, и множества тегов, которые не должны присутствовать
	TagSetSpec operator +(const TagSetSpec& other);

	// Меняет местами множества тегов, которые должны и не должны присутствовать
	TagSetSpec operator -() const;

private:
	typedef std::set<std::string> TagSet;

	boost::shared_ptr<TagSet> mPositiveSet;
	boost::shared_ptr<TagSet> mNegativeSet;
};

class TrajectoryVector {
	friend class TaggedDataSet;
public:
	int trajectoryCount() const { return (int) mTrajectories.size(); }
	const MultiTS* trajectory(int trajectoryIndex) const { return mTrajectories[(size_t)trajectoryIndex]; }

private:
	std::deque<MultiTS*> mTrajectories;
};

class TaggedDataSet : public DataSetBase
{
public:

    TaggedDataSet();
	~TaggedDataSet();

	TrajectoryVector getTrajectoriesWithTags(TagSetSpec tagSet) const;
//	const MultiTS* getTimeSeriesWithTags(TagSet tagSet, int tsNo) const;

	// takes ownership of multiTS
	void addTrajectory(MultiTS* multiTS, const std::set<std::string>& tags);

private:
	struct TaggedTrajectory {
		MultiTS* trajectory;
		std::set<std::string> tags;

		TaggedTrajectory(MultiTS* trajectory, const std::set<std::string>& tags)
			: trajectory(trajectory),  tags(tags) { }
	};

	std::deque<TaggedTrajectory> mTrajectories;
};

}

#endif // TAGGEDDATASET_H
