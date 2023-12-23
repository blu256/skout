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

// TDE
#include <tdeglobal.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kdebug.h>

// Skout
#include "skout_appletdb.h"

static SkoutAppletDB *appletDB = nullptr;

SkoutAppletDB* SkoutAppletDB::instance() {
    if (!appletDB) appletDB = new SkoutAppletDB();
    return appletDB;
}

SkoutAppletDB::SkoutAppletDB() : TQObject(0, "applet_db") {
    TDEGlobal::dirs()->addResourceType("applets",
        TDEGlobal::dirs()->kde_default("data") + "skout/applets");

    TQStringList applets =
        TDEGlobal::dirs()->findAllResources("applets", "*.desktop", true, true);

    for (TQStringList::ConstIterator it = applets.constBegin();
         it != applets.constEnd(); ++it)
    {
        KDesktopFile df((*it), true);
        if (df.readType() != "SkoutApplet" ||
            df.readEntry("X-TDE-Library").left(12) != "skoutapplet_")
        {
            kdWarning() << "SkoutPanel: " << df.fileName()
                        << " is not a valid applet" << endl;
            continue;
        }

        AppletData data;
        data.name = df.readName();
        data.icon = df.readIcon();
        data.library = df.readEntry("X-TDE-Library");
        data.comment = df.readComment();

        TQCString id = data.library.mid(12).latin1();
        data.id = id;
        if (m_applets.contains(id)) {
            kdWarning() << "SkoutPanel: applet data with id '" << id << "' "
                        << "already exists, skipping." << endl;
            continue;
        }
        m_applets[id] = data;
    }
}

TQValueList<TQCString> SkoutAppletDB::applets() {
    return m_applets.keys();
}

bool SkoutAppletDB::contains(TQString id) {
    return m_applets.contains(id.latin1());
}

AppletData &SkoutAppletDB::operator[](TQString id) {
    return m_applets[id.latin1()];
}

#include "skout_appletdb.moc"