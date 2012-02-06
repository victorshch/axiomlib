#include "extecbrowser.h"
#include "ElemConditionsFactory.h"

#include <iterator>
#include <stdexcept>
#include <QStringList>
#include <QDebug>
#include "relaxed_transform.h"
#include "QtGuiException.h"

ExtECBrowser::ExtECBrowser(QWidget *parent, ManagedFuzzyDataController *controller,
						   AxiomLib::ElemCondPlus ecPlus, 
						   const QString &concreteECName,
						   const QObject *identifier)
    : QWidget(parent), ecPlus(ecPlus), controller(controller)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	
	ui.setupUi(this);

	this->identifier = identifier;
	
	qDebug()<<"ui set up";
	
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	
	this->concreteECName = concreteECName;
	
	this->setWindowTitle(concreteECName + " - EC editor");

	topLevelLayout = new QVBoxLayout();
	
	QFormLayout *persistentParamsLayout = new QFormLayout();
	
	//dimensionSpinBox = new QSpinBox;
	comboBox_dimension = new QComboBox();
	
	QStringList dimensionNames;
	
	std::vector<std::string> paramNames = controller->paramNames();
	
	relaxed_transform(paramNames.begin() + 1, paramNames.end(),
				   std::back_inserter(dimensionNames),
				   QString::fromStdString);
	
	for(int i = 0; i < dimensionNames.size(); i++) {
		comboBox_dimension->addItem(dimensionNames[i], QVariant::fromValue(i + 1));
	}
	
	persistentParamsLayout->addRow("dimension", comboBox_dimension);
	
	signCheckBox = new QCheckBox;
	persistentParamsLayout->addRow("sign", signCheckBox);	
	
	ecParamsLayout = new QFormLayout();

	comboBox_ecNames = new QComboBox();
	
	qDebug()<<"Populating ECNames...";
	
	this->populateECNames();
	
	qDebug()<<"EC names populated";
	
	connect(comboBox_ecNames, SIGNAL(currentIndexChanged(QString)), 
			this, SLOT(ecNameChanged(QString)));
	
	ecMap.insert(
			std::make_pair(
					ecPlus.elemCondition->name(),
					ECPointer(new AxiomLib::ElemCondPlus(ecPlus))
					)
			);
	
	qDebug()<<"Map constructed";

	oldECName = ecPlus.elemCondition->name().c_str();	
	displayEC(ecPlus);
	setCurrentECName(ecPlus.elemCondition->name().c_str());
	
	qDebug()<<"Current EC name set";
	
	topLevelLayout->addWidget(comboBox_ecNames);
	
	topLevelLayout->addLayout(persistentParamsLayout);
		
	topLevelLayout->addLayout(ecParamsLayout);
	
	qDebug()<<"Creating buttons";
	
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	
	QPushButton *pushButton_ok = new QPushButton("OK", 0);
	QPushButton *pushButton_cancel = new QPushButton("Cancel", 0);
	
	buttonLayout->addWidget(pushButton_ok);
	buttonLayout->addWidget(pushButton_cancel);

	topLevelLayout->addLayout(buttonLayout);
	
	qDebug()<<"Connecting";
	
	connect(pushButton_cancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(pushButton_ok, SIGNAL(clicked()), this, SLOT(saveEC()));
	
	qDebug()<<"Setting layout";
	
	setLayout(topLevelLayout);
	
	qDebug()<<"Constructor exited";
}

ECPointer ExtECBrowser::getECByName(const QString &name) {
	std::string ecName = name.toStdString();
	ECMap::iterator it = ecMap.find(ecName);
	
	ECPointer ecPointer;
	
	if(it != ecMap.end()) {
		ecPointer = it->second;
	} else {
		AxiomLib::ElemConditionsFactory factory;
		AxiomLib::ElemCondPlus *pEcPlus = new AxiomLib::ElemCondPlus;
		pEcPlus->sign = true;
		pEcPlus->dimension = 1;
		pEcPlus->elemCondition = factory.createAxiomByName(ecName.c_str());
		ecPointer = ECPointer(pEcPlus);
		ecMap.insert(
				std::make_pair(ecName, ecPointer)
				);
	}
	return ecPointer;
}

void ExtECBrowser::populateECNames() {
	AxiomLib::ElemConditionsFactory factory;
	std::list<std::string> ecNamesList;
	
	//qDebug()<<"Getting ECNames...";
	
	factory.getECNames(std::back_inserter(ecNamesList));
	
	relaxed_transform(ecNamesList.begin(),
				   ecNamesList.end(),
				   std::back_inserter(ecNames),
				   QString::fromStdString);
	
	comboBox_ecNames->addItems(ecNames);	
}

