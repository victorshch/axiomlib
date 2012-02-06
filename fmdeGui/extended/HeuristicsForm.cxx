#include "FuzzyMultiDataExt/Heuristics.h"

#include "HeuristicsForm.h"
#include "ui_HeuristicsForm.h"

HeuristicsForm::HeuristicsForm(const std::string &paramName, const std::string &name, const QString &title, QWidget *parent) :
    AbstractParamsForm(parent),
    ui(new Ui::HeuristicsForm),
    m_name(name),
    m_paramName(paramName)
{
    ui->setupUi(this);
	
	paramsForm = new ParamsForm();
	
	ui->verticalLayout_params->addWidget(paramsForm);
		
	ui->groupBox->setTitle(title);
	
	connect(ui->pushButton_execute, SIGNAL(clicked()), this, SIGNAL(executed()));
}

std::string HeuristicsForm::strValue(const std::string &paramName) const {
	if(paramName == m_paramName) {
		return m_name;
	}
	
	return paramsForm->strValue(paramName);
}

void HeuristicsForm::setStrValue(const std::string &paramName, const std::string &newValue) {
	paramsForm->setStrValue(paramName, newValue);
}

std::list<std::string> HeuristicsForm::paramNames() const {
	auto result = paramsForm->paramNames();
	result.push_front(m_paramName);
	return result;
}

void HeuristicsForm::addParam(const StrategyParamSpec &spec, const QString &paramTitle) {
	StrategyParamSpec newSpec(spec);
	newSpec.paramName = AxiomLib::FuzzyMultiDataExt::Heuristics::getParamName(m_name, spec.paramName);
	paramsForm->addParam(newSpec, paramTitle);
}

void HeuristicsForm::enable() {
	enable(true);
}

void HeuristicsForm::disable() {
	enable(false);
}

void HeuristicsForm::enable(bool value) {
	ui->pushButton_execute->setEnabled(value);
}

HeuristicsForm::~HeuristicsForm()
{
    delete ui;
}
