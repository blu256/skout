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
#include <tqspinbox.h>
#include <tqgroupbox.h>
#include <tqlabel.h>
#include <tqvbox.h>
#include <tqwhatsthis.h>

// TDE
#include <tdeaboutdata.h>
#include <tdeapplication.h>
#include <kgenericfactory.h>
#include <tdemessagebox.h>
#include <kiconloader.h>
#include <kjanuswidget.h>
#include <tdelocale.h>
#include <dcopref.h>

// Skout
#include "tdecm_skout.h"
#include "skout_appletdb.h"
#include "skout_applet_selector.h"
#include "skoutsettings.h"
#include "version.h"

typedef KGenericFactory<SkoutConfig, TQWidget> SkoutConfigFactory;
K_EXPORT_COMPONENT_FACTORY(kcm_skout, SkoutConfigFactory("kcm_skout"))

static const char description[] = I18N_NOOP("Skout panel configuration module");

extern "C" {
    void start_skout() {
        if (!SkoutConfig::skoutAlive())
            kapp->startServiceByDesktopName("skout");
    }

    void stop_skout() {
        DCOPRef skout("skout", "SkoutIface");
        skout.call("quit()");
    }

    void start_kicker() {
        kapp->tdeinitExec("kicker");
    }

    void stop_kicker() {
        DCOPRef kicker("kicker", "kicker");
        kicker.call("quit()");
    }
}

SkoutConfig::SkoutConfig(TQWidget *parent, const char *name, const TQStringList &)
  : TDECModule(SkoutConfigFactory::instance(), parent, name)
{
    SkoutSettings::instance("skoutrc");

    TDEAboutData *about = new TDEAboutData(I18N_NOOP("kcm_skout"),
                          I18N_NOOP("SkoutConfig"), version, description,
                          TDEAboutData::License_GPL_V3, copyright);
    setAboutData(about);

    TDEGlobal::locale()->insertCatalogue("skout");

    setQuickHelp( i18n("<h1>Skout</h1> This control module can be used to enable and configure"
        " Skout. Skout is a Be-style panel for TDE. If you choose to use Skout, Kicker will be"
        " automatically disabled."));

    new TQVBoxLayout(this);

    m_groupBox = new TQGroupBox(this);
    layout()->setAutoAdd(true);

    new TQVBoxLayout(m_groupBox);
    m_groupBox->layout()->setAutoAdd(true);

#define ICON(x) TDEGlobal::iconLoader()->loadIcon(x, TDEIcon::Panel)
    m_tabWidget = new KJanusWidget(m_groupBox, 0, KJanusWidget::IconList);
    TQVBox *tabGeneral = m_tabWidget->addVBoxPage(i18n("General"),
                                                  "General settings",
                                                  ICON("configure"));

    TQVBox *tabLook = m_tabWidget->addVBoxPage((const TQString)"Appearance",
                                               "Appearance", ICON("icons"));

    TQVBox *tabApplets = m_tabWidget->addVBoxPage(i18n("Applets"),
                                                  "Applets", ICON("kicker"));
#undef ICON

#define VBOX_PAGE_PREPARE(tab, stretch) \
    tab->layout()->setAutoAdd(false); \
    if (stretch) ((TQVBoxLayout *)tab->layout())->addStretch();

    // General tab
    m_autostart = new TQCheckBox("Start Skout automatically on startup", tabGeneral);
    TQWhatsThis::add(m_autostart, "Start the Skout panel automatically when the user session starts.");

    m_replaceKicker = new TQCheckBox("Replace Kicker panel", tabGeneral);
    m_replaceKicker->setEnabled(m_autostart->isOn());
    TQWhatsThis::add(m_replaceKicker, "Prevent the Kicker panel from starting when Skout is selected to start together with the user session.");

    tabGeneral->layout()->add(m_autostart);
    tabGeneral->layout()->add(m_replaceKicker);

    connect(m_autostart, TQ_SIGNAL(toggled(bool)), m_replaceKicker, TQ_SLOT(setEnabled(bool)));
    connect(m_autostart, TQ_SIGNAL(toggled(bool)), TQ_SLOT(changed()));
    connect(m_replaceKicker, TQ_SIGNAL(toggled(bool)), TQ_SLOT(changed()));

    VBOX_PAGE_PREPARE(tabGeneral, true)

    // Appearance tab
    TQHBox *posBox = new TQHBox(tabLook);
    TQLabel *posLabel = new TQLabel(i18n("Panel position: "), posBox);

    m_grpPos = new TQButtonGroup(posBox);
    m_grpPos->setSizePolicy(TQSizePolicy::Maximum, TQSizePolicy::Fixed);

    new TQHBoxLayout(m_grpPos);
    m_grpPos->layout()->setAutoAdd(true);

    TQRadioButton *posTopLeft  = new TQRadioButton(i18n("Top left"), m_grpPos);
    TQRadioButton *posTopRight = new TQRadioButton(i18n("Top right"), m_grpPos);

    TQHBox *widthBox = new TQHBox(tabLook);
    TQLabel *widthLabel = new TQLabel(i18n("Panel width: "), widthBox);
    m_width = new TQSpinBox(100, 500, 1, widthBox);

    tabLook->layout()->add(posBox);
    tabLook->layout()->add(widthBox);

    connect(m_grpPos,    TQ_SIGNAL(clicked(int)),      TQ_SLOT(changed()));
    connect(m_width,     TQ_SIGNAL(valueChanged(int)), TQ_SLOT(changed()));

    VBOX_PAGE_PREPARE(tabLook, true)

    // Applets tab
    m_appletSelector = new SkoutAppletSelector(tabApplets);
    tabApplets->layout()->add(m_appletSelector);

    connect(m_appletSelector, TQ_SIGNAL(changed()), TQ_SLOT(changed()));

    VBOX_PAGE_PREPARE(tabApplets, false)

#undef VBOX_PAGE_PREPARE

    // Final touches
    setButtons(Help|Apply);
    load();
    show();
}

