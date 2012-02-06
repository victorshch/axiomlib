#ifndef DISTANCECLUSTERIZER_H
#define DISTANCECLUSTERIZER_H

#include <list>
#include <deque>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>

namespace AxiomLib {

template<class Element, class Vector>
class DistanceClusterizer {
public:
	typedef std::list<Element> Cluster;

	DistanceClusterizer(boost::function1<Vector,const Element &> vectorFunction, boost::function2<double, const Vector&, const Vector&> distanceFunction, double radius) :
			m_vectorFunction(vectorFunction),
			m_distanceFunction(distanceFunction),
			m_radius(radius)
	{

	}

	void clusterize(const Element &element) {
		auto vector = m_vectorFunction(element);

#ifdef DEBUG_DISTANCECLUSTERIZER
		std::cout<<"Clusterizing element "<<element<<" with vector "<<vector<<std::endl;
#endif

                for(auto it = m_clusters.begin(); it != m_clusters.end(); it++) {
#ifdef DEBUG_DISTANCECLUSTERIZER
			std::cout<<"Checking cluster..."<<std::endl;
#endif
                        const auto& currentCenter = it->template get<1>();
			double distance = m_distanceFunction(vector, currentCenter);
#ifdef DEBUG_DISTANCECLUSTERIZER
			std::cout<<"Distance : "<<distance<<std::endl;
#endif
			if(distance < m_radius) {
#ifdef DEBUG_DISTANCECLUSTERIZER
				std::cout<<"Match !"<<std::endl;
#endif
                                it->template get<0>().push_back(element);
				// TODO: обновление центра
				return;
			}
		}

		Cluster newCluster = Cluster();
		newCluster.push_back(element);

		m_clusters.push_front(ExtendedCluster(newCluster, vector));
	}

	int clusterCount() const { return m_clusters.size(); }
        const Cluster &getCluster(int i) const { return m_clusters[i].template get<0>(); }

	typedef boost::function1<Vector,const Element &> VectorFunction;
	typedef boost::function2<double, const Vector&, const Vector&> DistanceFunction;
	
private:
	typedef boost::tuples::tuple<Cluster, Vector> ExtendedCluster;
	typedef std::deque<ExtendedCluster> ExtendedClusterList;

	double m_radius;

	VectorFunction m_vectorFunction;
	DistanceFunction m_distanceFunction;

	ExtendedClusterList m_clusters;
};

template<class Element, class Vector, class VectorFunction, class DistanceFunction>
DistanceClusterizer<Element, Vector> make_distanceClusterizer(VectorFunction vectorFunction, DistanceFunction distanceFunction, double radius) {
	return DistanceClusterizer<Element, Vector>(
                    typename DistanceClusterizer<Element, Vector>::VectorFunction(vectorFunction),
                    typename DistanceClusterizer<Element, Vector>::DistanceFunction(distanceFunction),
	            radius);
}

}

#endif // DISTANCECLUSTERIZER_H
