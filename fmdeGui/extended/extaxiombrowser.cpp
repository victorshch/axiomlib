#include "extaxiombrowser.h"
#include "extecbrowser.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QDebug>



ExtAxiomBrowser::ExtAxiomBrowser(QWidget *parent, PAxiomExpr axiomExpr, ManagedFuzzyDataController *controller)
    : QWidget(parent), axiomExpr(axiomExpr)
{
	ui.setupUi(this);
	
	this->controller = controller;

	this->setAttribute(Qt::WA_DeleteOnClose, true);

	QSignalMapper * mapper = new QSignalMapper(this);

	QHBoxLayout * topLevelLayout = new QHBoxLayout();

	for(int i = 0; i < (int)axiomExpr->expression.size(); i++) {
		QVBoxLayout *conjunctionLayout = new QVBoxLayout();
		for(int j = 0; j <(int) axiomExpr->expression[i].size(); j++) {
			QPushButton *button = new QPushButton();
//			QHBoxLayout *buttonLayout = new QHBoxLayout();
//			button
			button->setText(
					axiomExpr->expression[i][j].elemCondition->name().c_str()
					);
			conjunctionLayout->addWidget(button);

			//mapper->setMapping(button, new IndexPair(mapper, i, j));
			//todo remove
			connect(button, SIGNAL(clicked(void)), mapper, SLOT(map(void)));
		}
		topLevelLayout->addLayout(conjunctionLayout);
	}

	connect(mapper, SIGNAL(mapped(QObject *)), this, SLOT(displayEC(QObject *)));

	setLayout(topLevelLayout);
}

ExtAxiomBrowser::~ExtAxiomBrowser()
{

}

void ExtAxiomBrowser::displayEC(QObject *indexPairObject) {
	// todo remove
	//IndexPair * indexPair = dynamic_cast<IndexPair *>(indexPairObject);
//	if(indexPair) {
//		qDebug()<<"constructing ECBrowser";
//		ExtECBrowser * ecBrowser = new ExtECBrowser(0, controller,
//				axiomExpr->expression[indexPair->i][ indexPair->j],
//				QString((axiomExpr->nameOfAxiomExpr+"_"+boost::lexical_cast<std::string>(indexPair->i)
//				+"_"+boost::lexical_cast<std::string>(indexPair->j)).c_str()),
//				indexPair
//				);
//		qDebug()<<"showing ECBrowser";
//		ecBrowser->show();
//		qDebug()<<"ecBrowser shown";
//	}
}
