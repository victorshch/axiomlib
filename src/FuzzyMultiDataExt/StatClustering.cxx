#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <cmath>

#include "../DistanceClusterizer.h"
#include "../FuzzyMultiDataExtAlgorithm.h"

#include "Common.h"

#include "StatClustering.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

StatClustering::StatClustering()
{
	m_radius = 1e-6;
	m_retain = 1;
}

void StatClustering::initFromEnvImpl(const Environment &env) {
	env.getParamValue(m_radius, getParamName("radius"));
	env.getParamValue(m_retain, getParamName("retain"));
}

template<class Value>
struct vectorElementAccess
{
	vectorElementAccess(const std::vector<Value> &v): m_v(v)
	{

	}

	Value operator()(int i)
	{
		return m_v[i];
	}

private:
	const std::vector<Value> &m_v;
};

void StatClustering::clusterize(const std::vector<StatVector> &v, std::vector<bool> &result) const {
	Logger::getInstance()->writeComment("Clusterizing "+str(v.size())+" items");
	
	auto clusterizer = make_distanceClusterizer<int, StatVector>(
				vectorElementAccess<StatVector>(v),
				StatVector::distance,
				m_radius
	);
				
	for(int n = 0; n < v.size(); n++) {
		clusterizer.clusterize(n);
	}
	result.clear();
	result.resize(v.size(), false);
	
	Logger::getInstance()->writeComment("Number of clusters : "+str(clusterizer.clusterCount()));
	for(int i = 0; i < clusterizer.clusterCount(); i++) {
		int selectedECs = 0;
		const auto& cluster = clusterizer.getCluster(i);
		Logger::getInstance()->writeComment("Number of items in "+str(i+1)+"'th cluster : "+str(cluster.size()));
		BOOST_FOREACH(int n, cluster) {
			if(selectedECs < m_retain) {
				result[n] = true;
				++selectedECs;
			} else {
				break;
			}
		}
	}	
}

	int StatClustering::radius() const {
		return m_radius;
	}
	
	int StatClustering::retain() const {
		return m_retain;
	}
