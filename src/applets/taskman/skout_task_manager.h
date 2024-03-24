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

#ifndef _SKOUT_TASKMANAGER_H
#define _SKOUT_TASKMANAGER_H

// TQt
#include <tqdict.h>
#include <tqintdict.h>

// TDE
#include <twinmodule.h>

// Skout
#include "skout_applet.h"

typedef TQValueList<WId> WIdList;

class SkoutPanel;
class SkoutTaskContainer;
class SkoutTask;

class SkoutTaskMan : public SkoutApplet {
  TQ_OBJECT

  public:
    SkoutTaskMan(SkoutPanel *panel);
    virtual ~SkoutTaskMan();

    static TQString className(WId w);
    static TQString classClass(WId w);

    bool valid() { return true; }
    KWinModule *twin();

  public slots:
    void addWindow(WId w);
    void removeWindow(WId w);
    void updateWindow(WId w, unsigned int changes);
    void savePinnedApplications();
    void relayout();

  signals:
    void windowActivated(WId w);

  private:
    TQDict<SkoutTaskContainer> m_containers;
    TQIntDict<SkoutTask> m_tasks;
    KWinModule *m_twin;

    void addContainer(SkoutTaskContainer *c);
    void removeContainer(SkoutTaskContainer *c);

  private slots:
    void slotPinChanged(bool pinned);

  friend class SkoutTaskContainer;
};

#endif // _SKOUT_TASKMANAGER_H