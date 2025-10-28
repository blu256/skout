/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-2025 Mavridis Philippe <mavridisf@gmail.com>

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

// TDE
#include <kservice.h>

// Skout
#include "skout_appletdb.h"
#include "skout_applet_panel_extension.h"

class KWinModule;

class Skout;
class SkoutMenuBtn;

enum PanelPosition {
    Saved = -1,
    TopLeft, TopRight
};

typedef TQPtrList<AppletData> AppletList;

class SkoutPanel : public TQFrame {
  TQ_OBJECT

  public:
    static SkoutPanel *instance();

    PanelPosition position() const { return m_pos; }

    TDEConfig *appletConfig() { return m_appletConfig; }

  public slots:
    void popup(TQString icon, TQString caption, TQString message);
    bool launch(KService::Ptr service, KURL::List urls = KURL::List());

    void launchMenuEditor();
    void configure();

  protected:
    SkoutPanel();
    ~SkoutPanel();

    void setPosition(PanelPosition pos, bool force = false);

    void moveEvent(TQMoveEvent *e);
    void showEvent(TQShowEvent *e);

  protected slots:
    void applyPosition();
    void applySize();

    void reconfigure();
    void reloadApplet(TQString appletId);
    void relayout();

  signals:
    void appletsReconfigure();

  private:
    SkoutMenuBtn *w_menubtn;
    SkoutAppletDB *m_appletDB;
    SkoutAppletPanelExtension *m_appletExt;
    AppletList m_applets;
    TDEConfig *m_appletConfig;

    PanelPosition m_pos;
    bool m_forcePos;

    KWinModule *m_twin;

    bool addApplet(AppletData &applet);
    void removeApplet(AppletData &applet);

    bool loadApplet(AppletData &applet);
    void unloadApplet(AppletData &applet);

    TQPoint originPos() const;

    void setWindowType();
    void reserveStrut();

  friend class Skout;
};

#endif // _SKOUT_PANEL_H

/* kate: replace-tabs true; tab-width 2; */