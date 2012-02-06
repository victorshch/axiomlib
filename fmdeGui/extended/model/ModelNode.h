#ifndef MODELNODE_H
#define MODELNODE_H

#include <vector>

#include <QVariant>
#include <QString>
#include <QList>
#include <QHash>
#include <QScopedPointer>

#include "DataHandler.h"

class ModelNode : public QObject
{
	Q_OBJECT
public:
	ModelNode(ModelNode* parent = 0);
	
	QVariant data(const QString& columnId) const;
	bool setData(const QString& columnId, const QVariant& newData);
	Qt::ItemFlags flags(const QString& columnId) const;
	
	int row() const;
	int childCount() const;
	
	ModelNode* parent();
	ModelNode* child(int row);
	
	void addChildren(const QList<ModelNode*>& newChildren);
	
	void removeChildren(int row, int count);
public slots:
	
signals:
	void addingChildren(ModelNode* parent, int row, int count);
	void finishedAddingChildren();
	void removingChildren(ModelNode* parent, int row, int count);
	void finishedRemovingChildren();
	
	void modified(ModelNode* item);
	// Обновление всех детей
	void childrenModified(ModelNode* item);
protected:
	
private:
	friend class DataHandler;
	
	typedef ModelNode* PModelNode;
	
	ModelNode* m_parent;
	
	int m_row;
	
	typedef QList<PModelNode> ChildrenContainer;
	ChildrenContainer m_children;
	typedef QMultiHash<QString, DataHandler* > HandlerContainer;
	HandlerContainer m_handlers;

	void setRow(int value);
	void setParent(ModelNode* newParent);
	void registerHandler(DataHandler* handler);
	
	typedef HandlerContainer::iterator Iterator;
	typedef HandlerContainer::const_iterator ConstIterator;
	typedef std::reverse_iterator<Iterator> ReverseIterator;
	typedef std::reverse_iterator<ConstIterator> ConstReverseIterator;
	std::pair<ReverseIterator, ReverseIterator> getRange(const QString& key);
	std::pair<ConstReverseIterator, ConstReverseIterator> getRange(const QString& key) const;
	
	template<class Self, class Iterator>
	static
	std::pair<std::reverse_iterator<Iterator>, std::reverse_iterator<Iterator> > getRange(Self& self, const QString& key) {
		Iterator iter = self.m_handlers.find(key);
		if(iter == self.m_handlers.end()) {
			auto value = std::reverse_iterator<Iterator>(self.m_handlers.begin());
			return std::make_pair(value, value);
		}
		auto iter2 = iter;
		while(iter2 != self.m_handlers.end() && iter2.key() == key) {
			++iter2;
		}
		return std::make_pair(std::reverse_iterator<Iterator>(iter2), std::reverse_iterator<Iterator>(iter));		
	}
	
private slots:
	void emitChildrenModified();
	void emitModified();
};

#endif // MODELNODE_H
