#include "vectorplot.h"
#include "MarkupQwtAdapter.h"
#include <algorithm>
#include <QFont>
#include <qwt_symbol.h>
#include <qwt_legend_itemmanager.h>
#include <qwt_legend_item.h>

VectorPlot::VectorPlot(QWidget *parent, const QString &title, SetLegend * externalLegend)
    : QWidget(parent), legend(externalLegend)
{
	this->setMinimumHeight(200);
	
	QwtText titleText(title);
	QFont titleTextFont(titleText.font());
	titleTextFont.setPointSize(10);
	titleText.setFont(titleTextFont);
	plot = new QwtPlot(titleText, this);

	plot->setCanvasBackground(Qt::white);

	//plot->setGeometry(ui.widget_plot->geometry());

	plot->setAxisScale(QwtPlot::xBottom, 0, 0.01);
	plot->setAxisScale(QwtPlot::yLeft, 0, 0.01);

	for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
    	plot->setAxisAutoScale(i);
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)plot->axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)plot->axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
	plot->canvas()->setFrameStyle(QFrame::Box|QFrame::Plain);
	plot->setAutoReplot(true);

	//plot->setMinimumSize(QSize(200, 200));
	//this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	plot->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	
	QPen pen(QColor(255, 0, 0));
	
	marker1 = new QwtPlotMarker();
	marker1->setLineStyle(QwtPlotMarker::VLine);
	marker1->setLinePen(pen);
	marker1->setXValue(0);
	
	marker2 = new QwtPlotMarker();
	marker2->setLineStyle(QwtPlotMarker::VLine);
	marker2->setLinePen(pen);
	marker2->setXValue(0);	

	zoomer = 0;
}

VectorPlot::~VectorPlot()
{

}

void VectorPlot::addLinePlot(const VectorQwtAdapter &adapter) {
	setupPlot(adapter.boundingRect());
	QwtPlotCurve *curve = new QwtPlotCurve();

    QwtSymbol sym;
    sym.setStyle(QwtSymbol::Cross);
    sym.setPen(QColor(Qt::black));
    sym.setSize(2);
	curve->setSymbol(sym);
	curve->setPen(QColor(Qt::darkGreen));

	curve->setData(adapter);
	curve->attach(plot);
}

void VectorPlot::addDotPlot(const MarkupQwtAdapter&adapter,
		QColor color,
		const QString &name,
		bool addToLegend) {
	QwtPlotCurve *curve = new QwtPlotCurve(name);

    QwtSymbol sym;
    sym.setPen(QColor(color));
    sym.setBrush(QColor(color));
    sym.setStyle(QwtSymbol::Rect);
    sym.setSize(
			QSize(1, 20)
			);
	curve->setSymbol(sym);
	curve->setStyle(QwtPlotCurve::NoCurve);

	if(adapter.size() > 0) {
		setupPlot(adapter.boundingRect());
		curve->setData(adapter);
		curve->attach(plot);
	}

	if(!legend) {
		return;
	}

    // Добавляем кривую в легенду
	QwtLegendItem *item = new QwtLegendItem(0);
	item->setText(curve->title());
	item->setSymbol(sym);
	item->setIdentifierMode(QwtLegendItem::ShowLine | QwtLegendItem::ShowSymbol | QwtLegendItem::ShowText);
	legend->insert(curve, item);
}

void VectorPlot::addMarking(const std::vector<int> &marking)
{
	QwtDoubleRect boundingRect(0, 1, marking.size() - 1, 2);

	setupPlot(boundingRect);

	// this is gonna be slow...
	for(unsigned i = 0; i < marking.size(); ++i) {
		QwtPlotMarker* marker = new QwtPlotMarker();
		QwtText text;
		text.setText(QString::number(marking[i]));

		marker->setLabel(text);

		marker->setXValue((double)i);
		marker->setYValue(0.0);

		marker->attach(plot);
	}
}

void VectorPlot::addPrelabelingPlot(const std::vector<double> &preLabeling)
{
	QwtDoubleRect boundingRect(0, 1, preLabeling.size() - 1, 2);

	setupPlot(boundingRect);

	QwtPlotCurve* curve = new QwtPlotCurve;

	QwtArray<double> xData, yData;
	for(unsigned i = 0; i < preLabeling.size(); ++i) {
		xData.push_back(i);
		yData.push_back(preLabeling[i] >= 0 ? preLabeling[i] : 1);
	}

	curve->setData(xData, yData);

	curve->attach(plot);
}

