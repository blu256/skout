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

// TQt
#include <tqobjectlist.h>

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>
#include <kdebug.h>

// Skout
#include "skout_task_container.h"
#include "skout_task_grouper.h"
#include "skout_task.h"
#include "skout_utils.h"

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent,
                                       TQString wclass, TQString appname)
  : TQVBox(parent),
    m_service(nullptr),
    m_wclass(wclass),
    m_appname(appname),
    m_active(false)
{
    init();
}

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent,
                                       KService::Ptr service, TQString wclass)
  : TQVBox(parent),
    m_service(service),
    m_wclass(wclass),
    m_active(false)
{
    init();
    m_appname = m_service->name();
    m_grouper->pin();
}

void SkoutTaskContainer::init() {
    m_grouper = new SkoutTaskGrouper(this, m_appname);

    connect(manager(), SIGNAL(windowActivated(WId)), SLOT(updateActive(WId)));
    connect(m_grouper, SIGNAL(pinChanged(bool)), SIGNAL(pinChanged(bool)));

    show();
}

SkoutTaskContainer::~SkoutTaskContainer() {
    ZAP(m_grouper)
}

TQObjectList SkoutTaskContainer::tasks() {
    return *(queryList("SkoutTask"));
}

TQPixmap SkoutTaskContainer::groupIcon() {
    // If we have identified the service, check its desktop file value
    if (m_service) {
        TQPixmap pix = m_service->pixmap(TDEIcon::Panel,
                                         SkoutTask::bigIconSize().height());
        if (!pix.isNull()) {
            return pix;
        }
    }

    // Otherwise get the icon of the first task
    TQObjectList tasklist = tasks();
    if (tasklist.count()) {
        SkoutTask *task = static_cast<SkoutTask *>(tasklist.getFirst());
        return task->icon(SkoutTask::bigIconSize());
    }

    // If both fail, fallback to default icon
    return SkoutTask::defaultIcon(SkoutTask::bigIconSize());
}

void SkoutTaskContainer::findService() {
    if (m_service) return;

    TQObjectList tasklist = tasks();
    TQObjectListIt it(tasklist);
    while (it.current()) {
        SkoutTask *task = static_cast<SkoutTask *>(it.current());

        m_service = KService::serviceByStorageId(task->className());
        if (m_service) return;

        m_service = KService::serviceByStorageId(task->classClass());
        if (m_service) return;

        m_service = KService::serviceByName(task->classClass());
        if (m_service) return;

        m_service = KService::serviceByStorageId(task->executable());
        if (m_service) return;

        // last resort: find by executable
        KService::List all = KService::allServices();
        KService::List::ConstIterator svc;
        for (svc = all.begin(); svc != all.end(); ++svc) {
            TQString exec((*svc)->exec());
            if (exec == task->executablePath() ||
                exec == task->executable())
            {
                m_service = (*svc);
                return;
            }
        }

        ++it;
    }
    kdWarning() << "Unable to find desktop file for window class " << windowClass() << endl;
}

void SkoutTaskContainer::update() {
    if (!tasks().count() && !pinned()) {
        deleteLater();
        return;
    }

    if (!m_service) {
        findService();
    }
    if (m_service) {
        m_appname = m_service->name();
    }
    m_grouper->update();
}

void SkoutTaskContainer::updateActive(WId w) {
    TQObjectList tasklist = tasks();
    TQObjectListIt it(tasklist);
    m_active = false;
    while (it.current()) {
        SkoutTask *task = static_cast<SkoutTask *>(it.current());
        if (w == task->windowID()) {
            m_active = true;
            break;
        }
        ++it;
    }

    m_grouper->repaint(true);

    it = TQObjectListIt(tasklist);
    while (it.current()) {
        SkoutTask *task = static_cast<SkoutTask *>(it.current());
        task->repaint();
        ++it;
    }
}

#include "skout_task_container.moc"