/*
  Copyright © 2017 Hasan Yavuz Özderya

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

#include <QPalette>

#include "barchart.h"

BarChart::BarChart(ChannelManager* channelMan)
{
    _channelMan = channelMan;
    setSpacing(0);
}

void BarChart::resample()
{
    setSamples(chartData());
}

QVector<double> BarChart::chartData() const
{
    unsigned numChannels = _channelMan->numOfChannels();
    unsigned numOfSamples = _channelMan->numOfSamples();
    QVector<double> data(numChannels);
    for (unsigned i = 0; i < numChannels; i++)
    {
        data[i] = _channelMan->channelBuffer(i)->sample(numOfSamples-1);
    }
    return data;
}

QwtColumnSymbol* BarChart::specialSymbol(int sampleIndex, const QPointF& sample) const
{
    unsigned numChannels = _channelMan->numOfChannels();
    if (sampleIndex < 0 || sampleIndex > (int) numChannels)
    {
        return NULL;
    }

    auto info = _channelMan->infoModel();
    auto color = info->color(sampleIndex);

    QwtColumnSymbol* symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setLineWidth(1);
    symbol->setFrameStyle(QwtColumnSymbol::Plain);
    symbol->setPalette(QPalette(color));

    return symbol;
}
