#ifndef STAGE1_H
#define STAGE1_H

#include <QObject>
#include <QtGui/QWidget>
#include <QObjectCleanupHandler>
#include <QMetaObject>
#include "ui_stage1.h"
#include "algorithm_controller/forwarddecls.h"
#include "algorithm_controller/vectortypes.h"
#include "VectorQwtAdapter.h"
#include "selectionmanager.h"
#include "Index.h"

#include "interval.h"
#include "filtergroup.h"
#include "trajselectgroupbox.h"
#include "clusteringform.h"
#include "ClassSelectForm.h"
#include "SeparateClassDisplay.h"
#include "stagebase.h"

class Stage1 : public StageBase
{
    Q_OBJECT

public:
    Stage1(QWidget *parent, ManagedFuzzyDataController *controller);
    ~Stage1();

	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool validatePage();
	void saveValuesToStage();
	void loadValuesFromStage();
private:
    Ui::stage1Class ui;
	
	ECMultiVector& ecs;
	
	SeparateClassDisplay* m_separateClassDisplay;
	
	FilterGroup *m_filterGroup;
	
	ClusteringForm *m_clusteringForm;
	
	SelectionManager *m_selectionManager;
	
	ParamsForm* m_paramsForm;
	
	ClassSelectForm* m_classSelectForm;
	
	void init();
	void buildTree();
	void initItem(QTreeWidgetItem *item, 
				  const QString &text, 
				  const AxiomLib::ElemCondPlusStat &ecPlus, 
				  bool checked);
	void initFreqFilter();
	void initFreqFilterSliders();
	
	DoubleInterval statAbnormInterval, statNormalInterval,
		diffInterval, occurrenceInterval;
	void computeIntervals();
	
	bool keepItem(const AxiomLib::ElemCondPlusStat &ec);
	
	void setupParams();
	void setupHeuristics();
	
	const AxiomLib::ElemCondPlusStat& getECByIndex(Index index) const;
	
private slots:
	void buildNodeForClass(int classNo, QTreeWidgetItem* item);
	void reloadSelection();
	void saveClass(int classNo, QTreeWidgetItem* item);	
	
public slots:
	
	void filterItems();
	
	void calculate();
	
	//void itemClicked(QTreeWidgetItem *item, int column);
	
	//void ECChanged(AxiomLib::ElemCondPlus, const QObject* identifier);

};

#endif // STAGE1_H
