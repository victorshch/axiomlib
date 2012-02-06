#ifndef PARAMSFORM_H
#define PARAMSFORM_H

#include <string>
#include "StdStringHash.h"

#include <QHash>
#include <QWidget>
#include <QItemEditorFactory>

#include "AbstractParamsForm.h"

namespace Ui {
    class ParamsForm;
}

class StrategyParamSpec {
	
	//friend class ParamsForm;
public:	
	
	std::string paramName;
	QVariant min, value, max, step;
	
	StrategyParamSpec();
	
	template<class T>
	StrategyParamSpec(T min, T max, T step, T value, const std::string& paramName);
};

template<class T>
StrategyParamSpec::StrategyParamSpec(T min, T max, T step, T value, const std::string& paramName) {
	this->min = QVariant::fromValue(min);
	this->max = QVariant::fromValue(max);
	this->step = QVariant::fromValue(step);
	this->value = QVariant::fromValue(value);
	this->paramName = paramName;
}


class ParamsForm : public AbstractParamsForm
{
    Q_OBJECT

public:
    explicit ParamsForm(QWidget *parent = 0);
	void addParam(const StrategyParamSpec& paramSpec, const QString& paramTitle);
    ~ParamsForm();

	QVariant value(const std::string& paramName) const;
	std::string strValue(const std::string& paramName) const;
	
	void setValue(const std::string& paramName, QVariant newValue);
	void setStrValue(const std::string& paramName, const std::string& newValue);
	
	std::list<std::string> paramNames() const;
	
private:
    Ui::ParamsForm *ui;
	
	QItemEditorFactory m_factory;
	
        QHash<QString, QPair<QWidget*, StrategyParamSpec> > m_params;
	
	QWidget* createEditor(const StrategyParamSpec& spec) const;
	QVariant getValue(const QWidget* editor, const StrategyParamSpec& spec) const;
	QVariant getValue(const QPair<QWidget*, StrategyParamSpec>& pair) const;
	void setValue(QWidget* editor, QVariant value, const StrategyParamSpec& spec) const;	
};

#endif // PARAMSFORM_H
