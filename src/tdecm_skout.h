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

#ifndef _TDECM_SKOUT_H
#define _TDECM_SKOUT_H

// TDE
#include <tdecmodule.h>

class TQGroupBox;
class TQPushButton;
class TQButtonGroup;
class TQSpinBox;
class KJanusWidget;

class SkoutSettings;
class SkoutAppletSelector;

class SkoutConfig : public TDECModule {
  TQ_OBJECT

  public:
    SkoutConfig(TQWidget *parent, const char *name, const TQStringList &);

    void load();
    void save();

    static bool kickerAlive();
    static bool skoutAlive();

  protected slots:
    void changed();
    void appletSelected();
    void appletAbout();
    void appletConfig();

  protected:
    void loadApplets();

    const KURL autostartPath();
    bool autostartInstalled();
    bool installAutostart();
    bool uninstallAutostart();

    const KURL panelAutostartPath();
    bool panelAutostartInstalled();
    bool installPanelAutostart();
    bool uninstallPanelAutostart();

  private:
    SkoutSettings *m_settings;

    TQGroupBox *m_groupBox;
    KJanusWidget *m_tabWidget;
    TQCheckBox *m_autostart;
    TQCheckBox *m_replaceKicker;
    TQButtonGroup *m_grpPos;
    TQSpinBox *m_width;

    // Applets tab
    SkoutAppletSelector *m_appletSelector;
    TQPushButton *m_appletAbout, *m_appletConfig;
};

#endif // _TDECM_SKOUT_H

/* kate: replace-tabs true; tab-width 2; */