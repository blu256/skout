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

#ifndef _SKOUT_TASK_BUTTON_H
#define _SKOUT_TASK_BUTTON_H

// TQt
#include <tqtoolbutton.h>

class SkoutTaskContainer;

class SkoutTaskButton : public TQToolButton {
  TQ_OBJECT

  public:
    SkoutTaskButton(SkoutTaskContainer *parent, bool bigIcon = false, bool frame = false);
    ~SkoutTaskButton();

    virtual TQString name();
    virtual TQPixmap icon();

    bool useBigIcon() const { return m_bigIcon; }
    bool drawFrame() const { return m_drawFrame; }

    SkoutTaskContainer *container() const;

    static TQPixmap defaultIcon(TQSize size = TQSize());
    static TQSize bigIconSize();
    static TQSize smallIconSize();
    TQSize iconSize() const;

    static TQFont normalFont();
    static TQFont boldFont();

  public slots:
    void update();

  protected:
    TQSize sizeHint() const;

    virtual TQFont font();
    virtual TQColorGroup colors();
    static TQPoint smallIconOffset();

    void drawButtonLabel(TQPainter *p);
    void drawButton(TQPainter *p);

  private:
    bool m_bigIcon;
    bool m_drawFrame;
    TQPoint m_margin;
};

#endif // _SKOUT_TASK_BUTTON_H