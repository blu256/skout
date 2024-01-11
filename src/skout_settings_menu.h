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

#ifndef _SKOUT_SETTINGS_MENU_H
#define _SKOUT_SETTINGS_MENU_H

// TDE
#include <kservice.h>

// Skout
#include "skout_menu.h"

#define KCONTROL_ITEM 100

class SkoutSettingsMenu : public SkoutMenu {
  TQ_OBJECT

  public:
    SkoutSettingsMenu(SkoutPanel *panel);
    virtual ~SkoutSettingsMenu();

  public slots:
    void launchKControl();

  private:
    KService::Ptr tdecontrol;
};

#endif // _SKOUT_SETTINGS_MENU_H