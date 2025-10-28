/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-2025 Philippe Mavridis <philippe.mavridis@yandex.com>

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

// Skout
#include "skout_applet.h"
#include "skout_panel.h"

SkoutApplet::SkoutApplet(SkoutPanel *parent, TDEConfig *cfg, const char *name)
  : TQFrame(parent, name),
    m_cfg(cfg)
{
    SkoutAppletPanelExtension::instance();
    connect(parent, TQ_SIGNAL(appletsReconfigure()), TQ_SLOT(reconfigure()));
}

SkoutApplet::~SkoutApplet() {}

bool SkoutApplet::valid()
{
    return false;
}

TQString SkoutApplet::lastErrorMessage()
{
    return TQString::null;
}

void SkoutApplet::resizeEvent(TQResizeEvent *e) {
    emit updateGeometry();
}

/// slot
void SkoutApplet::reconfigure()
{
}

#include "skout_applet.moc"

/* kate: replace-tabs true; tab-width 4; */