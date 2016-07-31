#ifndef SLIDERGROUP_H
#define SLIDERGROUP_H

#include <QFrame>
#include <QButtonGroup>

namespace Ui {
    class SliderGroup;
}

class SliderGroup : public QFrame {
    Q_OBJECT
public:
    SliderGroup(QWidget *parent, QButtonGroup *buttonGroup, const QString &name);
    ~SliderGroup();
	
	//void init(QButtonGroup *buttonGroup, int id);
	
	SliderGroup* setLimits(double minValue, double maxValue);
	SliderGroup *fitLimits(double value);
	SliderGroup *clearLimits();
	
	double value();
	double minValue();
	double maxValue();
	
	// false - меньше (т.е. отсекаем все, что больше), true - больше
	bool direction();
	
	bool checked();
	
	SliderGroup *setDefaults(double value, bool direction);
	SliderGroup *setDefaultValue(double value);
	SliderGroup *setDerfaultDirection(bool direction);
	
	bool testValue(double value);
	
public slots:
	SliderGroup* setValue(double value);
	SliderGroup* setDirection(bool direction);
	SliderGroup* setChecked(bool checked);
	SliderGroup* toDefault();
signals:
	// сигналит, когда изменяется значение или направление отсечения
	void valueChanged();

protected:
    void changeEvent(QEvent *e);

private:
	Ui::SliderGroup *ui;
	
	static const double sliderStep;
	static const double plusInfinity;
	static const double minusInfinity;
	static const int spinBoxDecimals = 4;
	static const int fontSize = 7;
	
	bool settingValue;
	
	bool limitsSet;
	
	double defaultValue;
	bool defaultDirection;
	
private slots:
	void toggleEnabled(bool enabled);
};

#endif // SLIDERGROUP_H
