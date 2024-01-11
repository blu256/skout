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

#ifndef _SKOUT_MENU_H
#define _SKOUT_MENU_H

// TDE
#include <tdepopupmenu.h>
#include <kservicegroup.h>

#define ESCAPE_AMPERSAND(x) x.replace("&", "&&")

class KLineEdit;
class KBookmarkMenu;

class SkoutPanel;

class SkoutMenu : public TDEPopupMenu {
  TQ_OBJECT

  public:
    SkoutMenu(SkoutPanel *panel);
    SkoutMenu(SkoutPanel *panel, KServiceGroup::Ptr group);
    virtual ~SkoutMenu();

    SkoutPanel *panel();

    int addSubmenu(KServiceGroup *group, int id = -1, int index = -1);
    int addSubmenu(TDEPopupMenu *menu, TQString icon, TQString name,
                    int id = -1, int index = -1);

    int addItem(KService *service, const TQObject *receiver, const char *member,
                 int id = -1, int index = -1);
    int addItem(TQString icon, TQString name, int id = -1, int index = -1);
    int addItem(TQString icon, TQString name,
                const TQObject *receiver, const char *member,
                int id = -1, int index = -1);

  public slots:
    void populate(KServiceGroup::Ptr group = nullptr);
    void launch(int item);

  private slots:
    void showSearch();
    void hideSearch();
    void search(const TQString &str = TQString::null);

  protected:
    void keyPressEvent(TQKeyEvent *e);

  private:
    KServiceGroup::Ptr m_group;
    KServiceGroup::List m_list;
    KLineEdit *m_search;

    int firstSearchMatch();
};

#endif // _SKOUT_MENU_H