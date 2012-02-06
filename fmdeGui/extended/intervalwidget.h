#ifndef INTERVALWIDGET_H
#define INTERVALWIDGET_H

#include <QWidget>

namespace Ui {
    class IntervalWidget;
}

class IntervalWidget : public QWidget {
    Q_OBJECT
public:
    IntervalWidget(int min, int max, QWidget *parent = 0);
    ~IntervalWidget();
	
	void setInterval(int left, int right);
	void setRange(int min, int max);
	
	int left() const;
	int right() const;
	
	int min() const;
	int max() const;
	
	static const int delta = 4;
	
signals:
	void intervalChanged(int left, int right);
	
protected:
    void changeEvent(QEvent *e);

private:
    Ui::IntervalWidget *ui;
	
	bool settingValue;
	
private slots:
	void leftValueChanged(double newValue);
	void rightValueChanged(double newValue);
};

#endif // INTERVALWIDGET_H
