#ifndef CLASSSELECTFORM_H
#define CLASSSELECTFORM_H

#include <string>
#include <vector>


#include <QWidget>

#include "AbstractParamsForm.h"

namespace Ui {
    class ClassSelectForm;
}

class ClassSelectForm : public AbstractParamsForm
{
    Q_OBJECT

public:
    explicit ClassSelectForm(const std::vector<std::string>& classNames, const std::string& paramName, QWidget *parent = 0);
    ~ClassSelectForm();
	
	virtual std::list<std::string> paramNames() const;
	virtual std::string strValue(const std::string& paramName) const;
	virtual void setStrValue(const std::string& paramName, const std::string& newValue);	
	
	int currentClass() const;
	void setCurrentClass(int newClass);
	
	int classCount() const;
	QString className(int i) const;
	
	void setTitle(const QString& newTitle);
signals:
	void classSelected(int classNo);
private:
    Ui::ClassSelectForm *ui;
	
	const std::string m_paramName;
	const std::vector<std::string> m_classNames;
private slots:
	void selectionChanged(int newIndex);
};

#endif // CLASSSELECTFORM_H
