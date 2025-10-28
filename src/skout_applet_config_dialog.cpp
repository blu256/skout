/*******************************************************************************
  Skout - a BeOS-inspired panel for TDE
  Copyright (C) 2024 Mavridis Philippe <mavridisf@gmail.com>

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
#include <tqlabel.h>

// TDE
#include <tdemessagebox.h>
#include <tdeglobal.h>
#include <klibloader.h>
#include <kiconloader.h>
#include <tdeconfig.h>
#include <tdelocale.h>
#include <dcopref.h>
#include <kdebug.h>

// Skout
#include "skout_applet_config.h"
#include "skout_applet_config_dialog.h"
#include "skout_applet_config_dialog.moc"

SkoutAppletConfigDialog::SkoutAppletConfigDialog(TQWidget *parent, AppletData applet)
: KDialogBase(parent, "skout_applet_config", true,
              i18n("Configure applet \"%1\"").arg(applet.name),
              KDialogBase::Default | KDialogBase::Cancel |
              KDialogBase::Apply | KDialogBase::Ok,
              KDialogBase::Ok, true)
{
    m_applet = applet;

    setIcon(TDEGlobal::iconLoader()->loadIcon(m_applet.icon, TDEIcon::Desktop));

    KLibrary *lib = KLibLoader::self()->library(m_applet.libPath());
    if (!lib) {
        TQString error = i18n("<qt><b>Error!</b><br>Unable to load configuration module for the \"%1\" applet!<br>%2</qt>")
                .arg(m_applet.name, KLibLoader::self()->lastErrorMessage());
        TQLabel *errorLabel = new TQLabel(error, this);
        setMainWidget(errorLabel);
        return;
    }

    void *sym = lib->symbol("config");
    SkoutAppletConfig *(*makeConfigWidget)(TQWidget *, const TQString&) = \
        (SkoutAppletConfig *(*)(TQWidget *, const TQString&))sym;

    m_config = makeConfigWidget(this, "skoutappletrc");
    setMainWidget(m_config);
    enableButtonApply(false);
    m_config->load();
    connect(m_config, TQ_SIGNAL(changed()), TQ_SLOT(slotChanged()));
}

SkoutAppletConfigDialog::~SkoutAppletConfigDialog()
{
    KLibLoader::self()->unloadLibrary(m_applet.libPath());
}

bool SkoutAppletConfigDialog::changed()
{
    return m_changed;
}

/// slot
void SkoutAppletConfigDialog::slotOk()
{
    if (changed())
    {
        m_config->save();
        reconfigure();
    }
    m_changed = false;
    KDialogBase::slotOk();
}

/// slot
void SkoutAppletConfigDialog::slotApply()
{
    m_config->save();
    reconfigure();

    m_changed = false;
    enableButtonApply(false);
    KDialogBase::slotApply();
}

/// slot
void SkoutAppletConfigDialog::slotDefault()
{
    m_config->reset();
    KDialogBase::slotDefault();
}

/// slot
void SkoutAppletConfigDialog::slotChanged()
{
    m_changed = true;
    enableButtonApply(true);
}

/// slot
void SkoutAppletConfigDialog::reconfigure()
{
    DCOPRef skout("skout", "SkoutIface");
    DCOPReply reply = skout.call("reloadApplet", (TQString)m_applet.id);
    kdDebug() << "reloading " << m_applet.id << endl;
    if (!reply.isValid())
    {
        KMessageBox::sorry(this,
            i18n("Cannot apply applet settings! Skout is probably not running."),
            i18n("Error contacting Skout")
        );
    }
}

// kate: replace-tabs true; tab-width 4; indent-width 4;