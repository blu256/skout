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
#include <tqgroupbox.h>
#include <tqbuttongroup.h>
#include <tqcheckbox.h>
#include <tqradiobutton.h>
#include <tqlabel.h>
#include <tqhbox.h>

// TDE
#include <dcopref.h>
#include <tdeaboutdata.h>
#include <tdeapplication.h>
#include <kgenericfactory.h>
#include <kdialog.h>
#include <tdelocale.h>

// Skout
#include "tdecm_skout.h"
#include "skout_config.h"
#include "skoutsettings.h"
#include "version.h"

typedef KGenericFactory<SkoutConfig, TQWidget> SkoutConfigFactory;
K_EXPORT_COMPONENT_FACTORY(kcm_skout, SkoutConfigFactory("kcm_skout"))

static const char description[] = I18N_NOOP("Skout panel configuration module");

SkoutConfig::SkoutConfig(TQWidget *parent, const char *name, const TQStringList &)
  : TDECModule(SkoutConfigFactory::instance(), parent, name)
{
    SkoutSettings::instance("skoutrc");

    TDEAboutData *about = new TDEAboutData(I18N_NOOP("kcm_skout"),
                          I18N_NOOP("SkoutConfig"), version, description,
                          TDEAboutData::License_GPL_V3, copyright);
    setAboutData(about);

    setQuickHelp( i18n("<h1>Skout</h1> This control module can be used to enable and configure"
        " Skout. Skout is a Be-style panel for TDE. If you choose to use Skout, Kicker will be"
        " automatically disabled."));

    new TQVBoxLayout(this);

    m_widget = new SkoutConfigWidget(this);
    layout()->add(m_widget);

    connect(m_widget->grp,         SIGNAL(toggled(bool)), SLOT(changed()));
    connect(m_widget->grpPosition, SIGNAL(clicked(int)),  SLOT(changed()));

    setButtons(Help|Apply);
    load();
}

void SkoutConfig::changed() {
    TDECModule::changed();
}

void SkoutConfig::load() {
    SkoutSettings::self()->readConfig();
    m_widget->grp->setChecked(SkoutSettings::enableSkout());
    m_widget->grpPosition->setButton(SkoutSettings::position());
}

void SkoutConfig::save() {
    SkoutSettings::setEnableSkout(m_widget->grp->isChecked());
    SkoutSettings::setPosition(m_widget->grpPosition->selectedId());
    SkoutSettings::self()->writeConfig();
}

extern "C"
{
  KDE_EXPORT void init_skout() {
    SkoutSettings::instance("skoutrc");
    if (SkoutSettings::enableSkout()) {
        // HACK-ish but should do for now
        DCOPRef kicker("kicker", "kicker");
        kicker.call("quit()");
        kapp->startServiceByDesktopName("skout");
    }
  };
}

#include "tdecm_skout.moc"