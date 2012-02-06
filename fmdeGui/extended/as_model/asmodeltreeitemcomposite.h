#ifndef ASMODELTREEITEMCOMPOSITE_H
#define ASMODELTREEITEMCOMPOSITE_H

#include "ASModelTreeItem.h"
#include <QList>


class ASModelTreeItemComposite : public ASModelTreeItem
{
	Q_OBJECT
private:
	QList<PASModelTreeItem> m_children;
	
	static bool compareToRawPointer(const PASModelTreeItem& p1, const ASModelTreeItem *p2);
protected:
	//QList<PASModelTreeItem> &children() { return children_; }
	
	// takes ownership
	void addChild(ASModelTreeItem *child);
	void clearChildren();
public:
    ASModelTreeItemComposite(ASModelTreeItem *parent);
	
	virtual int columnCount() const = 0;
	virtual QVariant data(int column) const = 0;	
	
	//standard implementation for methods:
	virtual ASModelTreeItem *child(int row) const;
	virtual int childCount() const;
	
	//TODO: перенести в protected
	virtual int rowOfChild(const ASModelTreeItem *child) const;
	
};

#endif // ASMODELTREEITEMCOMPOSITE_H
