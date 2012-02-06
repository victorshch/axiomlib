/****************************************************************************
*			stage0
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/

#ifndef STAGE0_H_
#define STAGE0_H_

#include <QtGui/QWizardPage>
#include <vector>
#include "ui_stage0.h"
#include "algorithm_controller/forwarddecls.h"
#include "algorithm_controller/vectortypes.h"
#include <algorithm>
#include "utilities.h"
#include "stagebase.h"

class Stage0: public StageBase
{
	Q_OBJECT
public:
	Stage0(QWidget *parent = 0, ManagedFuzzyDataController *controller = 0);
	~Stage0();
	//virtual int nextId() const;
	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool validatePage();

	void saveValuesToStage();
	void loadValuesFromStage();
signals:

private:
	Ui::WizardPageStage0 ui;

	ParamsForm* paramsForm;
	
	std::vector<AxiomLib::ElemCondPlus> allECTypes;
public slots:
	void selectAll();
	void deselectAll();
	void calculate();
private slots:
	void displayPlots();
};

#endif /* STAGE0_H_ */
