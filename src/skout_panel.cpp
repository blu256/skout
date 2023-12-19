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
#include <tqlayout.h>
#include <tqfile.h>

// TDE
#include <tdeapplication.h>
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

#define ERR_CHK_INSTALLATION I18N_NOOP("%1<br>Please check your installation.")

SkoutPanel::SkoutPanel() : SkoutPanel((PanelPosition)SkoutSettings::position())
{
}

SkoutPanel::SkoutPanel(PanelPosition pos, bool force)
  : TQFrame(0, "Skout", TQt::WStyle_Customize | TQt::WStyle_NoBorder |
                        TQt::WStyle_StaysOnTop | TQt::WDestructiveClose),
    m_pos(pos),
    m_forcePos(force),
    w_menubtn(nullptr)
{
    new TQVBoxLayout(this);
    layout()->setMargin(0);
    layout()->setSpacing(0);

    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Maximum);
    applySize();

    w_menubtn = new SkoutMenuBtn(this);
    layout()->add(w_menubtn);

    TDEGlobal::dirs()->addResourceType("applets",
        TDEGlobal::dirs()->kde_default("data") + "skout/applets");
    loadAppletDatabase();
    initApplets();

    setWindowType();
    show();

    applyPosition();
}

SkoutPanel::~SkoutPanel() {
    ZAP(w_menubtn)
}

TQPoint SkoutPanel::originPos() const {
    TQRect desktop = TQApplication::desktop()->geometry();
    switch (position()) {
        case PanelPosition::TopLeft:
            return desktop.topLeft();
            break;

        case PanelPosition::TopRight:
        default:
            return desktop.topRight() - TQPoint(SkoutSettings::panelWidth(), 0);
            break;
    }
}

void SkoutPanel::applyPosition() {
    if (!m_forcePos) {
        m_pos = (PanelPosition)SkoutSettings::position();
    }
    move(originPos());
    reserveStrut();
}

void SkoutPanel::applySize() {
    setFixedSize(SkoutSettings::panelWidth(), sizeHint().height());
    applyPosition();
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
            strut.left_width = SkoutSettings::panelWidth();
            break;

        case PanelPosition::TopRight:
        default:
            strut.right_start = y();
            strut.right_end = y() + height();
            strut.right_width = SkoutSettings::panelWidth();
            break;
    }

    KWin::setExtendedStrut(winId(),
      strut.left_width,   strut.left_start,   strut.left_end,
      strut.right_width,  strut.right_start,  strut.right_end,
      strut.top_width,    strut.top_start,    strut.top_end,
      strut.bottom_width, strut.bottom_start, strut.bottom_end);
}

void SkoutPanel::moveEvent(TQMoveEvent *me) {
  if (me->pos() != originPos()) {
      move(originPos());
  }
}

void SkoutPanel::popup(TQString icon, TQString caption, TQString message) {
    KPassivePopup::message(caption, message, SmallIcon(icon), this);
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
            popup("error", i18n("Unable to load \"%1\" applet!").arg(id),
                           i18n("Applet not found. Please ensure that it has "
                                "been properly installed."));
            continue;
        }

        AppletData data = m_appletdb[id];
        const char *libPath = TQFile::encodeName(data.library);
        KLibrary *lib = KLibLoader::self()->library(libPath);
        if (!lib) {
            TQString error(KLibLoader::self()->lastErrorMessage());
            popup("error", i18n("Unable to load \"%1\" applet!").arg(id),
                           TQString(ERR_CHK_INSTALLATION).arg(error));
            continue;
        }

        if (!lib->hasSymbol("init")) {
            popup("error", i18n("Unable to load \"%1\" applet!").arg(id),
                           i18n("This is not a valid applet."));
            KLibLoader::self()->unloadLibrary(libPath);
            continue;
        }

        void *init = lib->symbol("init");
        SkoutApplet *(*c)(SkoutPanel *) = (SkoutApplet *(*)(SkoutPanel *))init;
        SkoutApplet *applet = c(this);
        if (!applet->valid()) {
            TQString error(applet->lastErrorMessage());
            popup("error", i18n("Unable to load \"%1\" applet!").arg(id),
                  TQString(ERR_CHK_INSTALLATION).arg(error));
            delete applet;
            KLibLoader::self()->unloadLibrary(libPath);
            continue;
        }

        connect(applet, SIGNAL(showPopup(TQString, TQString, TQString)),
                        SLOT(popup(TQString, TQString, TQString)));

        connect(applet, SIGNAL(doLaunch(TQString, TQStringList, TQString)),
                        SLOT(launch(TQString, TQStringList, TQString)));

        m_applets.append(applet);
        layout()->add(applet);
    }
}

bool SkoutPanel::launch(TQString service, TQStringList args, TQString what) {
    TQString error;
    if (0 != kapp->startServiceByDesktopName(service, args, &error)) {
        popup("error", i18n("Unable to launch %1!").arg(what),
                       TQString(ERR_CHK_INSTALLATION).arg(error));
        return false;
    }
    return true;
}

void SkoutPanel::launchMenuEditor() {
    launch("kmenuedit", 0, "the menu editor");
}

void SkoutPanel::configure() {
    launch("tdecmshell", "skout_config", "the menu editor");
}

#include "skout_panel.moc"