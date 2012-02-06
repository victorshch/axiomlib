/****************************************************************************
*			utilities
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/

#include "utilities.h"
#include <qmessagebox.h>
#include <QTextCodec>
#include <cmath>

void displayException(const AxiomLib::AxiomLibException &exception) {
	QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
}

void displayException(const std::exception &exception) {
	QMessageBox::critical(0, "std::excepion", exception.what());
}

QString encode(const QByteArray &array) {
	/* static */ QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
	return codec->toUnicode(array);
}

bool doubleEq(double a, double b) {
	return std::abs(a - b) < eps;
}

void clearLayout(QLayout *layout) {
	if(layout->count() > 0) {
		QLayoutItem *child;
		 while ((child = layout->takeAt(0)) != 0) {
			 if(child->widget() != 0) {
				 child->widget()->hide();
			 }
			 delete child;
		 }
	}
}

std::string getFullECTypeName(const AxiomLib::ElemCondPlus &ecPlus) {
	return ecPlus.ecTypeName();
}
