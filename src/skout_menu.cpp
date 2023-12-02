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

/* TODO:
 * =====
 *  - if has focus, on typing a LineEdit widget is appended to index -1
 *    which displays search string and menu items are filtered like in KMenu
 */

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>
#include <dcopref.h>
#include <kdebug.h>

// Skout
#include "skout_menu.h"
#include "skout_panel.h"

SkoutMenu::SkoutMenu(SkoutPanel *panel, KServiceGroup::Ptr group)
  : TDEPopupMenu(panel, "SkoutMenu")
{
    m_group = group ? group : KServiceGroup::root();
    populate();
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

            item = insertItem(s->pixmap(TDEIcon::Small), s->name());
            connectItem(item, this, SLOT(launch(int)));
        }
        else if (p->isType(KST_KServiceGroup)){
            KServiceGroup *g = static_cast<KServiceGroup *>(p);
            if (g->noDisplay() || !g->childCount()) continue;

            TQPixmap icon = kapp->iconLoader()->loadIcon(
                g->icon(), TDEIcon::Small);

            SkoutMenu *sub = new SkoutMenu(panel(), g);
            item = insertItem(icon, g->caption(), sub);
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

#include "skout_menu.moc"