void VectorPlot::addClippingMarkers()
{
	marker1->attach(plot);
	marker2->attach(plot);
}

void VectorPlot::addHorizontalLine(double value, QString labelText)
{
	QPen pen(QColor(0, 0, 255));

	QwtPlotMarker* horizontalLineMarker = new QwtPlotMarker();
	horizontalLineMarker->setLineStyle(QwtPlotMarker::HLine);
	horizontalLineMarker->setLinePen(pen);
	horizontalLineMarker->setYValue(value);

	QwtText label;
	label.setText(labelText);
	QFont font = label.font();
	font.setPointSize(9);
	label.setFont(font);

	horizontalLineMarker->setLabel(label);
	horizontalLineMarker->setLabelAlignment(Qt::AlignTop | Qt::AlignLeft);

	horizontalLineMarker->attach(plot);
}

void VectorPlot::setupPlot(const QwtDoubleRect &rect) {
	QwtScaleDiv *hDiv = plot->axisScaleDiv(QwtPlot::xBottom);
	QwtScaleDiv *vDiv = plot->axisScaleDiv(QwtPlot::yLeft);
// In earlier qwt version some member function names are different from newer version
#if QWT_VERSION <= 0x050101
	double lLimit = std::min(hDiv->lBound(), rect.left());
	double rLimit = std::max(hDiv->hBound(), rect.right());
	double bLimit = std::min(vDiv->lBound(), rect.bottom());
	double tLimit = std::max(vDiv->hBound(), rect.top());
#else
	double lLimit = std::min(hDiv->lowerBound(), rect.left());
	double rLimit = std::max(hDiv->upperBound(), rect.right());
	double bLimit = std::min(vDiv->lowerBound(), rect.bottom());
	double tLimit = std::max(vDiv->upperBound(), rect.top());
#endif
	plot->setAxisScale(QwtPlot::xBottom, lLimit, rLimit);
	plot->setAxisScale(QwtPlot::yLeft, bLimit, tLimit);

	double xMargin = plot->invTransform(QwtPlot::xBottom, plot->margin());
	double yMargin = plot->invTransform(QwtPlot::yLeft, plot->margin());
	mBoundingRect = QwtDoubleRect(QwtDoublePoint(lLimit - xMargin, tLimit + yMargin),
								  QwtDoublePoint(rLimit + xMargin, bLimit - yMargin));

	qDebug() << "xMargin:" << xMargin;
	qDebug() << "yMargin:" << yMargin;
	qDebug() << "mBoundingRect:" << mBoundingRect;
//	mBoundingRect = rect;
}

void VectorPlot::resetPlot() {
	plot->setAxisScale(QwtPlot::xBottom, 0, 0);
	plot->setAxisScale(QwtPlot::yLeft, 0, 0);	
}

void VectorPlot::adjustMarkers(int left, int right) {
	marker1->setXValue(left);
	marker2->setXValue(right);
}

void VectorPlot::commit() {
	if(zoomer == 0) {
		zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot->canvas());
	
		zoomer->setRubberBand(QwtPicker::RectRubberBand);
		zoomer->setRubberBandPen(QColor(Qt::green));
		zoomer->setTrackerMode(QwtPicker::ActiveOnly);
		zoomer->setTrackerPen(QColor(Qt::white));

		connect(zoomer, SIGNAL(zoomed(QwtDoubleRect)), this, SIGNAL(zoomed(QwtDoubleRect)));

		zoomer->setEnabled(true);			
	}
	
    plot->replot();
}

void VectorPlot::clear() {
	marker1->detach();
	marker2->detach();
	plot->detachItems(QwtPlotItem::Rtti_PlotCurve);
	plot->detachItems(QwtPlotItem::Rtti_PlotMarker);
	delete zoomer;
	zoomer = 0;
	resetPlot();
}

QwtDoubleRect VectorPlot::boundingRect() const
{
	return mBoundingRect;
}

void VectorPlot::zoom(const QwtDoubleRect &rect)
{
	if(zoomer) {
		zoomer->zoom(rect);
	}
}

void VectorPlot::resizeEvent(QResizeEvent * event) {
	QSize newPlotSize(event->size());
	newPlotSize.setHeight(newPlotSize.height());
	newPlotSize.setWidth(newPlotSize.width());
	plot->resize(newPlotSize);
}
