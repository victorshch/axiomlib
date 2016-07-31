#include <stack>
#include <algorithm>

#include "clusteringform.h"
#include "ClassSelectForm.h"
#include "selectionmanager.h"
#include "plotbrowser.h"
#include "utilities.h"
#include "QtGuiException.h"

#include "SeparateClassDisplay.h"

const int SeparateClassDisplay::ExpandedRole = Qt::UserRole + 1;

SeparateClassDisplay::SeparateClassDisplay(ManagedFuzzyDataController *controller, int stage, int depth, ConditionFunction conditionFunction, QTreeWidget *treeWidget, ClassSelectForm *classSelectForm, SelectionManager *selectionManager, ClusteringForm *clusteringForm, QObject *parent) :
    QObject(parent), 
    m_controller(controller),
    m_treeWidget(treeWidget), 
    m_depth(depth),
    m_stage(stage),
    m_currentCheckedCount(-1), 
    m_currentClass(-1),
    m_canUpdateNumber(true),
    m_canSaveItemExpanded(true),
    m_classSelectForm(classSelectForm),
    m_selectionManager(selectionManager),
    m_clusteringForm(clusteringForm),
    m_conditionFunction(conditionFunction)  
{
	m_items.reserve(classSelectForm->classCount());
	
	for(int i = 0; i < classSelectForm->classCount(); ++i) {
		QTreeWidgetItem* currentItem = new QTreeWidgetItem();
		currentItem->setText(0, encode("Класс ") + classSelectForm->className(i));
		currentItem->setData(0, Qt::UserRole, QVariant::fromValue(i));
		m_items.append(currentItem);
	}
	
	classSelectForm->setEnabled(false);
	
	connect(classSelectForm, SIGNAL(classSelected(int)), this, SLOT(saveDisplayClass(int)));
	
	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(checkUpdateNumber(QTreeWidgetItem*,int)));
	connect(treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(saveItemExpanded(QTreeWidgetItem*)));
	connect(treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(saveItemCollapsed(QTreeWidgetItem*)));
	
	connect(selectionManager, SIGNAL(selectAllRequested()), this, SLOT(selectAll()));
	connect(selectionManager, SIGNAL(deselectAllRequested()), this, SLOT(deselectAll()));
	connect(selectionManager, SIGNAL(loadSelectionRequested()), this, SLOT(loadSelection()));
	connect(selectionManager, SIGNAL(saveSelectionRequested()), this, SLOT(saveSelection()));
	
	connect(clusteringForm, SIGNAL(clusteringRequested()), this, SLOT(clusterizeSelection()));
	connect(clusteringForm, SIGNAL(saveCheckedRequested()), this, SLOT(saveClustering()));
}

QTreeWidgetItem* SeparateClassDisplay::itemForClass(int classNo) const {
	return m_items[classNo];
}

int SeparateClassDisplay::classCount() const {
	return m_items.count();
}

void SeparateClassDisplay::buildNodes() {
	m_selectionManager->clear();
	

	for(int i = 0; i < m_items.size(); ++i) {
		clearChildren(m_items[i]);
		emit buildingNodeForClass(i, m_items[i]);
	}
	
	m_currentClass = -1;
	m_classSelectForm->setEnabled(true);
	displayClass(-1);
}

void SeparateClassDisplay::saveClass(int classNo) {
	if(classNo < 0) {
		for(int i = 0; i < m_items.count(); ++i) {
			emit savingClass(i, m_items[i]);
		}
	} else {
		emit savingClass(classNo, m_items[classNo]);
	}	
}

void SeparateClassDisplay::saveCurrentClass() {
	saveClass(m_currentClass);
}

void SeparateClassDisplay::selectAll() {
	auto function = [](QTreeWidgetItem* item) {
		if(!item->isHidden()) {
			item->setCheckState(0, Qt::Checked);
		}
	};
	
	beginUpdateItems();
	
	traverseCurrentDepth(ReducedTraverseFunction(function));
	
	endUpdateItems();
	
	updateNumber();
}

void SeparateClassDisplay::deselectAll() {
	auto function = [](QTreeWidgetItem* item) {
		if(!item->isHidden()) {
			item->setCheckState(0, Qt::Unchecked);
		}
	};
	
	beginUpdateItems();
	
	traverseCurrentDepth(ReducedTraverseFunction(function));
	
	endUpdateItems();
	
	updateNumber();
}

void SeparateClassDisplay::saveDisplayClass(int classNo) {
	saveCurrentClass();
	displayClass(classNo);
}

