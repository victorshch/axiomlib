#ifndef EXTAXIOMSETBROWSER_H
#define EXTAXIOMSETBROWSER_H

#include <QtGui/QWidget>
#include <QLineEdit>
#include <QList>
#include <QSignalMapper>

#include "ui_extaxiomsetbrowser.h"

#include "AxiomExprSetPlus.h"
#include "utilities.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"

class ExtAxiomSetBrowser : public QWidget
{
    Q_OBJECT

public:
    ExtAxiomSetBrowser(QWidget *parent, 
					   const AxiomLib::AxiomExprSetPlus &axiomSet, 
					   QObject *identifier,
					   ManagedFuzzyDataController *controller);
    ~ExtAxiomSetBrowser();

signals:
	void asChanged(AxiomLib::AxiomExprSetPlus newAS, QObject *identifier);
	
private:
    Ui::ExtAxiomSetBrowserClass ui;

	QObject *identifier;
	
    AxiomLib::AxiomExprSetPlus axiomSet;
	
	ManagedFuzzyDataController *controller;
	
	QList<QLineEdit *> editors;
	
	QSignalMapper *mapper;
	
	void displayMarkups();
	void displayAxioms();
	
	static QString markupToString(const std::vector<int> &markup);
	
	std::vector<int> stringToMarkup(const QString &string);
private slots:
	void displayAxiom(int number);
	void saveMarkup();
	void recalculateAxiomSet();
};

#endif // EXTAXIOMSETBROWSER_H
