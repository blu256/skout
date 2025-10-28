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

#ifndef _SKOUT_APPLET_H
#define _SKOUT_APPLET_H

// TQt
#include <tqframe.h>

// TDE
#include <tdeconfig.h>

// Skout
#include "skout_applet_panel_extension.h"

class SkoutPanel;

class SkoutApplet : public TQFrame {
  TQ_OBJECT

  public:
    SkoutApplet(SkoutPanel *parent, TDEConfig *cfg, const char *name = 0);
    virtual ~SkoutApplet();

    virtual bool valid();
    virtual TQString lastErrorMessage();

    TDEConfig *config() { return m_cfg; }

  signals:
    void updateGeometry();

  protected:
    void resizeEvent(TQResizeEvent *);

  private slots:
    virtual void reconfigure();

  private:
    TDEConfig *m_cfg;
};

#endif // _SKOUT_APPLET_H

/* kate: replace-tabs true; tab-width 2; */