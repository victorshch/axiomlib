#include "filtergroup.h"

FilterGroup::FilterGroup(QWidget *parent, const QString &name) :
    QGroupBox(parent)
{
	this->setTitle(name);
	
	topLevelLayout = new QVBoxLayout();
	this->setLayout(topLevelLayout);
	buttonGroup = new QButtonGroup(this);
	buttonGroup->setExclusive(false);
	
	signalMapper = new QSignalMapper(this);
	
	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(sliderSignal(int)));
}

SliderGroup *FilterGroup::addSlider(const QString &name)
{
	SliderGroup *newSlider = new SliderGroup(0, buttonGroup, name);
	topLevelLayout->addWidget(newSlider);
	sliderGroups.append(newSlider);
	
	signalMapper->setMapping(newSlider, sliderGroups.count() - 1);
	connect(newSlider, SIGNAL(valueChanged()), signalMapper, SLOT(map()));
	
	return newSlider;
}

SliderGroup *FilterGroup::slider(int index) 
{
	return sliderGroups[index];
}

bool FilterGroup::testValue(int index, double value) {
	return this->slider(index)->testValue(value);
}

FilterGroup *FilterGroup::clearAllLimits() {
	for(int i = 0; i < sliderGroups.size(); i++) {
		sliderGroups[i]->clearLimits();
	}
	return this;
}

void FilterGroup::sliderSignal(int id) {
	emit sliderSignaled(id, slider(id)->value(), slider(id)->direction());
}
