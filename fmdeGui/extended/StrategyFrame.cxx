#include <boost/lexical_cast.hpp>

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>

#include "QtGuiException.h"
#include "StrategyFrame.h"
#include "ui_StrategyFrame.h"

StrategyFrame::StrategyFrame(const std::string &strategyParamName, const QString &strategyLabel, QWidget *parent) :
    AbstractParamsForm(parent),
    ui(new Ui::StrategyFrame)
{
    ui->setupUi(this);
	
	ui->Label_strategyText->setText(strategyLabel);
	
	m_strategyParamName = strategyParamName;
	
	stackedLayout = new QStackedLayout();
	
	ui->verticalLayout_params->addLayout(stackedLayout);
	
	connect(ui->ComboBox_strategy, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentStrategy(int)));
}

void StrategyFrame::addStrategy(const QList<QPair<StrategyParamSpec, QString> > &paramSpecs, const std::string &strategyName, const QString &strategyTitle) {
	//m_params.insert(strategyName, ParamsContainer::mapped_type());
	ParamsForm* paramsForm = new ParamsForm();
	for(auto it = paramSpecs.begin(); it != paramSpecs.end(); ++it) {
		paramsForm->addParam(it->first, it->second);
	}
	
        //m_params.insert(strategyName, stackedLayout->count());
	
	stackedLayout->addWidget(paramsForm);
	
	ui->ComboBox_strategy->addItem(strategyTitle, QVariant::fromValue(QString::fromStdString(strategyName)));
}

StrategyFrame::~StrategyFrame()
{	
    delete ui;
}

std::string StrategyFrame::currentStrategy() const {
	return m_currentStrategy;
}

void StrategyFrame::setCurrentStrategy(const std::string &strategyName) {
	int index = ui->ComboBox_strategy->findData(QString::fromStdString(strategyName));
	if(index >= 0) {
		ui->ComboBox_strategy->setCurrentIndex(index);
		stackedLayout->setCurrentIndex(index);
	} else {
		throw QtGuiException(
		            QString("Strategy name %1 not supported in strategy %2").arg(strategyName.c_str(), m_strategyParamName.c_str()));
	}
	m_currentStrategy = strategyName;
}

std::string StrategyFrame::strValue(const std::string &paramName) const {
	if(paramName == m_strategyParamName) {
		return currentStrategy();
	}
	
	return getCurrentParamsForm()->strValue(paramName);
}

void StrategyFrame::setStrValue(const std::string &paramName, const std::string &newValue) {
	if(paramName == m_strategyParamName) {
		setCurrentStrategy(newValue);
	} else {
		getCurrentParamsForm()->setStrValue(paramName, newValue);
	}
}

std::list<std::string> StrategyFrame::paramNames() const {
	std::list<std::string> result = getCurrentParamsForm()->paramNames();
	result.push_front(m_strategyParamName);
	return result;
}

void StrategyFrame::setCurrentStrategy(int index) {
	stackedLayout->setCurrentIndex(index);
	QString strategyName = ui->ComboBox_strategy->itemData(index).toString();
	m_currentStrategy = strategyName.toStdString();
}

ParamsForm* StrategyFrame::getCurrentParamsForm() const {
	ParamsForm* current = dynamic_cast<ParamsForm*>(stackedLayout->currentWidget());
	if(current == 0) {
		throw QtGuiException("Invalid widget in stack layout");
	}
	return current;	
}
