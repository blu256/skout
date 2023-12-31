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

// Skout
#include "skout_applet.h"
#include "skout_panel.h"

SkoutApplet::SkoutApplet(SkoutPanel *parent, const char *name)
  : TQFrame(parent, name),
    m_panel(parent)
{
}

SkoutApplet::~SkoutApplet() {}

bool SkoutApplet::valid() {
    return false;
}

TQWidget *SkoutApplet::panel() {
    return static_cast<TQWidget *>(m_panel);
}

TQString SkoutApplet::lastErrorMessage() {
    return TQString::null;
}

void SkoutApplet::resizeEvent(TQResizeEvent *e) {
    emit updateGeometry();
}

void SkoutApplet::popup(TQString icon, TQString caption, TQString message) {
    emit showPopup(icon, caption, message);
}

void SkoutApplet::launch(TQString application, TQStringList args,
                         TQString description, bool isService)
{
    emit doLaunch(application, args, description, isService);
}

#include "skout_applet.moc"