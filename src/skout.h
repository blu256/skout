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

#ifndef _SKOUT_H
#define _SKOUT_H

// TDE
#include <tdeuniqueapplication.h>

// Skout
#include "skoutiface.h"
#include "skout_panel.h"

class Skout : public TDEUniqueApplication, SkoutIface {
  TQ_OBJECT

  public:
    Skout(PanelPosition pos);
    ~Skout();

    bool ping();
    void reconfigure();
    void reloadApplet(TQString applet);
    void quit();

  private:
    SkoutPanel *m_panel;
};

#endif // _SKOUT_H

/* kate: replace-tabs true; tab-width 2; */
