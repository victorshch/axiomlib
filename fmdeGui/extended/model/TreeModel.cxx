#include <QVector>
#include <QStack>

#include "TreeModel.h"

TreeModel::TreeModel(ModelNode *root, const Schema *schema, QObject *parent) :
    QAbstractItemModel(parent), m_root(root), m_schema(schema)
{
	//connect all nodes
	QStack<ModelNode*> stack;
	stack.push(root);
	do {
		ModelNode* current = stack.pop();
		establishConnections(current);
		int childCount = current->childCount();
		for(int i = 0; i < childCount; ++i) {
			stack.push(current->child(i));
		}
	} while (!stack.empty());
}

void TreeModel::establishConnections(ModelNode *node) {
	connect(node, SIGNAL(addingChildren(ModelNode*,int,int)), this, SLOT(nodeChildrenAdding(ModelNode*,int,int)));
	connect(node, SIGNAL(finishedAddingChildren()), this, SLOT(nodeChildrenAdded()));
	connect(node, SIGNAL(removingChildren(ModelNode*,int,int)), this, SLOT(nodeChildrenRemoving(ModelNode*,int,int)));
	connect(node, SIGNAL(finishedRemovingChildren()), this, SLOT(nodeChildrenRemoved()));
	connect(node, SIGNAL(modified(ModelNode*)), this, SLOT(nodeModified(ModelNode*)));
	connect(node, SIGNAL(childrenModified(ModelNode*)), this, SLOT(nodeChildrenModified(ModelNode*)));
}

TreeModel::~TreeModel() {
	delete m_root;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid()) {
		return QVariant();
	}
	
	ModelNode* item = indexToNode(index);
	
	return item->data(m_schema->getColumnId(index.column(), role));
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(!index.isValid()) {
		return false;
	}
	
	ModelNode* item = indexToNode(index);
	
	return item->setData(m_schema->getColumnId(index.column(), role), value);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
	QList<int> roles = m_schema->getRoles(index.column());
	
	ModelNode* node = indexToNode(index);
	
	Qt::ItemFlags result = Qt::NoItemFlags;
	
	for(int i = 0; i < roles.size(); i++) {
		result |= node->flags(m_schema->getColumnId(index.column(), i));
	}
	
	return result;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(orientation == Qt::Horizontal) {
		return m_root->data(m_schema->getColumnId(section, role));
	} else {
		return QVariant();
	}
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
	ModelNode* parentNode = indexToNode(parent);
	if(row < parentNode->childCount()) {
		throw std::runtime_error("Invalid row subscript");
	} else {
		return nodeToIndex(parentNode->child(row), column);
	}
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
	ModelNode* node = indexToNode(index);
	return nodeToIndex(node->parent());
}

int TreeModel::rowCount(const QModelIndex &parent) const {
	ModelNode* node = indexToNode(parent);
	if(node) {
		return node->childCount();
	} else {
		return 0;
	}
}

int TreeModel::columnCount(const QModelIndex &/*parent*/) const {
	return m_schema->columnCount();
}

ModelNode* TreeModel::indexToNode(const QModelIndex &index) const {
	if(index.isValid()) {
		return static_cast<ModelNode*>(index.internalPointer());
	} else {
		return m_root;
	}
}

QModelIndex TreeModel::nodeToIndex(ModelNode *node, int column) const {
	if(node == m_root) {
		return QModelIndex();
	} else {
		return createIndex(node->row(), column, node);
	}
}

void TreeModel::nodeChildrenAdding(ModelNode *node, int row, int count) {
	beginInsertRows(nodeToIndex(node), row, row + count - 1);
}

void TreeModel::nodeChildrenAdded() {
	endInsertRows();
}

void TreeModel::nodeChildrenRemoving(ModelNode *node, int row, int count) {
	beginRemoveRows(nodeToIndex(node), row, row + count - 1);
}

void TreeModel::nodeChildrenRemoved() {
	endRemoveRows();
}

void TreeModel::nodeModified(ModelNode *node) {
	QModelIndex index = nodeToIndex(node);
	emit dataChanged(index, nodeToIndex(node, columnCount(parent(index))));
}

void TreeModel::nodeChildrenModified(ModelNode *node) {
	ModelNode* startChild = node->child(0);
	ModelNode* endChild = node->child(node->childCount() - 1);
	emit dataChanged(nodeToIndex(startChild), nodeToIndex(endChild, columnCount(nodeToIndex(node))));
}
