/****************************************************************************
*			Condition
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef CONDITION_H_
#define CONDITION_H_

#include "ElemCondPlus.h"
#include "AxiomExpr.h"

#include "MarkupQwtAdapter.h"
#include "Index.h"
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include <QtDebug>

class Condition {
	Index m_identifier;
	int m_abnormalType;
public:
	Condition(Index identifier, int abnormalType) : m_identifier(identifier), m_abnormalType(abnormalType) { }
	virtual int check(unsigned long k, const std::vector<std::vector<double> > &ts) const = 0;
	virtual std::string name() const = 0;
	virtual const std::set<int> &dimensions() const = 0;

	Index identifier() const { return m_identifier; }
	int abnormalType() const { return m_abnormalType; }
	//void setIdentifier(Index newIdentifier) { identifier_ = newIdentifier; }
	
//	bool operator ==(const Condition &other) const {
//		return identifier()->compare(other.identifier());
//	}
	
	//TODO: убрать markUp из ElemCondPlus
	unsigned long markUp(const std::vector<std::vector<double> > &ts, std::vector<int> &markup) const;
};

typedef boost::shared_ptr<Condition> PCondition;

class ConditionEC : public Condition {
	AxiomLib::ElemCondPlusStat ec;
	std::string m_name;
	std::set<int> m_dimensions;
public:
	ConditionEC(const AxiomLib::ElemCondPlusStat &ec, Index identifier, int abnormalType);

	int check(unsigned long k, const std::vector<std::vector<double> > &ts) const {
		return ec.check(k, ts[ec.dimension]);
	}

	inline std::string name() const { return m_name; }

	inline const std::set<int> &dimensions() const { return m_dimensions; }
};

//typedef boost::shared_ptr<AxiomLib::AxiomExpr> PAxiomExpr;

class ConditionAxiom : public Condition {
	AxiomLib::AxiomExpr axiomExpr;
	std::string m_name;
	std::set<int> m_dimensions;
public:
	ConditionAxiom(AxiomLib::AxiomExpr axiomExpr, Index identifier, int abnormalType);

	int check(unsigned long k, const std::vector<std::vector<double> > &ts) const {
		return axiomExpr.check(k, ts);
	}

	inline std::string name() const { return m_name; }

	inline const std::set<int> &dimensions() const { return m_dimensions; }
};

#endif /* CONDITION_H_ */
