#include <QFileDialog>
#include <QMessageBox>

#include <boost/filesystem.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include "extaxiomsetbrowser.h"
#include "algorithm_controller/ManagedFuzzyDataController.h"
#include "stage3.h"
#include "plotbrowser.h"

#include "boost/serialization/serialization.hpp"

#include <QtDebug>
#include <QDebug>

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

	connect(ui.pushButton_showPlot, SIGNAL(clicked()), this, SLOT(showPlot()));
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
	
AxiomLib::AxiomExprSetPlus Stage3::loadAS(const QString &fileName) {
	QString extension = fileName.section(".", -1, -1);
	if(extension.toLower() == "as") {
		AxiomLib::AxiomExprSetStructure axiomSetStructure;

		QString baseFileName(fileName);
		baseFileName.chop(3);
			std::vector<std::string> paramNames = controller->paramNames();
			m_axoimSetBase.readFromAS(baseFileName.toStdString(), axiomSetStructure, paramNames);

		AxiomLib::AxiomExprSet result;
		result.initAxiomSetFromStructure(axiomSetStructure);
		return result;
	} else if (extension.toLower() == "xml") {
		AxiomLib::AxiomExprSetPlus asPlus;

		std::ifstream file(fileName.toStdString().c_str());
		std::string str((std::istreambuf_iterator<char>(file)),
						 std::istreambuf_iterator<char>());
		std::istringstream istr(str);
		if(file.good()) {
			boost::archive::xml_iarchive archive(istr);
			archive >> boost::serialization::make_nvp("AxiomExprSetPlus", asPlus);
		} else {
			qDebug() << "Could not open file " << fileName << " for reading";
		}
		return asPlus;
	} else {
		qCritical() << "Stage3::loadAS(): format " << extension << " not supported";
	}
}

void Stage3::loadAS() {
	QStringList asFiles = QFileDialog::getOpenFileNames(this,
		encode("Выберите файлы с системами аксиом"),
		QString::fromStdString(controller->getWorkDir()),
		encode("Сериализованные системы аксиом в xml ")+"(*.xml);;"
		+ encode("Системы аксиом в старом формате ")+"(*.as)");
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
        //controller->fuzzyMultiDataLearnAlgorithm.recalculateMatterAxiomSetFunc(axiomSets[i]);
        m_model->addAxiomSet(axiomSets[i]);
    }
}

void Stage3::showPlot()
{
	PlotBrowser* plotBrowser = new PlotBrowser(controller, 3, true, 0, AxiomLib::FuzzyDataSet::Testing);
	plotBrowser->setWindowModality(Qt::ApplicationModal);
	plotBrowser->setAttribute(Qt::WA_DeleteOnClose);

	QModelIndex currentIndex = ui.treeView_AxiomSets->currentIndex();
	if(currentIndex.isValid() && !currentIndex.parent().isValid()) {
		AxiomLib::AxiomExprSetPlus as = m_model->getAxiomSet(currentIndex.row());
		plotBrowser->setAxiomSet(as);
	}

	plotBrowser->replot();

	plotBrowser->showMaximized();
}

