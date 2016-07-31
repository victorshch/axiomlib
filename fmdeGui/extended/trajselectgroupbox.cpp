#include <QDebug>

#include "trajselectgroupbox.h"
#include "ui_trajselectgroupbox.h"
#include "utilities.h"
#include "QtGuiException.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"

TrajSelectGroupBox::TrajSelectGroupBox(QWidget *parent, 
									   ManagedFuzzyDataController *controller, 
									   bool showDimension, 
									   bool showTime) :
    QGroupBox(parent),
	showTime(showTime),
    ui(new Ui::TrajSelectGroupBox)
{
    ui->setupUi(this);
	
	ui->frame_dimension->setVisible(showDimension);
	
	paramNames = controller->paramNames();
	
	fillTSSizes(controller);
	
	qDebug()<<"Num classes: "<<numClasses();
	
	wireUp();
	
	fillClassComboBox();
}

void TrajSelectGroupBox::wireUp() {
	connect(ui->comboBox_class, SIGNAL(currentIndexChanged(int)), this, SLOT(classSelected(int)));
	connect(ui->comboBox_ts, SIGNAL(currentIndexChanged(int)), this, SLOT(tsSelected(int)));
	if(showTime) {
		connect(ui->comboBox_dimension, SIGNAL(currentIndexChanged(int)), this, SLOT(dimensionSelected(int)));
	}
}

void TrajSelectGroupBox::unWire() {
	ui->comboBox_class->disconnect(this);
	ui->comboBox_ts->disconnect(this);
	if(showTime) {
		ui->comboBox_dimension->disconnect(this);
	}
}

void TrajSelectGroupBox::fillTSSizes(ManagedFuzzyDataController *controller) {
	AxiomLib::FuzzyDataSet &dataSet = controller->fuzzyMultiDataLearnAlgorithm.getDataSet();
	
	std::vector<int> abnormalMultiTSSizes;
	int abnormalClassesN;
	
	dataSet.getClassSize(abnormalClassesN, abnormalMultiTSSizes, abnormalTSSizes);
	dataSet.getNormalClassSize(abnormalClassesN, normalTSSizes);
}

int TrajSelectGroupBox::numClasses() {
	return abnormalTSSizes.size();
}

int TrajSelectGroupBox::numTS(int classNo) {
	if(classNo >= 0 && classNo < numClasses()) {
		return abnormalTSSizes[classNo].size();
	} else if(classNo == -1) {
		return normalTSSizes.size();
	} else {
		throw QtGuiException("Unexpected class number "+QString::number(classNo));
	}
}

int TrajSelectGroupBox::numDimensions(int classNo, int tsNo) {
	if(classNo < 0) {
		return normalTSSizes[tsNo];
	} else {
		return abnormalTSSizes[classNo][tsNo];
	}
}

void TrajSelectGroupBox::fillClassComboBox() {
	//unWire();
	ui->comboBox_class->clear();
	ui->comboBox_class->addItem(encode("Нормальн."), QVariant::fromValue<int>(-1));
	for(int i = 0; i < numClasses(); i++) {
		ui->comboBox_class->addItem(QString::number(i+1), QVariant::fromValue(i));
	}
	//wireUp();
	ui->comboBox_class->setCurrentIndex(0);
}

void TrajSelectGroupBox::fillTSComboBox(int classNo) {
	unWire();
	qDebug()<<"clearing";
	ui->comboBox_ts->clear();
	qDebug()<<"adding items";
	qDebug()<<"number of TS:"<<numTS(classNo);
	for(int i = 0; i < numTS(classNo); i++) {
		ui->comboBox_ts->addItem(QString::number(i+1), QVariant::fromValue(i));
	}
	wireUp();
	qDebug()<<"setting current index";
	ui->comboBox_ts->setCurrentIndex(0);
}

void TrajSelectGroupBox::fillDimensionComboBox(int classNo, int tsNo) {
	unWire();
	ui->comboBox_dimension->clear();
	int start = showTime ? 0 : 1;
	for(int i = start; i < numDimensions(classNo, tsNo); i++) {
		ui->comboBox_dimension->addItem(
				QString::fromStdString(paramNames[i]),
				QVariant::fromValue(i)
				);
	}
	wireUp();
	ui->comboBox_dimension->setCurrentIndex(0);
}

int getCurrentComboBoxData(QComboBox *comboBox) {
	return comboBox->itemData(
			comboBox->currentIndex(),
			Qt::UserRole
			).toInt();
}

void setIndexByData(QComboBox *comboBox, int data) {
	int index = comboBox->findData(
			QVariant::fromValue(data),
			Qt::UserRole
			);
	comboBox->setCurrentIndex(index);
}

//signal is sent only once
TrajSelectGroupBox *TrajSelectGroupBox::setCurrentTS(int currentClass, int currentTS, int currentDimension) {
	unWire();
	setIndexByData(ui->comboBox_class, currentClass);
	setIndexByData(ui->comboBox_ts, currentTS);
	wireUp();
	setIndexByData(ui->comboBox_dimension, currentDimension);
	return this;
}

int TrajSelectGroupBox::currentClass() {
	return getCurrentComboBoxData(ui->comboBox_class);
}

int TrajSelectGroupBox::currentTS() {
	return getCurrentComboBoxData(ui->comboBox_ts);
}

int TrajSelectGroupBox::currentDimension() {
	return getCurrentComboBoxData(ui->comboBox_dimension);
}

void TrajSelectGroupBox::classSelected(int /*index*/) {
	qDebug()<<"filling ts combo box";
	fillTSComboBox(currentClass());
	qDebug()<<"current class: "<<currentClass();
	qDebug()<<"current ts: "<<currentTS();
	qDebug()<<"current dim: "<<currentDimension();
	dumpObjectInfo();
	qDebug()<<"emitting";
	emit trajSelected(currentClass(), currentTS(), currentDimension());
}

void TrajSelectGroupBox::tsSelected(int /*index*/) {
	fillDimensionComboBox(currentClass(), currentDimension());
	emit trajSelected(currentClass(), currentTS(), currentDimension());
}

void TrajSelectGroupBox::dimensionSelected(int /*index*/) {
	emit trajSelected(currentClass(), currentTS(), currentDimension());
}

void TrajSelectGroupBox::changeEvent(QEvent *e)
{
    QGroupBox::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

TrajSelectGroupBox::~TrajSelectGroupBox() {
	
}
