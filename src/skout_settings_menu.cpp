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

// Skout
#include "skout_settings_menu.h"
#include "skout_panel.h"

SkoutSettingsMenu::SkoutSettingsMenu(SkoutPanel *panel)
  : SkoutMenu(panel, KServiceGroup::group("Settings/"))
{
    tdecontrol = KService::serviceByDesktopName("kcontrol");
    if (tdecontrol->isValid()) {
        insertItem(SmallIcon(tdecontrol->icon()), tdecontrol->name(),
                   this, SLOT(launchControlCentre()), 0,
                   CONTROL_CENTRE_ITEM, 0);
        insertSeparator(1);
    }
}

SkoutSettingsMenu::~SkoutSettingsMenu() {
}

void SkoutSettingsMenu::launchControlCentre() {
    // TODO: error handling
    kapp->startServiceByDesktopPath(tdecontrol->desktopEntryPath());
}

#include "skout_settings_menu.moc"