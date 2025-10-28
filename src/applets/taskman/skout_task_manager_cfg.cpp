/*******************************************************************************
  Skout - a DeskBar-inspired panel for TDE
  Copyright (C) 2025 Mavridis Philippe <mavridisf@gmail.com>

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
#include <tqcheckbox.h>
#include <tqtabwidget.h>
#include <tqgroupbox.h>
#include <tqvbox.h>
#include <tqwhatsthis.h>

// TDE
#include <tdeconfig.h>
#include <tdelocale.h>
#include <kdialogbase.h>

// Skout
#include "skout_task_manager_cfg.h"
#include "skout_task_manager_cfg.moc"

SkoutTaskManConfig::SkoutTaskManConfig(TQWidget *parent, const TQString& cfg)
: SkoutAppletConfig(parent, cfg)
{
    new TQVBoxLayout(this);
    m_tabs = new TQTabWidget(this);
    layout()->add(m_tabs);

    // Behaviour
    TQVBox *behaviour = new TQVBox(m_tabs);
    behaviour->setMargin(KDialogBase::marginHint());

    m_defaultExpandGroupers = new TQCheckBox(i18n("Expand groupers by default"), behaviour);
    TQWhatsThis::add(m_defaultExpandGroupers, i18n("If this is checked, groupers will be shown in their expanded "
                                                   "state by default."));


    m_autoSaveGroupers = new TQCheckBox(i18n("Automatically save grouper states"), behaviour);
    TQWhatsThis::add(m_autoSaveGroupers, i18n("If this is checked, Skout will automatically store groupers states "
                                              "for all applications. Otherwise you can still save the grouper "
                                              "states manually only for the applications that you want to do so "
                                              "from the context menu of its grouper"));

    connect(m_defaultExpandGroupers, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));
    connect(m_autoSaveGroupers, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));

    // Appearance
    TQVBox *appearance = new TQVBox(m_tabs);
    appearance->setMargin(KDialogBase::marginHint());

    TQGroupBox *appearanceGroupers = new TQGroupBox(1, TQt::Vertical, i18n("Groupers"), appearance);

    m_bigGrouperIcons = new TQCheckBox(i18n("Use big icons in groupers"), appearanceGroupers);

    TQGroupBox *appearanceTasks = new TQGroupBox(3, TQt::Vertical, i18n("Tasks"), appearance);

    m_showAllDesktops = new TQCheckBox(i18n("Show tasks from all desktops"), appearanceTasks);
    m_showDesktopNumber = new TQCheckBox(i18n("Show desktop numbers on tasks"), appearanceTasks);
    m_showTaskIcons = new TQCheckBox(i18n("Show task icons"), appearanceTasks);

    connect(m_bigGrouperIcons, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));
    connect(m_showAllDesktops, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));
    connect(m_showDesktopNumber, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));
    connect(m_showTaskIcons, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));

    m_tabs->addTab(behaviour, i18n("Behaviour"));
    m_tabs->addTab(appearance, i18n("Appearance"));
}

SkoutTaskManConfig::~SkoutTaskManConfig()
{
}

void SkoutTaskManConfig::reset()
{
    // Behaviour
    m_defaultExpandGroupers->setChecked(true);
    m_autoSaveGroupers->setChecked(false);

    // Appearance
    m_bigGrouperIcons->setChecked(true);
    m_showTaskIcons->setChecked(true);

    // Desktops
    m_showAllDesktops->setChecked(true);
    m_showDesktopNumber->setChecked(false);
}

void SkoutTaskManConfig::load()
{
    auto cfg = config();
    cfg->setGroup("TaskMan");

    // Behaviour
    m_defaultExpandGroupers->setChecked(cfg->readBoolEntry("DefaultExpandGroupers", true));
    m_autoSaveGroupers->setChecked(cfg->readBoolEntry("AutoSaveGroupers", false));

    // Appearance
    m_bigGrouperIcons->setChecked(cfg->readBoolEntry("BigGrouperIcons", true));
    m_showAllDesktops->setChecked(cfg->readBoolEntry("ShowTasksFromAllDesktops", true));
    m_showDesktopNumber->setChecked(cfg->readBoolEntry("ShowDesktopNumber", false));
    m_showTaskIcons->setChecked(cfg->readBoolEntry("ShowTaskIcons", true));
}

void SkoutTaskManConfig::save()
{
    auto cfg = config();
    cfg->setGroup("TaskMan");

    // Behaviour
    cfg->writeEntry("DefaultExpandGroupers", m_defaultExpandGroupers->isChecked());
    cfg->writeEntry("AutoSaveGroupers", m_autoSaveGroupers->isChecked());

    // Appearance
    cfg->writeEntry("BigGrouperIcons", m_bigGrouperIcons->isChecked());
    cfg->writeEntry("ShowTasksFromAllDesktops", m_showAllDesktops->isChecked());
    cfg->writeEntry("ShowDesktopNumber", m_showDesktopNumber->isChecked());
    cfg->writeEntry("ShowTaskIcons", m_showTaskIcons->isChecked());

    cfg->sync();
}

// kate: replace-tabs true; tab-width 4; indent-width 4;