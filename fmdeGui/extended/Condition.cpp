#include "Condition.h"

unsigned long Condition::markUp(const std::vector<std::vector<double> > &ts, 
								std::vector<int> &markup) const {
	unsigned long result = 0;
	if(ts.size() == 0) {
		return 0;
	}
	int tsSize = ts[0].size();
	markup.resize(tsSize);
	//qDebug()<<"tsSize == "<<tsSize;
	for(int i = 0; i < tsSize; i++) {
		int checkResult = check(i, ts);
		if(checkResult == 1) {
			result++;
			markup[i] = 1;
		} else {
			markup[i] = 0;
		}
	}
	return result;
}

ConditionEC::ConditionEC(const AxiomLib::ElemCondPlusStat &ec, Index identifier, int abnormalType) 
	: Condition(identifier, abnormalType), ec(ec) {
	if(ec.dimension < 0) {
		throw AxiomLib::AxiomLibException("ConditionEC() : dimension not set");
	}
	m_dimensions.insert(ec.dimension);
	std::ostringstream ostr;
	ostr<<ec.name<<" ("<<ec.elemCondition->name()<<")";
	this->m_name = ostr.str();
}

ConditionAxiom::ConditionAxiom(AxiomLib::AxiomExpr axiomExpr, Index identifier, int abnormalType)
	: Condition(identifier, abnormalType), m_name(axiomExpr.name()), axiomExpr(axiomExpr) {
	for(unsigned int i = 0; i < axiomExpr.expression.size(); i++) {
		for(unsigned int j = 0; j < axiomExpr.expression[i].size(); j++) {
			m_dimensions.insert(axiomExpr.expression[i][j].dimension);
		}
	}
}
