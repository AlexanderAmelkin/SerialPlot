/*
  Copyright © 2015 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QRectF>
#include <QKeySequence>
#include <QColor>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include <math.h>

#include "plot.h"
#include "utils.h"

static const int SYMBOL_SIZE = 5;
static const int SYMBOL_SHOW_AT_WIDTH = 15;

Plot::Plot(QWidget* parent) :
    QwtPlot(parent),
    zoomer(this->canvas(), false),
    sZoomer(this, &zoomer),
    showGridAction("Grid", this),
    showMinorGridAction("Minor Grid", this),
    unzoomAction("Unzoom", this),
    darkBackgroundAction("Dark Background", this),
    showLegendAction("Legend", this)
{
    isAutoScaled = true;
    isSymbolsOn = true;

    QObject::connect(&zoomer, &Zoomer::unzoomed, this, &Plot::unzoomed);

    zoomer.setZoomBase();
    grid.attach(this);
    legend.attach(this);

    showGrid(false);
    darkBackground(false);

    showGridAction.setToolTip("Show Grid");
    showMinorGridAction.setToolTip("Show Minor Grid");
    unzoomAction.setToolTip("Unzoom the Plot");
    darkBackgroundAction.setToolTip("Enable Dark Plot Background");
    showLegendAction.setToolTip("Display the Legend on Plot");

    showGridAction.setShortcut(QKeySequence("G"));
    showMinorGridAction.setShortcut(QKeySequence("M"));

    showGridAction.setCheckable(true);
    showMinorGridAction.setCheckable(true);
    darkBackgroundAction.setCheckable(true);
    showLegendAction.setCheckable(true);

    showGridAction.setChecked(false);
    showMinorGridAction.setChecked(false);
    darkBackgroundAction.setChecked(false);
    showLegendAction.setChecked(true);

    showMinorGridAction.setEnabled(false);

    connect(&showGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &Plot::showGrid);
    connect(&showGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            &showMinorGridAction, &QAction::setEnabled);
    connect(&showMinorGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &Plot::showMinorGrid);
    connect(&unzoomAction, &QAction::triggered, this, &Plot::unzoom);
    connect(&darkBackgroundAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &Plot::darkBackground);
    connect(&showLegendAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            [this](bool enabled){legend.setVisible(enabled); replot();});

    connect(&zoomer, &QwtPlotZoomer::zoomed,
            [this](const QRectF &rect)
            {
                onXScaleChanged();
            });

    connect(this, &QwtPlot::itemAttached,
            [this](QwtPlotItem *plotItem, bool on)
            {
                if (isSymbolsOn) updateSymbols();
            });

    snapshotOverlay = NULL;
}

Plot::~Plot()
{
    if (snapshotOverlay != NULL) delete snapshotOverlay;
}

void Plot::setAxis(bool autoScaled, double yAxisMin, double yAxisMax)
{
    this->isAutoScaled = autoScaled;

    if (!autoScaled)
    {
        yMin = yAxisMin;
        yMax = yAxisMax;
    }

    zoomer.zoom(0);
    resetAxes();
}

QList<QAction*> Plot::menuActions()
{
    QList<QAction*> actions;
    actions << &showGridAction;
    actions << &showMinorGridAction;
    actions << &unzoomAction;
    actions << &darkBackgroundAction;
    actions << &showLegendAction;
    return actions;
}

void Plot::resetAxes()
{
    if (isAutoScaled)
    {
        setAxisAutoScale(QwtPlot::yLeft);
    }
    else
    {
        setAxisScale(QwtPlot::yLeft, yMin, yMax);
    }

    replot();
}

void Plot::unzoomed()
{
    setAxisAutoScale(QwtPlot::xBottom);
    resetAxes();
}

void Plot::showGrid(bool show)
{
    grid.enableX(show);
    grid.enableY(show);
    replot();
}

void Plot::showMinorGrid(bool show)
{
    grid.enableXMin(show);
    grid.enableYMin(show);
    replot();
}

void Plot::unzoom()
{
    zoomer.zoom(0);
}

void Plot::darkBackground(bool enabled)
{
    QColor gridColor;
    if (enabled)
    {
        setCanvasBackground(QBrush(Qt::black));
        gridColor.setHsvF(0, 0, 0.25);
        grid.setPen(gridColor);
        zoomer.setRubberBandPen(QPen(Qt::white));
        zoomer.setTrackerPen(QPen(Qt::white));
        sZoomer.setPickerPen(QPen(Qt::white));
        legend.setTextPen(QPen(Qt::white));
    }
    else
    {
        setCanvasBackground(QBrush(Qt::white));
        gridColor.setHsvF(0, 0, 0.80);
        grid.setPen(gridColor);
        zoomer.setRubberBandPen(QPen(Qt::black));
        zoomer.setTrackerPen(QPen(Qt::black));
        sZoomer.setPickerPen(QPen(Qt::black));
        legend.setTextPen(QPen(Qt::black));
    }
    replot();
}

/*
  Below crude drawing demostrates how color selection occurs for
  given channel index

  0°                     <--Hue Value-->                           360°
  |* . o . + . o . * . o . + . o . * . o . + . o . * . o . + . o . |

  * -> 0-3
  + -> 4-7
  o -> 8-15
  . -> 16-31

 */
QColor Plot::makeColor(unsigned int channelIndex)
{
    auto i = channelIndex;

    if (i < 4)
    {
        return QColor::fromHsv(360*i/4, 255, 230);
    }
    else
    {
        double p = floor(log2(i));
        double n = pow(2, p);
        i = i - n;
        return QColor::fromHsv(360*i/n + 360/pow(2,p+1), 255, 230);
    }
}

void Plot::flashSnapshotOverlay()
{
    if (snapshotOverlay != NULL) delete snapshotOverlay;

    QColor color;
    if (darkBackgroundAction.isChecked())
    {
        color = QColor(Qt::white);
    }
    else
    {
        color = QColor(Qt::black);
    }

    snapshotOverlay = new PlotSnapshotOverlay(this->canvas(), color);
    connect(snapshotOverlay, &PlotSnapshotOverlay::done,
            [this]()
            {
                delete snapshotOverlay;
                snapshotOverlay = NULL;
            });
}

void Plot::onXScaleChanged()
{
    auto sw = axisWidget(QwtPlot::xBottom);
    auto paintDist = sw->scaleDraw()->scaleMap().pDist();
    auto scaleDist = sw->scaleDraw()->scaleMap().sDist();
    double symDisPx = paintDist / scaleDist;

    if (symDisPx > SYMBOL_SHOW_AT_WIDTH)
    {
        isSymbolsOn = true;
    }
    else
    {
        isSymbolsOn = false;
    }

    updateSymbols();
}

void Plot::updateSymbols()
{
    const QwtPlotItemList curves = itemList( QwtPlotItem::Rtti_PlotCurve );

    if (curves.size() > 0)
    {
        for (unsigned i = 0; i < curves.size(); i++)
        {
            QwtSymbol* symbol = NULL;
            QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(curves[i]);
            if (isSymbolsOn)
            {
                symbol = new QwtSymbol(QwtSymbol::Ellipse,
                                       QBrush(Qt::white),
                                       curve->pen(),
                                       QSize(SYMBOL_SIZE, SYMBOL_SIZE));
            }
            curve->setSymbol(symbol);
        }
    }
}

void Plot::resizeEvent(QResizeEvent * event)
{
    QwtPlot::resizeEvent(event);
    onXScaleChanged();
}
