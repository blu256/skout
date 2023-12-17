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
#include <tqmap.h>

class KWinModule;

class SkoutMenuBtn;
class SkoutApplet;

enum PanelPosition {
    TopLeft, TopRight
};

struct AppletData {
    TQString name;
    TQString icon;
    TQString library;
};

typedef TQMap<TQString, AppletData> AppletDatabase;
typedef TQPtrList<SkoutApplet> AppletList;

class SkoutPanel : public TQFrame {
  TQ_OBJECT

  public:
    SkoutPanel();
    SkoutPanel(PanelPosition pos, bool force = false);
    ~SkoutPanel();

    PanelPosition position() { return m_pos; }
    void setPosition(PanelPosition pos);

  public slots:
    void applyPosition();

  private:
    SkoutMenuBtn *w_menubtn;
    AppletDatabase m_appletdb;
    AppletList m_applets;

    PanelPosition m_pos;
    bool m_forcePos;

    KWinModule *m_twin;

    void loadAppletDatabase();
    void initApplets();
    void setWindowType();
    void reserveStrut();
};

#endif // _SKOUT_PANEL_H