#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QDebug>

#include <boost/algorithm/string.hpp>

#include "extaxiomsetbrowser.h"
#include "ClickableFrame.h"
#include "extaxiombrowser.h"
#include "QtGuiException.h"

#include "undefs.h"

ExtAxiomSetBrowser::ExtAxiomSetBrowser(QWidget *parent, 
									   const AxiomLib::AxiomExprSetPlus &axiomSet, 
									   QObject *identifier,
									   ManagedFuzzyDataController *controller)
    : QWidget(parent), axiomSet(axiomSet), identifier(identifier)
{
	ui.setupUi(this);

	this->controller = controller;
	
	this->setAttribute(Qt::WA_DeleteOnClose, true);

	mapper = new QSignalMapper(this);
	
	displayAxioms();
	displayMarkups();
	
	connect(mapper, SIGNAL(mapped(int)), this, SLOT(displayAxiom(int)));
	connect(ui.pushButton_saveMarkUps, SIGNAL(clicked()), 
			this, SLOT(saveMarkup()));
	connect(ui.pushButton_recalculateMarkups, SIGNAL(clicked()), 
			this, SLOT(recalculateAxiomSet()));
}

ExtAxiomSetBrowser::~ExtAxiomSetBrowser()
{

}

void ExtAxiomSetBrowser::displayAxiom(int number) {
	PAxiomExpr axiomExpr = PAxiomExpr(axiomSet.axioms[number]->clone());
	ExtAxiomBrowser *browser = new ExtAxiomBrowser(0, axiomExpr, controller);
	browser->show();
}

void ExtAxiomSetBrowser::displayMarkups()
{
	std::vector<std::vector<int> >& markups = axiomSet.markUps;
	
	qDebug()<<"Number of markups: "<<markups.size();
	
	editors.clear();
	clearLayout(ui.verticalLayout_markups);
	
	for(int i = 0; i < (int) markups.size(); i++) {
		QLabel *label = new QLabel(QString::number(i+1)+": ", 0);
		QLineEdit *lineEdit = new QLineEdit(
				markupToString(markups[i]),
				0
				);
		
		editors.push_back(lineEdit);
		
		QHBoxLayout * layout = new QHBoxLayout();
		
		layout->addWidget(label);
		layout->addWidget(lineEdit);
		
		ui.verticalLayout_markups->addLayout(layout);
	}
}

void ExtAxiomSetBrowser::displayAxioms() {
	std::vector<AxiomLib::AxiomExpr *>& axioms = axiomSet.axioms;
	
	clearLayout(ui.verticalLayout_axioms);	
	
	for(int i = 0; i < (int) axioms.size(); i++) {
		ClickableFrame *frame = new ClickableFrame(0);
		QVBoxLayout *frameLayout = new QVBoxLayout();

		QLabel *label = new QLabel();
		label->setText(QString::number(i)+QString(": ")+
					axioms[i]->name().c_str()
				);
		frameLayout->addWidget(label);
		frame->setLayout(frameLayout);

		ui.verticalLayout_axioms->addWidget(frame);

		mapper->setMapping(frame, i);
		connect(frame, SIGNAL(clicked(void)), mapper, SLOT(map(void)));
	}
}

void ExtAxiomSetBrowser::saveMarkup() {
	try
	{
		std::vector<std::vector<int> > newMarkup(axiomSet.axioms.size());
		for(int i = 0; i < (int)axiomSet.axioms.size(); i++) {
			newMarkup[i] = stringToMarkup(
					editors[i]->text()
					);
		}
		axiomSet.markUps = newMarkup;
		
		recalculateAxiomSet();
	}
	catch(AxiomLib::AxiomLibException exception)
	{
		displayException(exception);
	}
	catch(std::exception exception)
	{
		displayException(exception);
	}
}

void ExtAxiomSetBrowser::recalculateAxiomSet() {	
	emit asChanged(axiomSet, identifier);	
}

QString ExtAxiomSetBrowser::markupToString(const std::vector<int>& markup) {
	std::vector<std::string> markupStr;
	markupStr.reserve(markup.size());
	
	std::transform(markup.begin(),
				   markup.end(),
				   std::back_inserter(markupStr),
				   boost::lexical_cast<std::string, int>);
	
	std::string result = boost::algorithm::join(markupStr, std::string(" "));
	
	return QString::fromStdString(result);
}

std::vector<int> ExtAxiomSetBrowser::stringToMarkup(const QString &str) {
	std::list<std::string> strings;
	std::string input = str.simplified().toStdString();
	boost::algorithm::split(strings, input, boost::algorithm::is_any_of(" "));
	std::vector<int> result;
	result.reserve(strings.size());
	
	std::transform(
		strings.begin(), strings.end(),
		std::back_inserter(result),
		boost::lexical_cast<int, std::string>
	);
	
	for(int i = 0; i < (int)result.size(); i++)
	{
		if(result[i] < 0 || result[i] >= (int) axiomSet.axioms.size())
		{
			throw QtGuiException("Invalid axiom number "+boost::lexical_cast<std::string>(i));
		}
	}
	
	return result;
}
