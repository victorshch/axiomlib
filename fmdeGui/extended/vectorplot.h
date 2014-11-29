#ifndef VECTORPLOT_H
#define VECTORPLOT_H

#include <QtGui/QWidget>
#include <QColor>
#include <QResizeEvent>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_scale_widget.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_legend.h>
#include <qwt_plot_marker.h>
#include <vector>
#include "VectorQwtAdapter.h"
#include "MarkupQwtAdapter.h"
#include "SetLegend.h"

class VectorPlot : public QWidget
{
    Q_OBJECT

public:
    VectorPlot(QWidget *parent, const QString &title, SetLegend * externalLegend = 0);
    ~VectorPlot();
    void addLinePlot(const VectorQwtAdapter &adapter);
    void addDotPlot(const MarkupQwtAdapter & adapter,
    		QColor color,
    		const QString &name = QString(), bool addToLegend = false);

	void addMarking(const std::vector<int>& marking);
	
	void addPrelabelingPlot(const std::vector<double>& preLabeling);

	void addClippingMarkers();
	void addHorizontalLine(double value, QString label);
	void adjustMarkers(int left, int right);

    void setLegend(SetLegend * newLegend) {
    	legend = newLegend;
    }

    void commit();
	void clear();

	QwtDoubleRect boundingRect() const;
	
public slots:
	void zoom(const QwtDoubleRect& rect);

signals:
	void zoomed(const QwtDoubleRect& rect);

protected:
    void resizeEvent(QResizeEvent * event);
private:
	
    QwtPlot *plot;
    SetLegend *legend;
	
	QwtPlotMarker *marker1, *marker2;

	QwtPlotZoomer *zoomer;

	QwtDoubleRect mBoundingRect;
	
    void setupPlot(const QwtDoubleRect & rect);
	void resetPlot();
};

#endif // VECTORPLOT_H