void SkoutConfig::changed() {
    TDECModule::changed();
}

void SkoutConfig::load() {
    SkoutSettings::self()->readConfig();

    // General tab
    m_autostart->setChecked(SkoutSettings::autostart());
    m_replaceKicker->setChecked(SkoutSettings::replaceKicker());

    // Appearance tab
    m_grpPos->setButton(SkoutSettings::position());
    m_width->setValue(SkoutSettings::panelWidth());

    // Applets tab
    loadApplets();
}

void SkoutConfig::save() {
    // General tab
    SkoutSettings::setAutostart(m_autostart->isChecked());
    SkoutSettings::setReplaceKicker(m_replaceKicker->isChecked());

    // General tab
    SkoutSettings::setPosition(m_grpPos->selectedId());
    SkoutSettings::setPanelWidth(m_width->value());

    // Applets tab
    TQStringList applets;
    TQListBoxItem *item = m_appletSelector->selectedListBox()->firstItem();

    // Prompt if the user wants to start/kill Skout right away
    if (m_autostart->isChecked() && !skoutAlive()) {
        int result = KMessageBox::questionYesNo(this,
            i18n("<qt>You have choosed to enable the Skout panel. Would you like to start it immediately?"
                 "<br><br>Note: even if you select 'No', Skout will be normally started the next time you log in.</qt>"));

        if (result == KMessageBox::Yes) {
            startStopSkout(true);
        }
    }
    else if (!m_autostart->isChecked() && skoutAlive()) {
        int result = KMessageBox::questionYesNo(this,
            i18n("<qt>You have choosed to disable the Skout panel. Would you like to stop it immediately?</qt>"));

        if (result == KMessageBox::Yes) {
            startStopSkout(false);
        }
    }

    // Update applets
    while (item) {
        applets << static_cast<SkoutAppletItem *>(item)->data().id;
        item = item->next();
    }
    SkoutSettings::setApplets(applets);

    // Save and ask Skout to re-read configuration
    SkoutSettings::self()->writeConfig();

    DCOPRef skout("skout", "SkoutIface");
    skout.call("reconfigure()");
}

void SkoutConfig::loadApplets() {
    TQStringList active = SkoutSettings::applets();
    SkoutAppletDB *appletdb = SkoutAppletDB::instance();
    TQValueList<TQCString> applets = appletdb->applets();
    TQValueList<TQCString>::iterator it;
    for (it = applets.begin(); it != applets.end(); ++it) {
        TQCString id((*it));
        AppletData &applet = (*appletdb)[id];
        int index;
        if ((index = active.findIndex(id)) != -1) {
            m_appletSelector->insertActiveApplet(applet, index);
        }
        else {
            m_appletSelector->insertApplet(applet);
        }
    }
}

void SkoutConfig::startStopSkout(bool enable) {
    if (enable) {
        if (SkoutSettings::replaceKicker()) {
            stop_kicker();
        }
        start_skout();
    }
    else {
        stop_skout();
        if (SkoutSettings::replaceKicker()) {
            start_kicker();
        }
    }
}

bool SkoutConfig::skoutAlive()
{
    DCOPRef skout("skout", "SkoutIface");
    DCOPReply reply = skout.call("ping()");
    return reply.isValid() && (bool)reply == true;
}

extern "C" {
    TDE_EXPORT void init_skout() {
        SkoutSettings::instance("skoutrc");

        if (SkoutSettings::autostart()) {
            if (SkoutSettings::replaceKicker()) {
                stop_kicker();
            }
            start_skout();
        }
    };
}

#include "tdecm_skout.moc"

/* kate: replace-tabs true; tab-width 4; */