void SeparateClassDisplay::displayClass(int classNo) {
	m_currentClass = classNo;
	
	while(m_treeWidget->topLevelItemCount() > 0) {
		m_treeWidget->takeTopLevelItem(0);
	}
	
	if(classNo < 0) {
		m_treeWidget->addTopLevelItems(m_items);
		for(int i = 0; i < classCount(); i++) {
			applyExpansion(m_items[i]);
		}
	} else {
		m_treeWidget->addTopLevelItem(m_items[classNo]);
		applyExpansion(m_items[classNo]);
	}
	
	updateNumber();
}

void SeparateClassDisplay::checkUpdateNumber(QTreeWidgetItem *item, int column) {
	
	if(!m_canUpdateNumber) {
		return;
	}
	
	auto pair = getCurrentRoot();
	
	if(checkDepth(item, pair.first, pair.second)) {
		updateNumber();
	}
}

void SeparateClassDisplay::saveSelection() {
	QList<Index> selection = getSelection(m_currentClass);
	std::for_each(selection.begin(), selection.end(),
	              [this](const Index& index) { this->m_selectionManager->addIndex(index); }
	              );
}



void SeparateClassDisplay::clearChildren(QTreeWidgetItem *item) const {
	auto childList = item->takeChildren();
	for(auto it = childList.begin(); it != childList.end(); ++it) {
		delete *it;
	}
}

void SeparateClassDisplay::traverseWithDepth(QTreeWidgetItem *item, TraverseFunction function) {
	std::stack<std::pair<QTreeWidgetItem*, int> > nodeStack;
	nodeStack.push(std::make_pair(item, 1));
	do {
		auto current = nodeStack.top();
		nodeStack.pop();
		bool traverseChildren = function(current.first, current.second);
		if(traverseChildren) {
			int childCount = current.first->childCount();
			for(int i = 0; i < childCount; ++i) {
				nodeStack.push(std::make_pair(current.first->child(i), current.second + 1));
			}			
		}
	} while (!nodeStack.empty());
}

void SeparateClassDisplay::traverseCurrentDepth(ReducedTraverseFunction function) const {
	auto pair = getCurrentRoot();
	int depth = pair.second;
//	auto traverseFunction = [depth, function](QTreeWidgetItem* item, int currentDepth) ->bool {
//		if(currentDepth == depth) {
//			function(item);
//			return false;
//		} else if (currentDepth < depth) {
//			return true;
//		} else {
//			return false;
//		}
//	};
	
//	traverseWithDepth(pair.first, TraverseFunction(traverseFunction));
        // предыдущий код не компилится в icc (вылетает с internal error, видимо, из-за замыкания function),
        // поэтому пришлось написать дополнительную функцию
        traverseDepth(pair.first, function, depth);
}

int SeparateClassDisplay::countChecked() const {
	int result = 0;
	
	auto function = [&result](QTreeWidgetItem* currentItem) {
		if(!currentItem->isHidden() && currentItem->checkState(0) == Qt::Checked) {
			++result;
		}
	};
	
	traverseCurrentDepth(ReducedTraverseFunction(function));
	
	return result;
}

bool SeparateClassDisplay::checkDepth(QTreeWidgetItem *item, QTreeWidgetItem *root, int depth) const {
	QTreeWidgetItem* current = item;
	do {
		if(current == root && depth == 1) {
			return true;
		} else if (depth < 1) {
			return false;
		}
		--depth;
	} while(current = current->parent());
	return false;
}

void SeparateClassDisplay::updateNumber() {
	m_currentCheckedCount = countChecked();
	emit numberChanged(m_currentCheckedCount);
}

QPair<QTreeWidgetItem*, int> SeparateClassDisplay::getCurrentRoot() const {
	int depth;
	QTreeWidgetItem* root;
	if(m_currentClass >= 0) {
		depth = m_depth;
		root = m_items[m_currentClass];
	} else {
		depth = m_depth + 1;
		root = m_treeWidget->invisibleRootItem();
	}	
	return qMakePair(root, depth);
}

QList<Index> SeparateClassDisplay::getSelection(int classNo) const {
	QList<Index> result;
	result.reserve(m_currentCheckedCount);
	
	auto function = [&result](QTreeWidgetItem* item) {
		if(!item->isHidden() && item->checkState(0) == Qt::Checked) {
			QVariant index = item->data(0, Qt::UserRole);
			if(!index.canConvert<Index>()) {
				throw QtGuiException("SeparateClassDisplay::getSelection(): invalid data in tree node");
			}
			result.push_back(index.value<Index>());
		}
	};
	
	traverseCurrentDepth(ReducedTraverseFunction(function));
	
	return result;
}

void SeparateClassDisplay::applySelection(const QList<Index> &selection, bool keepOld) {
	if(!keepOld) {
		deselectAll();
	}
	for(auto it = selection.begin(); it != selection.end(); ++it) {
		QTreeWidgetItem* item = itemForIndex(*it);
		if(!item->isHidden()) {
			item->setCheckState(0, Qt::Checked);
		}
	}
}

