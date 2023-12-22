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

// Skout
#include "skout_appletdb.h"

class KWinModule;

class SkoutMenuBtn;

enum PanelPosition {
    TopLeft, TopRight
};

typedef TQPtrList<AppletData> AppletList;

class SkoutPanel : public TQFrame {
  TQ_OBJECT

  public:
    SkoutPanel();
    SkoutPanel(PanelPosition pos, bool force = false);
    ~SkoutPanel();

    PanelPosition position() const { return m_pos; }
    void setPosition(PanelPosition pos);

    TQPoint originPos() const;

  public slots:
    void applyPosition();
    void applySize();

    void popup(TQString icon, TQString caption, TQString message);
    bool launch(TQString application, TQStringList args,
                TQString description, bool isService = true);

    void launchMenuEditor();
    void configure();

    void reconfigure();
    void relayout();

  protected:
    void moveEvent(TQMoveEvent *e);

  private:
    SkoutMenuBtn *w_menubtn;
    SkoutAppletDB *m_appletdb;
    AppletList m_applets;

    PanelPosition m_pos;
    bool m_forcePos;

    KWinModule *m_twin;

    bool addApplet(AppletData &applet);
    bool loadApplet(AppletData &applet);
    void unloadApplet(AppletData &applet);

    void setWindowType();
    void reserveStrut();
};

#endif // _SKOUT_PANEL_H