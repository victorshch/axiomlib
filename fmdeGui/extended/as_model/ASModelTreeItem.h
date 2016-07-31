#ifndef ASMODELTREEITEM_H
#define ASMODELTREEITEM_H

#include <QObject>
#include <QVariant>

#include <boost/shared_ptr.hpp>

#include "../algorithm_controller/forwarddecls.h"

class ASModelTreeItem : public QObject {
	Q_OBJECT
private:
	ASModelTreeItem *m_parent;
protected:
	void emitItemChanged(ASModelTreeItem *item);
	void propagateChanges(ASModelTreeItem *item);
	virtual bool handleChanges(ASModelTreeItem *item);
	
	virtual ManagedFuzzyDataController* controller() const;
public:
	ASModelTreeItem(ASModelTreeItem *parent);

	virtual int columnCount() const = 0;
	virtual QVariant data(int column) const = 0;

	
	virtual ASModelTreeItem *child(int row) const = 0;
	virtual int childCount() const = 0;	
	//TODO: перенести в protected
	virtual int rowOfChild(const ASModelTreeItem *child) const = 0;
	
	ASModelTreeItem *parent() const;
	
	// standart implementation via parent()->rowOfChild(this)
	virtual int row() const;
	
	virtual ~ASModelTreeItem() {}
signals:
	void itemChanged(ASModelTreeItem *item);
};

typedef boost::shared_ptr<ASModelTreeItem> PASModelTreeItem;

#endif // ASMODELTREEITEM_H
