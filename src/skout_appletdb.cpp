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

// TDE
#include <tdeglobal.h>
#include <tdeaboutdata.h>
#include <tdestandarddirs.h>
#include <tdedesktopfile.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_appletdb.h"
#include "version.h"
#include <config.h>

static SkoutAppletDB *appletDB = nullptr;

SkoutAppletDB* SkoutAppletDB::instance() {
    if (!appletDB) appletDB = new SkoutAppletDB();
    return appletDB;
}

SkoutAppletDB::SkoutAppletDB() : TQObject(0, "applet_db")
{
    TDEGlobal::locale()->insertCatalogue("skout-applets");

    TQString dataPath = TDEGlobal::dirs()->kde_default("data") + "skout/";
    TDEGlobal::dirs()->addResourceType("applets", dataPath + "applets");

    TQStringList applets =
        TDEGlobal::dirs()->findAllResources("applets", "*.desktop", true, true);

    TQStringList::ConstIterator it;
    for (it = applets.constBegin(); it != applets.constEnd(); ++it)
    {
        AppletData data;
        data.desktopFile = (*it);
        loadApplet(data);
    }

#ifdef WITH_KICKER_APPLETS
    dataPath = TDEGlobal::dirs()->kde_default("data") + "kicker/";
    TDEGlobal::dirs()->addResourceType("kicker_applets", dataPath + "applets");

    applets = TDEGlobal::dirs()->findAllResources("kicker_applets", "*.desktop", true, true);

    for (it = applets.constBegin(); it != applets.constEnd(); ++it)
    {
        AppletData data;
        data.desktopFile = (*it);
        data.isKickerApplet = true;
        loadApplet(data);
    }
#endif

}

bool SkoutAppletDB::loadApplet(AppletData &data)
{
    if (data.desktopFile.isEmpty()) return false;

    TDEDesktopFile df(data.desktopFile, true);
#ifdef WITH_KICKER_APPLETS
    if (data.isKickerApplet)
    {
        if (df.readBoolEntry("Hidden", false))
        {
            return true; // hidden applet, ignore
        }
    }
    else
#endif
    if (df.readType() != "SkoutApplet" ||
        df.readEntry("X-TDE-Library").left(12) != "skoutapplet_")
    {
        kdWarning() << "SkoutPanel: " << df.fileName()
                    << " is not a valid applet" << endl;
        return false;
    }

    data.name = df.readName();
    data.icon = df.readIcon();
    data.library = df.readEntry("X-TDE-Library");
    data.comment = df.readComment();
    data.unique = df.readBoolEntry("X-TDE-UniqueApplet", false); // TODO

    TQCString id;
#ifdef WITH_KICKER_APPLETS
    if (data.isKickerApplet)
    {
        id = "kicker_" + data.library.local8Bit();
        data.name.append(TQString(" (%1)").arg(i18n("Kicker")));
    }
    else
#endif
        id = data.library.mid(12).latin1();

    data.id = id;

    if (m_applets.contains(id))
    {
        kdWarning() << "SkoutPanel: applet data with id '" << id << "' "
                    << "already exists, skipping." << endl;
        return false;
    }

    const char *libPath = data.libPath();
    KLibrary *lib = KLibLoader::self()->library(libPath);
    if (!lib) {
        TQString error(KLibLoader::self()->lastErrorMessage());
        kdWarning() << "Unable to load applet " << data.name << ":"
                    << error << endl;
        return false;
    }

#ifdef WITH_KICKER_APPLETS
    if (!data.isKickerApplet)
#endif
    {
        if (lib->hasSymbol("about"))
        {
            void *aboutSym = lib->symbol("about");
            TDEAboutData *(*about)(void) = (TDEAboutData *(*)(void))aboutSym;
            data.about = about();
        }
        else
        {
            data.about = new TDEAboutData(
                data.id, data.name.utf8(),
                df.readEntry("Version", skout::version).utf8(),
                data.comment.utf8(),
                licenseFromString(df.readEntry("License")),
                df.readEntry("Copyright").utf8(),
                nullptr,
                df.readEntry("Homepage").utf8(),
                df.readEntry("BugReports").utf8()
            );

            TQPixmap icon = TDEGlobal::iconLoader()->loadIcon(data.icon, TDEIcon::Desktop);
            data.about->setProgramLogo(icon.convertToImage());
        }
        data.hasConfig = lib->hasSymbol("config");
    }

    KLibLoader::self()->unloadLibrary(libPath);
    data.loaded = true;
    m_applets[id] = data;
    return true;
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

int SkoutAppletDB::licenseFromString(TQString str)
{
    if (str.isNull()) return TDEAboutData::License_Unknown;

    TQString s(str.lower());

    if (s == "gpl" || s == "gplv2")
    {
        return TDEAboutData::License_GPL_V2;
    }

    else if (s == "gplv3")
    {
        return TDEAboutData::License_GPL_V3;
    }

    else if (s == "lgpl" || s == "lgplv2")
    {
        return TDEAboutData::License_LGPL_V2;
    }

    else if (s == "bsd")
    {
        return TDEAboutData::License_BSD;
    }

    else if (s == "artistic")
    {
        return TDEAboutData::License_Artistic;
    }

    else if (s == "mit")
    {
        return TDEAboutData::License_MIT;
    }

    return TDEAboutData::License_Unknown;
}

#include "skout_appletdb.moc"

/* kate: replace-tabs true; tab-width 4; */