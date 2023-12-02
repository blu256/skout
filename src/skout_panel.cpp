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
#include <tqapplication.h>
#include <tqlayout.h>

// TDE
#include <twin.h>
#include <twinmodule.h>
#include <kdebug.h>
#include <netwm_def.h>
#include <kspy.h>

// Skout
#include "skout_panel.h"
#include "skout_menu_button.h"
#include "skout_system_tray.h"
#include "skout_task_manager.h"
#include "skout_utils.h"

SkoutPanel::SkoutPanel(PanelPosition pos)
  : TQFrame(0, "Skout", TQt::WStyle_Customize | TQt::WStyle_NoBorder |
                        TQt::WStyle_StaysOnTop | TQt::WDestructiveClose),
    m_pos(pos),
    m_width(200),
    m_twin(nullptr),
    w_menubtn(nullptr),
    w_systray(nullptr),
    w_taskman(nullptr),
    m_initialized(false)
{
    m_twin = new KWinModule(this);

    new TQVBoxLayout(this);
    layout()->setAutoAdd(true);
    layout()->setResizeMode(TQLayout::Fixed);
    initWidgets();

    setSizePolicy(TQSizePolicy::Ignored, TQSizePolicy::MinimumExpanding);
    setFrameStyle(TQFrame::Panel | TQFrame::Raised);

    setWindowType();
    show();

    reserveStrut();
    applyPosition();
    m_initialized = true;
}

SkoutPanel::~SkoutPanel() {
    ZAP(w_menubtn)
    ZAP(w_systray)
    ZAP(w_taskman)
    ZAP(m_twin);
}

KWinModule *SkoutPanel::twin() {
    Q_ASSERT(m_twin);
    return m_twin;
}

void SkoutPanel::applyPosition() {
    TQRect desktop = TQApplication::desktop()->geometry();
    TQSize panelSize(sizeHint());

    TQPoint origin;
    switch (position()) {
        case PanelPosition::TopLeft:
            origin = desktop.topLeft();
            break;

        case PanelPosition::TopRight:
        default:
            origin = desktop.topRight() - TQPoint(panelSize.width(), 0);
            break;
    }

    move(origin);
}

void SkoutPanel::setWindowType() {
    KWin::setType(winId(), NET::Dock);
    KWin::setState(winId(), NET::Sticky);
    KWin::setOnAllDesktops(winId(), true);
}

void SkoutPanel::reserveStrut() {
    NETExtendedStrut strut;

    switch (position()) {
        case PanelPosition::TopLeft:
            strut.left_start = y();
            strut.left_end = y() + height();
            strut.left_width = width();
            break;

        case PanelPosition::TopRight:
        default:
            strut.right_start = y();
            strut.right_end = y() + height();
            strut.right_width = width();
            break;
    }

    KWin::setExtendedStrut(winId(),
      strut.left_width,   strut.left_start,   strut.left_end,
      strut.right_width,  strut.right_start,  strut.right_end,
      strut.top_width,    strut.top_start,    strut.top_end,
      strut.bottom_width, strut.bottom_start, strut.bottom_end);
}

void SkoutPanel::initWidgets() {
    w_menubtn = new SkoutMenuBtn(this);
    w_systray = new SkoutSysTray(this);
    w_taskman = new SkoutTaskMan(this);
}

void SkoutPanel::resizeEvent(TQResizeEvent *) {
    setFixedWidth(200);
    if (!m_initialized) return;
    reserveStrut();
    applyPosition();
}

#include "skout_panel.moc"