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

#ifndef _SKOUT_TASK_CONTAINER_H
#define _SKOUT_TASK_CONTAINER_H

// TDE
#include <kservice.h>
#include <kurl.h>

// TQt
#include <tqvbox.h>

// Skout
#include "skout_task_manager.h"
#include "skout_task_grouper.h"

class SkoutTaskMan;
class SkoutTaskGrouper;
class SkoutTask;

class SkoutTaskContainer : public TQVBox {
  TQ_OBJECT

  public:
    SkoutTaskContainer(SkoutTaskMan *parent, TQString wclass, TQString appname);
    ~SkoutTaskContainer();

    // Constructor for pinned applications
    SkoutTaskContainer(SkoutTaskMan *parent, KService::Ptr service, TQString wclass);

    TQString windowClass() const { return m_wclass; }
    TQString applicationName() const { return m_appname; }
    TQPixmap groupIcon();

    TQObjectList tasks();

    KService::Ptr service() { return m_service; }
    const KURL desktopPath();

    bool active()   { return m_active; }
    bool pinned()   { return m_grouper->pinned(); }
    bool pinnable() { return m_grouper->pinnable(); }

    bool allIconified();

    SkoutTaskMan *manager() const { return static_cast<SkoutTaskMan *>(parent()); }
    SkoutTaskGrouper *grouper() const { return m_grouper; }
    KWinModule *twin() { return manager()->twin(); }

  public slots:
    void update();
    void updateActive(WId w);
    void toggleIconifiedAll();

    void slotPinChanged(bool pinned);
    void slotDesktopFileChanged(const KURL&, KURL&);

  signals:
    void pinChanged(bool pinned);

  protected:
    TQSize sizeHint() const;

  private:
    KService::Ptr m_service;
    TQString m_wclass;
    TQString m_appname;
    SkoutTaskGrouper *m_grouper;
    bool m_active;

  private slots:
    void findService();
};

#endif // _SKOUT_TASK_CONTAINER_H