#ifndef EXTMAINWINDOW_H
#define EXTMAINWINDOW_H

#include <QtGui/QDialog>
#include <QtGui/QWizard>
#include "ui_mainwindowcontent.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"

#include "stage0.h"
#include "stage1.h"
#include "stage2.h"
#include "stage3.h"
#include "signalproxy.h"


class MainWindowContent : public QWidget
{
    Q_OBJECT

public:
    MainWindowContent(QWidget *parent, const QStringList &arguments, const QString &workDir);
    ~MainWindowContent();

    ManagedFuzzyDataController* const controller;

private:
    Ui::MainWindowContentClass ui;

    Stage0 *stage0;
    Stage1 *stage1;
    Stage2 *stage2;
    Stage3 *stage3;
	
	StageBase* getStage(int number);
public:
	void wireUp(SignalProxy *signalProxy)
	{
		connect(signalProxy, SIGNAL(refreshStage(int)), this, SLOT(refreshStage(int)));
		connect(signalProxy, SIGNAL(commitStage(int)), this, SLOT(commitStage(int)));
		
		connect(stage0, SIGNAL(stageCalculated(int)), signalProxy, SIGNAL(refreshStages()));
		connect(stage1, SIGNAL(stageCalculated(int)), signalProxy, SIGNAL(refreshStages()));
		connect(stage2, SIGNAL(stageCalculated(int)), signalProxy, SIGNAL(refreshStages()));
		connect(stage3, SIGNAL(stageCalculated(int)), signalProxy, SIGNAL(refreshStages()));
	}

public slots:
	void refreshStage(int);
	void commitStage(int);
	void initStage(int);
	void reloadStage(int);
	
signals:
	//void stageCalculated();
	//void stageRefresh(int stageToRefresh);
};

#endif // EXTMAINWINDOW_H
