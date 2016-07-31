#include <QtDebug>

#include "clusteringform.h"
#include "ui_clusteringform.h"
#include "QtGuiException.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"

const char *helpString = "";

ClusteringForm::ClusteringForm(ManagedFuzzyDataController *controller, QWidget *parent) :
    QWidget(parent), 
	controller(controller),
    ui(new Ui::ClusteringForm)
{
    ui->setupUi(this);
	
	filterGroup = new FilterGroup(0, encode("Фильтры"));
	
	ui->verticalLayout_filters->addWidget(filterGroup);
	
	filterGroup->addSlider(encode("Кл. на тр."));
	filterGroup->addSlider(encode("Макс. размер кл."));
//	filterGroup->addSlider(encode("ЦФ1"));
//	filterGroup->addSlider(encode("ЦФ2"));
	
	conditionModel = new ConditionModel(controller, this);
	
	ui->treeView_conditions->setModel(conditionModel);
	
	for(int i = 0; i < conditionModel->columnCount(); i++) {
		ui->treeView_conditions->setColumnWidth(i, 60);
	}
	
	qDebug()<<"Setting filter initial value...";
	
	setFilterInitialValue(0);
	
	qDebug()<<"Connecting...";
	
	connect(filterGroup, SIGNAL(sliderSignaled(int,double,bool)), this, SLOT(filterItems()));
	connect(ui->pushButton_getConditions, SIGNAL(clicked()), this, SIGNAL(clusteringRequested()));
	connect(ui->pushButton_saveSelection, SIGNAL(clicked()), this, SIGNAL(saveCheckedRequested()));
	connect(ui->SpinBox_minDistance, SIGNAL(valueChanged(int)), this, SLOT(clusterize()));
}

void ClusteringForm::addCondition(PCondition condition) {
	conditionModel->addCondition(condition);
}

void ClusteringForm::clusterize() {
	qDebug()<<"entering clusterize()";
	conditionModel->clusterize(
			ui->SpinBox_minDistance->value()
			);
	setupFilters();
	filterItems();
}

void ClusteringForm::filterItems() {
	for(int row = 0; row < conditionModel->rowCount(); row++) {
		ui->treeView_conditions->setRowHidden(
				row,
				QModelIndex(),
				!this->checkItem(row)
				);
	}
}

bool ClusteringForm::checkItem(int row) {
	int colCount = conditionModel->columnCount(
			QModelIndex()
			);
	for(int i = 1; i < colCount; i++) {
		QModelIndex index = conditionModel->index(row, i, QModelIndex());
		if(!filterGroup->testValue(
				i - 1, conditionModel->data(index, Qt::DisplayRole).toDouble()
				)) {
			return false;
		}
	}
	return true;
}

void ClusteringForm::setupFilters() {
	filterGroup->clearAllLimits();
	for(int i = 0; i < conditionModel->rowCount(); i++) {
		for(int j = 1; j < conditionModel->columnCount(); j++) {
			QModelIndex index = conditionModel->index(i, j, QModelIndex());
			bool ok;
			double value = conditionModel->data(index, Qt::DisplayRole).toDouble(&ok);
			if(!ok) {
				throw QtGuiException(QString("Invalid value for index %1").arg(j));
			}
			filterGroup->slider(j - 1)->fitLimits(
					value
					);
		}
	}
}

void ClusteringForm::setFilterInitialValue(double value) {
	for(int j = 1; j < conditionModel->columnCount(); j++) {
		filterGroup->slider(j - 1)->setValue(
				value
				);
	}	
}

void ClusteringForm::clear() {
	conditionModel->clear();
}

int ClusteringForm::conditionsCount() const {
	return conditionModel->conditionCount();
}

const Condition & ClusteringForm::condition(int number) const {
	return conditionModel->condition(number);
}

bool ClusteringForm::conditionChecked(int number) const {
	return conditionModel->conditionChecked(number) 
			&& !ui->treeView_conditions->isRowHidden(number, QModelIndex());
}

//QVariant ClusteringForm::getValueByIndex(const Clusterizer &clusterizer, int index) {
//	switch(index) {
//		case 0: return clusterizer.clustersPerTraj();
//		case 1: return clusterizer.maxSize();
//		case 2: return clusterizer.goal1();
//		case 3: return clusterizer.goal2();
//		default: throw QtGuiException(QString("Unknown value index %1").arg(index));
//	}
//}

ClusteringForm::~ClusteringForm()
{
    delete ui;
}

void ClusteringForm::changeEvent(QEvent *e)
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
