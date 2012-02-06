#ifndef VALUEDSLIDERWIDGET_H
#define VALUEDSLIDERWIDGET_H

#include <QWidget>

namespace Ui {
    class ValuedSliderWidget;
}

class ValuedSliderWidget : public QWidget {
    Q_OBJECT
public:
    ValuedSliderWidget(bool track = false, QWidget *parent = 0);
    ~ValuedSliderWidget();
	
	void setInterval(double min, double max, double step = 0.01, int decimals = 2);
	
	double value() const;
public slots:
	void setValue(double newValue);
signals:
	void valueChanged(double newValue);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ValuedSliderWidget *ui;
	
	bool settingValue;	
};

#endif // VALUEDSLIDERWIDGET_H
