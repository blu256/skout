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

#ifndef _SKOUT_TASK_H
#define _SKOUT_TASK_H

// TDE
#include <twin.h>

// Skout
#include "skout_task_button.h"

class SkoutTaskContainer;

class SkoutTask : public SkoutTaskButton {
  TQ_OBJECT

  public:
    SkoutTask(SkoutTaskContainer *parent, WId w);
    ~SkoutTask();

    TQString name();
    TQPixmap icon();
    TQPixmap icon(TQSize size);
    TQString className();
    TQString classClass();

    WId windowID() { return m_window_id; }
    KWin::WindowInfo info();

    bool active();

  public slots:
    void setIconified(bool iconified);
    void setMaximized(bool maximized);
    //void setFullscreen(bool fullscreen);
    void close();
    void activate();

  protected:
    void mousePressEvent(TQMouseEvent *);
    TQFont font();
    TQColorGroup colors();

  private:
    WId m_window_id;
};

typedef TQPtrList<SkoutTask> TaskList;

#endif // _SKOUT_TASK_H