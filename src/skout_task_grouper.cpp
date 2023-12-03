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
#include <tqobjectlist.h>
#include <tqpainter.h>

// TDE
#include <tdeapplication.h>
#include <kiconloader.h>

// Skout
#include "skout_task_grouper.h"
#include "skout_task_container.h"

static TQPixmap *pxHide = nullptr;
static TQPixmap *pxExpand = nullptr;

SkoutTaskGrouper::SkoutTaskGrouper(SkoutTaskContainer *parent, TQString name)
  : SkoutTaskButton(parent, true, true),
    m_name(name),
    m_expanded(true)
{
    if (!pxHide || !pxExpand || pxHide->isNull() || pxExpand->isNull()) {
        updateStaticPixmaps();
    }
    setOn(true);
    connect(this, SIGNAL(toggled(bool)), SLOT(toggle(bool)));
}

SkoutTaskGrouper::~SkoutTaskGrouper() {
}

void SkoutTaskGrouper::updateStaticPixmaps() {
    int h = smallIconSize().height();
    pxHide = new TQPixmap(kapp->iconLoader()->loadIcon("1uparrow", TDEIcon::Small, h));
    pxExpand = new TQPixmap(kapp->iconLoader()->loadIcon("1downarrow", TDEIcon::Small, h));
}

TQString SkoutTaskGrouper::name() {
    return m_name;
}

TQPixmap SkoutTaskGrouper::icon() {
    return m_icon.isNull() ? defaultIcon() : m_icon;
}

void SkoutTaskGrouper::setIcon(TQPixmap icon) {
    m_icon = icon;
    update();
}

void SkoutTaskGrouper::toggle(bool show) {
    setExpanded(show);
    TQObjectList widgets = container()->tasks();
    TQObjectListIt it(widgets);
    TQWidget *w;
    while ((w = static_cast<TQWidget *>(it.current())) != nullptr) {
        if (show) w->show();
        else w->hide();
        ++it;
    }
}

TQFont SkoutTaskGrouper::font() {
    return container()->active() ? boldFont() : normalFont() ;
}

TQColorGroup SkoutTaskGrouper::colors() {
    TQColorGroup cg = palette().active();
    if (container()->active()) {
      cg.setColor(TQColorGroup::Button,     cg.highlight());
      cg.setColor(TQColorGroup::ButtonText, cg.highlightedText());
    }
    return cg;
}

void SkoutTaskGrouper::drawButton(TQPainter *p) {
    SkoutTaskButton::drawButton(p);

    TQPixmap pix = *(expanded() ? pxHide : pxExpand);
    TQPoint offset = smallIconOffset();

    TQPoint origin(width() - margin().x() - pix.width() - offset.x(),
                   margin().y() + offset.y());
    p->drawPixmap(origin, pix);
}

#include "skout_task_grouper.moc"