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

#ifndef _SKOUT_PANEL_H
#define _SKOUT_PANEL_H

// TQt
#include <tqframe.h>

// Skout
// #include "skoutsettings.h"

class KWinModule;

class SkoutMenuBtn;
class SkoutSysTray;
class SkoutTaskMan;

//typedef SkoutSettings::EnumPosition::type PanelPosition;
enum PanelPosition {
  TopLeft, TopRight
};

class SkoutPanel : public TQFrame {
  TQ_OBJECT

  public:
    SkoutPanel(PanelPosition pos);
    ~SkoutPanel();

    bool initialized() { return m_initialized; }

    PanelPosition position() { return m_pos; }
    KWinModule *twin();

  public slots:
    void applyPosition();

  protected:
    void resizeEvent(TQResizeEvent *);

  private:
    SkoutMenuBtn *w_menubtn;
    SkoutSysTray *w_systray;
    SkoutTaskMan *w_taskman;

    int m_width;
    bool m_initialized;

    KWinModule *m_twin;

    PanelPosition m_pos;

    void initWidgets();
    void setWindowType();
    void reserveStrut();
};

typedef TQValueList<WId> WIdList;

#endif // _SKOUT_PANEL_H