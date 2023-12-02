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

// TDE
#include <twinmodule.h>
#include <twin.h>
#include <netwm.h>
#include <kdebug.h>

// Skout
#include "skout_task_manager.h"
#include "skout_task_container.h"
#include "skout_task.h"
#include "skout_widget.h"

// X11
#include <X11/Xutil.h>

SkoutTaskMan::SkoutTaskMan(SkoutPanel *panel)
  : SkoutWidget(panel, "SkoutTaskMan")
{
    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Preferred);
    setMaximumWidth(panel->width());

    new TQVBoxLayout(this);
    layout()->setAutoAdd(true);

    WIdList windows(panel->twin()->windows());
    WIdList::ConstIterator it;
    for (it = windows.begin(); it != windows.end(); ++it) {
        addWindow((*it));
    }

    connect(panel->twin(), SIGNAL(windowAdded(WId)),
                           SLOT(addWindow(WId)));

    connect(panel->twin(), SIGNAL(windowRemoved(WId)),
                           SLOT(removeWindow(WId)));

    connect(panel->twin(), SIGNAL(windowChanged(WId, unsigned int)),
                           SLOT(updateWindow(WId, unsigned int)));

    connect(panel->twin(), SIGNAL(activeWindowChanged(WId)),
                           SIGNAL(windowActivated(WId)));
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

    TQString windowClass = classClass(w);

    SkoutTask *t;
    SkoutTaskContainer *c = m_containers[windowClass];
    if (!c) {
        c = new SkoutTaskContainer(this, windowClass);
        connect(c, SIGNAL(destroyed()), this, SLOT(containerDeleted()));
        m_containers.insert(windowClass, c);
    }
    t = new SkoutTask(c, w);
    m_tasks.insert(w, t);

    if (w == panel()->twin()->activeWindow()) {
        emit windowActivated(w);
    }
}

void SkoutTaskMan::removeWindow(WId w) {
    SkoutTask *t = m_tasks[w];
    if (!t) return;
    m_tasks.remove(w);
    t->deleteLater();
}

void SkoutTaskMan::containerDeleted() {
    const TQObject *obj = TQObject::sender();
    if (!obj) return;

    const SkoutTaskContainer *c = static_cast<const SkoutTaskContainer *>(obj);
    Q_ASSERT(m_containers.remove(c->applicationName()));
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

#include "skout_task_manager.moc"