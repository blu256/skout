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

/*** Use this applet as a reference when implementing your own Skout applet ***/

#ifndef _SKOUT_CALENDAR_H
#define _SKOUT_CALENDAR_H

// Skout
#include "skout_applet.h" // here it's local, but it might be a system header

// Class definitions so that we don't have to pollute the header with includes
class SkoutPanel;
class KDateTable;

// Definition of our applet class. It should inherit from SkoutApplet and have
// the below basic structure.
class SkoutCalendar : public SkoutApplet {
  TQ_OBJECT

  public:
    SkoutCalendar(SkoutPanel *panel, TDEConfig *cfg); // constructor
    ~SkoutCalendar(); // destructor

    // This function tells Skout whether the applet has been initialized
    // successfully. If this returns false, Skout will refuse to embed the
    // applet and will immediately unload it with a popup message of error
    // as returned by lastErrorMessage().
    bool valid() { return true; }

    // This function is called to get an error message if initialization of this
    // applet has failed (that is, valid() has returned false).
    TQString lastErrorMessage() { return TQString::null; }

    // This function is called when the configuration options of the applet
    // have changed. Since this applet has none, we will leave this out.
    // Configuration is currently beyond the scope of this example.
    // void reconfigure();

    // This is an implementation detail of the example and is not part of the
    // mandatory basic structure.
  private:
    KDateTable *m_calendar;
};

#endif // _SKOUT_CALENDAR_H