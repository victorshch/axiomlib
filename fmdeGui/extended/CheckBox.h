/****************************************************************************
*			CheckBox
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include <QString>
#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <qwt_legend_item.h>

#include "ClickableFrame.h"

class CheckBox : public QWidget {
	Q_OBJECT

	QString name;

	QCheckBox *checkBox;
public:
	CheckBox(QWidget *parent, ClickableFrame *item, const QString &name): QWidget(parent) {
		QHBoxLayout * layout = new QHBoxLayout();
		checkBox = new QCheckBox();
		layout->addWidget(checkBox);
		layout->addWidget(item);
		this->setLayout(layout);

		this->name = name;

		connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(acceptCheckSignal(int)));
		connect(item, SIGNAL(clicked(void)), this, SLOT(acceptClickSignal(void)));
	}

	void setCheckState(Qt::CheckState state) {
		checkBox->setCheckState(state);
	}
private slots:
	void acceptCheckSignal(int state) {
		emit stateChanged(state, name);
	}
	void acceptClickSignal() {
		emit clicked(name);
	}

public slots:
	void select() {
		setCheckState(Qt::Checked);
	}

	void deselect() {
		setCheckState(Qt::Unchecked);
	}

signals:
	void stateChanged(int state, const QString &name);
	void clicked(const QString &name);
};

#endif /* CHECKBOX_H_ */
