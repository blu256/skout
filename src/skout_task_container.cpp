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

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent, TQString appname)
  : TQVBox(parent),
    m_appname(appname),
    m_active(false)
{
    m_grouper = new SkoutTaskGrouper(this, appname);

    connect(parent, SIGNAL(windowActivated(WId)), SLOT(updateActive(WId)));

    show();
}

SkoutTaskContainer::~SkoutTaskContainer() {
    ZAP(m_grouper)
}

TQObjectList SkoutTaskContainer::tasks() {
    return *(queryList("SkoutTask"));
}

TQPixmap SkoutTaskContainer::groupIcon() {
    TQObjectList tasklist = tasks();
    if (tasklist.count()) {
        SkoutTask *task = static_cast<SkoutTask *>(tasklist.getFirst());
        return task->icon(SkoutTask::bigIconSize());
    }
    return SkoutTask::defaultIcon(SkoutTask::bigIconSize());
}

void SkoutTaskContainer::update() {
    if (!tasks().count()) {
        deleteLater();
        return;
    }
    m_grouper->setIcon(groupIcon());
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