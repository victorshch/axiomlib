#ifndef FORM_INFORMATION_H
#define FORM_INFORMATION_H

#include <QtGui/QMainWindow>
#include <QString>
#include "ui_MainWindow.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "signalproxy.h"
#include "utilities.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent, ManagedFuzzyDataController *controller);
    ~MainWindow();

	void wireUp(SignalProxy *signalProxy); 
public slots:
	void refreshStages();
	
    void loadStage(int number);
    void saveStage(int number);	
	
	void saveAxiomLibStage(int number);
signals:
	void refreshStage(int);
	void commitStage(int);
private:
    Ui::MainWindow* ui;
    ManagedFuzzyDataController *controller;
	
	void enableActions(QAction* action1, QAction* action2, bool enabled) const;
};

#endif // FORM_INFORMATION_H
