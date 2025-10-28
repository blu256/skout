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

#ifndef _SKOUT_KICKER_APPLET_H
#define _SKOUT_KICKER_APPLET_H

// TDE
#include <kpanelapplet.h>

// Skout
#include "skout_applet.h"
#include "skout_appletdb.h"

class SkoutPanel;
class KLibrary;

class SkoutKickerAppletInterface : public KPanelApplet
{
  public:
    int heightFor(int width); // HACK
};

class SkoutKickerApplet : public SkoutApplet
{
  TQ_OBJECT

  public:
    SkoutKickerApplet(SkoutPanel *parent, AppletData data);
    virtual ~SkoutKickerApplet();

    virtual bool valid();
    virtual TQString lastErrorMessage();

  protected:
    void about();
    void preferences();
    void resizeEvent(TQResizeEvent *);
    void loadApplet();
    void unloadApplet();

  private slots:
    virtual void reconfigure();
    void slotUpdateGeometry();

  private:
    AppletData m_data;
    KLibrary *m_lib;
    SkoutKickerAppletInterface *m_applet;
    bool m_valid;
};


#endif // _SKOUT_KICKER_APPLET_H

/* kate: replace-tabs true; tab-width 2; */