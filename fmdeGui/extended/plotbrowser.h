#ifndef PLOTBROWSER_H
#define PLOTBROWSER_H

#include <QtGui/QWidget>
#include <qmessagebox.h>
#include <list>
#include <vector>
#include <string>
#include <map>
#include "ui_plotbrowser.h"
#include "vectorplot.h"
#include "ElemCondPlus.h"
#include "Condition.h"
#include "intervalwidget.h"
#include "trajselectgroupbox.h"
#include "algorithm_controller/forwarddecls.h"

class PlotBrowser : public QWidget
{
    Q_OBJECT

public:	
	PlotBrowser(ManagedFuzzyDataController *controller, int stage, QWidget *parent = 0);
	
	void addCondition(PCondition condition);
	
	void setInterval(int left, int right);
	
    ~PlotBrowser();
public slots:
	void replot() const;
	
signals:
	void intervalChanged(int left, int right);
private:
    Ui::PlotBrowserClass ui;

	ManagedFuzzyDataController *controller;
	
	int stage;
	
	QList<VectorPlot *> vectorPlotList;
	QList<PCondition> conditionList;
	
	IntervalWidget *intervalWidget;
	
	TrajSelectGroupBox *trajSelectGroupBox;
	
	QHBoxLayout *topLevelLayout;
	
	QVBoxLayout *plotsLayout;
	
	SetLegend * legend;
	
	QScrollArea *scrollAreaLegend;
private slots:
	void adjustInterval(int left, int right);
	
	void displayMultiTS(int classNo, int multiTSNo);
};

#endif // PLOTBROWSER_H
