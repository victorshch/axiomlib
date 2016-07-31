#ifndef ASMODELTREEITEMLEAF_H
#define ASMODELTREEITEMLEAF_H

#include "ASModelTreeItem.h"

class ASModelTreeItemLeaf : public ASModelTreeItem
{
	Q_OBJECT
private:
public:
    ASModelTreeItemLeaf(ASModelTreeItem *parent);
	
	virtual int columnCount() const = 0;
	virtual QVariant data(int column) const = 0;
	
	virtual ASModelTreeItem *child(int row) const;
	virtual int childCount() const;	
	//TODO: перенести в protected
	virtual int rowOfChild(const ASModelTreeItem *child) const;
	
};

#endif // ASMODELTREEITEMLEAF_H
