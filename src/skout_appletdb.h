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

#ifndef _SKOUT_APPLETDB_H
#define _SKOUT_APPLETDB_H

// TQt
#include <tqfile.h>

// TDE
#include <tqmap.h>

class SkoutApplet;

struct AppletData {
    TQCString id;
    TQString name;
    TQString icon;
    TQString library;
    TQString comment;
    SkoutApplet *ptr = nullptr;

    bool valid() {
        return !id.isNull() && !name.isNull()
            && !library.isNull();
    }

    const char *libPath() {
        return TQFile::encodeName(library);
    }
};

typedef TQMap<TQCString, AppletData> AppletMap;

class SkoutAppletDB : public TQObject {
  public:
    static SkoutAppletDB *instance();

    bool contains(TQString id);

    TQValueList<TQCString> applets();

    AppletData &operator[](TQString id);

  private:
    AppletMap m_applets;
    SkoutAppletDB();
    ~SkoutAppletDB() = default;
};

#endif // _SKOUT_APPLETDB_H

/* kate: replace-tabs true; tab-width 2; */