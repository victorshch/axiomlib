#include <QApplication>

#include "selectionmanager.h"
#include "ui_selectionmanager.h"

SelectionManager::SelectionManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectionManager)
{
    ui->setupUi(this);
	
	ui->comboBox_selection->setEditText("sel_1");
	
	ui->toolButton_saveSelection->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveFDIcon));
	ui->toolButton_loadSelection->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
	
	connect(ui->toolButton_loadSelection, SIGNAL(clicked()), this, SLOT(loadSelection()));
	connect(ui->toolButton_saveSelection, SIGNAL(clicked()), this, SLOT(saveSelection()));
	
	connect(ui->pushButton_selectAll, SIGNAL(clicked()), this, SIGNAL(selectAllRequested()));
	connect(ui->pushButton_deselectAll, SIGNAL(clicked()), this, SIGNAL(deselectAllRequested()));
}

QString SelectionManager::currentSelectionName() const {
	return ui->comboBox_selection->currentText();
}

QList<Index >& SelectionManager::currentIndexList() {
	return indexMap[currentSelectionName()];
}

int SelectionManager::indexCount() {
	return currentIndexList().size();
}

Index SelectionManager::index(int number) {
	return currentIndexList()[number];
}

void SelectionManager::addIndex(Index index) {
	currentIndexList().push_back(index);
}

void SelectionManager::clear() {
	ui->comboBox_selection->clear();
	ui->comboBox_selection->setEditText("sel_1");
	indexMap.clear();
}

void SelectionManager::saveSelection() {
	if(ui->comboBox_selection->findText(currentSelectionName()) < 0) {
		ui->comboBox_selection->insertItem(0, currentSelectionName());
	}
	emit saveSelectionRequested();
}

void SelectionManager::loadSelection() {
	emit loadSelectionRequested();
}

SelectionManager::~SelectionManager()
{
    delete ui;
}

void SelectionManager::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QList<Index> SelectionManager::currentIndexList() const {
	return indexMap[currentSelectionName()];
}
