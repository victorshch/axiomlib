#ifndef SEPARATECLASSDISPLAY_H
#define SEPARATECLASSDISPLAY_H

#include <boost/function.hpp>

#include <QObject>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QList>
#include <QPair>

#include "Index.h"
#include "Condition.h"

#include "algorithm_controller/forwarddecls.h"

// Forward-декларации для уменьшения include dependencies
class ClassSelectForm;
class SelectionManager;
class ClusteringForm;

typedef boost::function1<PCondition, Index> ConditionFunction;

class SeparateClassDisplay : public QObject
{
    Q_OBJECT
public:
    explicit SeparateClassDisplay(ManagedFuzzyDataController* controller,
	                              int stage,
								  int depth, 
	                              ConditionFunction conditionFunction,
	                              QTreeWidget* treeWidget, 
	                              ClassSelectForm* classSelectForm, 
	                              SelectionManager* selectionManager, 
	                              ClusteringForm* clusteringForm,
	                              QObject *parent);

	QTreeWidgetItem* itemForClass(int classNo) const;
	
	int classCount() const;
	//int checkedCount() const;
	
	QList<Index> getSelection(int classNo) const;
	QList<Index> getCurrentSelection() const;
	void setSelection(const Index& index, bool value = true);
	void applySelection(const QList<Index>& selection, bool keepOld = true);
	void hideItem(const Index& index, bool hide = true);
	//void hideItems(const QList<Index>& itemsToHide);
	
	void beginUpdateItems();
	void endUpdateItems();
signals:
	void buildingNodeForClass(int classNo, QTreeWidgetItem* item);
	void savingClass(int classNo, QTreeWidgetItem* item);
	void numberChanged(int newNumber);
public slots:
	void buildNodes();
	
	void saveCurrentClass();
	
	void selectAll();
	void deselectAll();
	
	void updateNumber();
	
	void displayPlot();
private slots:
	void saveClass(int classNo);
	void saveDisplayClass(int classNo);
	void displayClass(int classNo);
	void checkUpdateNumber(QTreeWidgetItem* item, int column);
	
	void saveSelection();
	void loadSelection();
	
	void clusterizeSelection();
	void saveClustering();
	
	void showAll();
	
	void saveItemExpanded(QTreeWidgetItem* item);
	void saveItemCollapsed(QTreeWidgetItem* item);
private:
	QTreeWidget* m_treeWidget;
	ClassSelectForm* m_classSelectForm;
	SelectionManager* m_selectionManager;
	ClusteringForm* m_clusteringForm;
	ConditionFunction m_conditionFunction;
	
	QList<QTreeWidgetItem*> m_items;
	QList<bool> m_initialExpanded;
	int m_currentClass;
	int m_currentCheckedCount;
	
	const int m_depth;
	const int m_stage;
	
	ManagedFuzzyDataController* m_controller;
	
	bool m_canUpdateNumber;
	bool m_canSaveItemExpanded;
	
	static const int ExpandedRole;
	
	void clearChildren(QTreeWidgetItem* item) const;
	
	// Функция обхода дерева. Получает на вход вершину и глубину.
	// Возвращает true, если нужно обходить детей. Иначе false.
	typedef boost::function2<bool, QTreeWidgetItem*, int> TraverseFunction;
	typedef boost::function1<void, QTreeWidgetItem*> ReducedTraverseFunction;
	
	void traverseCurrentDepth(ReducedTraverseFunction function) const;
        static void traverseDepth(QTreeWidgetItem* item, ReducedTraverseFunction function, int depth);
	static void traverseWithDepth(QTreeWidgetItem* item, TraverseFunction function);
	
	int countChecked() const;
	
	// Expands items according to value stored in them
	void applyExpansion(QTreeWidgetItem* item);
	
	bool checkDepth(QTreeWidgetItem* item, QTreeWidgetItem* root, int depth) const;
	
	QPair<QTreeWidgetItem*, int> getCurrentRoot() const;
	
	QTreeWidgetItem* itemForIndex(Index index) const;
	
	//bool initialized() const;
	
	// Stores expansion value in item
	static void setItemExpanded(QTreeWidgetItem* item, bool expanded);
	// Gets expansion value from item
	static bool isItemExpanded(QTreeWidgetItem* item);
};

#endif // SEPARATECLASSDISPLAY_H
