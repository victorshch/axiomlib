#include "thread.h"

Thread::Thread(ThreadStart threadStart, QObject *parent) :
	QThread(parent),
    threadStart(threadStart)
{
	qRegisterMetaType<AxiomLib::AxiomLibException>();
}

void Thread::run() {
	try {
		threadStart();
	} catch (AxiomLib::AxiomLibException e) {
		emit exception(e);
	}
	catch(std::exception e) {
		emit exception(AxiomLib::AxiomLibException(e.what()));
	}
	catch(...) {
		emit exception(AxiomLib::AxiomLibException("Unknown exception"));
	}
}
