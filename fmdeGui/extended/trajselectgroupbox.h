#ifndef TRAJSELECTGROUPBOX_H
#define TRAJSELECTGROUPBOX_H

#include <vector>

#include <QGroupBox>

#include "algorithm_controller/forwarddecls.h"

namespace Ui {
    class TrajSelectGroupBox;
}

class TrajSelectGroupBox : public QGroupBox {
    Q_OBJECT
public:
    TrajSelectGroupBox(QWidget *parent, 
					   ManagedFuzzyDataController *controller, 
					   bool showDimension = false,
					   bool showTime = false);
    ~TrajSelectGroupBox();
		
	// не считая нормальное поведение
	int numClasses();
	int numTS(int classNo);
	int numDimensions(int classNo, int tsNo);

	// нумерация от 0, -1 - нормальное поведение
	int currentClass();
	// нумерация от 0
	int currentTS();
	// нумерация от 0
	int currentDimension();
public slots:
	TrajSelectGroupBox *setCurrentTS(int currentClass, int currentTS, int currentDimension);
	
signals:
	void trajSelected(int classNo, int tsNo, int dimNo);
	
protected:
    void changeEvent(QEvent *e);

private:
    Ui::TrajSelectGroupBox *ui;
	
	std::vector<std::vector<int> > abnormalTSSizes;
	
	std::vector<int> normalTSSizes;
	
	std::vector<std::string> paramNames;
	
	bool showTime;
	
	void fillTSSizes(ManagedFuzzyDataController *controller);
	
	void fillClassComboBox();
	void fillTSComboBox(int classNo);
	void fillDimensionComboBox(int classNo, int tsNo);
	void wireUp();
	void unWire();
private slots:
	
	void classSelected(int index);
	void tsSelected(int index);
	void dimensionSelected(int index);
};

#endif // TRAJSELECTGROUPBOX_H
