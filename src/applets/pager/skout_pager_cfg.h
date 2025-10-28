/*******************************************************************************
  Skout - a BeOS-inspired panel for TDE
  Copyright (C) 2025 Mavridis Philippe <mavridisf@gmail.com>

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

class SkoutPagerConfig : public SkoutAppletConfig
{
    TQ_OBJECT

    public:
        SkoutPagerConfig(TQWidget *parent, const TQString& cfg);
        ~SkoutPagerConfig();

    public slots:
        void reset();
        void load();
        void save();

    private:
        TQTabWidget *m_tabs;
        TQCheckBox *m_inline, *m_miniature;
};

#endif // _SKOUT_TASK_MANAGER_CFG_H