#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QVariant>

//#include "ModelNode.h"
class ModelNode;

class DataHandler : public QObject
{
	Q_OBJECT
public:
    DataHandler(ModelNode* owner);
	
	QString handledColumn(int i) const;
	int handledColumnCount() const;
	
	virtual QVariant data(const QString& columnId) const = 0;
	virtual bool setData(const QString& columnId, const QVariant& newData) = 0;
	virtual Qt::ItemFlags flags(const QString& columnId) const = 0;
signals:
	void modified();
protected:
	void addHandledColumn(const QString& columnName);
	ModelNode* owner() const;
	
private:
	QList<QString> m_handledColumns;
	ModelNode* m_owner;
};

#endif // DATAHANDLER_H
