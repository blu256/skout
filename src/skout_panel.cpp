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

// TDE
#include <tdeapplication.h>
#include <kpassivepopup.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <tdelocale.h>
#include <twin.h>
#include <kdebug.h>

// Skout
#include "skout_panel.h"
#include "skout_applet.h"
#include "skout_appletdb.h"
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
    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed);

    m_appletdb = SkoutAppletDB::instance();

    setWindowType();

    new TQVBoxLayout(this);
    layout()->setAutoAdd(false);

    w_menubtn = new SkoutMenuBtn(this);
    layout()->add(w_menubtn);

    reconfigure();
    show();
}

SkoutPanel::~SkoutPanel() {
    ZAP(w_menubtn)
}

void SkoutPanel::reconfigure() {
    SkoutSettings::self()->readConfig();
    relayout();
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
    setFixedSize(SkoutSettings::panelWidth(), layout()->minimumSize().height());
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

bool SkoutPanel::loadApplet(AppletData &applet) {
    if (!applet.valid()) {
        popup("error", i18n("Unable to load \"%1\" applet!").arg(applet.name),
                       i18n("This is not a valid applet."));
        return false;
    }

    const char *libPath = applet.libPath();
    KLibrary *lib = KLibLoader::self()->library(libPath);
    if (!lib) {
        TQString error(KLibLoader::self()->lastErrorMessage());
        popup("error", i18n("Unable to load \"%1\" applet!").arg(applet.name),
                        TQString(ERR_CHK_INSTALLATION).arg(error));
        return false;
    }

    if (!lib->hasSymbol("init")) {
        popup("error", i18n("Unable to load \"%1\" applet!").arg(applet.name),
                        i18n("This is not a valid applet."));
        KLibLoader::self()->unloadLibrary(libPath);
        return false;
    }

    void *init = lib->symbol("init");
    SkoutApplet *(*c)(SkoutPanel *) = (SkoutApplet *(*)(SkoutPanel *))init;
    applet.ptr = c(this);
    if (!applet.ptr->valid()) {
        TQString error(applet.ptr->lastErrorMessage());

        popup("error", i18n("Unable to load \"%1\" applet!").arg(applet.name),
              TQString(ERR_CHK_INSTALLATION).arg(error));

        unloadApplet(applet);
        return false;
    }

    connect(applet.ptr, SIGNAL(updateGeometry()), SLOT(applySize()));

    connect(applet.ptr, SIGNAL(showPopup(TQString, TQString, TQString)),
                        SLOT(popup(TQString, TQString, TQString)));

    connect(applet.ptr, SIGNAL(doLaunch(TQString, TQStringList, TQString, bool)),
                        SLOT(launch(TQString, TQStringList, TQString, bool)));

    applet.ptr->show();
    return true;
}

void SkoutPanel::unloadApplet(AppletData &applet) {
    delete applet.ptr; applet.ptr = nullptr;
    KLibLoader::self()->unloadLibrary(applet.libPath());
}

bool SkoutPanel::addApplet(AppletData &applet) {
    if (!applet.ptr) {
        if (!loadApplet(applet) || !applet.ptr) {
            return false;
        }
    }
    layout()->add(applet.ptr);
    m_applets.append(&applet);
    return true;
}

void SkoutPanel::relayout() {
    TQStringList applets = SkoutSettings::applets();

    // Unload unneeded applets
    AppletData *old = m_applets.first();
    for (; old; old = m_applets.next()) {
        if (!applets.contains(old->id)) {
            layout()->remove(old->ptr);
            unloadApplet((*old));
        }
    }

    m_applets.clear();

    // Relayout
    TQStringList::Iterator it;
    for (it = applets.begin(); it != applets.end(); ++it) {
        AppletData &applet = (*m_appletdb)[(*it)];
        layout()->remove(applet.ptr);
        addApplet(applet);
    }
    applySize();
}

bool SkoutPanel::launch(TQString application, TQStringList args,
                        TQString description, bool isService)
{
    TQString error;
    int ret;
    if (isService) {
        ret = kapp->startServiceByDesktopName(application, args, &error);
    }
    else {
        ret = kapp->tdeinitExec(application, args, &error);
    }

    if (0 != ret) {
        popup("error", i18n("Unable to launch %1!").arg(description),
                        TQString(ERR_CHK_INSTALLATION).arg(error));
        return false;
    }
    return true;
}

void SkoutPanel::launchMenuEditor() {
    launch("kmenuedit", 0, "the menu editor");
}

void SkoutPanel::configure() {
    launch("tdecmshell", "skout_config", "the menu editor", false);
}

#include "skout_panel.moc"