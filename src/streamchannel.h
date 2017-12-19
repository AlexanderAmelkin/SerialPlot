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

#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H

// IMPORTANT TODO: rename to "framebuffer.h" when stream work is done.
#include "framebuffer2.h"

#include "channelinfomodel.h"

class StreamChannel
{
public:
    /**
     * Creates a stream channel.
     *
     * @param i index of the channel
     * @param x x axis buffer
     * @param y data buffer of this channel, takes ownership
     * @param info channel info model
     */
    StreamChannel(unsigned i,
                  const FrameBuffer* x,
                  FrameBuffer* y,
                  ChannelInfoModel* info);
    ~StreamChannel();

    unsigned index() const;
    QString name() const;
    QColor color() const;
    bool visible() const;
    const FrameBuffer* xData() const;
    FrameBuffer* yData();
    const FrameBuffer* yData() const;
    const ChannelInfoModel* info() const;


private:
    unsigned _index;
    const FrameBuffer* _x;
    FrameBuffer* _y;
    ChannelInfoModel* _info;
};

#endif // STREAMCHANNEL_H
