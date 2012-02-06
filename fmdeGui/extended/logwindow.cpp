#include "logwindow.h"
#include "ui_logwindow.h"

#include "utilities.h"

LogWindow::LogWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LogWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    debugStream = new QDebugStream(std::cout, ui->textEdit_log);

	connect(ui->checkBox_autoscroll, SIGNAL(clicked(bool)), &(debugStream->loggerServer()), SLOT(setAutoScroll(bool)));

	running = true;
}

LogWindow::~LogWindow()
{
    delete ui;

    delete debugStream;
}


void LogWindow::closeEvent(QCloseEvent *event) {
	if(!running) {
		event->accept();
	}
}

void LogWindow::finish() {
	running = false;
	ui->pushButton_cancel->setEnabled(true);
	this->setWindowTitle(encode("Вычисление завершено"));
}
