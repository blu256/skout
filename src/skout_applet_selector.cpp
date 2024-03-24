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
#include <tqfontmetrics.h>

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>

// Skout
#include "skout_applet_selector.h"

#define ICON_SIZE 24
#define ICON_MARGIN 3

/* --- SkoutAppletItem class ------------------------------------------------ */
SkoutAppletItem::SkoutAppletItem(AppletData &appletData)
  : TQListBoxPixmap(kapp->iconLoader()->loadIcon(appletData.icon,
                                                 TDEIcon::Panel, ICON_SIZE)),
    m_data(appletData)
{}

int SkoutAppletItem::height(const TQListBox *lb) const {
    return height(lb->fontMetrics());
}

int SkoutAppletItem::height(const TQFontMetrics fm) const {
    return TQMAX((2 * ICON_MARGIN) + ICON_SIZE,
                 (3 * ICON_MARGIN) + (2 * fm.height()));
}

void SkoutAppletItem::paint(TQPainter *p) {
    TQPoint pos(ICON_MARGIN, ICON_MARGIN);

    TQFont f(p->font());
    TQFontMetrics fm(f);

    int h = height(fm);
    TQRect pixRect(0, 0, (2 * ICON_MARGIN) + ICON_SIZE, h);
    TQRect pix(pos, TQSize(ICON_SIZE, ICON_SIZE));
    pix.moveCenter(pixRect.center());
    p->drawPixmap(pix, *pixmap());

    pos.setX(pos.x() + (2 * ICON_MARGIN) + ICON_SIZE);
    pos.setY(fm.height() * 2 + ICON_MARGIN);

    p->drawText(pos, m_data.comment);

    pos.setY(pos.y() - fm.height() - ICON_MARGIN);

    f.setBold(true);
    p->setFont(f);
    fm = TQFontMetrics(f);

    p->drawText(pos, m_data.name);

    pos.setY(pos.y() + ICON_MARGIN);
    p->drawLine(pos.x(), pos.y(),
                pos.x() + fm.width(m_data.name), pos.y());
}

/* --- SkoutAppletSelector class -------------------------------------------- */
SkoutAppletSelector::SkoutAppletSelector(TQWidget *parent, const char *name)
  : TDEActionSelector(parent, name)
{
    connect(this, TQ_SIGNAL(added(TQListBoxItem *)), TQ_SIGNAL(changed()));
    connect(this, TQ_SIGNAL(removed(TQListBoxItem *)), TQ_SIGNAL(changed()));
    connect(this, TQ_SIGNAL(movedUp(TQListBoxItem *)), TQ_SIGNAL(changed()));
    connect(this, TQ_SIGNAL(movedDown(TQListBoxItem *)), TQ_SIGNAL(changed()));
}

void SkoutAppletSelector::insertApplet(AppletData &applet) {
    availableListBox()->insertItem(new SkoutAppletItem(applet));
}

void SkoutAppletSelector::insertActiveApplet(AppletData &applet, int index) {
    selectedListBox()->insertItem(new SkoutAppletItem(applet), index);
}

#include "skout_applet_selector.moc"