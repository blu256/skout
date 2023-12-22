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

// TDE
#include <tdeaboutdata.h>
#include <tdeapplication.h>
#include <kgenericfactory.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kjanuswidget.h>
#include <tdeactionselector.h>
#include <tdelocale.h>
#include <dcopref.h>

// Skout
#include "tdecm_skout.h"
#include "skout_appletdb.h"
#include "skoutsettings.h"
#include "version.h"

typedef KGenericFactory<SkoutConfig, TQWidget> SkoutConfigFactory;
K_EXPORT_COMPONENT_FACTORY(kcm_skout, SkoutConfigFactory("kcm_skout"))

static const char description[] = I18N_NOOP("Skout panel configuration module");

extern "C" {
    void start_skout() {
        DCOPRef kicker("kicker", "kicker");
        kicker.call("quit()");
        kapp->startServiceByDesktopName("skout");
    }

    void stop_skout() {
        DCOPRef skout("skout", "SkoutIface");
        skout.call("quit()");
        kapp->tdeinitExec("kicker");
    }
}

class SkoutAppletItem : public TQListBoxPixmap {
    public:
      SkoutAppletItem(AppletData applet, TQCString id)
        : TQListBoxPixmap(SmallIcon(applet.icon), applet.name),
          m_id(id) {}

      TQCString id() { return m_id; }

    private:
        TQCString m_id;
};

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


    m_groupBox = new TQGroupBox(this);
    m_groupBox->setCheckable(true);
    m_groupBox->setTitle(i18n("Enable Skout"));
    m_groupBox->setFlat(false);
    layout()->setAutoAdd(true);

    new TQVBoxLayout(m_groupBox);
    m_groupBox->layout()->setAutoAdd(true);

#define ICON(x) TDEGlobal::iconLoader()->loadIcon(x, TDEIcon::Panel)
    m_tabWidget = new KJanusWidget(m_groupBox, 0, KJanusWidget::IconList);
    TQVBox *tabGeneral = m_tabWidget->addVBoxPage((const TQString)"General",
                                                  "General settings",
                                                  ICON("configure"));

    /*
    TQVBox *tabLook = m_tabWidget->addVBoxPage((const TQString)"Appearance",
                                               "Appearance", ICON("icons"));
    */

    TQVBox *tabApplets = m_tabWidget->addVBoxPage((const TQString)"Applets",
                                                  "Applets", ICON("kicker"));
#undef ICON

#define VBOX_PAGE_PREPARE(tab, stretch) \
    tab->layout()->setAutoAdd(false); \
    if (stretch) ((TQVBoxLayout *)tab->layout())->addStretch();

    // General tab
    TQHBox *posBox = new TQHBox(tabGeneral);
    TQLabel *posLabel = new TQLabel(i18n("Panel position: "), posBox);

    m_grpPos = new TQButtonGroup(posBox);
    m_grpPos->setSizePolicy(TQSizePolicy::Maximum, TQSizePolicy::Fixed);

    new TQHBoxLayout(m_grpPos);
    m_grpPos->layout()->setAutoAdd(true);

    TQRadioButton *posTopLeft  = new TQRadioButton(i18n("Top left"), m_grpPos);
    TQRadioButton *posTopRight = new TQRadioButton(i18n("Top right"), m_grpPos);

    TQHBox *widthBox = new TQHBox(tabGeneral);
    TQLabel *widthLabel = new TQLabel(i18n("Panel width: "), widthBox);
    m_width = new TQSpinBox(100, 500, 1, widthBox);

    tabGeneral->layout()->add(posBox);
    tabGeneral->layout()->add(widthBox);

    connect(m_groupBox, SIGNAL(toggled(bool)),     SLOT(changed()));
    connect(m_grpPos,   SIGNAL(clicked(int)),      SLOT(changed()));
    connect(m_width,    SIGNAL(valueChanged(int)), SLOT(changed()));

    VBOX_PAGE_PREPARE(tabGeneral, true)

    // Applets tab
    m_appletSelector = new TDEActionSelector(tabApplets);
    tabApplets->layout()->add(m_appletSelector);

    connect(m_appletSelector, SIGNAL(added(TQListBoxItem *)), SLOT(changed()));
    connect(m_appletSelector, SIGNAL(removed(TQListBoxItem *)), SLOT(changed()));
    connect(m_appletSelector, SIGNAL(movedUp(TQListBoxItem *)), SLOT(changed()));
    connect(m_appletSelector, SIGNAL(movedDown(TQListBoxItem *)), SLOT(changed()));

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
    m_groupBox->setChecked(SkoutSettings::enableSkout());
    m_grpPos->setButton(SkoutSettings::position());
    m_width->setValue(SkoutSettings::panelWidth());

    // Applets tab
    loadApplets();
}

void SkoutConfig::save() {
    bool enable = m_groupBox->isChecked();
    if (SkoutSettings::enableSkout() != enable) {
        startStopSkout(enable);
    }
    SkoutSettings::setEnableSkout(enable);

    // General tab
    SkoutSettings::setPosition(m_grpPos->selectedId());
    SkoutSettings::setPanelWidth(m_width->value());

    // Applets tab
    TQStringList applets;
    TQListBoxItem *item =
        m_appletSelector->selectedListBox()->firstItem();

    while (item) {
        applets << static_cast<SkoutAppletItem *>(item)->id();
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
        AppletData applet = (*appletdb)[id];
        TQListBox *listBox;
        if (active.contains(id)) {
            listBox = m_appletSelector->selectedListBox();
        }
        else {
            listBox = m_appletSelector->availableListBox();
        }
        listBox->insertItem(new SkoutAppletItem(applet, id));
    }
}

void SkoutConfig::startStopSkout(bool enable) {
    if (enable) {
        start_skout();
    }
    else {
        stop_skout();
    }
}

extern "C" {
    KDE_EXPORT void init_skout() {
        SkoutSettings::instance("skoutrc");

        // HACK-ish but should do for now
        if (SkoutSettings::enableSkout()) {
            start_skout();
        }
    };
}

#include "tdecm_skout.moc"