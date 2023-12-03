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
#include <tqtimer.h>
#include <tqpainter.h>
#include <tqpalette.h>

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>
#include <twinmodule.h>
#include <netwm.h>
#include <kdebug.h>

// Skout
#include "skout_task.h"
#include "skout_task_container.h"

SkoutTask::SkoutTask(SkoutTaskContainer *parent, WId w)
  : SkoutTaskButton(parent),
    m_window_id(w)
{
    setOn(info().isMinimized());
    connect(this, SIGNAL(toggled(bool)), SLOT(setIconified(bool)));
    parent->update();
}

SkoutTask::~SkoutTask() {
    TQTimer::singleShot(0, container(), SLOT(update()));
}

KWin::WindowInfo SkoutTask::info() {
    return KWin::windowInfo(windowID());
}

TQString SkoutTask::name() {
    return info().visibleNameWithState();
}

TQPixmap SkoutTask::icon() {
    return icon(smallIconSize());
}

TQPixmap SkoutTask::icon(TQSize size) {
    TQPixmap ico = KWin::icon(windowID(), size.width(), size.height(), true);
    if (!ico.isNull()) {
        return ico;
    }

    ico = kapp->iconLoader()->loadIcon(className().lower(),
                                       TDEIcon::Panel, size.height(),
                                       TDEIcon::DefaultState,
                                       0, true);
    if (!ico.isNull()) {
        return ico;
    }

    return defaultIcon(size);
}

TQString SkoutTask::className() {
    return SkoutTaskMan::className(windowID());
}

TQString SkoutTask::classClass() {
    return SkoutTaskMan::classClass(windowID());
}

void SkoutTask::mousePressEvent(TQMouseEvent *me) {
    switch (me->button()) {
        case LeftButton:
            activate();
            return;
        case MidButton:
            toggle();
            return;
        default:
            me->ignore();
    }
}

void SkoutTask::setMaximized(bool maximized) {
    unsigned int state = info().state();
    if (maximized) {
        KWin::setState(windowID(), state & (NET::MaxVert|NET::MaxHoriz));
    }
    else {
        KWin::setState(windowID(), state & ~(NET::MaxVert|NET::MaxHoriz));
    }
}

void SkoutTask::setIconified(bool iconified) {
    if (iconified) {
        KWin::iconifyWindow(windowID());
    }
    else {
        KWin::deIconifyWindow(windowID());
    }
}

#if 0
void SkoutTask::toggleFullscreen() {
    unsigned int state = info().state();
    if (state & NET::FullScreen) {
        KWin::setState(windowID(), state & ~NET::FullScreen);
    }
    else {
        KWin::setState(windowID(), state & NET::FullScreen);
    }
}
#endif

void SkoutTask::close() {
    NETRootInfo ri(tqt_xdisplay(), NET::CloseWindow);
    ri.closeWindowRequest(windowID());
}

void SkoutTask::activate() {
    KWin::forceActiveWindow(windowID());
}

bool SkoutTask::active() {
    return container()->manager()->panel()->twin()->activeWindow() == windowID();
}

TQFont SkoutTask::font() {
    return active() ? boldFont() : normalFont();
}

TQColorGroup SkoutTask::colors() {
    TQColorGroup cg = palette().active();
    if (active()) {
        TQColor highlight = blendColors(cg.button(), cg.highlight());
        cg.setColor(TQColorGroup::Button,     highlight);
        cg.setColor(TQColorGroup::ButtonText, cg.highlightedText());
    }
    else if (!container()->active()) {
        cg.setColor(TQColorGroup::Button,     cg.background());
        cg.setColor(TQColorGroup::ButtonText, cg.foreground());
    }
    return cg;
}

#include "skout_task.moc"