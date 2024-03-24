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
#include <dcopclient.h>

// Skout
#include "skout.h"
#include "skoutsettings.h"
#include "skout_utils.h"

Skout::Skout(PanelPosition pos) : DCOPObject("SkoutIface") {
    disableSessionManagement();

    if (!kapp->dcopClient()->isRegistered()) {
        kapp->dcopClient()->registerAs("skout");
    }
    kapp->dcopClient()->setDefaultObject("SkoutIface");

    SkoutSettings::instance("skoutrc");

    m_panel = new SkoutPanel();
    if (pos != PanelPosition::Saved) {
        m_panel->setPosition(pos, true);
    }
    setTopWidget(m_panel);
    m_panel->show();
}

Skout::~Skout() {
    ZAP(m_panel)
}

bool Skout::ping() {
    return true;
}

void Skout::reconfigure() {
    m_panel->reconfigure();
}

void Skout::quit() {
    kapp->quit();
}

#include "skout.moc"