#ifndef STAGE2_H
#define STAGE2_H

#include <boost/function.hpp>

#include <QtGui/QWizardPage>
#include "ui_stage2.h"
#include "algorithm_controller/forwarddecls.h"
#include "algorithm_controller/vectortypes.h"
#include "VectorQwtAdapter.h"
#include "filtergroup.h"
#include "interval.h"
#include "Index.h"
#include "clusteringform.h"
#include "selectionmanager.h"
#include "StrategyFrame.h"
#include "stagebase.h"

class SeparateClassDisplay;
class ClassSelectForm;

class Stage2 : public StageBase
{
    Q_OBJECT

public:
    Stage2(QWidget *parent, ManagedFuzzyDataController *controller);
    ~Stage2();

	virtual void initializePage();

	void saveValuesToStage();
	void loadValuesFromStage();
private:
    Ui::Stage2Class ui;

	FilterGroup *filterGroup;
	
	ClusteringForm *clusteringForm;
		
	std::vector<std::vector<AxiomLib::AxiomExprPlus> > allAxioms;
	
	SelectionManager *selectionManager;

	ClassSelectForm* m_classSelectForm;
	
	SeparateClassDisplay* m_separateClassDisplay;
	
	void init();

	void initFreqFilter();
	void initFreqFilterSliders();	
	
	void setupHeuristics();
	void setupParams();
	
	void computeIntervals();
	
	DoubleInterval statAbnormInterval;
	DoubleInterval statNormalInterval;
	DoubleInterval statVariationInterval;
	DoubleInterval statOccurenceInterval;
	DoubleInterval diffInterval;

	void filterItems();
	bool checkFilter(const AxiomLib::AxiomExprPlus &ax);
	
	AxiomLib::AxiomExprPlus getAxiomByIndex(const Index& index) const;
	
public slots:
	void buildNodeForClass(int classNo, QTreeWidgetItem* item);
	void reloadSelection();
	void saveClass(int classNo, QTreeWidgetItem* item);

	void calculate();
	
	void filterChanged(int id, double value, bool direction);
	
};
#endif // STAGE2_H
