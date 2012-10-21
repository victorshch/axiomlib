#ifndef TAGGEDLIST_H
#define TAGGEDLIST_H

#include <algorithm>
#include <iterator>

#include <boost/shared_ptr.hpp>

namespace AxiomLib {

// Спецификация запроса к набору данных.
// Содержит набор тегов, каждый из которых должен присутствовать
// у возвращаемых траекторий, и набор тегов, каждый из которых не должен
// присутствовать у возвращаемых траекторий
template<class Tag>
class TagSetQuery {
public:
	TagSetQuery() { init(); }

	TagSetQuery(const Tag& tag) { init(); mPositiveSet->insert(tag); }

	// Проверка того, подходит ли данная спецификация ко множеству тегов target
	bool match (const std::set<Tag>& target) const;

	// Объединение запросов -- объединяются множества тегов, которые
	// должны присутствовать, и множества тегов, которые не должны присутствовать
	TagSetQuery<Tag> operator +(const TagSetQuery<Tag>& other) {
		TagSetQuery<Tag> result;
		make_union(*mPositiveSet, *other.mPositiveSet, *result.mPositiveSet);
		make_union(*mNegativeSet, *other.mNegativeSet, *result.mNegativeSet);
		return result;
	}

	// Меняет местами множества тегов, которые должны и не должны присутствовать
	TagSetQuery<Tag> operator -() const {
		TagSetQuery<Tag> result(*this);
		std::swap(result.mPositiveSet, result.mNegativeSet);
		return result;
	}

private:
	typedef std::set<Tag> TagSet;

	boost::shared_ptr<TagSet> mPositiveSet;
	boost::shared_ptr<TagSet> mNegativeSet;

	void init() {
		mPositiveSet = boost::shared_ptr<TagSet>(new TagSet);
		mNegativeSet = boost::shared_ptr<TagSet>(new TagSet);
	}

	static void make_union(const std::set<Tag>& set1, const std::set<Tag>& set2,
						   std::set<Tag>& out) {
		std::set_union(set1.begin(), set1.end(),
					   set2.begin(), set2.end(),
					   std::inserter(out, out.begin()));
	}
};

template<class Value>
class QueryResult {
	friend class TaggedList;
public:
	int count() const { return (int) mValues.size(); }
	Value operator[](int elementIndex) const { return mValues[(size_t)elementIndex]; }

private:
	std::deque<Value> mValues;
};

#endif // TAGGEDLIST_H
