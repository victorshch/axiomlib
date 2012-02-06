#ifndef ABSTRACTPARAMSFORM_H
#define ABSTRACTPARAMSFORM_H

#include <list>
#include <string>

#include <QWidget>

class AbstractParamsForm : public QWidget
{
	Q_OBJECT
public:
    AbstractParamsForm(QWidget* parent = 0);
	
	virtual std::list<std::string> paramNames() const = 0;
	virtual std::string strValue(const std::string& paramName) const = 0;
	virtual void setStrValue(const std::string& paramName, const std::string& newValue) = 0;
};

#endif // ABSTRACTPARAMSFORM_H
