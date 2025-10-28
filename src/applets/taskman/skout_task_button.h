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

#ifndef _SKOUT_TASK_BUTTON_H
#define _SKOUT_TASK_BUTTON_H

// TQt
#include <tqbutton.h>

class SkoutTaskContainer;

class SkoutTaskButton : public TQButton
{
  TQ_OBJECT

  public:
    enum ButtonType
    {
      Task = 0,
      Grouper,
      BUTTONTYPE_MAX
    };

    SkoutTaskButton(SkoutTaskContainer *parent, ButtonType type);
    ~SkoutTaskButton();

    ButtonType buttonType() const { return m_buttonType; }

    virtual TQString name();
    virtual TQPixmap icon();

    SkoutTaskContainer *container() const;

    bool showFrame() const;
    bool showIcon() const;
    bool useBigIcon() const;

    static TQPixmap defaultIcon(TQSize size = TQSize());

    static TQSize bigIconSize();
    static TQSize smallIconSize();

    TQSize iconSize() const;
    TQPoint iconOffset() const;

    static TQFont normalFont();
    static TQFont boldFont();

    static TQColor blendColors(const TQColor c1, const TQColor c2);

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
    ButtonType m_buttonType;
};

#endif // _SKOUT_TASK_BUTTON_H

/* kate: replace-tabs true; tab-width 2; */