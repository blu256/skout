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

#ifndef _SKOUT_APPLET_SELECTOR_H
#define _SKOUT_APPLET_SELECTOR_H

// TQt
#include <tqlistbox.h>
#include <tqpainter.h>

// TDE
#include <tdeactionselector.h>

// Skout
#include "skout_appletdb.h"

/* --- SkoutAppletItem class ------------------------------------------------ */
class SkoutAppletItem : public TQListBoxPixmap {
  public:
    SkoutAppletItem(AppletData &appletData);
    AppletData& data() { return m_data; }
    int height(const TQListBox *) const;
    int height(const TQFontMetrics fm) const;

  protected:
    void paint(TQPainter *);

  private:
    AppletData& m_data;
};

/* --- SkoutAppletSelector class -------------------------------------------- */
class SkoutAppletSelector : public TDEActionSelector {
  TQ_OBJECT

  public:
    SkoutAppletSelector(TQWidget *parent, const char *name = 0);

    void insertApplet(AppletData &applet);
    void insertActiveApplet(AppletData &applet, int index = -1);

  signals:
    void changed();
};

#endif // _SKOUT_APPLET_SELECTOR_H