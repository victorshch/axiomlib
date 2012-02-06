#ifndef EXTAXIOMBROWSER_H
#define EXTAXIOMBROWSER_H

#include <QtGui/QWidget>
#include "ui_extaxiombrowser.h"
#include "utilities.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include <boost/lexical_cast.hpp>
#include "Index.h"

class ExtAxiomBrowser : public QWidget
{
    Q_OBJECT

public:
    ExtAxiomBrowser(QWidget *parent, PAxiomExpr axiomExpr, ManagedFuzzyDataController * controller);
    ~ExtAxiomBrowser();
public slots:
	void displayEC(QObject *indexPair);
private:
    Ui::ExtAxiomBrowserClass ui;

	ManagedFuzzyDataController *controller;
	
    PAxiomExpr axiomExpr;
};

#endif // EXTAXIOMBROWSER_H
