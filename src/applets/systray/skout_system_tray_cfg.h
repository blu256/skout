/*******************************************************************************
  Skout - a BeOS-inspired panel for TDE
  Copyright (C) 2024 Mavridis Philippe <mavridisf@gmail.com>

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

#ifndef _SKOUT_SYSTEM_TRAY_CFG_H
#define _SKOUT_SYSTEM_TRAY_CFG_H

// Skout
#include "skout_applet_config.h"

class TQCheckBox;
class TDEConfig;

class SkoutSysTrayConfig : public SkoutAppletConfig
{
    TQ_OBJECT

    public:
        SkoutSysTrayConfig(TQWidget *parent, const TQString& cfg);
        ~SkoutSysTrayConfig();

    public slots:
        void reset();
        void load();
        void save();

    private:
        TQCheckBox *m_enableStatus;
};

#endif // _SKOUT_SYSTEM_TRAY_CFG_H