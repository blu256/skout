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

#ifndef _SKOUT_PAGER_H
#define _SKOUT_PAGER_H

// TQt
#include <tqptrlist.h>

// Skout
#include "skout_applet.h"

class SkoutPanel;
class TQGridLayout;
class KWinModule;

class SkoutPager : public SkoutApplet
{
  TQ_OBJECT

  public:
    SkoutPager(SkoutPanel *panel, TDEConfig *cfg);
    ~SkoutPager();

    bool valid() { return m_valid; }
    TQString lastErrorMessage() { return m_lastError; }

    void reconfigure();
    void relayout();

    bool useViewports() { return m_useViewports; }
    int currentDesktop() { return m_curDesktop; }

    bool hasComposite() { return m_hasComposite(); }

  public slots:
    void update();
    void gotoDesktop(int desktop);

  protected:
    void recreateDesktops();
    void checkComposite();

  private slots:
    void currentDesktopChanged(int desktop);
    void currentDesktopViewportChanged(int desktop, const TQPoint& viewpoint);
    void numberOfDesktopsChanged(int desktops);
    void desktopGeometryChanged(int desktop);
    void desktopNamesChanged();
    void windowChanged(WId window);
    void desktopUpdated(int desktop);
    void updateCurrentDesktop();

  private:
    KWinModule *m_twin;
    TQGridLayout *m_layout;
    TQPtrList<TQWidget> m_desktops;
    bool m_useViewports, m_hasComposite;
    uint m_curDesktop;

    bool m_valid;
    TQString m_lastError;

    bool m_inline, m_miniature;
};

#endif // _SKOUT_PAGER_H

/* kate: replace-tabs true; tab-width 2; */