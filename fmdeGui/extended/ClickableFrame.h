/****************************************************************************
*			ClickableFrame
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef CLICKABLEFRAME_H_
#define CLICKABLEFRAME_H_

#include <QWidget>
#include <QFrame>

class ClickableFrame: public QFrame {

	Q_OBJECT

protected:
	void mouseDoubleClickEvent ( QMouseEvent * event ) {
		QFrame::mouseDoubleClickEvent(event);
		emit doubleClicked();
	}

	void mousePressEvent(QMouseEvent * event) {
		QFrame::mousePressEvent(event);
		emit clicked();
	}

public:
	ClickableFrame(QWidget *parent = 0) : QFrame(parent) {}


signals:
	void clicked();
	void doubleClicked();
};

#endif /* CLICKABLEFRAME_H_ */
