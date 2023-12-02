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

#ifndef _MAIN_H
#define _MAIN_H

// TDE
#include <tdecmodule.h>

class SkoutConfigWidget;
class SkoutSettings;

class SkoutConfig : public TDECModule {
  TQ_OBJECT

  public:
    SkoutConfig(TQWidget *parent, const char *name, const TQStringList &);

    void load();
    void save();

  protected slots:
    void changed();

  private:
    SkoutConfigWidget *m_widget;
    SkoutSettings *m_settings;
};

#endif // _MAIN_H