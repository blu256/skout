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

  Portions based on Kicker's TDEMenu:

    Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
    AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
    AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Improvements and feedback are welcome!
*******************************************************************************/

// TDE
#include <kbookmarkmenu.h>
#include <tderecentdocument.h>
#include <tdemessagebox.h>
#include <dcopref.h>
#include <krun.h>

// dmctl
#include "dmctl.h"

// Skout
#include "skout_root_menu.h"
#include "skout_settings_menu.h"
#include "skout_panel.h"
#include "skout_utils.h"

SkoutRootMenu::SkoutRootMenu(SkoutPanel *panel)
  : SkoutMenu(panel, nullptr)
{
    insertSeparator();
    m_bookmarkMenu = new SkoutMenu(panel);
    m_bookmarks = new KBookmarkMenu(
        KBookmarkManager::userBookmarksManager(),
        new KBookmarkOwner(),
        m_bookmarkMenu,
        nullptr, true);
    addSubmenu(m_bookmarkMenu, "bookmark_folder", i18n("Bookmarks"));

    m_recentsMenu = new SkoutMenu(panel);
    addSubmenu(m_recentsMenu, "clock", i18n("Recent documents"));

    insertSeparator();

    m_settingsMenu = new SkoutSettingsMenu(panel);
    addSubmenu(m_settingsMenu, "kcontrol", i18n("Settings"));

    insertSeparator();

    if (kapp->authorize("run_command")) {
        addItem("system-run", i18n("Run command..."), this, SLOT(runCommand()));
    }

    m_sessionMenu = new SkoutMenu(panel);
    addSubmenu(m_sessionMenu, "switchuser", i18n("Switch User"));

    if (kapp->authorize("lock_screen")) {
        addItem("system-lock-screen", i18n("Lock Session"), this, SLOT(lockScreen()));
    }

    if (kapp->authorize("logout"))
    {
        addItem("system-log-out", i18n("Log Out..."), this, SLOT(logOut()));
    }

    connect(m_sessionMenu, SIGNAL(aboutToShow()), SLOT(populateSessions()));
    connect(m_sessionMenu, SIGNAL(activated(int)), SLOT(activateSession(int)));

    connect(m_recentsMenu, SIGNAL(aboutToShow()), SLOT(populateRecentDocs()));
    connect(m_recentsMenu, SIGNAL(activated(int)), SLOT(openRecentDoc(int)));
}

SkoutRootMenu::~SkoutRootMenu() {
    ZAP(m_bookmarks)
    ZAP(m_bookmarkMenu)
    ZAP(m_settingsMenu)
    ZAP(m_sessionMenu)
}

void SkoutRootMenu::runCommand() {
    DCOPRef kdesktop("kdesktop", "KDesktopIface");
    kdesktop.send("popupExecuteCommand()");
}

void SkoutRootMenu::lockScreen() {
    DCOPRef kdesktop("kdesktop", "KScreensaverIface");
    kdesktop.send("lock()");
}

void SkoutRootMenu::logOut() {
    hide();
    kapp->requestShutDown();
}

void SkoutRootMenu::populateSessions() {
    DM dm;
    m_sessionMenu->clear();
    int p = dm.numReserve();
    if (kapp->authorize("start_new_session") && p >= 0) {
        if (kapp->authorize("lock_screen")) {
            m_sessionMenu->addItem("system-lock-screen",
                                   i18n("Lock Current && Start New Session"),
                                   SessionMenuItem::LockAndNewSession);

            m_sessionMenu->addItem("switchuser",
                                   i18n("Start New Session"),
                                   SessionMenuItem::NewSession);

            if (!p) {
                m_sessionMenu->setItemEnabled(SessionMenuItem::LockAndNewSession,
                                              false);

                m_sessionMenu->setItemEnabled(SessionMenuItem::NewSession,
                                              false);
            }
        }
        m_sessionMenu->insertSeparator();
    }

    SessList sessions;
    if (dm.localSessions(sessions)) {
        SessList::ConstIterator it = sessions.begin();
        for (; it != sessions.end(); ++it) {
            SessEnt s(*it);
            int id = m_sessionMenu->addItem(TQString::null, DM::sess2Str(s), s.vt);
            m_sessionMenu->setItemEnabled(id, s.vt);
            m_sessionMenu->setItemChecked(id, s.self);
        }
    }
}

void SkoutRootMenu::activateSession(int item) {
    if (item == SessionMenuItem::LockAndNewSession) {
        startNewSession();
    }
    else if (item == SessionMenuItem::NewSession) {
        startNewSession(false);
    }
    else if (!m_sessionMenu->isItemChecked(item)) {
        DM().lockSwitchVT(item);
    }
}

void SkoutRootMenu::startNewSession(bool lockCurrent) {
    int result = KMessageBox::warningContinueCancel(0,
        i18n("<p>You have chosen to open another desktop session.<br>"
               "The current session will be hidden "
               "and a new login screen will be displayed.<br>"
               "An F-key is assigned to each session; "
               "F%1 is usually assigned to the first session, "
               "F%2 to the second session and so on. "
               "You can switch between sessions by pressing "
               "Ctrl, Alt and the appropriate F-key at the same time. "
               "Additionally, the TDE Panel and Desktop menus have "
               "actions for switching between sessions.</p>")
                           .arg(7).arg(8),
        i18n("Warning - New Session"),
        KGuiItem(i18n("&Start New Session"), "fork"),
        ":confirmNewSession",
        KMessageBox::PlainCaption | KMessageBox::Notify);

    if (result == KMessageBox::Cancel) return;

    if (lockCurrent) lockScreen();
    DM().startReserve();
}

void SkoutRootMenu::populateRecentDocs() {
    m_recentsMenu->clear();
    m_recentDocs = TDERecentDocument::recentDocuments();
    TQStringList::ConstIterator it;
    int index = 100;
    for (it = m_recentDocs.begin(); it != m_recentDocs.end(); ++it) {
        KDesktopFile df((*it));
        m_recentsMenu->addItem(df.readIcon(), df.readName(), index);
        ++index;
    }
}

void SkoutRootMenu::openRecentDoc(int item) {
    TQString path = m_recentDocs[item - 100];
    if (!KDesktopFile::isDesktopFile(path)) return;
    (void) new KRun(path);
}

#include "skout_root_menu.moc"