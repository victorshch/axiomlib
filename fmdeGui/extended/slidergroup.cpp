#include "slidergroup.h"
#include "ui_slidergroup.h"
#include "utilities.h"

#include <QtDebug>

const double SliderGroup::sliderStep = 0.0001;
const double SliderGroup::plusInfinity = 100000;
const double SliderGroup::minusInfinity = -100000;

SliderGroup::SliderGroup(QWidget *parent, QButtonGroup *buttonGroup, const QString &name) :
    QFrame(parent),
    ui(new Ui::SliderGroup),
	limitsSet(false)
{
    ui->setupUi(this);
	
	ui->checkBox_name->setText(name);
	
	settingValue = false;
	
	buttonGroup->addButton(ui->checkBox_name);
	
	ui->Slider_value->setOrientation(Qt::Horizontal);
	ui->Slider_value->setRange(0, 1, sliderStep);
	ui->Slider_value->setScalePosition(QwtSlider::BottomScale);
	ui->Slider_value->setValue(0.5);
	
	QFont font = ui->Slider_value->font();
	
	font.setPointSize(fontSize);
	
	ui->Slider_value->setFont(
			font
			);
	
	
	ui->doubleSpinBox_value->setDecimals(spinBoxDecimals);
	ui->doubleSpinBox_value->setSingleStep(sliderStep);
	ui->doubleSpinBox_value->setMinimum(0);
	ui->doubleSpinBox_value->setMaximum(1);
	ui->doubleSpinBox_value->setValue(0.5);
	
	setDirection(true);
	
	defaultValue = 0.5;
	defaultDirection = false;
	
	//toggleEnabled(false);
	
	connect(ui->radioButton_greater, SIGNAL(toggled(bool)), this, SIGNAL(valueChanged()));
	//connect(ui->radioButton_name, SIGNAL(toggled(bool)), this, SLOT(toggleEnabled(bool)));
	connect(ui->doubleSpinBox_value, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
	connect(ui->Slider_value, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
	connect(ui->checkBox_name, SIGNAL(toggled(bool)), this, SIGNAL(valueChanged()));
}

SliderGroup* SliderGroup::setLimits(double minValue, double maxValue) {
	if(minValue < minusInfinity || minValue > plusInfinity) {
		minValue = this->minValue();
	}
	if(maxValue > plusInfinity || maxValue < minusInfinity) {
		maxValue = this->maxValue();
	}
	qDebug()<<"Setting range: "<<minValue<<" "<<maxValue;
	
	ui->Slider_value->setRange(minValue, maxValue, sliderStep);
	
	ui->doubleSpinBox_value->setMinimum(minValue);
	ui->doubleSpinBox_value->setMaximum(maxValue);
	limitsSet = true;
	qDebug()<<"Range set";
	return this;
}

SliderGroup *SliderGroup::fitLimits(double value) {
	if(!limitsSet) {
		setLimits(value, value);
	} else if(value < minValue()) {
		setLimits(value, maxValue());
	} else if(value > maxValue()) {
		setLimits(minValue(), value);
	}
	return this;
}

SliderGroup *SliderGroup::clearLimits() {
	setLimits(0, 0);
	limitsSet = false;
	return this;
}

double SliderGroup::value() {
	return ui->doubleSpinBox_value->value();
}

double SliderGroup::minValue() {
	return ui->doubleSpinBox_value->minimum();
}

double SliderGroup::maxValue() {
	return ui->doubleSpinBox_value->maximum();
}

SliderGroup* SliderGroup::setValue(double value) {
	if(settingValue) {
		return this;
	}
	
	settingValue = true;
	
	qDebug()<<"setting spin box value";
	ui->doubleSpinBox_value->setValue(value);
	qDebug()<<"setting slider value";
	ui->Slider_value->setValue(value);
	qDebug()<<"all values set";
	
	settingValue = false;
	
	emit valueChanged();
	
	qDebug()<<"exiting SliderGroup::setValue()";
	return this;
}

bool SliderGroup::checked() {
	return ui->checkBox_name->isChecked();
}

SliderGroup* SliderGroup::setChecked(bool checked) {
	ui->checkBox_name->setChecked(checked);
	return this;
}

bool SliderGroup::direction() {
	return ui->radioButton_greater->isChecked();
}

SliderGroup* SliderGroup::setDirection(bool direction) {
	if(direction) {
		ui->radioButton_greater->setChecked(true);
	} else {
		ui->radioButton_less->setChecked(true);
	}
	emit valueChanged();
	return this;
}

void SliderGroup::toggleEnabled(bool enabled) {
	if(!enabled) 
	{
		setValue(defaultValue);
		setDirection(defaultDirection);
	}
	
	ui->Slider_value->setEnabled(enabled);
	ui->doubleSpinBox_value->setEnabled(enabled);
	ui->radioButton_greater->setEnabled(enabled);
	ui->radioButton_less->setEnabled(enabled);
}

SliderGroup *SliderGroup::setDefaults(double value, bool direction)
{
	defaultValue = value;
	defaultDirection = direction;
	return this;
}

SliderGroup *SliderGroup::setDefaultValue(double value)
{
	defaultValue = value;
	return this;
}

SliderGroup *SliderGroup::setDerfaultDirection(bool direction)
{
	defaultDirection = direction;
	return this;
}

SliderGroup *SliderGroup::toDefault()
{
	qDebug()<<"setting default value";
	setValue(defaultValue);
	qDebug()<<"setting default direction";
	setDirection(defaultDirection);
	return this;
}

bool SliderGroup::testValue(double val) {
	if(!checked()) {
		return true;
	} else {
		if(direction()) {
			return val >= this->value();
		} else {
			return val <= this->value();
		}
	}
}

SliderGroup::~SliderGroup()
{
    delete ui;
}

void SliderGroup::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
