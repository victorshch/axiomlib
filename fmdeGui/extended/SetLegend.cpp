/****************************************************************************
*			SetLegend
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/

#include "SetLegend.h"

SetLegend::SetLegend(QWidget *parent) : QFrame(parent) {
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	signalMapper = new QSignalMapper(this);
	signalMapperCheck = new QSignalMapper(this);

	QPushButton *selectAllButton = new QPushButton(encode("Выбрать все"), 0);
	QPushButton *deselectAllButton = new QPushButton(encode("Отменить все"), 0);

	QVBoxLayout *topLevelLayout = new QVBoxLayout();

	itemsFrame = new QFrame;

	itemsLayout = new QVBoxLayout();
	itemsFrame->setLayout(itemsLayout);
	itemsScrollArea = new QScrollArea;
	//itemsScrollArea->setWidget(itemsFrame);

//		QVBoxLayout *stretchLayout = new QVBoxLayout();

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(selectAllButton);
	buttonsLayout->addWidget(deselectAllButton);

//		stretchLayout->addLayout(buttonsLayout);
//		stretchLayout->addStretch(1);

	topLevelLayout->addWidget(itemsScrollArea);
	//topLevelLayout->addLayout(itemsLayout);
	topLevelLayout->addLayout(buttonsLayout);
	//topLevelLayout->addStretch(0);

	this->setLayout(topLevelLayout);

	connect(selectAllButton, SIGNAL(clicked(void)), this, SIGNAL(selectAll(void)));
	connect(deselectAllButton, SIGNAL(clicked(void)), this, SIGNAL(deselectAll(void)));
}

void SetLegend::commit() {
	itemsLayout->addStretch(1);
	itemsScrollArea->setWidget(itemsFrame);
}

void SetLegend::clear() {
	clearLayout(itemsLayout);
	curvesList.clear();
	curvesMap.clear();
}

void SetLegend::highlightItem(const QString &text) {
	for(ListType::iterator it = curvesList.begin(); it != curvesList.end(); it++) {
		if(it.key() == text) {
			QwtSymbol symbol = it.value()->symbol();
			if(symbol.style() == QwtSymbol::Ellipse) {
				symbol.setStyle(QwtSymbol::Diamond);
			} else {
				symbol.setStyle(QwtSymbol::Ellipse);
			}
//			symbol.setPen(
//					highlightColor
//					);
			it.value()->setSymbol(symbol);
			//it->second->draw(0, it->second->dataSize());
		} else {
			QwtSymbol symbol = it.value()->symbol();
			symbol.setStyle(QwtSymbol::Ellipse);
			it.value()->setSymbol(symbol);
			//it->second->draw(0, it->second->dataSize());
		}
	}

	emit itemClicked(text);

	//qDebug()<<"Legend clicked";
}

void SetLegend::switchVisibility(int state, const QString & text) {
	bool visible = (state == Qt::Checked) ? true : false;
	Q_FOREACH(QwtPlotCurve * curve, curvesList.values(text)) {
		curve->setVisible(visible);
	}
}

void SetLegend::insert(QwtLegendItemManager *newItem, QWidget *widget) {
	QwtPlotCurve *newCurve = dynamic_cast<QwtPlotCurve *>(newItem);
	if (newCurve) {
		QString title = newCurve->title().text();
		curvesList.insert(title, newCurve);

		if(!curvesMap.contains(title)) {
			QHBoxLayout *buttonLayout = new QHBoxLayout();

			ClickableFrame *frame = new ClickableFrame();

			frame->setGeometry(widget->geometry());
			frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
			widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
			buttonLayout->addWidget(widget);

			frame->setLayout(buttonLayout);

			CheckBox * box = new CheckBox(this, frame, title);
			box->setCheckState(Qt::Checked);

			itemsLayout->addWidget(box);

			curvesMap.insert(title);

			//Задаем mapping для сигнала нажатия legend item
			//signalMapper->setMapping(frame, title);
			connect(box, SIGNAL(clicked(const QString&)), this, SLOT(highlightItem(const QString&)));
			connect(box,
					SIGNAL(stateChanged(int, const QString&)),
					this,
					SLOT(switchVisibility(int, const QString&))
					);

			connect(this, SIGNAL(selectAll(void)), box, SLOT(select(void)));
			connect(this, SIGNAL(deselectAll(void)), box, SLOT(deselect(void)));
//				signalMapperCheck->setMapping(frame, title);
//				connect(frame, SIGNAL(stateChanged(void)), signalMapperCheck, SLOT(map(void)));
		}
	} else {
		//TODO implement
		//QwtLegend::insert(newItem, widget);
	}
}
