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

// TQt
#include <tqtooltip.h>
#include <tqtimer.h>
#include <tqimage.h>
#include <tqpainter.h>
#include <tqpalette.h>
#include <tqstyle.h>

// TDE
#include <tdeapplication.h>
#include <tdeglobalsettings.h>
#include <kiconloader.h>
#include <kdebug.h>

// Skout
#include "skout_task_button.h"
#include "skout_task_container.h"

SkoutTaskButton::SkoutTaskButton(SkoutTaskContainer *parent, ButtonType type)
  : TQButton(parent),
    m_buttonType(type)
{
    setSizePolicy(TQSizePolicy::Ignored, TQSizePolicy::Fixed);
    setToggleButton(true);
    show();
    TQTimer::singleShot(0, this, TQ_SLOT(update()));
}

SkoutTaskButton::~SkoutTaskButton()
{}

void SkoutTaskButton::update()
{
    TQToolTip::add(this, name());
    TQTimer::singleShot(0, this, TQ_SLOT(repaint()));
}

TQString SkoutTaskButton::name()
{
    return TQString::null;
}

TQPixmap SkoutTaskButton::icon()
{
    return defaultIcon();
}

bool SkoutTaskButton::showFrame() const
{
    return buttonType() == Grouper;
}

bool SkoutTaskButton::showIcon() const
{
    return buttonType() != Task || container()->manager()->showTaskIcons();
}

bool SkoutTaskButton::useBigIcon() const
{
    return buttonType() == Grouper && container()->manager()->bigGrouperIcons();
}

TQColorGroup SkoutTaskButton::colors()
{
    return container()->palette().active();
}

TQColor SkoutTaskButton::blendColors(const TQColor c1, const TQColor c2)
{
    TQRgb rgb1 = c1.rgb();
    TQRgb rgb2 = c2.rgb();
    return TQColor((tqRed(rgb1)   + tqRed(rgb2))   / 2,
                   (tqGreen(rgb1) + tqGreen(rgb2)) / 2,
                   (tqBlue(rgb1)  + tqBlue(rgb2))  / 2);
}

SkoutTaskContainer *SkoutTaskButton::container() const
{
    return static_cast<SkoutTaskContainer *>(parent());
}

TQPixmap SkoutTaskButton::defaultIcon(TQSize size)
{
    if (size.isNull()) size = bigIconSize();
    return tdeApp->iconLoader()->loadIcon("xapp", TDEIcon::Panel, size.height());
}

TQSize SkoutTaskButton::bigIconSize()
{
    int size = tdeApp->iconLoader()->currentSize(TDEIcon::Panel);
    return TQSize(size, size);
}

TQSize SkoutTaskButton::smallIconSize()
{
    int size = tdeApp->iconLoader()->currentSize(TDEIcon::Small);
    return TQSize(size, size);
}

TQPoint SkoutTaskButton::smallIconOffset()
{
    TQRect big(TQPoint(0, 0), bigIconSize());
    TQRect small(TQPoint(0, 0), smallIconSize());
    small.moveCenter(big.center());
    return small.topLeft();
}

TQSize SkoutTaskButton::iconSize() const
{
    return (useBigIcon() ? bigIconSize() : smallIconSize());
}

TQPoint SkoutTaskButton::iconOffset() const
{
    TQPoint margin(2, 2);

    if (!useBigIcon())
    {
        margin += TQPoint(0, smallIconOffset().y() / 2);
    }

    return margin;
}

TQFont SkoutTaskButton::font()
{
    return normalFont();
}

TQFont SkoutTaskButton::normalFont()
{
    return TDEGlobalSettings::taskbarFont();
}

TQFont SkoutTaskButton::boldFont()
{
    TQFont f = normalFont();
    f.setBold(true);
    return f;
}

TQSize SkoutTaskButton::sizeHint() const
{
    int height = iconSize().height() + (iconOffset().y() * 2);
    return TQSize(container()->width(), height);
}

void SkoutTaskButton::drawButtonLabel(TQPainter *p)
{
    TQPoint smallOffset = smallIconOffset();
    TQPoint origin = iconOffset();
    int offset = 0;

    // Render icon
    if (showIcon())
    {
        TQImage img = icon().convertToImage();
        TQPixmap pix = img.scale(iconSize());

        if (isOn())
        {
            origin += TQPoint(1, 1);
        }

        if (!useBigIcon())
        {
            origin += TQPoint(smallOffset.x(), 0);
        }

        p->drawPixmap(origin, pix);
        offset = pix.width() + iconOffset().x();
    }

    // Render label
    p->setFont(font());

    TQFontMetrics fm(p->fontMetrics());
    TQPoint textOffset(origin);
    textOffset += TQPoint(offset, fm.ascent());

    if (useBigIcon())
    {
        textOffset += TQPoint(0, smallOffset.y());
    }
    else
    {
        textOffset += TQPoint(smallOffset.x(), 0);
    }

    TQString label(name());
    int spaceForLabel = width() - textOffset.x();
    TQString ellipsis = TQString::fromUtf8("…");
    int ellipsisWidth = fm.width(ellipsis);
    int charLimit = 0;
    for (int ci = 0; ci < label.length(); ++ci)
    {
        if (fm.width(label, ci) > spaceForLabel - ellipsisWidth - iconOffset().x())
        {
            charLimit = ci - 1;
            break;
        }
    }
    if (charLimit != 0)
    {
        label.truncate(charLimit);
        label.append(ellipsis);
    }

    p->setPen(colors().buttonText());
    p->drawText(textOffset, label);
}

void SkoutTaskButton::drawButton(TQPainter *p)
{
    TQColor bg = colors().button();
    p->setBrush(bg);
    p->setPen(bg);
    p->drawRect(rect());

    if (showFrame())
    {
      style().drawControl(TQStyle::CE_PushButton, p, this, rect(), colors());
    }

    drawButtonLabel(p);
}

#include "skout_task_button.moc"

/* kate: replace-tabs true; tab-width 4; */
