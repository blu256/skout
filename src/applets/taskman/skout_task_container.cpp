/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-2025 Philippe Mavridis <philippe.mavridis@yandex.com>

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
#include <tqtimer.h>

// TDE
#include <tdeapplication.h>
#include <tdestandarddirs.h>
#include <tdedesktopfile.h>
#include <tdemessagebox.h>
#include <kiconloader.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_task_container.h"
#include "skout_task_grouper.h"
#include "skout_task.h"
#include "skout_utils.h"

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent, TQString wclass, TQString aclass)
  : TQVBox(parent),
    m_service(nullptr),
    m_wclass(wclass),
    m_aclass(aclass),
    m_appname(aclass),
    m_active(false)
{
    m_grouper = new SkoutTaskGrouper(this, manager()->config(), m_appname);

    connect(manager(), TQ_SIGNAL(windowActivated(WId)), TQ_SLOT(updateActiveTask(WId)));
    connect(m_grouper, TQ_SIGNAL(pinChanged(bool)), TQ_SLOT(slotPinChanged(bool)));

    setSizePolicy(TQSizePolicy::MinimumExpanding, TQSizePolicy::Fixed);
    show();
}

SkoutTaskContainer::SkoutTaskContainer(SkoutTaskMan *parent, KService::Ptr service, TQString aclass)
  : SkoutTaskContainer(parent, service->name(), aclass)
{
    m_service = service;
    m_grouper->pin();
}

SkoutTaskContainer::~SkoutTaskContainer()
{
    ZAP(m_grouper)
}

TQObjectList SkoutTaskContainer::tasks() const
{
    return *(queryList("SkoutTask"));
}

uint SkoutTaskContainer::count() const
{
    return tasks().count();
}

TQObjectList SkoutTaskContainer::visibleTasks() const
{
    TQObjectList visibleList;
    FOREACH_TASK(t, tasks())
    {
        if (t->isShown())
        {
            visibleList.append(t);
        }
    }
    END_FOREACH_TASK
    return visibleList;
}

uint SkoutTaskContainer::visibleCount() const
{
    return visibleTasks().count();
}

TQObjectList SkoutTaskContainer::currentDesktopTasks() const
{
    TQObjectList currentDesktopList;
    FOREACH_TASK(t, tasks())
    {
        if (t->isOnCurrentDesktop())
        {
            currentDesktopList.append(t);
        }
    }
    END_FOREACH_TASK
    return currentDesktopList;
}

uint SkoutTaskContainer::currentDesktopCount() const
{
    return currentDesktopTasks().count();
}

TQSize SkoutTaskContainer::sizeHint() const
{
    int height = m_grouper->height();
    if (m_grouper->expanded())
    {
        FOREACH_TASK(t, tasks())
        {
            if (t->isShown())
            {
                height += t->height();
            }
        }
        END_FOREACH_TASK
    }
    return TQSize(width(), height);
}

TQPixmap SkoutTaskContainer::groupIcon()
{
    TDEIconLoader *il = tdeApp->iconLoader();
    TQPixmap pix;
    int size = SkoutTask::bigIconSize().height();

    // First we have some common overrides for system components
    // that do not have their own (user-visible) desktop files
    if (windowClass() == "kdesktop")
    {
        pix = il->loadIcon("desktop", TDEIcon::Panel, size);
    }
    if (!pix.isNull()) return pix;

    // If we have identified the service, check its desktop file value
    if (m_service)
    {
        TDEDesktopFile desktopFile(m_service->desktopEntryPath());
        pix = il->loadIcon(desktopFile.readIcon(), TDEIcon::Panel, size);
    }

    if (!pix.isNull())
    {
        return pix;
    }

    // Otherwise get the icon of the first task
    TQObjectList taskList = tasks();
    if (taskList.count())
    {
        SkoutTask *task = static_cast<SkoutTask *>(taskList.getFirst());
        return task->icon(SkoutTask::bigIconSize());
    }

    // If all of the above fail, fallback to default icon
    return SkoutTask::defaultIcon(SkoutTask::bigIconSize());
}

void SkoutTaskContainer::slotPinChanged(bool pinned)
{
    emit pinChanged(pinned);
    update();
}

void SkoutTaskContainer::reconfigure()
{
    repaintAll();
}

