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

/* TODO
 * ----
 * => some sort of pager widget, possibly integrate what Kicker has
 * => watch for settings change via ipc and adapt
 * => should not always stay on top, possibly trigger by dcop call
 *    and active corner
 * => options dlg (minimal as in Tracker?)
 *    => should integrate into KControl
 *    => checkbox "Use Skout as my desktop panel instead of Kicker"
 *       (automatically launches or closes Skout)
 * => reorg filenames
 * => commit first edition to git =)
 * => shortcuts
 * => some sort of system monitors (minimal and very integrated look)
 * => ability to pin programs in taskman
 * => help (e.g. user manual)
 * => more layouts/positions
 */

Skout::Skout(int pos) : DCOPObject("SkoutIface") {
    disableSessionManagement();

    if (!kapp->dcopClient()->isRegistered()) {
        kapp->dcopClient()->registerAs("skout");
    }
    kapp->dcopClient()->setDefaultObject("SkoutIface");

    SkoutSettings::instance("skoutrc");

    if (pos > -1) {
        m_panel = new SkoutPanel((PanelPosition)pos, true);
    }
    else m_panel = new SkoutPanel();
    setTopWidget(m_panel);
    m_panel->show();
}

Skout::~Skout() {
    delete m_panel;
    m_panel = nullptr;
}

void Skout::reconfigure() {
    SkoutSettings::self()->readConfig();
    m_panel->applyPosition();
}

void Skout::quit() {
    kapp->quit();
}

#include "skout.moc"