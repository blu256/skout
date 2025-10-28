/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-2025 Mavridis Philippe <mavridisf@gmail.com>

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

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <dcopref.h>

// Skout
#include "skout_menu.h"
#include "skout_panel.h"
#include "skout_utils.h"

SkoutMenu::SkoutMenu(SkoutPanel *panel)
  : TDEPopupMenu(panel, "SkoutMenu"),
    m_search(nullptr)
{
    connect(this, TQ_SIGNAL(aboutToHide()), TQ_SLOT(hideSearch()));
}

SkoutMenu::SkoutMenu(SkoutPanel *panel, KServiceGroup::Ptr group)
 : SkoutMenu(panel)
{
    populate(group);
}

SkoutMenu::~SkoutMenu() {
    ZAP(m_search)
}

SkoutPanel *SkoutMenu::panel() {
    return static_cast<SkoutPanel *>(parent());
}

int SkoutMenu::addSubmenu(KServiceGroup *group, int id, int index) {
    if (group->noDisplay() || !group->childCount()) return -1;
    SkoutMenu *submenu = new SkoutMenu(panel(), group);
    return insertItem(SmallIconSet(group->icon()),
                      ESCAPE_AMPERSAND(group->caption()),
                      submenu, id, index);
}

int SkoutMenu::addSubmenu(TDEPopupMenu *menu, TQString icon, TQString name,
                           int id, int index)
{
    TQIconSet iconSet;
    if (!icon.isNull()) {
        iconSet = SmallIconSet(icon);
    }
    return insertItem(iconSet, name, menu, id, index);
}

int SkoutMenu::addItem(KService *service, const TQObject *receiver, const char *member,
                       int id, int index)
{
    if (service->noDisplay()) return -1;
    return insertItem(SmallIconSet(service->icon()),
                      ESCAPE_AMPERSAND(service->name()),
                      receiver, member, 0, id, index);
}

int SkoutMenu::addItem(TQString icon, TQString name, int id, int index) {
    TQIconSet iconSet;
    if (!icon.isNull()) {
        iconSet = SmallIconSet(icon);
    }
    return insertItem(iconSet, ESCAPE_AMPERSAND(name), id, index);
}

int SkoutMenu::addItem(TQString icon, TQString name,
                       const TQObject *receiver, const char *member,
                       int id, int index)
{
    TQIconSet iconSet;
    if (!icon.isNull()) {
        iconSet = SmallIconSet(icon);
    }
    return insertItem(iconSet, ESCAPE_AMPERSAND(name), receiver, member, 0,
                      id, index);
}

void SkoutMenu::populate(KServiceGroup::Ptr group) {
    m_group = group ? group : KServiceGroup::root();

    clear();
    if (!m_group || !m_group->isValid()) return;

    m_list = m_group->entries(true);
    KServiceGroup::List::ConstIterator it;
    int id = 100;
    for (it = m_list.begin(); it != m_list.end(); ++it) {
        KSycocaEntry *p = (*it);
        if (p->isType(KST_KService)) {
            KService *s = static_cast<KService *>(p);
            addItem(s, this, TQ_SLOT(launch(int)), id);
        }
        else if (p->isType(KST_KServiceGroup)){
            KServiceGroup *g = static_cast<KServiceGroup *>(p);
            addSubmenu(g, id);
        }
        ++id;
    }
}

void SkoutMenu::launch(int id) {
    id -= 100;
    KSycocaEntry *entry = m_list[id];
    KService *service = static_cast<KService *>(entry);

    if (panel()->launch(service)) {
        // Inform TDE Menu about launched application
        DCOPRef kickerKMenuIface("kicker", "KMenu");
        kickerKMenuIface.call("slotServiceStartedByStorageId(TQString,TQString)",
                              "SkoutMenu", service->desktopEntryPath());
    }
}

void SkoutMenu::keyPressEvent(TQKeyEvent *e) {
    if (e->key() == TQt::Key_Return) {
        if (m_search) {
            int index = firstSearchMatch();
            if (index != -1) {
                activateItemAt(index);
                hideSearch();
            }
        }
        else {
            // Pass event to the menu
            TDEPopupMenu::keyPressEvent(e);
            return;
        }
    }

    else if (e->key() == TQt::Key_Escape) {
        if (m_search) {
            hideSearch();
        }
        else {
            hide();
        }
    }

    else if (m_search && e->key() == TQt::Key_Tab) {
        int index = firstSearchMatch();
        if (index != -1) {
            setActiveItem(index);
        }
    }

    else if (m_search || e->text().isEmpty()) {
        e->ignore();
        return;
    }

    else {
        showSearch();
        m_search->setText(e->text());
    }

    e->accept();
}

void SkoutMenu::showSearch() {
    if (m_search) return;
    m_search = new KLineEdit(this);
    insertItem(m_search, 1000);
    m_search->show();
    m_search->setFocus();
    connect(m_search, TQ_SIGNAL(textChanged(const TQString &)),
                      TQ_SLOT(search(const TQString &)));
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

int SkoutMenu::firstSearchMatch() {
    if (m_search) {
        for (int i = 0; i < count(); ++i) {
            if (isItemEnabled(idAt(i))) {
                return i;
            }
        }
    }
    return -1;
}

#include "skout_menu.moc"

/* kate: replace-tabs true; tab-width 4; */