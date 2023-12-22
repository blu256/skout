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

#ifndef _SKOUT_SYSTEM_GRAPH_H
#define _SKOUT_SYSTEM_GRAPH_H

// TQt
#include <tqwidget.h>

// Skout
#include "skout_status_widget.h"

struct Tool {
    TQString service;
    TQStringList args;
    TQString icon;
    TQString name;

    bool isValid() {
        return !service.isNull();
    }

    TQString getIcon() {
        return icon.isNull() ? service : icon;
    }
};

class TQLabel;
class TQTimer;
class TDEProcess;
class TDEPopupMenu;

class SkoutSystemGraph : public TQWidget {
  TQ_OBJECT

  public:
    SkoutSystemGraph(SkoutStatusWidget *status, const char *name,
                     TQStringList &endpoints, bool aggregateMaxValues = true);
    virtual ~SkoutSystemGraph();

    SkoutStatusWidget *statusWidget() {
        return static_cast<SkoutStatusWidget *>(parent());
    }

    void launch(Tool tool);
    const Tool systemMonitor();
    const Tool processManager();
    const Tool terminalEmulator();

  protected:
    void paintEvent(TQPaintEvent *e);
    void mousePressEvent(TQMouseEvent *e);

  private slots:
    void slotDaemonExited(TDEProcess *);
    void slotDaemonStdout(TDEProcess *, char *buffer, int buflen);
    void slotDaemonStderr(TDEProcess *, char *buffer, int buflen);

    void init();
    void deinit();
    bool startDaemon();
    void resetRetryCount();

    void reset();
    void update();

    void launchMenuItem(int item);

  private:
    TQStringList m_endpoints;
    TQStringList::Iterator m_endpoint;

    TDEProcess *m_ksgrd;
    int m_retryCount;

    TQValueList<int> m_readings;
    TQString m_unit;
    int m_maxValue;
    int m_value;

    bool m_isPolling;
    bool m_isReceiving;
    bool m_isOnline;
    bool m_isUpdating;
    int m_pollingFrequency;
    bool m_aggregateMaxValues;

    TDEPopupMenu *m_contextMenu;
    TQValueList<Tool> m_tools;

    bool write(TQString msg);

    TQRect labelRect() const;
    TQRect graphRect() const;
};

#endif // _SKOUT_SYSTEM_GRAPH_H