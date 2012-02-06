#include <QDebug>

#include "intervalwidget.h"
#include "ui_intervalwidget.h"

IntervalWidget::IntervalWidget(int min, int max, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IntervalWidget)
{
	qDebug()<<"entering IntervalWidget::IntervalWidget()";
	qDebug()<<"min = "<<min<<" max = "<<max;
    ui->setupUi(this);
	
	ui->Slider_left->setOrientation(Qt::Horizontal);
	ui->Slider_left->setRange(min, max, 1);
	ui->Slider_left->setValue(min);
	ui->Slider_left->setScalePosition(QwtSlider::NoScale);
	
	ui->Slider_right->setOrientation(Qt::Horizontal);
	ui->Slider_right->setRange(min, max, 1);
	ui->Slider_right->setValue(max);
	ui->Slider_right->setScalePosition(QwtSlider::NoScale);
	
	settingValue = false;
	
	connect(ui->Slider_left, SIGNAL(valueChanged(double)), this, SLOT(leftValueChanged(double)));
	connect(ui->Slider_right, SIGNAL(valueChanged(double)), this, SLOT(rightValueChanged(double)));
	qDebug()<<"exiting IntervalWidget::IntervalWidget()";
}

IntervalWidget::~IntervalWidget()
{
    delete ui;
}

void IntervalWidget::setInterval(int left, int right) {
	qDebug()<<"Entering IntervalWidget::setInterval() "<<left<<" "<<right;
	qDebug()<<"min: "<<min()<<"max: "<<max();
	if(right - left < delta) {
		if(max() - right < delta) {
			left = right - delta;
		} else if (left - min() < delta) {
			right = left + delta;
		} else {
			right = (right + left + delta) / 2 + 1;
			left = (right + left - delta) / 2 - 1;
		}
	}
	settingValue = true;
	qDebug()<<"Setting slider values : "<<left<<" "<<right;
	ui->Slider_left->setValue(left);
	ui->Slider_right->setValue(right);
	qDebug()<<"Slider values set";
	settingValue = false;
	
	emit intervalChanged(left, right);
	qDebug()<<"Leaving IntervalWidget::setInterval()";
}

void IntervalWidget::setRange(int min, int max) {
	settingValue = true;
	ui->Slider_left->setRange(min, max, 1);
	ui->Slider_right->setRange(min, max, 1);
	settingValue = false;
}

int IntervalWidget::left() const {
	return ui->Slider_left->value();
}

int IntervalWidget::right() const {
	return ui->Slider_right->value();
}

int IntervalWidget::min() const {
	return ui->Slider_left->minValue();
}

int IntervalWidget::max() const {
	return ui->Slider_left->maxValue();
}

void IntervalWidget::leftValueChanged(double newValue) {
	if(!settingValue) {
		setInterval(newValue, right());
	}
}

void IntervalWidget::rightValueChanged(double newValue) {
	if(!settingValue) {
		setInterval(left(), newValue);
	}
}

void IntervalWidget::changeEvent(QEvent *e)
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
