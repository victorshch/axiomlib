#ifndef STAGE3_H
#define STAGE3_H

#include <QtGui/QWidget>

#include "AxiomSetBase.h"

#include "ui_stage3.h"
#include "algorithm_controller/forwarddecls.h"
#include "algorithm_controller/vectortypes.h"
#include "utilities.h"

#include "stagebase.h"

#include "asmodel.h"

class Stage3 : public StageBase
{
    Q_OBJECT

public:
    Stage3(QWidget *parent, ManagedFuzzyDataController *controller);
    ~Stage3();

	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool validatePage();
	void saveValuesToStage();
	void loadValuesFromStage();
public slots:
//	void treeItemClicked(QTreeWidgetItem *item);
	void loadAS();
	void showPlot();
	
private:
    Ui::stage3Class ui;

	ASModel* m_model;
	
	AxiomLib::AxiomSetBase m_axoimSetBase;
	
	void init();
	void buildTree();
	
	AxiomLib::AxiomExprSetPlus loadAS(const QString& fileName);

};

#endif // STAGE3_H
