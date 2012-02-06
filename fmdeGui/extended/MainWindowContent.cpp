#include "MainWindowContent.h"
#include <QStringList>
#include <QMessageBox>
#include <QEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QtDebug>
#include <string>

#include "QtGuiException.h"

const char* defaultBaseFileName = "autosave";

//   QString  std::string
class QStringToStdString {
public:
	std::string operator()(const QString &str) {
		return str.toStdString();
	}
};

MainWindowContent::MainWindowContent(QWidget *parent, const QStringList &arguments, const QString &workDir)
	: QWidget(parent), controller(ManagedFuzzyDataController::getInstance())
{
	qDebug()<<"Entering ExtMainWindow::ExtMainWindow()";
	
	ui.setupUi(this);

	try {
		qDebug()<<"Reading env...";
	controller->readEnv(arguments.begin(), arguments.end(), QStringToStdString());
	} catch (AxiomLib::AxiomLibException exception) {
		QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
		return;
	}
	qDebug()<<"initializing controller";
	controller->init(workDir.toStdString());
	//QMessageBox::information(0, "Information", "Environment initialized successfully");
	
	ui.tabWidget_stages->clear();
	
	ui.tabWidget_stages->addTab(
			stage0 = new Stage0(this, controller),
			encode("Параметры 1-го этапа")
			);
	ui.tabWidget_stages->addTab(
			stage1 = new Stage1(this, controller),
			encode("Результаты 1-го этапа")
			);
	ui.tabWidget_stages->addTab(
			stage2 = new Stage2(this, controller),
			encode("Результаты 2-го этапа")
			);
	ui.tabWidget_stages->addTab(
			stage3 = new Stage3(this, controller),
			encode("Результаты 3-го этапа")
			);
	
	connect(stage0, SIGNAL(stageCalculated(int)), this, SLOT(initStage(int)));
	connect(stage1, SIGNAL(stageCalculated(int)), this, SLOT(initStage(int)));
	connect(stage2, SIGNAL(stageCalculated(int)), this, SLOT(initStage(int)));
	
	connect(stage1, SIGNAL(stageRecalculated(int)), this, SLOT(reloadStage(int)));
	connect(stage2, SIGNAL(stageRecalculated(int)), this, SLOT(reloadStage(int)));
	
	initStage(0);
}

MainWindowContent::~MainWindowContent()
{
	delete controller;
}

void MainWindowContent::initStage(int number) {
	qDebug()<<"Initializing stage "<<number;
	controller->createStage(number);
	if(number > 0) {
		QString fileName = defaultBaseFileName + QString(".stage%1").arg(number);
		controller->saveStage(number, fileName.toStdString());
	}
	refreshStage(number);
	//emit stageCalculated();
}

void MainWindowContent::reloadStage(int number) {
	controller->createStage(number);
	getStage(number)->reloadValuesFromStage();
}

void MainWindowContent::refreshStage(int number) {
	getStage(number)->refresh();
}

void MainWindowContent::commitStage(int number) {
	getStage(number)->saveValuesToStage();
}

StageBase* MainWindowContent::getStage(int number) {
	switch(number) {
	case 0: return stage0;
	case 1: return stage1;
	case 2: return stage2;
	case 3: return stage3;
	default: throw QtGuiException(
		            QString("MainWindowContent::getStage(): unexpected stage number : %1").arg(number));
	}
}

