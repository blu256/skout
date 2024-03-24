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
#include <krun.h>
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

#define ERR_CHK_INSTALLATION "%1<br>Please check your installation."

static SkoutPanel *skoutPanel = nullptr;

SkoutPanel::SkoutPanel()
  : TQFrame(0, "Skout", TQt::WStyle_Customize | TQt::WStyle_NoBorder |
                        TQt::WStyle_StaysOnTop | TQt::WDestructiveClose),
    w_menubtn(nullptr),
    m_pos(PanelPosition::Saved),
    m_forcePos(false)
{
    if (skoutPanel) {
        kdWarning() << "Global panel pointer is not null! Overriding" << endl;
        delete skoutPanel;
    }
    skoutPanel = this;

    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed);

    m_appletDB = SkoutAppletDB::instance();
    m_appletExt = SkoutAppletPanelExtension::instance();
    connect(m_appletExt, TQ_SIGNAL(popupRequest(TQString, TQString, TQString)),
                         TQ_SLOT(popup(TQString, TQString, TQString)));
    connect(m_appletExt, TQ_SIGNAL(launchRequest(KService::Ptr, KURL::List)),
                         TQ_SLOT(launch(KService::Ptr, KURL::List)));

    setWindowType();

    new TQVBoxLayout(this);
    layout()->setAutoAdd(false);

    w_menubtn = new SkoutMenuBtn(this);
    layout()->add(w_menubtn);
}

SkoutPanel::~SkoutPanel() {
    ZAP(w_menubtn)
}

SkoutPanel *SkoutPanel::instance() {
    return skoutPanel;
}

void SkoutPanel::showEvent(TQShowEvent *e) {
    reconfigure();
}

void SkoutPanel::setPosition(PanelPosition pos, bool force) {
    m_pos = pos;
    m_forcePos = force;
    if (isShown()) {
        relayout();
    }
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
                       i18n(ERR_CHK_INSTALLATION).arg(error));
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
                       i18n(ERR_CHK_INSTALLATION).arg(error));

        unloadApplet(applet);
        return false;
    }

    connect(applet.ptr, TQ_SIGNAL(updateGeometry()), TQ_SLOT(applySize()));
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
        AppletData &applet = (*m_appletDB)[(*it)];
        layout()->remove(applet.ptr);
        addApplet(applet);
    }
    applySize();
}



void SkoutPanel::popup(TQString icon, TQString caption, TQString message) {
    KPassivePopup::message(caption, message, SmallIcon(icon), this);
}

bool SkoutPanel::launch(KService::Ptr service, KURL::List urls) {
    if (!service->isValid()) return false;

    TQStringList args = KRun::processDesktopExec(*service, urls, false, false);
    TQString app = args[0];
    args.pop_front();

    TQString error;
    if (0 != kapp->tdeinitExec(app, args, &error)) {
        popup("error", i18n("Unable to launch %1!").arg(service->name()),
                       i18n(ERR_CHK_INSTALLATION).arg(error));
        return false;
    }
    return true;
}

void SkoutPanel::launchMenuEditor() {
    launch(KService::serviceByDesktopName("kmenuedit"));
}

void SkoutPanel::configure() {
    launch(KService::serviceByDesktopName("skout_config"));
}

#include "skout_panel.moc"

/* kate: replace-tabs true; tab-width 4; */