#ifndef CLUSTERINGFORM_H
#define CLUSTERINGFORM_H

#include <QWidget>
#include <deque>
#include <utility>
#include "valuedsliderwidget.h"
#include "filtergroup.h"
#include "clusterizer.h"
#include "Condition.h"
#include "conditionmodel.h"
#include "algorithm_controller/forwarddecls.h"

namespace Ui {
    class ClusteringForm;
}

class ClusteringForm : public QWidget {
    Q_OBJECT
public:
    ClusteringForm(ManagedFuzzyDataController *controller, QWidget *parent = 0);
    ~ClusteringForm();
	
	void addCondition(PCondition condition);
	void clear();
	
	int conditionsCount() const;
	const Condition &condition(int number) const;
	bool conditionChecked(int number) const;
public slots:
	void clusterize();
signals:
	void clusteringRequested();
	void saveCheckedRequested();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ClusteringForm *ui;
	
	ManagedFuzzyDataController *controller;
	
	std::deque<std::pair<PCondition, Clusterizer> > conditionList;
	
	FilterGroup *filterGroup;
	
	ConditionModel *conditionModel;
	
	void setupHeader();
	
	//QVariant getValueByIndex(const Clusterizer &clusterizer, int index);
	
	void processClusterList();
	void fillTable();
	
	bool checkItem(const Clusterizer &clusterizer);
	bool checkItem(int row);
	void setupFilters();
	void setFilterInitialValue(double value);
private slots:
	void filterItems();
};

#endif // CLUSTERINGFORM_H
