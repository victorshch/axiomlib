#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QSignalMapper>

#include "slidergroup.h"

class FilterGroup : public QGroupBox
{
Q_OBJECT
public:
    explicit FilterGroup(QWidget *parent, const QString &name);

	SliderGroup *addSlider(const QString &name);
	
	SliderGroup *slider(int index);
	
	bool testValue(int index, double value);
	
	FilterGroup *clearAllLimits();
signals:
	void sliderSignaled(int id, double value, bool direction);
	
public slots:

private:
	QList<SliderGroup *> sliderGroups;
	
	QVBoxLayout *topLevelLayout;
	
	QButtonGroup *buttonGroup;
	
	QSignalMapper *signalMapper;
	
private slots:
	void sliderSignal(int id);
};

#endif // FILTERGROUP_H
