#include <QString>
#include <QByteArray>
#include <QItemEditorFactory>
#include <QLabel>

#include "ParamsForm.h"
#include "ui_ParamsForm.h"

StrategyParamSpec::StrategyParamSpec() {}

ParamsForm::ParamsForm(QWidget *parent) :
    AbstractParamsForm(parent),
    ui(new Ui::ParamsForm)
{
    ui->setupUi(this);
	
	//ui->formLayout_params->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	ui->formLayout_params->setRowWrapPolicy(QFormLayout::WrapLongRows);
}

void ParamsForm::addParam(const StrategyParamSpec &paramSpec, const QString &paramTitle) {
	QWidget* editor = createEditor(paramSpec);
	QLabel* label = new QLabel(paramTitle);
	//label->setWordWrap(true);
	ui->formLayout_params->addRow(label, editor);
	//ui->formLayout_params->activate();
        m_params[QString::fromStdString(paramSpec.paramName)] = qMakePair(editor, paramSpec);
}

ParamsForm::~ParamsForm()
{	
    delete ui;
}

QVariant ParamsForm::value(const std::string &paramName) const {
        auto it = m_params.find(QString::fromStdString(paramName));
	if(it == m_params.end()) {
		return QVariant();
	}
	
	return getValue(it.value().first, it.value().second);
}

std::string ParamsForm::strValue(const std::string &paramName) const {
	return value(paramName).toString().toStdString();
}

void ParamsForm::setValue(const std::string &paramName, QVariant newValue) {
        auto it = m_params.find(QString::fromStdString(paramName));
	if(it == m_params.end()) {
		return;
	}

        setValue(it.value().first, newValue, it.value().second);
}

void ParamsForm::setStrValue(const std::string &paramName, const std::string &newValueStr) {
        auto it = m_params.find(QString::fromStdString(paramName));
	if(it == m_params.end()) {
		return;
	}
	
	QVariant newValue;
	
	if(newValueStr.empty()) {
		// Если строка пустая, ставим по дефолту
		newValue = it.value().second.value;
	} else {	
		newValue = QString::fromStdString(newValueStr);
		newValue.convert(it.value().second.value.type());
	}
	
	setValue(it.value().first, newValue, it.value().second);
}

std::list<std::string> ParamsForm::paramNames() const {
	std::list<std::string> result;
	for(auto it = m_params.begin(); it != m_params.end(); ++it) {
                result.push_back(it.key().toStdString());
	}
	return result;
}

QWidget* ParamsForm::createEditor(const StrategyParamSpec &spec) const {
	QWidget* result = m_factory.createEditor(spec.value.type(), 0);
	
	result->setProperty(m_factory.valuePropertyName(spec.value.type()).constData(), spec.value);
	
	if(result->metaObject()->indexOfProperty("maximum") != -1) {
		result->setProperty("maximum", spec.max);
	}
	
	if(result->metaObject()->indexOfProperty("minimum") != -1) {
		result->setProperty("minimum", spec.min);
	}
	
	if(result->metaObject()->indexOfProperty("singleStep") != -1) {
		result->setProperty("singleStep", spec.step);
	}
	
	if(result->metaObject()->indexOfProperty("decimals") != -1) {
		result->setProperty("decimals", 3);
	}
	
	if(result->metaObject()->indexOfProperty("frame") != -1) {
		result->setProperty("frame", true);
	}
	
	return result;
}

QVariant ParamsForm::getValue(const QPair<QWidget *, StrategyParamSpec> &pair) const {
	return getValue(pair.first, pair.second);
}

QVariant ParamsForm::getValue(const QWidget *editor, const StrategyParamSpec &spec) const {
	return editor->property(m_factory.valuePropertyName(spec.value.type()).constData());
}

void ParamsForm::setValue(QWidget *editor, QVariant value, const StrategyParamSpec &spec) const {
	editor->setProperty(m_factory.valuePropertyName(spec.value.type()).constData(), value);
}
