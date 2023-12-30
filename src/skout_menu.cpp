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

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <dcopref.h>

// Skout
#include "skout_menu.h"
#include "skout_panel.h"
#include "skout_utils.h"

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

            item = insertItem(s->pixmap(TDEIcon::Small),
                              ESCAPE_AMPERSAND(s->name()),
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