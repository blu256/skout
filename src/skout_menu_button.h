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

#ifndef _SKOUT_MENU_BUTTON_H
#define _SKOUT_MENU_BUTTON_H

#include <kpushbutton.h>

class SkoutPanel;
class SkoutMenu;

class SkoutMenuBtn : public KPushButton {
  TQ_OBJECT

  public:
    SkoutMenuBtn(SkoutPanel *panel);
    virtual ~SkoutMenuBtn();

  public slots:
    void showMenu();
    void hideMenu();

  protected:
    TQSize sizeHint() const;

  private:
    SkoutMenu *m_menu;
};

#endif // _SKOUT_MENU_BUTTON_H