void SkoutTaskContainer::repaintAll()
{
    TQTimer::singleShot(0, m_grouper, TQ_SLOT(repaint()));
    FOREACH_TASK(t, tasks())
    {
        TQTimer::singleShot(0, t, TQ_SLOT(repaint()));
        ++it;
    }
    END_FOREACH_TASK
}

void SkoutTaskContainer::update()
{
    if (!tasks().count() && !isPinned())
    {
        manager()->removeContainer(this);
        return;
    }

    TQTimer::singleShot(0, manager(), TQ_SLOT(relayout()));

    // No point in trying too hard for some cases
    TQString aClass = applicationClass();
    if (aClass == "Kdesktop" || aClass == "Kded" || aClass == "Ksmserver")
    {
        m_appname = i18n("TDE Desktop");
        return;
    }

    if (!m_service)
    {
        findService();
    }

    if (m_service)
    {
        TDEDesktopFile desktopFile(desktopPath().path());
        TQString appname = desktopFile.readName();
        if (appname.isNull())
        {
            appname = m_service->name();
        }

        if (appname.isNull())
        {
            appname = windowClass();
        }

        m_appname = appname;
    }
}

void SkoutTaskContainer::findService()
{
    if (m_service) return;

    FOREACH_TASK(t, tasks())
    {
        // Common special cases
        if (t->className() == "tdecmshell")
        {
            m_service = KService::serviceByDesktopName("kcontrol");
        }
        if (m_service) return;

        // Query desktop files via KSycoca
        m_service = KService::serviceByStorageId(t->classClass());
        if (m_service) return;

        m_service = KService::serviceByName(t->classClass());
        if (m_service) return;

        m_service = KService::serviceByStorageId(t->executable());
        if (m_service) return;

        // Last resort: find KSycoca entry by executable name/path
        KService::List all = KService::allServices();
        KService::List::ConstIterator svc;
        for (svc = all.begin(); svc != all.end(); ++svc)
        {
            // Get rid of any argument placeholders might be present
            TQString exec = TQStringList::split(" ", (*svc)->exec())[0];
            if (exec == t->executablePath() || exec == t->executable())
            {
                m_service = (*svc);
                return;
            }
        }
    }
    END_FOREACH_TASK
    kdWarning() << "Unable to find desktop file for application class "
                << applicationClass() << endl;
}

void SkoutTaskContainer::updateActiveTask(WId w)
{
    bool foundActive = false;
    FOREACH_TASK(t, tasks())
    {
        bool active = w == t->windowID();
        t->setOn(active);
        if (active) foundActive = true;
    }
    END_FOREACH_TASK
    m_active = foundActive;
    TQTimer::singleShot(0, m_grouper, TQ_SLOT(repaint()));
}

void SkoutTaskContainer::updateTaskVisibility()
{
    FOREACH_TASK(t, tasks())
    {
        t->updateVisibility();
    }
    END_FOREACH_TASK
    TQTimer::singleShot(0, manager(), TQ_SLOT(relayout()));
}

void SkoutTaskContainer::toggleIconifiedAll()
{
    FOREACH_TASK(t, tasks())
    {
        t->toggleIconified();
    }
    END_FOREACH_TASK
}

void SkoutTaskContainer::closeAll()
{
    if (KMessageBox::Yes == KMessageBox::warningYesNo(
        this, i18n("<qt>Are you sure you want to close all the "
        "windows of <b>%1</b>?</qt>").arg(application())))
    {
        FOREACH_TASK(t, tasks())
        {
            t->close();
        }
        END_FOREACH_TASK
    }
}

bool SkoutTaskContainer::allIconified()
{
    FOREACH_TASK(t, tasks())
    {
        if (!t->isIconified())
        {
            return false;
        }
    }
    END_FOREACH_TASK
    return true;
}

const KURL SkoutTaskContainer::desktopPath()
{
    if (!m_service) return KURL();
    return KURL(locate("apps", m_service->desktopEntryPath()));
}

void SkoutTaskContainer::slotDesktopFileChanged(const KURL& oldUrl, KURL& newUrl)
{
    if (oldUrl != desktopPath().url() || oldUrl == newUrl) return;
    m_service = new KService(newUrl.path());
    update();
}

#include "skout_task_container.moc"

/* kate: replace-tabs true; tab-width 4; */