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
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <tdepopupmenu.h>
#include <kpropertiesdialog.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_task_grouper.h"
#include "skout_task_container.h"

static TQPixmap *pxHide = nullptr;
static TQPixmap *pxExpand = nullptr;
static TQPixmap *pxLink = nullptr;

SkoutTaskGrouper::SkoutTaskGrouper(SkoutTaskContainer *parent, TQString name)
  : SkoutTaskButton(parent, true, true),
    m_expanded(true),
    m_pinned(false)
{
    if (!pxHide || !pxExpand || !pxLink ||
        pxHide->isNull() || pxExpand->isNull() || pxLink->isNull())
    {
        updateStaticPixmaps();
    }

    setOn(true);
    connect(this, SIGNAL(toggled(bool)), SLOT(setExpanded(bool)));
}

SkoutTaskGrouper::~SkoutTaskGrouper() {
}

#define ICON(icon, group, size) \
    new TQPixmap(kapp->iconLoader()->loadIcon(icon, group, size));
#define ICON_SMALL(icon) ICON(icon, TDEIcon::Small, smallIconSize().height())
#define ICON_BIG(icon)   ICON(icon, TDEIcon::Panel, bigIconSize().height())
void SkoutTaskGrouper::updateStaticPixmaps() {
    TDEIconLoader *il = kapp->iconLoader();
    pxHide   = new TQPixmap(il->loadIcon("1uparrow",
                                         TDEIcon::Small, smallIconSize().height(),
                                         TDEIcon::DefaultState, nullptr, true));

    pxExpand = new TQPixmap(il->loadIcon("1downarrow",
                                         TDEIcon::Small, smallIconSize().height(),
                                         TDEIcon::DefaultState, nullptr, true));

    pxLink   = new TQPixmap(il->loadIcon(il->theme()->linkOverlay(),
                                         TDEIcon::Panel, bigIconSize().height(),
                                         TDEIcon::DefaultState, nullptr, true));
}
#undef ICON
#undef ICON_SMALL
#undef ICON_BIG

TQString SkoutTaskGrouper::name() {
    return container()->applicationName();
}

TQPixmap SkoutTaskGrouper::icon() {
    TQPixmap groupIcon = container()->groupIcon();
    return groupIcon.isNull() ? defaultIcon() : groupIcon;
}

void SkoutTaskGrouper::setExpanded(bool expanded) {
    m_expanded = expanded;
    TQObjectList widgets = container()->tasks();
    TQObjectListIt it(widgets);
    TQWidget *w;
    while ((w = static_cast<TQWidget *>(it.current())) != nullptr) {
        if (expanded) w->show();
        else w->hide();
        ++it;
    }
    repaint();
}

bool SkoutTaskGrouper::pinnable() {
    return (container()->service() != nullptr);
}

void SkoutTaskGrouper::pin() {
    if (!pinnable()) return;
    m_pinned = true;
    repaint();
    emit pinChanged(true);
}

void SkoutTaskGrouper::unpin() {
    m_pinned = false;
    repaint();
    emit pinChanged(false);
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

    // arrow
    if (container()->tasks().count() > 0) {
        TQPixmap pix = *(expanded() ? pxHide : pxExpand);
        TQPoint offset = smallIconOffset();

        TQPoint origin(width() - margin().x() - pix.width() - offset.x(),
                      margin().y() + offset.y());
        p->drawPixmap(origin, pix);
    }

    // link
    if (m_pinned) {
        TQPoint origin = TQPoint(margin());
        p->drawPixmap(origin, *pxLink);
    }
}

void SkoutTaskGrouper::contextMenuEvent(TQContextMenuEvent *cme) {
    TDEPopupMenu ctx;
    int item;

    if (container()->tasks().count()) {
        item = ctx.insertItem(SmallIcon("taskbar"), i18n("Minimize all"),
                              container(), SLOT(toggleIconifiedAll()));
        ctx.setItemChecked(item, container()->allIconified());
    }

    if (pinnable()) {
      if (pinned()) {
          ctx.insertItem(TQPixmap(locate("data", "skout/pics/pindown.png")),
                        i18n("Unpin application"), this, SLOT(unpin()));
      }
      else {
          ctx.insertItem(TQPixmap(locate("data", "skout/pics/pinup.png")),
                        i18n("Pin application"), this, SLOT(pin()));
      }
    }

    KService::Ptr s = container()->service();
    if (s && s->desktopEntryPath() != TQString::null) {
        ctx.insertSeparator();
        ctx.insertItem(SmallIcon("document-properties"), i18n("Properties"),
                       this, SLOT(showPropertiesDialog()));
    }

    if (ctx.count()) {
        ctx.exec(cme->globalPos());
        cme->accept();
    }
}

void SkoutTaskGrouper::mousePressEvent(TQMouseEvent *me) {
    switch (me->button()) {
        case LeftButton:
            if (container()->tasks().count() && !(me->state() & ControlButton)) {
                toggle();
            }
            return;
        case MidButton:
            container()->toggleIconifiedAll();
            return;
        default:
            me->ignore();
    }
}
void SkoutTaskGrouper::mouseDoubleClickEvent(TQMouseEvent *me) {
    if (me->button() == LeftButton && container()->service()) {
        KService::Ptr svc = container()->service();
        TQString path(svc->desktopEntryPath());
        if (path.isNull()) return;

        TQString error;
        if (kapp->startServiceByDesktopPath(path, TQString::null, &error) != 0) {
            container()->manager()->popup("error",
                i18n("Unable to launch %1").arg(svc->name()),
                i18n(error.local8Bit()));
        }

        // HACK the second click gets consumed
        if (container()->tasks().count() && !(me->state() & ControlButton)) {
            toggle();
        }
    }
    me->ignore();
}

void SkoutTaskGrouper::showPropertiesDialog() {
    const KURL path = container()->desktopPath();
    if (!path.isValid()) { // should never happen actually
        kdWarning() << container()->service()->name()
                    << "does not have a desktop entry!" << endl;
        return;
    }
    KPropertiesDialog *d = new KPropertiesDialog(KURL(path), this);
    connect(d, SIGNAL(applied()), container(), SLOT(update()));
    connect(d, SIGNAL(saveAs(const KURL&, KURL&)),
            container(), SLOT(slotDesktopFileChanged(const KURL&, KURL&)));
}

#include "skout_task_grouper.moc"