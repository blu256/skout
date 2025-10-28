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

#ifndef _SKOUT_TASK_H
#define _SKOUT_TASK_H

// TDE
#include <kservice.h>
#include <twin.h>

// Skout
#include "skout_task_button.h"

class SkoutTaskContainer;

class SkoutTask : public SkoutTaskButton
{
  TQ_OBJECT

  public:
    SkoutTask(SkoutTaskContainer *parent, WId w);
    ~SkoutTask();

    TQString name();
    TQPixmap icon();
    TQPixmap icon(TQSize size);
    TQString applicationName();
    TQString className();
    TQString classClass();

    SkoutTaskContainer *container();
    KWinModule *twin();

    //KService::Ptr service() { return m_service; }
    WId windowID() { return m_window_id; }
    KWin::WindowInfo info();
    pid_t pid();
    TQString executablePath();
    TQString executable();
    TQString cmdline();

    bool isActive();
    bool staysAbove();
    bool staysBelow();
    bool isIconified();
    bool isMaximized();
    bool isFullScreen();
    bool isShaded();
    int desktop();
    bool isOnCurrentDesktop();

  public slots:
    void sendToTray();
    void setIconified(bool iconified);
    void setMaximized(bool maximized);
    void setFullScreen(bool fullscreen);
    void setStayAbove(bool stay);
    void setStayBelow(bool stay);
    void setShaded(bool shaded);
    void setDesktop(int desktop);

    void toggleStayAbove();
    void toggleStayBelow();
    void toggleIconified();
    void toggleMaximized();
    void toggleFullScreen();
    void toggleShaded();
    void close();
    void activate();

    void updateVisibility();

  protected:
    void mousePressEvent(TQMouseEvent *);
    void contextMenuEvent(TQContextMenuEvent *);
    TQFont font();
    TQColorGroup colors();

  private:
    SkoutTaskContainer *m_container;
    WId m_window_id;

    bool checkWindowState(unsigned long state);
    void addWindowState(unsigned long state);
    void removeWindowState(unsigned long state);
    void setWindowState(unsigned long state, bool set);

  friend class SkoutTaskContainer;
};

typedef TQPtrList<SkoutTask> TaskList;

#endif // _SKOUT_TASK_H

/* kate: replace-tabs true; tab-width 2; */