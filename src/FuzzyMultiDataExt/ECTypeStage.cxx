#include <algorithm>

#include "../ElemConditionsFactory.h"

#include "ECTypeStage.h"

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

void setSelected(ECTypeSelection& ecPlus)
{
	ecPlus.setSelected(true);
}

ECTypeStage::ECTypeStage()
{
	//Заполнение вектора типов ЭУ - по умолчанию используются все типы ЭУ
	ElemConditionsFactory ecf;
	this->ecTypes.resize (2*N_EL_AXIOMS);
	for (unsigned int i = 0; i < N_EL_AXIOMS; i++) {
		ecTypes[2*i].element().elemCondition = ecf.createAxiomByNum(i+1);
		ecTypes[2*i].element().sign = true;
		ecTypes[2*i].element().dimension = -1;
		ecTypes[2*i+1].element().elemCondition = ecf.createAxiomByNum(i+1);
		ecTypes[2*i+1].element().sign = false;
		ecTypes[2*i+1].element().dimension = -1;
	}	
	
	std::for_each(ecTypes.begin(), ecTypes.end(), setSelected);
}

ECTypeSelection ecTypeSelectionCreator(const ElemCondPlus& ec) {return ECTypeSelection(ec);}


void ECTypeStage::setECTypes(const std::vector<ElemCondPlus> &value) {
	relaxed_transform(value.begin(), value.end(), this->ecTypes.begin(), 
					  ecTypeSelectionCreator
	);	
}

void ECTypeStage::setLogger(Logger *logger) {
	this->logger = logger;
}

void ECTypeStage::initFromEnv(const Environment &/*env*/) {
	// Пока у этого этапа нет параметров
}

const ElemCondPlus &ECTypeStage::getECType(int n) const {
	return ecTypes[n].element();
}

int ECTypeStage::getECTypeSize() const {
	return ecTypes.size();
}

bool ECTypeStage::isECTypeSelected(int n) const {
	return ecTypes[n].selected();
}

void ECTypeStage::setECTypeSelected(int n, bool value) {
	ecTypes[n].setSelected(value);
}
