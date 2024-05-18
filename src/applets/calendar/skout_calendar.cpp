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

/*** Use this applet as a reference when implementing your own Skout applet ***/

// TQt
#include <tqlayout.h> // TQVBoxLayout class
#include <tqwhatsthis.h> // TQWhatsThis::add(...)
#include <tqfontmetrics.h> // TQFontMetrics class

// TDE
#include <tdeglobalsettings.h> // TDEGlobalSettings::generalFont()
#include <kdatetbl.h> // KDateTable class
#include <tdelocale.h> // i18n(...)

// Skout
#include "skout_calendar.h" // SkoutCalendar class
#include "skout_utils.h" // ZAP(...)

extern "C" {
    // Here we export the applet constructor so that Skout can understand it.
    // Notice that the function accepts a SkoutPanel* argument, which it then
    // passes on to the constructor below - this argument is mandatory.
    TDE_EXPORT SkoutApplet *init(SkoutPanel *parent) {
        return new SkoutCalendar(parent);
    }
}

// The applet constructor. You have to inherit from SkoutApplet. Notice the
// SkoutPanel* argument we saw below - here we pass it on to the constructor of
// SkoutApplet, along with an optional (but recommended) const char *name.
// SkoutApplet is defined in the skout_applet.h header, which is by default
// installed in your TDE include directory.
SkoutCalendar::SkoutCalendar(SkoutPanel *parent)
  : SkoutApplet(parent, "SkoutCalendar")
{
    // SkoutApplet is essentially a TQFrame
    setFrameStyle(TQFrame::StyledPanel | TQFrame::Sunken);

    // Purely aesthetic ;-)
    setFixedHeight(TQFontMetrics(TDEGlobalSettings::generalFont()).height() * 7);

    // SkoutApplet has no layout by default, here we create one.
    // We don't need to keep a reference to it manually, as we can refer to it
    // at any time by calling layout().
    new TQVBoxLayout(this);

    // Initialize a KDateTable with us as a parent. Here it will be our only
    // widget.
    m_calendar = new KDateTable(this);

    // Add the widget to our layout, otherwise its geometry would be incorrect.
    // If you have many widgets, you can consider enabling auto add mode, see
    // the TQt documentation on the TQLayout class.
    layout()->add(m_calendar);

    // Optionally add a What's this message. The What's this mode can be invoked
    // from the context menu of the Skout menu button. Similarly, you can add
    // a tooltip by calling TQToolTip::add(this, ...);
    TQWhatsThis::add(this, i18n("This is a calendar applet."));

    // That's all, the applet will get shown automatically when the time is
    // right.
}

// The applet destructor. You can delete unneeded stuff here. You can use the
// ZAP(...) macro to quickly delete the object pointed to by a pointer and set
// its value to nullptr. This macro is part of skout_utils.h, which does NOT get
// installed on your system. If you want to package an applet separate from the
// main source tree, keep that in mind.
SkoutCalendar::~SkoutCalendar() {
    ZAP(m_calendar);
}

// Don't forget to include this or your build will probably fail.
#include "skout_calendar.moc"

/* kate: replace-tabs true; tab-width 4; */