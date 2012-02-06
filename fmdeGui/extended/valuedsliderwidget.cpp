#include "valuedsliderwidget.h"
#include "ui_valuedsliderwidget.h"

ValuedSliderWidget::ValuedSliderWidget(bool track, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ValuedSliderWidget)
{
    ui->setupUi(this);
	
	ui->Slider_value->setTracking(track);
	
	settingValue = false;
	
	connect(ui->doubleSpinBox_value, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
	connect(ui->Slider_value, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
}

void ValuedSliderWidget::setInterval(double min, double max, double step, int digits) {
	settingValue = true;
	ui->Slider_value->setRange(min, max, step);
	settingValue = false;
	ui->doubleSpinBox_value->setRange(min, max);
	ui->doubleSpinBox_value->setSingleStep(step);
	ui->doubleSpinBox_value->setDecimals(digits);
}

double ValuedSliderWidget::value() const {
	return ui->doubleSpinBox_value->value();
}

void ValuedSliderWidget::setValue(double newValue) {
	if(!settingValue) {
		settingValue = true;
		ui->doubleSpinBox_value->setValue(newValue);
		ui->Slider_value->setValue(newValue);
		settingValue = false;
		emit valueChanged(newValue);
	}
}

ValuedSliderWidget::~ValuedSliderWidget()
{
    delete ui;
}

void ValuedSliderWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
