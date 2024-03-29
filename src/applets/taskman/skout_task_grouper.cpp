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
#include <tdeglobal.h>
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
    updateStaticPixmaps();

    auto config = TDEGlobal::config();
    config->setGroup("GroupersState");
    setExpanded(config->readBoolEntry(container()->applicationClass()));

    connect(this, TQ_SIGNAL(toggled(bool)), TQ_SLOT(setExpanded(bool)));
}

SkoutTaskGrouper::~SkoutTaskGrouper() {
}

#define ICON(iconVar, icon, group, size) \
    if (!iconVar || iconVar->isNull()) iconVar = new TQPixmap( \
        kapp->iconLoader()->loadIcon( \
            icon, group, size.height(), TDEIcon::DefaultState, nullptr, true \
        ) \
    );
#define ICON_SMALL(iconVar, icon) \
    ICON(iconVar, icon, TDEIcon::Small, smallIconSize())
#define ICON_BIG(iconVar, icon) \
    ICON(iconVar, icon, TDEIcon::Panel, bigIconSize())

void SkoutTaskGrouper::updateStaticPixmaps() {
    ICON_SMALL(pxHide, "1uparrow")
    ICON_SMALL(pxExpand, "1downarrow")
    ICON_BIG(pxLink, kapp->iconLoader()->theme()->linkOverlay())
}
#undef ICON
#undef ICON_SMALL
#undef ICON_BIG

TQString SkoutTaskGrouper::name() {
    return container()->application();
}

TQPixmap SkoutTaskGrouper::icon() {
    TQPixmap groupIcon = container()->groupIcon();
    return groupIcon.isNull() ? defaultIcon() : groupIcon;
}

void SkoutTaskGrouper::setExpanded(bool expanded) {
    m_expanded = expanded;

    // persist grouper state
    TQString aclass = container()->applicationClass();
    TDEConfig *config = TDEGlobal::config();
    config->setGroup("GroupersState");
    config->writeEntry(aclass, expanded);
    config->sync();

    // hide/show tasks
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
                              container(), TQ_SLOT(toggleIconifiedAll()));
        ctx.setItemChecked(item, container()->allIconified());
    }

    if (pinnable()) {
      if (pinned()) {
          ctx.insertItem(TQPixmap(locate("data", "skout/pics/pindown.png")),
                        i18n("Unpin application"), this, TQ_SLOT(unpin()));
      }
      else {
          ctx.insertItem(TQPixmap(locate("data", "skout/pics/pinup.png")),
                        i18n("Pin application"), this, TQ_SLOT(pin()));
      }
    }

    KService::Ptr s = container()->service();
    if (s && s->desktopEntryPath() != TQString::null) {
        ctx.insertSeparator();
        ctx.insertItem(SmallIcon("document-properties"), i18n("Properties"),
                       this, TQ_SLOT(showPropertiesDialog()));
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
        panelExt->launch(container()->service());

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
    auto c = container();
    connect(d, TQ_SIGNAL(applied()),
            c, TQ_SLOT(update()));
    connect(d, TQ_SIGNAL(saveAs(const KURL&, KURL&)),
            c, TQ_SLOT(slotDesktopFileChanged(const KURL&, KURL&)));
}

#include "skout_task_grouper.moc"

/* kate: replace-tabs true; tab-width 4; */