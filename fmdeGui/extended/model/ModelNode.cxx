#include "ModelNode.h"

ModelNode::ModelNode(ModelNode *parent): QObject(parent), m_parent(parent)
{
}

QVariant ModelNode::data(const QString &columnId) const {
	auto range = getRange(columnId);
	for(auto it = range.first; it != range.second; ++it) {
		QVariant data = (*it)->data(columnId);
		if(data.isValid()) {
			return data;
		}
	}
	return QVariant();
}

bool ModelNode::setData(const QString &columnId, const QVariant &newData) {
	auto range = getRange(columnId);
	for(auto it = range.first; it != range.second; ++it) {
		if((*it)->setData(columnId, newData)) {
			return true;
		}
	}
	return false;
}

Qt::ItemFlags ModelNode::flags(const QString &columnId) const {
	auto range = getRange(columnId);
	Qt::ItemFlags result = Qt::NoItemFlags;
	for(auto it = range.first; it != range.second; ++it) {
		result |= (*it)->flags(columnId);
	}
	return result;
}

int ModelNode::row() const {
	return m_row;
}

int ModelNode::childCount() const {
	return m_children.size();
}

ModelNode* ModelNode::parent() {
	return m_parent;
}

ModelNode* ModelNode::child(int row) {
	return m_children[row];
}

void ModelNode::addChildren(const QList<ModelNode *> &newChildren) {
	emit addingChildren(this, m_children.size(), newChildren.size());
	for(auto it = newChildren.begin(); it != newChildren.end(); ++it) {
		(*it)->setRow(m_children.size());
		(*it)->setParent(this);
		m_children.push_back(*it);
	}
	emit finishedAddingChildren();
}

void ModelNode::removeChildren(int row, int count) {
	if(row >= m_children.size() || row < 0 || count <= 0) {
		return;
	}
	auto beginErase = m_children.begin() + row;
	auto endErase = beginErase + count;
	for(auto it = beginErase; it != endErase; ++it) {
		if(it == m_children.end()) {
			endErase = m_children.end();
			break;
		}
	}
	count = std::distance(beginErase, endErase);
	emit removingChildren(this, row, count);
	for(auto it = beginErase; it != endErase; ++it) {
		delete *it;
	}
	auto it = m_children.erase(m_children.begin() + row, endErase);
	int i = m_children.size();
	for(; it != m_children.end(); ++it) {
		(*it)->setRow(i++);
	}
	emit finishedRemovingChildren();
}

void ModelNode::setRow(int value) {
	m_row = value;
}

void ModelNode::setParent(ModelNode *newParent) {
	m_parent = newParent;
}

void ModelNode::registerHandler(DataHandler *handler) {
	for(int i = 0; i < handler->handledColumnCount(); i++) {
		m_handlers.insert(handler->handledColumn(i), handler);
	}
	connect(handler, SIGNAL(modified()), this, SLOT(emitModified()));
}

std::pair<ModelNode::ReverseIterator, ModelNode::ReverseIterator> ModelNode::getRange(const QString &key) {
	return getRange<ModelNode, ModelNode::Iterator>(*this, key);
//	Iterator iter = m_handlers.find(key);
//	if(iter == m_handlers.end()) {
//		auto value = std::reverse_iterator<Iterator>(m_handlers.begin());
//		return std::make_pair(value, value);
//	}
//	auto iter2 = iter;
//	while(iter2 != m_handlers.end() && iter2.key() == key) {
//		++iter2;
//	}
//	return std::make_pair(ReverseIterator(iter2), ReverseIterator(iter));
}

std::pair<ModelNode::ConstReverseIterator, ModelNode::ConstReverseIterator> ModelNode::getRange(const QString &key) const {
	return getRange<const ModelNode, ModelNode::ConstIterator>(*this, key);
//	ConstIterator iter = m_handlers.find(key);
//	if(iter == m_handlers.end()) {
//		auto value = std::reverse_iterator<ConstIterator>(m_handlers.begin());
//		return std::make_pair(value, value);
//	}
//	auto iter2 = iter;
//	while(iter2 != m_handlers.end() && iter2.key() == key) {
//		++iter2;
//	}
//	return std::make_pair(ConstReverseIterator(iter2), ConstReverseIterator(iter));
}

void ModelNode::emitModified() {
	emit modified(this);
}

void ModelNode::emitChildrenModified() {
	emit childrenModified(this);
}