void ExtECBrowser::setCurrentECName(const QString &currentECName) {
	int index = ecNames.indexOf(
			QRegExp(currentECName, Qt::CaseSensitive, QRegExp::FixedString)
			);
	if(index < 0) {
		throw new std::logic_error(
				("ExtECBrowser::populateECNames(): EC name not found: " + currentECName)
				.toStdString()
				);
	}
	comboBox_ecNames->setCurrentIndex(index);
	
}

void ExtECBrowser::displayEC(const AxiomLib::ElemCondPlus &ecPlus) {
	qDebug()<<"entering displayEC()";
	
	if(ecParamsLayout->count() > 0) {
		qDebug()<<"clearing layout...";
		QLayoutItem *child;
		while ((child = ecParamsLayout->takeAt(0)) != 0) {
			QWidget *widget = child->widget();
			if(widget != 0) {
				widget->hide();	
			}
			delete child;
		}
		qDebug()<<"layout cleared";
	}

	//dimensionSpinBox->setValue(ecPlus.dimension);
	qDebug()<<"dimension: "<<ecPlus.dimension;
	comboBox_dimension->setCurrentIndex(
			comboBox_dimension->findData(ecPlus.dimension)
			);

	signCheckBox->setChecked(ecPlus.sign);
	
	std::vector<std::string> paramNames;
	
	ecPlus.elemCondition->getECParamNames(paramNames);
	
	paramDoubleSpinBoxes.clear();
	
	for(int i = 0; i < (int)paramNames.size(); i++) {
		double value;
		ecPlus.elemCondition->getParamValue(value, paramNames[i]);
		
		QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
		doubleSpinBox->setMaximum(100);
		doubleSpinBox->setMinimum(-100);
		doubleSpinBox->setDecimals(2);
		doubleSpinBox->setValue(value);
		
		ecParamsLayout->addRow(paramNames[i].c_str(), doubleSpinBox);
		
		paramDoubleSpinBoxes.append(doubleSpinBox);
	}
	qDebug()<<"exiting displayEC()";
}

void ExtECBrowser::commitEC(AxiomLib::ElemCondPlus &ecPlus) {
	//ecPlus.dimension = dimensionSpinBox->value();
	qDebug()<<"entering commitEC";
	
	qDebug()<<"getting dim and sign";
	ecPlus.dimension = comboBox_dimension->itemData(
			comboBox_dimension->currentIndex(),
			Qt::UserRole
			).toInt();
	
	ecPlus.sign = signCheckBox->checkState() == Qt::Checked;

	std::vector<std::string> paramNames;
	
	qDebug()<<"getting param names";
	
	ecPlus.elemCondition->getECParamNames(paramNames);

	if(paramDoubleSpinBoxes.size() != paramNames.size()) {
		throw std::runtime_error("ExtECBrowser::commitEC(): name and value vector sized don't match :"
							 +boost::lexical_cast<std::string>(paramNames.size())
							 + " " + boost::lexical_cast<std::string>(paramDoubleSpinBoxes.size()));
	}
	
	qDebug()<<"copying parameters";
	
	for(int i = 0; i < paramDoubleSpinBoxes.size(); i++) {
		ecPlus.elemCondition->setParamValue(
					paramDoubleSpinBoxes[i]->value(),
					paramNames[i]
				);
	}
	qDebug()<<"exited commitEC";
}

void ExtECBrowser::ecNameChanged(const QString &newECName) {
	qDebug()<<"Entering ecNameChanged()";
	
	std::string ecName = newECName.toStdString();
	
	qDebug()<<"Getting old EC";
	
	AxiomLib::ElemCondPlus &oldEC = *(getECByName(oldECName));
	
	qDebug()<<"Commiting old EC";
	
	commitEC(oldEC);
	
	oldECName = newECName;
		
	qDebug()<<"Displaying EC "<<newECName;
	
	displayEC(*getECByName(newECName));
	
	qDebug()<<"Exiting ecNameChanged()";
}

void ExtECBrowser::saveEC() {	
	QString currentECName = comboBox_ecNames->currentText();
	
	ECMap::iterator it = ecMap.find(currentECName.toStdString());
	
	if(it == ecMap.end()) {
		throw QtGuiException(
				QString("ExtECBrowser::saveEC(): ec name %1 not found in ecMap")
				.arg(currentECName)
				.toStdString()
				);
	}
	
	commitEC(*it->second);
	
	ECPointer currentEC = it->second;
	
	emit ECChanged(*currentEC, identifier);
	
	close();
}

ExtECBrowser::~ExtECBrowser()
{

}
