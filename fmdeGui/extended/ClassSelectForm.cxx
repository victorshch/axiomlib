#include "Common.h"

#include "utilities.h"
#include "QtGuiException.h"

#include "ClassSelectForm.h"
#include "ui_ClassSelectForm.h"

ClassSelectForm::ClassSelectForm(const std::vector<std::string> &classNames, const std::string &paramName, QWidget *parent) :
    AbstractParamsForm(parent),
    ui(new Ui::ClassSelectForm),
    m_classNames(classNames),
    m_paramName(paramName)
{	
    ui->setupUi(this);
	
	ui->comboBox_abnormalType->addItem(encode("Все"), QVariant::fromValue(-1));
	for(int i = 0; i < classNames.size(); ++i) {
		ui->comboBox_abnormalType->addItem(QString::fromStdString(classNames[i]), QVariant::fromValue(i));
	}
	
	connect(ui->comboBox_abnormalType, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionChanged(int)));
	
	ui->comboBox_abnormalType->setCurrentIndex(0);
}

std::list<std::string> ClassSelectForm::paramNames() const {
	std::list<std::string> result;
	result.push_back(m_paramName);
	return result;
}

std::string ClassSelectForm::strValue(const std::string &paramName) const {
	if(paramName == m_paramName) {
		if(currentClass() == -1) {
			return "";
		} else {
			return className(currentClass()).toStdString();
		}
	} else {
		return "";
	}
}

void ClassSelectForm::setStrValue(const std::string &paramName, const std::string &newValue) {
	if(paramName == m_paramName) {
		int index = AxiomLib::isIn(newValue, m_classNames);
		if(index >= 0) {
			setCurrentClass(index);
		} else {
			setCurrentClass(-1);
		}
	}
}

int ClassSelectForm::currentClass() const {
	QVariant data = ui->comboBox_abnormalType->itemData(
	            ui->comboBox_abnormalType->currentIndex()
	            );
	if(!data.canConvert<int>()) {
		throw QtGuiException("ClassSelectForm::currentClass(): incorrect data in combobox");
	}
	return data.toInt();
}

void ClassSelectForm::setCurrentClass(int newClass) {
	int index = ui->comboBox_abnormalType->findData(QVariant::fromValue(newClass));
	if(index >= 0) {
		ui->comboBox_abnormalType->setCurrentIndex(index);
	}
}

int ClassSelectForm::classCount() const {
	return ui->comboBox_abnormalType->count() - 1;
}

QString ClassSelectForm::className(int i) const {
	return ui->comboBox_abnormalType->itemText(i + 1);
}

void ClassSelectForm::setTitle(const QString &newTitle) {
	ui->label_title->setText(newTitle);
}

void ClassSelectForm::selectionChanged(int newIndex) {
	QVariant data = ui->comboBox_abnormalType->itemData(newIndex);
	bool ok;
	int newClassNo = data.toInt(&ok);
	if(!ok) {
		throw QtGuiException("ClassSelectForm::selectionChanged(): incorrect data in combobox");
	}
	emit classSelected(newClassNo);
}

ClassSelectForm::~ClassSelectForm()
{
    delete ui;
}