QTreeWidgetItem* SeparateClassDisplay::itemForIndex(Index index) const {
	QTreeWidgetItem* result = itemForClass(index[0]);
	for(int i = 1; i < m_depth; ++i) {
		if(result == 0) {
			return 0;
		}
		result = result->child(index[i]);
	}
	return result;
}

//bool SeparateClassDisplay::initialized() const {
//	return m_items.count() > 0;
//}

void SeparateClassDisplay::loadSelection() {
	applySelection(const_cast<const SelectionManager*>(m_selectionManager)->currentIndexList());
}

QList<Index> SeparateClassDisplay::getCurrentSelection() const {
	return getSelection(m_currentClass);
}

void SeparateClassDisplay::setSelection(const Index &index, bool value) {
	itemForIndex(index)->setCheckState(0, value ? Qt::Checked : Qt::Unchecked);
}

void SeparateClassDisplay::hideItem(const Index &index, bool hide) {
	QTreeWidgetItem* item = itemForIndex(index);
	if(item) {
		item->setHidden(hide);
	}
}

void SeparateClassDisplay::beginUpdateItems() {
	m_canUpdateNumber = false;
}

void SeparateClassDisplay::endUpdateItems() {
	m_canUpdateNumber = true;
	updateNumber();
}

void SeparateClassDisplay::clusterizeSelection() {
	QList<Index> currentSelection = getCurrentSelection();
	m_clusteringForm->clear();
	std::for_each(currentSelection.begin(), currentSelection.end(),
	              [this](const Index& index){ this->m_clusteringForm->addCondition(
	                  this->m_conditionFunction(index)
	                  );
	});
	m_clusteringForm->clusterize();
}

void SeparateClassDisplay::saveClustering() {
	//todo
	for(int i = 0; i < m_clusteringForm->conditionsCount(); ++i) {
		Index index = m_clusteringForm->condition(i).identifier();
		setSelection(index, m_clusteringForm->conditionChecked(i));
	}
}

void SeparateClassDisplay::displayPlot() {
	PlotBrowser* plotBrowser = new PlotBrowser(m_controller, m_stage, 0);
	
	plotBrowser->setAttribute(Qt::WA_DeleteOnClose, true);
	plotBrowser->setWindowModality(Qt::ApplicationModal);
	
	QList<Index> currentSelection = getCurrentSelection();
	
	std::for_each(currentSelection.begin(), currentSelection.end(),
	              [plotBrowser, this](const Index& index){ plotBrowser->addCondition(
	                  this->m_conditionFunction(index)
	                  );
				});

	plotBrowser->replot();
	
	plotBrowser->showMaximized();
}
	
void SeparateClassDisplay::showAll() {
	auto function = [](QTreeWidgetItem* item) { item->setHidden(false); };
	
	traverseCurrentDepth(ReducedTraverseFunction(function));
}

void SeparateClassDisplay::saveItemExpanded(QTreeWidgetItem *item) {
	if(!m_canSaveItemExpanded) {
		return;
	}
	
	setItemExpanded(item, true);
}

void SeparateClassDisplay::saveItemCollapsed(QTreeWidgetItem *item) {
	if(!m_canSaveItemExpanded) {
		return;
	}
	
	setItemExpanded(item, false);
}

void SeparateClassDisplay::applyExpansion(QTreeWidgetItem *item) {
	auto function = [this](QTreeWidgetItem* currentItem, int currentDepth)->bool {
		if(currentDepth < this->m_depth) {
			currentItem->setExpanded(isItemExpanded(currentItem));
			return currentDepth < this->m_depth - 1;
		} else {
			return false;
		}
	};
	//todo сдедать через disconnect
	m_canSaveItemExpanded = false;
	traverseWithDepth(item, TraverseFunction(function));
	m_canSaveItemExpanded = true;
}

void SeparateClassDisplay::setItemExpanded(QTreeWidgetItem *item, bool expanded) {
	item->setData(0, ExpandedRole, expanded);
}

bool SeparateClassDisplay::isItemExpanded(QTreeWidgetItem *item) {
	QVariant value = item->data(0, ExpandedRole);
	if(value.isNull()) {
		// По умолчанию вершина раскрыта
		setItemExpanded(item, true);
		return true;
	} else {
		return value.toBool();
	}
}

void SeparateClassDisplay::traverseDepth(QTreeWidgetItem *item,
                                         ReducedTraverseFunction function,
                                         int depth) {
    if(depth <= 1) {
        function(item);
    } else {
        for(int i = 0; i < item->childCount(); ++i) {
            traverseDepth(item->child(i), function, depth - 1);
        }
    }
}
