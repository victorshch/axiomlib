#include "LogThread.h"

LogThread::LogThread(ThreadStart threadStart, QObject *parent) :
    QObject(parent),
    thread(new Thread(threadStart, this)),
    logWindow(new LogWindow(0))
{
	logWindow->setWindowModality(Qt::ApplicationModal);
	
	connect(thread, SIGNAL(finished()), this, SIGNAL(finished()));
	connect(thread, SIGNAL(finished()), logWindow, SLOT(finish()));
	
	connect(thread, SIGNAL(exception(AxiomLib::AxiomLibException)), this, SIGNAL(exception(AxiomLib::AxiomLibException)), Qt::QueuedConnection);
}

LogThread::~LogThread() {
	if(!logWindow.isNull()) {
		delete logWindow.data();
	}
}

void LogThread::start(QThread::Priority priority) {
	//logWindow->setParent(0);
	setParent(logWindow);
	
	logWindow->show();
	logWindow = 0;
	thread->start(priority);
}
