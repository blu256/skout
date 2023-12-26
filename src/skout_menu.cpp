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
#include <tdeapplication.h>
#include <kiconloader.h>
#include <kbookmarkmenu.h>
#include <tderecentdocument.h>
#include <tdemessagebox.h>
#include <klineedit.h>
#include <dcopref.h>
#include <krun.h>
#include <kdebug.h>

// Skout
#include "skout_menu.h"
#include "skout_panel.h"
#include "skout_utils.h"

// dmctl
#include "dmctl.h"

SkoutMenu::SkoutMenu(SkoutPanel *panel, KServiceGroup::Ptr group)
  : TDEPopupMenu(panel, "SkoutMenu"),
    m_search(nullptr)
{
    m_group = group ? group : KServiceGroup::root();
    populate();

    connect(this, SIGNAL(aboutToHide()), SLOT(hideSearch()));
}

SkoutMenu::~SkoutMenu() {
}

SkoutPanel *SkoutMenu::panel() {
    return static_cast<SkoutPanel *>(parent());
}

void SkoutMenu::populate() {
    clear();
    if (!m_group || !m_group->isValid()) return;

    m_list = m_group->entries(true);
    KServiceGroup::List::ConstIterator it;
    int index = -1, item;
    for (it = m_list.begin(); it != m_list.end(); ++it) {
        ++index;
        KSycocaEntry *p = (*it);
        if (p->isType(KST_KService)) {
            KService *s = static_cast<KService *>(p);
            if (s->noDisplay()) continue;

            insertItem(s->pixmap(TDEIcon::Small), ESCAPE_AMPERSAND(s->name()),
                       this, SLOT(launch(int)));
        }
        else if (p->isType(KST_KServiceGroup)){
            KServiceGroup *g = static_cast<KServiceGroup *>(p);
            if (g->noDisplay() || !g->childCount()) continue;

            TQPixmap icon = kapp->iconLoader()->loadIcon(
                g->icon(), TDEIcon::Small);

            SkoutMenu *sub = new SkoutMenu(panel(), g);
            item = insertItem(icon, ESCAPE_AMPERSAND(g->caption()), sub);
        }
        else continue;
        setItemParameter(item, index);
    }
}

void SkoutMenu::launch(int item) {
    int index = itemParameter(item);
    KSycocaEntry *entry = m_list[index];
    KService *service = static_cast<KService *>(entry);
    TQString desktop = service->desktopEntryPath();

    int started = TDEApplication::startServiceByDesktopPath(desktop);
    if (started == 0) {
        // Inform TDE Menu about launched application
        DCOPRef kickerKMenuIface("kicker", "KMenu");
        kickerKMenuIface.call("slotServiceStartedByStorageId(TQString,TQString)",
                              "SkoutMenu", desktop);
    }
}

void SkoutMenu::keyPressEvent(TQKeyEvent *e) {
    if (e->key() == TQt::Key_Escape) {
        if (m_search) {
            hideSearch();
        }
        else {
            hide();
        }
        e->accept();
        return;
    }

    if (m_search || e->text().isEmpty()) {
        e->ignore();
        return;
    }

    showSearch();
    m_search->setText(e->text());
    e->accept();
}

void SkoutMenu::showSearch() {
    if (m_search) return;
    m_search = new KLineEdit(this);
    insertItem(m_search, 1000);
    m_search->show();
    m_search->setFocus();
    connect(m_search, SIGNAL(textChanged(const TQString &)),
                      SLOT(search(const TQString &)));
}

void SkoutMenu::hideSearch() {
    if (m_search) {
        removeItem(1000);
        search();
        m_search = nullptr;
    }
}

void SkoutMenu::search(const TQString &str) {
    for (int i = 0; i < count(); ++i) {
        int id = idAt(i);
        if (id != 1000) {
            setItemEnabled(id, text(id).lower().contains(str.lower()));
        }
    }
}

SkoutRootMenu::SkoutRootMenu(SkoutPanel *panel)
  : SkoutMenu(panel, nullptr)
{
    insertSeparator();
    m_bookmarkMenu = new TDEPopupMenu(panel);
    m_bookmarks = new KBookmarkMenu(
        KBookmarkManager::userBookmarksManager(),
        new KBookmarkOwner(),
        m_bookmarkMenu,
        nullptr, true);
    insertItem(SmallIcon("user-bookmarks"), i18n("Bookmarks"), m_bookmarkMenu);

    m_recentsMenu = new TDEPopupMenu(panel);
    insertItem(SmallIcon("clock"), i18n("Recent documents"), m_recentsMenu);

    insertSeparator();

    if (kapp->authorize("run_command")) {
        insertItem(SmallIcon("system-run"), i18n("Run command..."),
                   this, SLOT(runCommand()));
    }

    m_sessionMenu = new TDEPopupMenu(panel);
    insertItem(SmallIcon("switchuser"), i18n("Switch User"), m_sessionMenu);

    if (kapp->authorize("lock_screen")) {
        insertItem(SmallIcon("system-lock-screen"), i18n("Lock Session"),
                   this, TQT_SLOT(lockScreen()));
    }

    if (kapp->authorize("logout"))
    {
        insertItem(SmallIcon("system-log-out"), i18n("Log Out..."),
                   this, TQT_SLOT(logOut()));
    }

    connect(m_sessionMenu, SIGNAL(aboutToShow()), SLOT(populateSessions()));
    connect(m_sessionMenu, SIGNAL(activated(int)), SLOT(activateSession(int)));

    connect(m_recentsMenu, SIGNAL(aboutToShow()), SLOT(populateRecentDocs()));
    connect(m_recentsMenu, SIGNAL(activated(int)), SLOT(openRecentDoc(int)));
}

SkoutRootMenu::~SkoutRootMenu() {
    ZAP(m_bookmarks)
    ZAP(m_bookmarkMenu)
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
            m_sessionMenu->insertItem(SmallIcon("system-lock-screen"),
                                      i18n("Lock Current && Start New Session"),
                                      SessionMenuItem::LockAndNewSession);

            m_sessionMenu->insertItem(SmallIcon("switchuser"),
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
            int id = m_sessionMenu->insertItem(DM::sess2Str(s), s.vt);
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
        m_recentsMenu->insertItem(SmallIcon(df.readIcon()), df.readName(), index);
        ++index;
    }
}

void SkoutRootMenu::openRecentDoc(int item) {
    TQString path = m_recentDocs[item - 100];
    if (!KDesktopFile::isDesktopFile(path)) return;
    (void) new KRun(path);
}

#include "skout_menu.moc"