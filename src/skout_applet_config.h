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

#ifndef _SKOUT_APPLET_CONFIG_H
#define _SKOUT_APPLET_CONFIG_H

// TQt
#include <tqframe.h>

// TDE
#include <tdeconfig.h>

class SkoutAppletConfig : public TQFrame
{
    TQ_OBJECT

    public:
        SkoutAppletConfig(TQWidget *parent, const TQString &cfg);
        virtual ~SkoutAppletConfig();

        TDEConfig *config() { return m_cfg; }

    public slots:
        virtual void reset() = 0;
        virtual void load() = 0;
        virtual void save() = 0;

    signals:
        void changed();

    private:
        TDEConfig *m_cfg;
};

#endif // _SKOUT_APPLET_CONFIG_H