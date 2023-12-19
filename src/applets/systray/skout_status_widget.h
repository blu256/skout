/*******************************************************************************
  Skout - a Be-style panel for TDE
  Copyright (C) 2023 Mavridis Philippe <mavridisf@gmail.com>

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

  Improvements and feedback are welcome!
*******************************************************************************/

#ifndef _SKOUT_STATUS_WIDGET_H
#define _SKOUT_STATUS_WIDGET_H

// TQt
#include <tqhbox.h>

// Skout
#include "skout_system_tray.h"

enum DateTimeFormat {
    FormatDateTime,
    FormatDateTimeShort,
    FormatDateTimeShortSecs,
    FormatDateTimeSecs,
    FormatDate,
    FormatDateShort,
    FormatTime,
    FormatTimeShort,
    DATETIMEFORMAT_END
};

class TQLabel;
class TQTimer;

class SkoutSystemGraph;

class SkoutStatusWidget : public TQHBox {
  TQ_OBJECT

  public:
    SkoutStatusWidget(SkoutSysTray *tray);
    virtual ~SkoutStatusWidget();

    SkoutSysTray *sysTray() const {
        return static_cast<SkoutSysTray *>(parent());
    }

  public slots:
    void configureDateTime();
    void configureDateTimeFormat();

  protected:
    void mousePressEvent(TQMouseEvent *e);

  private slots:
    void updateClock();
    void copyDateTime(int choice);

  private:
    TQLabel *m_clock;
    TQTimer *m_clockTimer;
    SkoutSystemGraph *m_cpuGraph;
    SkoutSystemGraph *m_memGraph;

    const TQString formatDateTime(TQDateTime dt, DateTimeFormat f);
};

#endif // _SKOUT_STATUS_WIDGET_H