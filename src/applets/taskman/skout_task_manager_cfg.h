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

#ifndef _SKOUT_TASK_MANAGER_CFG_H
#define _SKOUT_TASK_MANAGER_CFG_H

// Skout
#include "skout_applet_config.h"

class TQTabWidget;
class TQCheckBox;
class TDEConfig;

class SkoutTaskManConfig : public SkoutAppletConfig
{
    TQ_OBJECT

    public:
        SkoutTaskManConfig(TQWidget *parent, const TQString& cfg);
        ~SkoutTaskManConfig();

    public slots:
        void reset();
        void load();
        void save();

    private:
        TQTabWidget *m_tabs;
        TQCheckBox *m_autoSaveGroupers, *m_defaultExpandGroupers,
                   *m_showTaskIcons, *m_bigGrouperIcons,
                   *m_showAllDesktops, *m_showDesktopNumber;
};

#endif // _SKOUT_TASK_MANAGER_CFG_H