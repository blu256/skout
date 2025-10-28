/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-25 Mavridis Philippe <mavridisf@gmail.com>

  Portions from Kicker taskbar applet.
    Copyright (c) 2001 Matthias Elter <elter@kde.org>
    Copyright (c) 2001 John Firebaugh <jfirebaugh@kde.org>

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
#include <tdestandarddirs.h>
#include <kiconloader.h>
#include <twinmodule.h>
#include <tdepopupmenu.h>
#include <kpassivepopup.h>
#include <tdelocale.h>
#include <tdeprocess.h>
#include <kdebug.h>

// Skout
#include "skout_task.h"
#include "skout_task_container.h"

// NetWM
#include <netwm.h>

// Other
#include <cerrno>

SkoutTask::SkoutTask(SkoutTaskContainer *parent, WId w)
  : SkoutTaskButton(parent, SkoutTaskButton::Task),
    m_container(parent),
    m_window_id(w)
{
    setOn(info().isMinimized());
    parent->update();
    updateVisibility();
}

SkoutTask::~SkoutTask()
{
    TQTimer::singleShot(0, container(), TQ_SLOT(update()));
}

KWin::WindowInfo SkoutTask::info()
{
    return KWin::windowInfo(windowID());
}

SkoutTaskContainer* SkoutTask::container()
{
    return m_container;
}

KWinModule* SkoutTask::twin()
{
    return container()->twin();
}

TQString SkoutTask::name()
{
    TQString taskName = info().visibleNameWithState();

    if (container()->manager()->showDesktopNumber())
    {
        TQString deskName;
        int deskNo = desktop();
        if (deskNo > 1)
        {
            deskName = TQString::number(deskNo);
        }
        else if (deskNo == 0)
        {
            deskName = "*";
        }
        taskName = TQString("[%1] %2").arg(deskNo).arg(taskName);
    }

    return taskName;
}

TQPixmap SkoutTask::icon()
{
    return icon(smallIconSize());
}

TQPixmap SkoutTask::icon(TQSize size)
{
    TQPixmap ico = KWin::icon(windowID(), size.width(), size.height(), true);
    if (!ico.isNull()) {
        return ico;
    }

    ico = tdeApp->iconLoader()->loadIcon(className().lower(),
                                         TDEIcon::Panel, size.height(),
                                         TDEIcon::DefaultState,
                                         0, true);
    if (!ico.isNull())
    {
        return ico;
    }

    return container()->groupIcon();
}

TQString SkoutTask::className()
{
    return SkoutTaskMan::className(windowID());
}

TQString SkoutTask::classClass()
{
    return SkoutTaskMan::classClass(windowID());
}

TQString SkoutTask::applicationName()
{
    return container()->application();
}

void SkoutTask::updateVisibility()
{
    setShown(
      container()->grouper()->expanded() &&
      (container()->manager()->showAllDesktops() || isOnCurrentDesktop())
    );
}

void SkoutTask::mousePressEvent(TQMouseEvent *me)
{
    switch (me->button())
    {
        case LeftButton:
            if (me->state() & (ControlButton | AltButton))
            {
                close();
            }
            else
            {
                activate();
            }
            return;

        case MidButton:
            toggleIconified();
            return;

        default:
            me->ignore();
    }
}

void SkoutTask::contextMenuEvent(TQContextMenuEvent *cme)
{
    TDEPopupMenu ctx(this), desks(this);
    ctx.setCheckable(true);

    int item;
    item = ctx.insertItem(SmallIcon("go-top"), i18n("Stays on &top"),
                          this, TQ_SLOT(toggleStayAbove()));
    ctx.setItemChecked(item, staysAbove());

    item = ctx.insertItem(SmallIcon("go-bottom"), i18n("Stays on &bottom"),
                          this, TQ_SLOT(toggleStayBelow()));
    ctx.setItemChecked(item, staysBelow());

    ctx.insertSeparator();

    int numberOfDesktops = KWin::numberOfDesktops();

    if (numberOfDesktops > 1)
    {
        desks.clear();
        desks.setCheckable(true);

        int currentDesktop = KWin::currentDesktop();
        if (desktop() != currentDesktop)
        {
            item = ctx.insertItem(SmallIcon("desktop"), i18n("Move to &current desktop"), this, TQ_SLOT(setDesktop(int)));
            ctx.setItemParameter(item, currentDesktop);
        }

        item = desks.insertItem(i18n("&All desktops"), this, TQ_SLOT(setDesktop(int)));
        desks.setItemParameter(item, 0);
        desks.setItemChecked(item, desktop() == 0);

        desks.insertSeparator();

        for (int i = 1; i <= numberOfDesktops; ++i)
        {
            TQString deskName = twin()->desktopName(i).replace("&", "&&");
            item = desks.insertItem(TQString("[&%1] %2").arg(i).arg(deskName),
                                    this, TQ_SLOT(setDesktop(int)));
            desks.setItemParameter(item, i);
            desks.setItemChecked(item, desktop() == i);
        }

        ctx.insertItem(SmallIcon("kpager"), i18n("Move to &desktop..."), &desks);
        ctx.insertSeparator();
    }

    item = ctx.insertItem(i18n("S&hade"), this, TQ_SLOT(toggleShaded()));
    ctx.setItemChecked(item, isShaded());

    ctx.insertItem(SmallIcon("kicker"), i18n("&Hide to system tray"),
                   this, TQ_SLOT(sendToTray()));

    ctx.insertSeparator();

    item = ctx.insertItem(SmallIcon("view-fullscreen"), i18n("&Fullscreen"),
                          this, TQ_SLOT(toggleFullScreen()));
    ctx.setItemChecked(item, isFullScreen());

    ctx.insertSeparator();

    item = ctx.insertItem(TQPixmap(locate("data", "skout/pics/iconify.png")),
                          i18n("Mi&nimize"), this, TQ_SLOT(toggleIconified()));
    ctx.setItemChecked(item, isIconified());

    item = ctx.insertItem(TQPixmap(locate("data", "skout/pics/maximize.png")),
                          i18n("Ma&ximize"), this, TQ_SLOT(toggleMaximized()));
    ctx.setItemChecked(item, isMaximized());

    ctx.insertSeparator();

    ctx.insertItem(TQPixmap(locate("data", "skout/pics/close.png")),
                   i18n("&Close"), this, TQ_SLOT(close()));

    ctx.exec(cme->globalPos());
}

