/****************************************************************************
*			SetLegend
*
*	Description:
*	Author:		wictor
*	History:	
*
****************************************************************************/


#ifndef SETLEGEND_H_
#define SETLEGEND_H_

#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <QSet>
#include <QList>
#include <QPair>
#include <QString>
#include <QSignalMapper>
#include <QPushButton>
#include <qcheckbox.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

#include "ClickableFrame.h"
#include "CheckBox.h"
#include "utilities.h"

#include <QtDebug>

const QColor highlightColor = Qt::white;
const QColor nonHighlightColor = Qt::black;

class SetLegend: public QFrame {

	Q_OBJECT


	typedef QSet<QString> MapType;

	typedef QPair<QString, QwtPlotCurve *> LegendItemPair;
	typedef QMultiMap<QString, QwtPlotCurve *> ListType;

	MapType curvesMap;
	ListType curvesList;

	QSignalMapper *signalMapper, *signalMapperCheck;

	QScrollArea * itemsScrollArea;
	QVBoxLayout * itemsLayout;
	QFrame * itemsFrame;
public:
	SetLegend(QWidget *parent = 0);
	
	void commit();
	void clear();
	
	void insert(QwtLegendItemManager *newItem, QWidget *widget);
public slots:
	void highlightItem(const QString &text);
	void switchVisibility(int state, const QString &text);

signals:
	void itemClicked(const QString &text);
	void selectAll();
	void deselectAll();
};

#endif /* SETLEGEND_H_ */
