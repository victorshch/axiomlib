#ifndef EXTECBROWSER_H
#define EXTECBROWSER_H

#include <QtGui/QWidget>
#include "ui_extecbrowser.h"
#include "ElemCondPlus.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "algorithm_controller/ManagedFuzzyDataController.h"

typedef boost::shared_ptr<AxiomLib::ElemCondPlus> ECPointer;

class ExtECBrowser : public QWidget
{
    Q_OBJECT

public:
    ExtECBrowser(QWidget *parent, ManagedFuzzyDataController *controller, 
				 AxiomLib::ElemCondPlus ecPlus, const QString &ecName,
				 const QObject *identifier);
    ~ExtECBrowser();

signals:
	void ECChanged(AxiomLib::ElemCondPlus ecPlus, const QObject *identifier);
	
private:
    Ui::ExtECBrowserClass ui;
	
	ManagedFuzzyDataController *controller;

    AxiomLib::ElemCondPlus ecPlus;
	
	QString concreteECName;
	
	QVBoxLayout * topLevelLayout;
	
	QFormLayout * ecParamsLayout;
	
	QComboBox *comboBox_ecNames;
	
	//QSpinBox *dimensionSpinBox;
	QComboBox *comboBox_dimension;
	
	QCheckBox *signCheckBox;
	
	QList<QDoubleSpinBox*> paramDoubleSpinBoxes;
	
	QStringList ecNames;
	
	QString fileName;
	
	QString oldECName;
	
	const QObject *identifier;
	
	typedef std::map<std::string, ECPointer > ECMap;
	
	ECMap ecMap;
	
	void populateECNames();
	void setCurrentECName(const QString &currentECName);
	
	void displayEC(const AxiomLib::ElemCondPlus &ecPlus);
	
	ECPointer getECByName(const QString &name);
	
	void commitEC(AxiomLib::ElemCondPlus &ecPlus);
	
public slots:
	void ecNameChanged(const QString &newECName);
	void saveEC();
};

#endif // EXTECBROWSER_H
