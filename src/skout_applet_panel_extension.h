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

#ifndef _SKOUT_APPLET_PANEL_EXTENSION_H
#define _SKOUT_APPLET_PANEL_EXTENSION_H

// TQt
#include <tqobject.h>

// TDE
#include <kservice.h>
#include <kurl.h>

#define panelExt SkoutAppletPanelExtension::instance()

class SkoutAppletPanelExtension : public TQObject {
  TQ_OBJECT

  public:
    static SkoutAppletPanelExtension *instance();

    void popup(TQString icon, TQString caption, TQString message);
    void launch(KService::Ptr service, KURL::List urls = KURL::List());

  signals:
    void popupRequest(TQString icon, TQString caption, TQString message);
    void launchRequest(KService::Ptr service, KURL::List urls = KURL::List());

  private:
    SkoutAppletPanelExtension();
    ~SkoutAppletPanelExtension();
};

#endif // _SKOUT_H