void SkoutTask::sendToTray()
{
    TDEProcess ksystray;
    ksystray << "ksystraycmd" << "--hidden"
             << "--wid" << TQString::number(windowID());
    bool ok = ksystray.start(TDEProcess::DontCare);
    if (!ok) {
        panelExt->popup("error",
            i18n("Unable to send \"%1\" to tray").arg(name()),
            i18n(strerror(errno)));
    }
}

void SkoutTask::setDesktop(int desktop)
{
    KWin::setOnAllDesktops(windowID(), desktop == 0);
    if (desktop > 0)
    {
        KWin::setOnDesktop(windowID(), desktop);
    }
}

bool SkoutTask::checkWindowState(unsigned long state)
{
    KWin::WindowInfo i(info());
    return i.valid() && (i.state() & state);
}

void SkoutTask::addWindowState(unsigned long state)
{
    KWin::setState(windowID(), state);
}

void SkoutTask::removeWindowState(unsigned long state)
{
    KWin::clearState(windowID(), state);
}

void SkoutTask::setWindowState(unsigned long state, bool set)
{
    if (set) addWindowState(state);
    else removeWindowState(state);
}

void SkoutTask::setMaximized(bool maximized)
{
    setWindowState(NET::MaxVert|NET::MaxHoriz, maximized);
}

void SkoutTask::setIconified(bool iconified)
{
    if (iconified) KWin::iconifyWindow(windowID());
    else KWin::deIconifyWindow(windowID());
}

void SkoutTask::setFullScreen(bool fullscreen)
{
    setWindowState(NET::FullScreen, fullscreen);
}

void SkoutTask::setStayAbove(bool stay)
{
    if (stay && staysBelow()) {
        setStayBelow(false);
    }
    setWindowState(NET::StaysOnTop, stay);
}

void SkoutTask::setStayBelow(bool stay)
{
    if (stay && staysAbove()) {
        setStayAbove(false);
    }
    setWindowState(NET::KeepBelow, stay);
}

void SkoutTask::setShaded(bool shaded)
{
    setWindowState(NET::Shaded, shaded);
}

void SkoutTask::toggleStayAbove()
{
    setStayAbove(!staysAbove());
}

void SkoutTask::toggleStayBelow()
{
    setStayBelow(!staysBelow());
}

void SkoutTask::toggleIconified()
{
    setIconified(!isIconified());
}

void SkoutTask::toggleMaximized()
{
    setMaximized(!isMaximized());
}

void SkoutTask::toggleFullScreen()
{
    setFullScreen(!isFullScreen());
}

void SkoutTask::toggleShaded()
{
    setShaded(!isShaded());
}

void SkoutTask::close()
{
    NETRootInfo ri(tqt_xdisplay(), NET::CloseWindow);
    ri.closeWindowRequest(windowID());
}

void SkoutTask::activate()
{
    KWin::forceActiveWindow(windowID());
}

bool SkoutTask::isActive()
{
    return container()->twin()->activeWindow() == windowID();
}

bool SkoutTask::staysAbove()
{
    return checkWindowState(NET::StaysOnTop);
}

bool SkoutTask::staysBelow()
{
    return checkWindowState(NET::KeepBelow);
}

bool SkoutTask::isIconified()
{
    KWin::WindowInfo i(info());
    return i.valid() && i.isMinimized();
}

bool SkoutTask::isMaximized()
{
    return checkWindowState(NET::MaxVert|NET::MaxHoriz);
}

bool SkoutTask::isFullScreen()
{
    return checkWindowState(NET::FullScreen);
}

bool SkoutTask::isShaded()
{
    return checkWindowState(NET::Shaded);
}

int SkoutTask::desktop()
{
    KWin::WindowInfo i(info());
    if (!i.valid()) return -1;

    if (i.onAllDesktops())
    {
        return 0;
    }

    return i.desktop();
}

bool SkoutTask::isOnCurrentDesktop()
{
    int deskno = desktop();
    return deskno == 0 || deskno == twin()->currentDesktop();
}

TQFont SkoutTask::font()
{
    return isActive() ? boldFont() : normalFont();
}

TQColorGroup SkoutTask::colors()
{
    TQColorGroup cg = SkoutTaskButton::colors();
    if (isActive())
    {
        cg.setColor(TQColorGroup::Button,     cg.highlight());
        cg.setColor(TQColorGroup::Background, cg.highlight());
        cg.setColor(TQColorGroup::ButtonText, cg.highlightedText());
    }
    else if (!container()->isActive())
    {
        cg.setColor(TQColorGroup::Button,     cg.background());
        cg.setColor(TQColorGroup::ButtonText, cg.foreground());
    }
    return cg;
}

/* Yes, I know this is deprecated and the actual application might be on another
   host, but this is the best way I can think of to get the application's
   executable and figure out its corresponding desktop file. */
pid_t SkoutTask::pid()
{
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

/* kate: replace-tabs true; tab-width 4; */