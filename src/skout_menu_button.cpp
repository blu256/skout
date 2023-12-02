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

// TQt
#include <tqsizepolicy.h>
#include <tqfontmetrics.h>
#include <tqimage.h>
#include <tqtooltip.h>

// TDE
#include <tdeglobalsettings.h>
#include <tdeapplication.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kimageeffect.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_menu_button.h"
#include "skout_menu.h"
#include "skout_panel.h"

SkoutMenuBtn::SkoutMenuBtn(SkoutPanel *panel)
  : KPushButton(panel, "SkoutMenuBtn")
{
    m_menu = new SkoutMenu(panel);

    setSizePolicy(TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    setBackgroundOrigin(AncestorOrigin);

    setText("Menu");
    setIconSet(kapp->iconLoader()->loadIconSet("go", TDEIcon::Panel, 22));

    TQToolTip::add(this, i18n("Skout Menu"));

    connect(this, SIGNAL(clicked()), SLOT(showMenu()));
}

SkoutMenuBtn::~SkoutMenuBtn() {
}

TQSize SkoutMenuBtn::sizeHint() const {
    TQFontMetrics fm(TDEGlobalSettings::generalFont());
    return TQSize(width(), fm.height() * 2);
}

void SkoutMenuBtn::showMenu() {
    SkoutPanel *panel = static_cast<SkoutPanel *>(parent());

    kdDebug() << m_menu->sizeHint().width() << endl;

    TQPoint origin;
    switch (panel->position()) {
        case PanelPosition::TopLeft:
            origin = geometry().topRight();
            break;

        case PanelPosition::TopRight:
        default:
            origin = geometry().topLeft() - TQPoint(m_menu->sizeHint().width(), 0);
            break;
    }
    m_menu->exec(mapToGlobal(origin));
}

void SkoutMenuBtn::hideMenu() {
}

#include "skout_menu_button.moc"