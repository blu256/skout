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
#include <tqfileinfo.h>

// TDE
#include <tdeapplication.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <twinmodule.h>
#include <tdepopupmenu.h>
#include <kpassivepopup.h>
#include <tdelocale.h>
#include <kprocess.h>
#include <kdebug.h>

// Skout
#include "skout_task.h"
#include "skout_task_container.h"

// NetWM
#include <netwm.h>

// Other
#include <cerrno>

SkoutTask::SkoutTask(SkoutTaskContainer *parent, WId w)
  : SkoutTaskButton(parent),
    m_window_id(w)
{
    setOn(info().isMinimized());
    connect(this, SIGNAL(toggled(bool)), SLOT(setIconified(bool)));
    parent->update();

    if (!parent->grouper()->expanded()) {
        hide();
    }
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

TQString SkoutTask::applicationName() {
    return container()->applicationName();
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

void SkoutTask::contextMenuEvent(TQContextMenuEvent *cme) {
    TDEPopupMenu ctx(this);
    ctx.setCheckable(true);

    int item;
    item = ctx.insertItem(SmallIcon("go-top"), i18n("Stays on top"),
                          this, SLOT(toggleStayAbove()));
    ctx.setItemChecked(item, staysAbove());

    item = ctx.insertItem(SmallIcon("go-bottom"), i18n("Stays on bottom"),
                          this, SLOT(toggleStayBelow()));
    ctx.setItemChecked(item, staysBelow());

    ctx.insertSeparator();

    ctx.insertItem(SmallIcon("kicker"), i18n("Hide into tray"),
                   this, SLOT(sendToTray()));

    ctx.insertSeparator();

    item = ctx.insertItem(SmallIcon("view-fullscreen"), i18n("Fullscreen"),
                          this, SLOT(toggleFullScreen()));
    ctx.setItemChecked(item, fullScreen());

    ctx.insertSeparator();

    item = ctx.insertItem(TQPixmap(locate("data", "skout/pics/iconify.png")),
                          i18n("Minimize"), this, SLOT(toggleIconified()));
    ctx.setItemChecked(item, iconified());

    item = ctx.insertItem(TQPixmap(locate("data", "skout/pics/maximize.png")),
                          i18n("Maximize"), this, SLOT(toggleMaximized()));
    ctx.setItemChecked(item, maximized());

    ctx.insertSeparator();

    ctx.insertItem(TQPixmap(locate("data", "skout/pics/close.png")),
                   i18n("Close"), this, SLOT(close()));

    ctx.exec(cme->globalPos());
}

void SkoutTask::sendToTray() {
    TDEProcess ksystray;
    ksystray << "ksystraycmd" << "--hidden"
             << "--wid" << TQString::number(windowID());
    bool ok = ksystray.start(TDEProcess::DontCare);
    if (!ok) {
        container()->manager()->popup("error",
            i18n("Unable to send \"%1\" to tray").arg(name()),
            i18n(strerror(errno)));
    }
}

bool SkoutTask::checkWindowState(unsigned long state) {
    KWin::WindowInfo i(info());
    return i.valid() && (i.state() & state);
}

void SkoutTask::addWindowState(unsigned long state) {
    KWin::setState(windowID(), state);
}

void SkoutTask::removeWindowState(unsigned long state) {
    KWin::clearState(windowID(), state);
}

void SkoutTask::setWindowState(unsigned long state, bool set) {
    if (set) addWindowState(state);
    else removeWindowState(state);
}

void SkoutTask::setMaximized(bool maximized) {
    setWindowState(NET::MaxVert|NET::MaxHoriz, maximized);
}

void SkoutTask::setIconified(bool iconified) {
    if (iconified) KWin::iconifyWindow(windowID());
    else KWin::deIconifyWindow(windowID());
}

void SkoutTask::setFullScreen(bool fullscreen) {
    setWindowState(NET::FullScreen, fullscreen);
}

void SkoutTask::setStayAbove(bool stay) {
    if (stay && staysBelow()) {
        setStayBelow(false);
    }
    setWindowState(NET::StaysOnTop, stay);
}

void SkoutTask::setStayBelow(bool stay) {
    if (stay && staysAbove()) {
        setStayAbove(false);
    }
    setWindowState(NET::KeepBelow, stay);
}

void SkoutTask::toggleStayAbove() {
    setStayAbove(!staysAbove());
}

void SkoutTask::toggleStayBelow() {
    setStayBelow(!staysBelow());
}

void SkoutTask::toggleIconified() {
    setIconified(!iconified());
}

void SkoutTask::toggleMaximized() {
    setMaximized(!maximized());
}

void SkoutTask::toggleFullScreen() {
    setFullScreen(!fullScreen());
}

void SkoutTask::close() {
    NETRootInfo ri(tqt_xdisplay(), NET::CloseWindow);
    ri.closeWindowRequest(windowID());
}

void SkoutTask::activate() {
    KWin::forceActiveWindow(windowID());
}

bool SkoutTask::active() {
    return container()->twin()->activeWindow() == windowID();
}

bool SkoutTask::staysAbove() {
    return checkWindowState(NET::StaysOnTop);
}

bool SkoutTask::staysBelow() {
    return checkWindowState(NET::KeepBelow);
}

bool SkoutTask::iconified() {
    KWin::WindowInfo i(info());
    return i.valid() && i.isMinimized();
}

bool SkoutTask::maximized() {
    return checkWindowState(NET::MaxVert|NET::MaxHoriz);
}

bool SkoutTask::fullScreen() {
    return checkWindowState(NET::FullScreen);
}

bool SkoutTask::shaded() {
    return checkWindowState(NET::Shaded);
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

/* Yes, I know this is deprecated and the actual application might be on another
   host, but this is the best way I can think of to get the application's
   executable and figure out its corresponding desktop file. */
pid_t SkoutTask::pid() {
    KWin::Info i = KWin::info(windowID());
    return i.pid;
}

TQString SkoutTask::cmdline() {
    TQFile file(TQString("/proc/%1/cmdline").arg(pid()));
    if (file.exists() && file.open(IO_ReadOnly)) {
        return TQString::fromLocal8Bit(file.readAll());
    }
    return TQString::null;
}

TQString SkoutTask::executablePath() {
    TQFileInfo info(TQString("/proc/%1/exe").arg(pid()));
    if (info.exists() && info.isSymLink()) {
        return info.readLink();
    }
    return TQString::null;
}

TQString SkoutTask::executable() {
    return TQFileInfo(executablePath()).fileName();
}

#include "skout_task.moc"