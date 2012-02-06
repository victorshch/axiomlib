#include <QFileDialog>
#include <QMessageBox>

#include "extaxiomsetbrowser.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "stage3.h"

Stage3::Stage3(QWidget *parent, ManagedFuzzyDataController *controller)
	: StageBase(parent, controller, 3)
{
	ui.setupUi(this);
	
	m_model = new ASModel(controller, this);
	
	ui.treeView_AxiomSets->setModel(m_model);
	ui.treeView_AxiomSets->setRootIndex(QModelIndex());
	
	connect(ui.treeView_AxiomSets, SIGNAL(doubleClicked(QModelIndex)), m_model, SLOT(itemClicked(QModelIndex)));
	
	connect(ui.pushButton_loadAS, SIGNAL(clicked()), this, SLOT(loadAS()));
	
	//ui.treeWidget_AxiomSets->setColumnCount(4);
	
	//connect(ui.treeWidget_AxiomSets, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(treeItemClicked(QTreeWidgetItem *)));
}

Stage3::~Stage3()
{
	//delete m_model;
}

void Stage3::initializePage() {
	//controller->createStage3();
	init();
}

void Stage3::cleanupPage() {
	
}

bool Stage3::validatePage() {
	return true;
}

void Stage3::saveValuesToStage() {
	
}

void Stage3::loadValuesFromStage() {
	loadParams();
	//controller->stage3.setCalculated(true);
	init();
}

void Stage3::init() {
	buildTree();
}

void Stage3::buildTree() {
	AxiomSetVector &bestAxiomSets = controller->stage3.bestAxiomSets;
	
	m_model->clear();
	
	std::for_each(bestAxiomSets.begin(), bestAxiomSets.end(), 
	              [this](AxiomLib::AxiomExprSetPlus& as) { this->m_model->addAxiomSet(as); }
	);	
}
	
AxiomLib::AxiomExprSet Stage3::loadAS(const QString &fileName) {
	AxiomLib::AxiomExprSetStructure axiomSetStructure;
	
	QString baseFileName(fileName);
	baseFileName.chop(3);
        std::vector<std::string> paramNames = controller->paramNames();
        m_axoimSetBase.readFromAS(baseFileName.toStdString(), axiomSetStructure, paramNames);
	
	AxiomLib::AxiomExprSet result;
	result.initAxiomSetFromStructure(axiomSetStructure);
	return result;
}

void Stage3::loadAS() {
	QStringList asFiles = QFileDialog::getOpenFileNames(this, encode("Выберите файлы с системами аксиом"), QString(), encode("Системы аксиом ")+"(*.as)");
	if(asFiles.empty()) {
		return;
	}
	
	auto& axiomSets = controller->stage3.bestAxiomSets;
	int oldSize = axiomSets.size();
	
	axiomSets.reserve(
	            axiomSets.size() + asFiles.size()
	            );
	try {
		for(int i = 0; i < asFiles.size(); i++) {
			AxiomLib::AxiomExprSetPlus as = loadAS(asFiles[i]);
			axiomSets.push_back(as);
		}
	} catch (AxiomLib::AxiomLibException e) {
		axiomSets.resize(oldSize);
		QMessageBox::warning(0, encode("Ошибка загрузки системы аксиом"), QString::fromStdString(e.error()));
		return;
	} catch (...) {
		axiomSets.resize(oldSize);
		throw;
	}
	
	for(int i = oldSize; i < axiomSets.size(); i++) {
		controller->fuzzyMultiDataLearnAlgorithm.recalculateMatterAxiomSetFunc(axiomSets[i]);
		m_model->addAxiomSet(axiomSets[i]);
	}
}

