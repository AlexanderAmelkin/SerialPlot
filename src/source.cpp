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

#include <QtGlobal>

#include "source.h"

void Source::connect(Sink* sink)
{
    Q_ASSERT(!sinks.contains(sink));

    sinks.append(sink);
}

void Source::disconnect(Sink* sink)
{
    Q_ASSERT(sinks.contains(sink));

    sinks.removeOne(sink);
}

void Source::feedOut(const SamplePack& data) const
{
    for (auto sink : sinks)
    {
        sink->feedIn(data);
    }
}

/// Updates "number of channels" of connected sinks
void Source::feedNumChannels(unsigned nc, bool x) const
{
    for (auto sink : sinks)
    {
        sink->setNumChannels(nc, x);
    }
}
