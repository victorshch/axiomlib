/***************************************************************************
*					AxiomBrowser
*
*	Description:	Класс, отражающий функциональность окна с описанием 
*					аксиомы и ее параметрами
*	Author:			dk
*	History:
*
****************************************************************************/
#ifndef AXIOM_BROWSER_H
#define AXIOM_BROWSER_H

#include "ui_AxiomBrowser.h"
#include "AxiomSetStructure.h"
#include "AxiomStructure.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include <QWidget>
#include <QString>
#include <QTextStream>

class AxiomBrowser : public QWidget, public Ui::aBrowser {

	Q_OBJECT

 private:

 public:
	
	// Конструктор класса
	AxiomBrowser (AxiomStructure &as);
	// Деструктор класса
	~AxiomBrowser();

 public slots:
	
}; // end of class

#endif // AXIOM_BROWSER_H
