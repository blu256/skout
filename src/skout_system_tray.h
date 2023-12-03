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

#ifndef _SKOUT_SYSTRAY_H
#define _SKOUT_SYSTRAY_H

// TQt
#include <tqframe.h>
#include <qxembed.h>
#include <tqvaluevector.h>
#include <tqlayout.h>

// Skout
#include "skout_widget.h"

// X11
#include <X11/Xlib.h>

#define SYSTRAY_REQUEST_DOCK 0

class TrayEmbed : public QXEmbed {
  TQ_OBJECT

  public:
    TrayEmbed(bool _tde_tray, TQWidget *parent = nullptr);
    ~TrayEmbed();

    bool tdeTray() const { return tde_tray; }

  private:
    bool tde_tray;
};

typedef TQValueVector<TrayEmbed*> TrayEmbedList;

class SkoutPanel;

class SkoutSysTray : public SkoutWidget {
  TQ_OBJECT

  public:
    SkoutSysTray(SkoutPanel *panel);
    ~SkoutSysTray();

    TQSize iconSize() const { return TQSize(m_icon_size, m_icon_size); }
    int iconPadding() const { return m_icon_padding; }
    int margin() const { return m_margin; }

    void setIconSize(int px) { m_icon_size = px; }
    void setIconPadding(int px) { m_icon_padding = px; }
    void setMargin(int px) { m_margin = px; }

  public slots:
    void acquireSystemTray();
    void relayout(bool force = false);

  protected:
    bool x11Event(XEvent *xe);
    void resizeEvent(TQResizeEvent *);

  private slots:
    void trayWindowAdded(WId w);
    void paletteChanged();
    void updateTrayWindows();

  private:
    TrayEmbedList m_tray;
    Atom net_system_tray_selection;
    Atom net_system_tray_opcode;
    TQGridLayout *m_layout;
    bool m_doingRelayout;
    int m_cols;
    int m_icon_size, m_icon_padding, m_margin;

    void embedWindow(WId w, bool tde_tray);
    bool isWinManaged(WId w);
};

#endif // _SKOUT_SYSTRAY_H