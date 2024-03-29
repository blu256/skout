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
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kiconloader.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_task_container.h"
#include "skout_task_grouper.h"
#include "skout_task.h"
#include "skout_utils.h"

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent,
                                       TQString wclass, TQString aclass)
  : TQVBox(parent),
    m_service(nullptr),
    m_wclass(wclass),
    m_aclass(aclass),
    m_appname(aclass),
    m_active(false)
{
    m_grouper = new SkoutTaskGrouper(this, m_appname);

    connect(manager(), TQ_SIGNAL(windowActivated(WId)), TQ_SLOT(updateActive(WId)));
    connect(m_grouper, TQ_SIGNAL(pinChanged(bool)), TQ_SLOT(slotPinChanged(bool)));

    setSizePolicy(TQSizePolicy::MinimumExpanding, TQSizePolicy::Fixed);
    show();
}

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent,
                                       KService::Ptr service, TQString wclass)
  : SkoutTaskContainer(parent, wclass, service->name())
{
    m_service = service;
    m_grouper->pin();
}

SkoutTaskContainer::~SkoutTaskContainer() {
    ZAP(m_grouper)
}

TQObjectList SkoutTaskContainer::tasks() {
    return *(queryList("SkoutTask"));
}

uint SkoutTaskContainer::count() {
    return tasks().count();
}

TQPixmap SkoutTaskContainer::groupIcon() {
    TDEIconLoader *il = kapp->iconLoader();
    TQPixmap pix;
    int size = SkoutTask::bigIconSize().height();

    // First we have some common overrides for system components
    // that do not have their own (user-visible) desktop files
    if (windowClass() == "kdesktop") {
        pix = il->loadIcon("desktop", TDEIcon::Panel, size);
    }
    if (!pix.isNull()) return pix;

    // If we have identified the service, check its desktop file value
    if (m_service) {
        KDesktopFile desktopFile(m_service->desktopEntryPath());
        pix = il->loadIcon(desktopFile.readIcon(), TDEIcon::Panel, size);
    }
    if (!pix.isNull()) return pix;

    // Otherwise get the icon of the first task
    TQObjectList tasklist = tasks();
    if (tasklist.count()) {
        SkoutTask *task = static_cast<SkoutTask *>(tasklist.getFirst());
        return task->icon(SkoutTask::bigIconSize());
    }

    // If all of the above fail, fallback to default icon
    return SkoutTask::defaultIcon(SkoutTask::bigIconSize());
}

void SkoutTaskContainer::findService() {
    if (m_service) return;

    TQObjectList tasklist = tasks();
    TQObjectListIt it(tasklist);
    while (it.current()) {
        SkoutTask *task = static_cast<SkoutTask *>(it.current());

        // Common special cases
        if (task->className() == "tdecmshell") {
            m_service = KService::serviceByDesktopName("kcontrol");
        }
        if (m_service) return;

        // Query desktop files via KSycoca
        m_service = KService::serviceByStorageId(task->className());
        if (m_service) return;

        m_service = KService::serviceByStorageId(task->classClass());
        if (m_service) return;

        m_service = KService::serviceByName(task->classClass());
        if (m_service) return;

        m_service = KService::serviceByStorageId(task->executable());
        if (m_service) return;

        // Last resort: find KSycoca entry by executable name/path
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

void SkoutTaskContainer::slotPinChanged(bool pinned) {
    emit pinChanged(pinned);
    update();
}

void SkoutTaskContainer::update() {
    if (!tasks().count() && !pinned()) {
        manager()->removeContainer(this);
        return;
    }

    // No point in trying too hard for this case
    if (windowClass() == "kdesktop") {
        m_appname = i18n("TDE Desktop");
        return;
    }

    if (!m_service) {
        findService();
    }
    if (m_service) {
        KDesktopFile desktopFile(desktopPath().path());
        TQString appname = desktopFile.readName();
        if (appname.isNull())
            appname = m_service->name();
        if (appname.isNull())
            appname = windowClass();
        m_appname = appname;
    }
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

void SkoutTaskContainer::toggleIconifiedAll() {
    TQObjectList tasklist = tasks();
    TQObjectListIt it(tasklist);
    while (it.current()) {
        static_cast<SkoutTask *>(it.current())->toggleIconified();
        ++it;
    }
}

bool SkoutTaskContainer::allIconified() {
    bool all = true;
    TQObjectList tasklist = tasks();
    TQObjectListIt it(tasklist);
    while (it.current()) {
        if (!static_cast<SkoutTask *>(it.current())->iconified()) {
            all = false;
        }
        ++it;
    }
    return all;
}

const KURL SkoutTaskContainer::desktopPath() {
    if (!m_service) return KURL();
    return KURL(locate("apps", m_service->desktopEntryPath()));
}

void SkoutTaskContainer::slotDesktopFileChanged(const KURL& oldUrl, KURL& newUrl) {
    if (oldUrl != desktopPath().url() || oldUrl == newUrl) return;
    m_service = new KService(newUrl.path());
    update();
}

#include "skout_task_container.moc"

/* kate: replace-tabs true; tab-width 4; */