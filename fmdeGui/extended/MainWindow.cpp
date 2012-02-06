

#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent, ManagedFuzzyDataController *controller)
    : QMainWindow(parent), ui(new Ui::MainWindow())
{
	ui->setupUi(this);

	this->controller = controller;
	
	setStatusBar(0);

	QSignalMapper *loadMapper = new QSignalMapper(this);
	QSignalMapper *saveMapper = new QSignalMapper(this);
	QSignalMapper *saveAxiomLibMapper = new QSignalMapper(this);

	connect(ui->action_load0, SIGNAL(triggered()), loadMapper, SLOT(map(void)));
	connect(ui->action_load1, SIGNAL(triggered()), loadMapper, SLOT(map(void)));
	connect(ui->action_load2, SIGNAL(triggered()), loadMapper, SLOT(map(void)));
	connect(ui->action_load3, SIGNAL(triggered()), loadMapper, SLOT(map(void)));
	
	loadMapper->setMapping(ui->action_load0, 0);
	loadMapper->setMapping(ui->action_load1, 1);
	loadMapper->setMapping(ui->action_load2, 2);
	loadMapper->setMapping(ui->action_load3, 3);
	
	connect(loadMapper, SIGNAL(mapped(int)), this, SLOT(loadStage(int)));

	connect(ui->action_save0, SIGNAL(triggered(void)), saveMapper, SLOT(map(void)));
	connect(ui->action_save1, SIGNAL(triggered(void)), saveMapper, SLOT(map(void)));
	connect(ui->action_save2, SIGNAL(triggered(void)), saveMapper, SLOT(map(void)));
	connect(ui->action_save3, SIGNAL(triggered(void)), saveMapper, SLOT(map(void)));
	
	saveMapper->setMapping(ui->action_save0, 0);
	saveMapper->setMapping(ui->action_save1, 1);
	saveMapper->setMapping(ui->action_save2, 2);
	saveMapper->setMapping(ui->action_save3, 3);
	
	connect(saveMapper,	SIGNAL(mapped(int)), this, SLOT(saveStage(int)));
	
	connect(ui->action_AxiomLib_save1, SIGNAL(triggered()), saveAxiomLibMapper, SLOT(map()));
	connect(ui->action_AxiomLib_save2, SIGNAL(triggered()), saveAxiomLibMapper, SLOT(map()));
	connect(ui->action_AxiomLib_save3, SIGNAL(triggered()), saveAxiomLibMapper, SLOT(map()));
	
	saveAxiomLibMapper->setMapping(ui->action_AxiomLib_save1, 1);
	saveAxiomLibMapper->setMapping(ui->action_AxiomLib_save2, 2);
	saveAxiomLibMapper->setMapping(ui->action_AxiomLib_save3, 3);
	
	connect(saveAxiomLibMapper, SIGNAL(mapped(int)), this, SLOT(saveAxiomLibStage(int)));
	
	connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::wireUp(SignalProxy *signalProxy) {
	connect(signalProxy, SIGNAL(refreshStages()), this, SLOT(refreshStages()));
	
	connect(this, SIGNAL(refreshStage(int)), signalProxy, SIGNAL(refreshStage(int)));
	connect(this, SIGNAL(commitStage(int)), signalProxy, SIGNAL(commitStage(int)));	
}

void MainWindow::refreshStages() {
	enableActions(ui->action_save0, 0, controller->stage0.calculated());
	enableActions(ui->action_save1, ui->action_AxiomLib_save1, controller->stage1.calculated());
	enableActions(ui->action_save2, ui->action_AxiomLib_save2, controller->stage2.calculated());
	enableActions(ui->action_save3, ui->action_AxiomLib_save3, controller->stage3.calculated());
}

void MainWindow::loadStage(int number) {
	try {
		QString title = QString("Load stage %1").arg(number);
		QString filter = QString("Stage %1 files (*.stage%1)").arg(number);
		QString fileName = QFileDialog::getOpenFileName(this, title, QString(), filter);
		if(!fileName.isNull()) {
			controller->loadStage(number, fileName.toStdString());
			emit refreshStage(number);
			refreshStages();
		}
	} catch (AxiomLib::AxiomLibException exception) {
		QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
	} catch (...) {
		QMessageBox::critical(0, "Fatal exception", "Couldn't load stage");
	}
}

void MainWindow::saveStage(int number) {
	try {
		emit commitStage(number);
		QString title = QString("Save stage %1").arg(number);
		QString filter = QString("Stage %1 files (*.stage%1)").arg(number);
		QString fileName = QFileDialog::getSaveFileName(this, title, QString(), filter);
		if(!fileName.isNull()) {
			controller->saveStage(number, fileName.toStdString());
			refreshStages();
		}
	} catch (AxiomLib::AxiomLibException exception) {
		QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
	} catch (...) {
		QMessageBox::critical(0, "Fatal exception", "Couldn't save stage");
	}
}

void MainWindow::saveAxiomLibStage(int number) {
	try {
		emit commitStage(number);
		QString title = QString("Save stage %1 using AxiomLib format").arg(number);
		QString defFolder = number < 3 ? 
						 QString::fromStdString(controller->ecFolder()) :
						 QString::fromStdString(controller->asFolder());
		QString folder = QFileDialog::getExistingDirectory(0, title, defFolder);
		if(!folder.isNull()) {
			controller->saveStageAxiomLib(number, folder.toStdString());
		}
		refreshStages();
	} catch (AxiomLib::AxiomLibException exception) {
		QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
	} catch (std::logic_error &exception) {
		QMessageBox::critical(0, "std::logic_error",exception.what());
	} catch (std::ofstream::failure exception) {
		QMessageBox::critical(0, "std::ofstream::failure", exception.what());
	} catch(std::exception exception) {
		QMessageBox::critical(0, "std::exception", exception.what());
	} catch (...) {
		QMessageBox::critical(0, "Fatal exception", "Couldn't save stage");
	}

}

void MainWindow::enableActions(QAction* action1, QAction* action2, bool enabled) const {
	if(action1) {
		action1->setEnabled(enabled);
	}
	if(action2) {
		action2->setEnabled(enabled);
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}
