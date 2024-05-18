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
#include <tqlayout.h>
#include <tqwhatsthis.h>

// TDE
#include <tdeapplication.h>
#include <twinmodule.h>
#include <twin.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_task_manager.h"
#include "skout_task_container.h"
#include "skout_task.h"
#include "skoutsettings.h"

// X11
#include <X11/Xutil.h>

// NetWM
#include <netwm.h>

extern "C" {
    TDE_EXPORT SkoutApplet *init(SkoutPanel *parent) {
        return new SkoutTaskMan(parent);
    }
}

SkoutTaskMan::SkoutTaskMan(SkoutPanel *panel)
  : SkoutApplet(panel, "SkoutTaskMan")
{
    m_tasks.setAutoDelete(true);
    m_containers.setAutoDelete(true);

    m_twin = new KWinModule(this);

    setSizePolicy(TQSizePolicy::MinimumExpanding, TQSizePolicy::Fixed);
    new TQVBoxLayout(this);

    TQWhatsThis::add(this, i18n("The task list provides access to application "
                                "windows, common window operations and pinned "
                                "applications."));

    // Create containers for pinned applications
    TQStringList pinned = SkoutSettings::pinnedApplications();
    for (TQStringList::Iterator it = pinned.begin(); it != pinned.end(); ++it) {
        int index = (*it).find(":");
        if (index == -1) {
            kdWarning() << "Malformed pinned entry: " << (*it) << endl;
            continue;
        }

        TQString storageId((*it).mid(0, index));
        TQString windowClass((*it).mid(++index));

        KService::Ptr svc = KService::serviceByStorageId(storageId);
        if (!svc) {
            kdWarning() << "Cannot find pinned entry " << storageId << endl;
            continue;
        }

        addContainer(new SkoutTaskContainer(this, svc, windowClass));
    }

    // Add windows
    WIdList windows(m_twin->windows());
    for (WIdList::ConstIterator it = windows.begin(); it != windows.end(); ++it) {
        addWindow((*it));
    }

    connect(m_twin, TQ_SIGNAL(windowAdded(WId)),
                    TQ_SLOT(addWindow(WId)));

    connect(m_twin, TQ_SIGNAL(windowRemoved(WId)),
                    TQ_SLOT(removeWindow(WId)));

    connect(m_twin, TQ_SIGNAL(windowChanged(WId, unsigned int)),
                    TQ_SLOT(updateWindow(WId, unsigned int)));

    connect(m_twin, TQ_SIGNAL(activeWindowChanged(WId)),
                    TQ_SIGNAL(windowActivated(WId)));
}

SkoutTaskMan::~SkoutTaskMan() {
}

void SkoutTaskMan::addWindow(WId w) {
    if (m_tasks[w] != nullptr) return;

    KWin::WindowInfo info = KWin::windowInfo(w);
    NET::WindowType type = info.windowType(NET::NormalMask | NET::DockMask |
                                           NET::DesktopMask | NET::ToolbarMask |
                                           NET::OverrideMask | NET::DialogMask |
                                           NET::MenuMask | NET::TopMenuMask |
                                           NET::UtilityMask | NET::SplashMask);

    if ((type != NET::Normal && type != NET::Dialog && type != NET::Utility &&
         type != NET::Unknown && type != NET::Override) ||
        (info.state() & NET::SkipTaskbar) != 0) return;

    TQString windowClass = className(w);
    TQString appClass = classClass(w);

    if (appClass.isNull()) appClass = windowClass;
    if (appClass.isNull()) return;

    kdDebug() << "windowClass: " << windowClass << endl
              << "   appClass: " << appClass<< endl;

    SkoutTaskContainer *c = m_containers[appClass];
    if (!c) {
        c = new SkoutTaskContainer(this, windowClass, appClass);
        addContainer(c);
    }
    SkoutTask *t = new SkoutTask(c, w);
    m_tasks.insert(w, t);

    if (w == m_twin->activeWindow()) {
        emit windowActivated(w);
    }
}

void SkoutTaskMan::removeWindow(WId w) {
    SkoutTask *t = m_tasks[w];
    if (!t) return;

    if (t->container()->count() < 2 && !t->container()->pinned()) {
      SkoutTaskContainer *c = t->container();
      m_tasks.remove(w);
      removeContainer(c);
    }
    else m_tasks.remove(w);

    parentWidget()->layout()->invalidate();
}

void SkoutTaskMan::addContainer(SkoutTaskContainer *c) {
    if (!c) return;
    m_containers.insert(c->applicationClass(), c);
    connect(c, TQ_SIGNAL(pinChanged(bool)), TQ_SLOT(slotPinChanged(bool)));
    layout()->add(c);
}

void SkoutTaskMan::removeContainer(SkoutTaskContainer *c) {
    if (!c) return;
    m_containers.remove(c->applicationClass());
}

void SkoutTaskMan::relayout() {
    TQDictIterator<SkoutTaskContainer> it(m_containers);

    // Move non-pinned applications after pinned ones
    for (; it.current(); ++it) {
      if (!it.current()->pinned()) {
          layout()->remove(it.current());
          layout()->add(it.current());
      }
    }
}

void SkoutTaskMan::updateWindow(WId w, uint changes) {
    SkoutTask *t = m_tasks[w];
    if (!t) return;

    if (changes & NET::WMState) {
        NETWinInfo info (tqt_xdisplay(),  w, tqt_xrootwin(), NET::WMState | NET::XAWMState);

        if (info.state() & NET::SkipTaskbar) {
            removeWindow(w);
        }
        else {
            addWindow(w);
        }
    }

    if (changes & NET::WMVisibleName || changes & NET::WMName || changes & NET::WMIcon)
    {
        t->update();
    }
}

void SkoutTaskMan::savePinnedApplications() {
    TQStringList pinned;
    TQDictIterator<SkoutTaskContainer> it(m_containers);
    for (; it.current(); ++it) {
        SkoutTaskContainer *c = it.current();
        if (c->pinned() && c->pinnable()) {
            TQString entry;
            entry += c->service()->storageId();
            entry += ":";
            entry += c->windowClass();
            pinned << entry;
        }
    }
    SkoutSettings::setPinnedApplications(pinned);
    SkoutSettings::writeConfig();
}

void SkoutTaskMan::slotPinChanged(bool pinned) {
    savePinnedApplications();
    relayout();
}

TQString SkoutTaskMan::className(WId w) {
    XClassHint hint;
    if (!XGetClassHint(tqt_xdisplay(), w, &hint)) {
        return TQString::null;
    }

    TQString nh(hint.res_name);
    XFree(hint.res_name);
    XFree(hint.res_class);
    return nh;
}

TQString SkoutTaskMan::classClass(WId w) {
    XClassHint hint;
    if (!XGetClassHint(tqt_xdisplay(), w, &hint)) {
        return TQString::null;
    }

    TQString nh(hint.res_class);
    XFree(hint.res_name);
    XFree(hint.res_class);
    return nh;
}

KWinModule *SkoutTaskMan::twin() {
    return m_twin;
}

#include "skout_task_manager.moc"

/* kate: replace-tabs true; tab-width 4; */