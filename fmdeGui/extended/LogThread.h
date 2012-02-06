#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QObject>
#include <QPointer>

#include "thread.h"
#include "logwindow.h"

class LogThread : public QObject
{
    Q_OBJECT
public:
    explicit LogThread(ThreadStart threadStart, QObject* parent = 0);
	
	void start(QThread::Priority proprity = QThread::HighestPriority);
	
	~LogThread();

signals:
	void finished();
	void exception(AxiomLib::AxiomLibException e);
public slots:
private:
	Thread* thread;
	QPointer<LogWindow> logWindow;
	//LogWindow* logWindow;

};

#endif // LOGTHREAD_H
