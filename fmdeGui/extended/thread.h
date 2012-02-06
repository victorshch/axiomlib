#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QMetaType>
#include <boost/function.hpp>

#include "AxiomLibException.h"

typedef boost::function0<void> ThreadStart;

Q_DECLARE_METATYPE(AxiomLib::AxiomLibException);

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(ThreadStart threadStart, QObject *parent = 0);

protected:
    virtual void run();

signals:
	void exception(AxiomLib::AxiomLibException e);
public slots:

private:
    ThreadStart threadStart;
};

#endif // THREAD_H
