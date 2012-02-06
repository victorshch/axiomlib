#ifndef HEURISTICSFORM_H
#define HEURISTICSFORM_H

#include <QWidget>

#include "AbstractParamsForm.h"
#include "ParamsForm.h"

namespace Ui {
    class HeuristicsForm;
}

class HeuristicsForm : public AbstractParamsForm
{
    Q_OBJECT
	
public:
    HeuristicsForm(const std::string& paramName, const std::string& name, const QString& title, QWidget* parent = 0);
	~HeuristicsForm();
	
	std::string strValue(const std::string &paramName) const;
	void setStrValue(const std::string &paramName, const std::string &newValue);
	
	std::list<std::string> paramNames() const;
	
	void addParam(const StrategyParamSpec& spec, const QString& paramTitle);
public slots:
	void enable();
	void disable();
signals:
	void executed();

private:
    Ui::HeuristicsForm *ui;
	
	ParamsForm* paramsForm;
	
	std::string m_paramName;
	std::string m_name;
	
	void enable(bool value);
};

#endif // HEURISTICSFORM_H
