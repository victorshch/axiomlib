#include "asmodeltreeitemcomposite.h"

#include <iterator>
#include <algorithm>

#include <boost/lambda/bind.hpp>

ASModelTreeItemComposite::ASModelTreeItemComposite(ASModelTreeItem *parent) 
	: ASModelTreeItem(parent)
{
}

bool ASModelTreeItemComposite::compareToRawPointer(const PASModelTreeItem &p1, const ASModelTreeItem *p2) {
	return p1.get() == p2;
}

void ASModelTreeItemComposite::addChild(ASModelTreeItem *child) {
	m_children.push_back(
			PASModelTreeItem(child)
			);
}

void ASModelTreeItemComposite::clearChildren() {
	m_children.clear();
}

ASModelTreeItem *ASModelTreeItemComposite::child(int row) const {
	if(row >= 0 && row < childCount()) {
		return m_children[row].get();
	} else {
		return 0;
	}
}

int ASModelTreeItemComposite::childCount() const {
	return m_children.size();
}

int ASModelTreeItemComposite::rowOfChild(const ASModelTreeItem *child) const {
	QList<PASModelTreeItem>::const_iterator it = 
			std::find_if(m_children.begin(), m_children.end(),
						 boost::lambda::bind(
								 ASModelTreeItemComposite::compareToRawPointer,
								 boost::lambda::_1,
								 child
								 )
						 );
	
	if(it == m_children.end()) {
		return -1;
	} else {
		return std::distance(m_children.begin(), it);
	}
}
