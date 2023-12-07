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

// TQt
#include <tqapplication.h>
#include <tqlayout.h>
#include <tqfile.h>

// TDE
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kpassivepopup.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <tdelocale.h>
#include <twin.h>
#include <kdebug.h>

// Skout
#include "skout_panel.h"
#include "skout_applet.h"
#include "skout_menu_button.h"
#include "skout_utils.h"
#include "skoutsettings.h"

// NetWM
#include <netwm_def.h>

SkoutPanel::SkoutPanel(PanelPosition pos)
  : TQFrame(0, "Skout", TQt::WStyle_Customize | TQt::WStyle_NoBorder |
                        TQt::WStyle_StaysOnTop | TQt::WDestructiveClose),
    m_pos(pos),
    m_width(200),
    w_menubtn(nullptr),
    m_initialized(false)
{
    new TQVBoxLayout(this);
    layout()->setAutoAdd(true);
    layout()->setResizeMode(TQLayout::Fixed);
    layout()->setMargin(0);
    layout()->setSpacing(0);

    w_menubtn = new SkoutMenuBtn(this);


    TDEGlobal::dirs()->addResourceType("applets",
        TDEGlobal::dirs()->kde_default("data") + "skout/applets");
    loadAppletDatabase();
    initApplets();

    setSizePolicy(TQSizePolicy::Ignored, TQSizePolicy::MinimumExpanding);
    setFrameStyle(TQFrame::Panel | TQFrame::Raised);

    setWindowType();
    show();

    reserveStrut();
    applyPosition();
    m_initialized = true;
}

SkoutPanel::~SkoutPanel() {
    ZAP(w_menubtn)
}

void SkoutPanel::applyPosition() {
    TQRect desktop = TQApplication::desktop()->geometry();

    TQPoint origin;
    switch (position()) {
        case PanelPosition::TopLeft:
            origin = desktop.topLeft();
            break;

        case PanelPosition::TopRight:
        default:
            origin = desktop.topRight() - TQPoint(width(), 0);
            break;
    }

    move(origin);
}

void SkoutPanel::setWindowType() {
    KWin::setType(winId(), NET::Dock);
    KWin::setState(winId(), NET::Sticky);
    KWin::setOnAllDesktops(winId(), true);
}

void SkoutPanel::reserveStrut() {
    NETExtendedStrut strut;

    switch (position()) {
        case PanelPosition::TopLeft:
            strut.left_start = y();
            strut.left_end = y() + height();
            strut.left_width = width();
            break;

        case PanelPosition::TopRight:
        default:
            strut.right_start = y();
            strut.right_end = y() + height();
            strut.right_width = width();
            break;
    }

    KWin::setExtendedStrut(winId(),
      strut.left_width,   strut.left_start,   strut.left_end,
      strut.right_width,  strut.right_start,  strut.right_end,
      strut.top_width,    strut.top_start,    strut.top_end,
      strut.bottom_width, strut.bottom_start, strut.bottom_end);
}

void SkoutPanel::loadAppletDatabase() {
    TQStringList applets = TDEGlobal::dirs()->findAllResources("applets",
                                                               "*.desktop",
                                                               true, true);
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

        TQString id = data.library.mid(12);
        if (m_appletdb.contains(id)) {
            kdWarning() << "SkoutPanel: applet data with id '" << id << "' "
                        << "already exists, skipping." << endl;
            continue;
        }
        m_appletdb[id] = data;
    }
}

void SkoutPanel::initApplets() {
    TQStringList applets = SkoutSettings::applets();
    TQStringList::ConstIterator it;
    for (it = applets.constBegin(); it != applets.constEnd(); ++it) {
        TQString id((*it));
        if (!m_appletdb.contains(id)) {
            KPassivePopup::message(
                i18n("Unable to load \"%1\" applet!").arg(id),
                i18n("Applet not found. Please check if the applet is properly "
                     "installed."),
                SmallIcon("error"),
                this);
            continue;
        }

        AppletData data = m_appletdb[id];
        const char *libPath = TQFile::encodeName(data.library);
        KLibrary *lib = KLibLoader::self()->library(libPath);
        if (!lib) {
            KPassivePopup::message(
                i18n("Unable to load \"%1\" applet!").arg(id),
                KLibLoader::self()->lastErrorMessage(),
                SmallIcon("error"),
                this);
            continue;
        }

        if (!lib->hasSymbol("init")) {
            KPassivePopup::message(
                i18n("Unable to load \"%1\" applet!").arg(id),
                i18n("This is not a valid applet."),
                SmallIcon("error"),
                this);
            KLibLoader::self()->unloadLibrary(libPath);
            continue;
        }

        void *init = lib->symbol("init");
        SkoutApplet *(*c)(SkoutPanel *) = (SkoutApplet *(*)(SkoutPanel *))init;
        SkoutApplet *applet = c(this);
        if (!applet->valid()) {
            KPassivePopup::message(
                i18n("Unable to load \"%1\" applet!").arg(id),
                applet->lastErrorMessage(),
                SmallIcon("error"),
                this);
            delete applet;
            KLibLoader::self()->unloadLibrary(libPath);
            continue;
        }
        m_applets.append(applet);
    }
}

void SkoutPanel::resizeEvent(TQResizeEvent *) {
    setFixedWidth(200);
    if (!m_initialized) return;
    reserveStrut();
    applyPosition();
}

void SkoutPanel::moveEvent(TQMoveEvent *) {
    applyPosition();
}

#include "skout_panel.moc"