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

#ifndef ASCIIREADER_H
#define ASCIIREADER_H

#include <QSettings>

#include "abstractreader.h"
#include "asciireadersettings.h"

class AsciiReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit AsciiReader(QIODevice* device, ChannelManager* channelMan,
                         DataRecorder* recorder, QObject *parent = 0);
    QWidget* settingsWidget();
    unsigned numOfChannels();
    void enable(bool enabled = true);
    /// Stores settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads settings from a `QSettings`.
    void loadSettings(QSettings* settings);

public slots:
    void pause(bool);

private:
    AsciiReaderSettings _settingsWidget;
    unsigned _numOfChannels;
    /// number of channels will be determined from incoming data
    unsigned autoNumOfChannels;
    bool paused;

    // We may have (usually true) started reading in the middle of a
    // line, so its a better idea to just discard first line.
    bool discardFirstLine;

private slots:
    void onDataReady();
};

#endif // ASCIIREADER_H
