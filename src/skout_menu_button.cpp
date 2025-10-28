/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-2025 Mavridis Philippe <mavridisf@gmail.com>

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
#include <tqwhatsthis.h>

// TDE
#include <tdeglobalsettings.h>
#include <tdeapplication.h>
#include <tdestandarddirs.h>
#include <kiconloader.h>
#include <kimageeffect.h>
#include <tdepopupmenu.h>
#include <khelpmenu.h>
#include <tdelocale.h>

// Skout
#include "skout_menu_button.h"
#include "skout_root_menu.h"
#include "skout_panel.h"

static KHelpMenu *helpMenu = nullptr;

SkoutMenuBtn::SkoutMenuBtn(SkoutPanel *panel)
  : KPushButton(panel, "SkoutMenuBtn")
{
    if (!helpMenu)
    {
        helpMenu = new KHelpMenu(0, tdeApp->aboutData());
    }

    m_menu = new SkoutRootMenu(panel);

    setSizePolicy(TQSizePolicy::Expanding, TQSizePolicy::Fixed);
    setBackgroundOrigin(AncestorOrigin);

    setText(i18n("Menu"));
    setIconSet(tdeApp->iconLoader()->loadIconSet("go", TDEIcon::Panel, 22));

    TQToolTip::add(this, i18n("Skout Menu"));
    TQWhatsThis::add(this, i18n("The Skout Menu provides access to installed "
                                "applications, locations and common actions."));
}

SkoutMenuBtn::~SkoutMenuBtn()
{}

TQSize SkoutMenuBtn::sizeHint() const
{
    TQFontMetrics fm(TDEGlobalSettings::generalFont());
    return TQSize(width(), fm.height() * 2);
}

void SkoutMenuBtn::mousePressEvent(TQMouseEvent *e)
{
    if (e->button() == TQt::LeftButton)
    {
        showMenu();
    }
    else if (e->button() == TQt::RightButton)
    {
        TDEIconLoader *il = TDEGlobal::iconLoader();
        TDEPopupMenu ctx;

        if (tdeApp->authorizeTDEAction("menuedit"))
        {
            ctx.insertItem(il->loadIconSet("kmenuedit", TDEIcon::Small),
                           i18n("Edit menu"),
                           panel(), TQ_SLOT(launchMenuEditor()));
            ctx.insertSeparator();
        }

        ctx.insertItem(il->loadIconSet("configure", TDEIcon::Small),
                       i18n("Skout Preferences"),
                       panel(), TQ_SLOT(configure()));
        ctx.insertSeparator();

        ctx.insertItem(il->loadIconSet("help", TDEIcon::Small),
                       i18n("Help..."), helpMenu->menu());

        ctx.exec(mapToGlobal(e->pos()));
    }
}

void SkoutMenuBtn::showMenu() {
    SkoutPanel *panel = static_cast<SkoutPanel *>(parent());

    TQPoint origin;
    switch (panel->position())
    {
        case PanelPosition::TopLeft:
            origin = geometry().topRight();
            break;

        case PanelPosition::TopRight:
        default:
            origin = geometry().topLeft() - TQPoint(m_menu->sizeHint().width(), 0);
            break;
    }
    m_menu->popup(mapToGlobal(origin));
}

#include "skout_menu_button.moc"

/* kate: replace-tabs true; tab-width 4; */