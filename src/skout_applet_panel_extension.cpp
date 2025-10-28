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

// TDE
#include <kdebug.h>

// Skout
#include "skout_applet_panel_extension.h"

static SkoutAppletPanelExtension *skoutAppletPanelExtension = nullptr;
SkoutAppletPanelExtension *SkoutAppletPanelExtension::instance() {
    if (!skoutAppletPanelExtension) {
        skoutAppletPanelExtension = new SkoutAppletPanelExtension();
    }
    return skoutAppletPanelExtension;
}

SkoutAppletPanelExtension::SkoutAppletPanelExtension()
: TQObject()
{}

SkoutAppletPanelExtension::~SkoutAppletPanelExtension() {}

void SkoutAppletPanelExtension::popup(TQString icon, TQString caption,
                                      TQString message)
{
    emit popupRequest(icon, caption, message);
}

void SkoutAppletPanelExtension::launch(KService::Ptr service, KURL::List urls)
{
    emit launchRequest(service, urls);
}

#include "skout_applet_panel_extension.moc"

/* kate: replace-tabs true; tab-width 4; */