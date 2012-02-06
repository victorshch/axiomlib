#include "ModelNode.h"

#include "DataHandler.h"

DataHandler::DataHandler(ModelNode *owner) : QObject(owner), m_owner(owner)
{
	owner->registerHandler(this);
}

QString DataHandler::handledColumn(int i) const {
	return m_handledColumns[i];
}

int DataHandler::handledColumnCount() const {
	return m_handledColumns.size();
}

void DataHandler::addHandledColumn(const QString &columnName) {
	m_handledColumns.push_back(columnName);
}

ModelNode* DataHandler::owner() const {
	return m_owner;
}
