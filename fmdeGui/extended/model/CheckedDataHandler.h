#ifndef CHECKEDDATAHANDLER_H
#define CHECKEDDATAHANDLER_H

#include "DataHandler.h"

class CheckedDataHandler : public DataHandler
{
public:
	// column ids: checked (Qt::CheckState), isChecking(bool), isCounted(bool)
    CheckedDataHandler(ModelNode* owner, bool isChecked = true, bool isCounted = false);
	
	virtual QVariant data(const QString &columnId) const;
	virtual bool setData(const QString &columnId, const QVariant &newData);
	virtual Qt::ItemFlags flags(const QString &columnId) const;
private:
	bool m_isCounted;
	bool m_isChecking;
	bool m_isChecked;
};

#endif // CHECKEDDATAHANDLER_H
