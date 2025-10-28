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

// TQt
#include <tqpainter.h>
#include <tqdrawutil.h>
#include <tqtooltip.h>

// TDE
#include <tdeapplication.h>
#include <tdelocale.h>
#include <kpixmap.h>
#include <kdebug.h>

// Skout
#include "skout_pager_desktop.h"
#include "skout_pager.h"
#include "skout_utils.h"

SkoutPagerDesktop::SkoutPagerDesktop(SkoutPager *pager, int desktop,
                                     const TQPoint &viewport)
: TQFrame(pager),
  m_desktopNo(desktop),
  m_viewport(viewport),
  m_pager(pager),
  m_miniature()
{
    TQToolTip::add(this, i18n("Desktop %1").arg(m_desktopNo));
}

SkoutPagerDesktop::~SkoutPagerDesktop()
{
    delete m_miniature;
    m_miniature = nullptr;
}

bool SkoutPagerDesktop::isCurrent()
{
    return m_pager->currentDesktop() == m_desktopNo;
}

void SkoutPagerDesktop::paintEvent(TQPaintEvent *pe)
{
    TQPainter p(this);
    TQRect r(rect());

    if (m_pager->hasComposite() || false)
    {

    }
    else
    {
        delete m_miniature;
        m_miniature = new KRootPixmap(this);
    }

    drawContents(&p);
    qDrawShadePanel(&p, r.x(), r.y(), r.width(), r.height(),
                    palette().active(), isCurrent(), 1, nullptr);
    p.end();
}

void SkoutPagerDesktop::mouseReleaseEvent(TQMouseEvent *me)
{
    switch (me->button())
    {
        case LeftButton:
            m_pager->gotoDesktop(m_desktopNo);
            break;
    }
}

void SkoutPagerDesktop::drawContents(TQPainter *p)
{
    p->drawText(contentsRect(), TQt::WordBreak | TQt::AlignCenter,
                TQString::number(m_desktopNo));
}

/* kate: replace-tabs true; tab-width 4; */