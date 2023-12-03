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

#ifndef _SKOUT_TASK_GROUPER_H
#define _SKOUT_TASK_GROUPER_H

// Skout
#include "skout_task_button.h"

class SkoutTaskContainer;

class SkoutTaskGrouper : public SkoutTaskButton {
  TQ_OBJECT

  public:
    SkoutTaskGrouper(SkoutTaskContainer *parent, TQString name);
    ~SkoutTaskGrouper();

    TQString name();
    TQPixmap icon();

    void setIcon(TQPixmap icon);

    bool expanded() { return m_expanded; }
    void setExpanded(bool expanded) { m_expanded = expanded; repaint(); }

    static void updateStaticPixmaps();

  public slots:
    void toggle(bool show);

  protected:
    TQFont font();
    TQColorGroup colors();
    void drawButton(TQPainter *p);

  private:
    bool m_expanded;
    TQString m_name;
    TQPixmap m_icon;
};

#endif // _SKOUT_TASK_GROUPER_H