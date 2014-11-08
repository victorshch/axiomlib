#ifndef INDEXPAIR_H
#define INDEXPAIR_H

/*

#include <map>
#include <deque>
#include <string>

#include <QObject>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>


*/

#include <algorithm>

#include <boost/array.hpp>

#include <QMetaType>

const int maxIndexSize = 4;

// Индекс, идентифицирующий ЭУ или аксиому в наборе
typedef boost::array<int, maxIndexSize> Index;

Q_DECLARE_METATYPE(Index);

class IndexPair;

bool operator==(const Index& i1, const Index& i2);

std::ostream& operator<<(std::ostream& ostr, const Index& index);

Index makeIndex(int i, int j, int k, int l);

Index makeIndex(int i, int j);

Index makeIndex(int i);

/*

class Index;

typedef boost::shared_ptr<Index> PIndex;

class Index : public QObject {
	Q_OBJECT
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive &a, unsigned int version) {
		
	}

public:
	Index(QObject *parent): QObject(parent) { }
	
	//virtual bool compare(Index* other) const = 0;
	
	// not used
	virtual PIndex clone() const = 0;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Index);

typedef std::map<std::string, std::deque<PIndex> > IndexSelections;

class IndexPair : public Index {
	Q_OBJECT
private:
	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive &a, unsigned int version) {
		a & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Index);
		a & BOOST_SERIALIZATION_NVP(i);
		a & BOOST_SERIALIZATION_NVP(j);
	}

public:
	int i, j;
	IndexPair(QObject *parent, int i = 0, int j = 0) : Index(parent), i(i), j(j) {}
	
	//virtual bool compare(Index *other) const;
	
	PIndex clone() const { return PIndex(new IndexPair(0, i, j)); }
};



class IndexQuadruple : public Index {
	Q_OBJECT
private:
	friend class boost::serialization::access;
	
	template<class Archive>
	void serialize(Archive &a, unsigned int version) {
		a & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Index);
		a & boost::serialization::make_nvp("i", indices.get<0>());
		a & boost::serialization::make_nvp("j", indices.get<1>());
		a & boost::serialization::make_nvp("k", indices.get<2>());
		a & boost::serialization::make_nvp("l", indices.get<3>());
	}
	
public:
	boost::tuples::tuple<int, int, int, int> indices;
	
	//IndexQuadruple() {}
	
	IndexQuadruple(QObject *parent, int i, int j, int k, int l) : Index(parent),
	indices(boost::tuples::make_tuple(i, j, k, l)) { }
	
	int i() const { return indices.get<0>(); }
	int j() const { return indices.get<1>(); }
	int k() const { return indices.get<2>(); }
	int l() const { return indices.get<3>(); }
	
	//virtual bool compare(Index *other) const;
	
	PIndex clone() const { 
		IndexQuadruple *result = new IndexQuadruple(0, i(), j(), k(), l());
		return PIndex(result);
	}
};

*/

#endif // INDEXPAIR_H
