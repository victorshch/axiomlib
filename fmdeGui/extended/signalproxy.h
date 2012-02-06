#ifndef SIGNALPROXY_H
#define SIGNALPROXY_H

#include <QObject>

class SignalProxy : public QObject
{
Q_OBJECT
public:
    explicit SignalProxy(QObject *parent = 0);

signals:
	void refreshStage(int);
	void commitStage(int);
	
	void refreshStages();
public slots:

};

#endif // SIGNALPROXY_H
