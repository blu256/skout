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
#include <tqvbox.h>
#include <tqwhatsthis.h>

// TDE
#include <tdeconfig.h>
#include <tdelocale.h>

// Skout
#include "skout_pager_cfg.h"
#include "skout_pager_cfg.moc"

SkoutPagerConfig::SkoutPagerConfig(TQWidget *parent, const TQString& cfg)
: SkoutAppletConfig(parent, cfg)
{
    new TQVBoxLayout(this);
    m_tabs = new TQTabWidget(this);
    layout()->add(m_tabs);

    // Appearance
    TQVBox *appearance = new TQVBox(m_tabs);

    m_inline = new TQCheckBox(i18n("Show desktops in single row"), appearance);
    m_miniature = new TQCheckBox(i18n("Show desktop miniatures"), appearance);

    connect(m_inline, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));
    connect(m_miniature, TQ_SIGNAL(toggled(bool)), TQ_SIGNAL(changed()));

    m_tabs->addTab(appearance, i18n("Appearance"));
}

SkoutPagerConfig::~SkoutPagerConfig()
{
}

void SkoutPagerConfig::reset()
{
    m_inline->setChecked(true);
    m_miniature->setChecked(false);
}

void SkoutPagerConfig::load()
{
    auto cfg = config();
    cfg->setGroup("Pager");

    // Appearance
    m_inline->setChecked(cfg->readBoolEntry("ShowInline", true));
    m_miniature->setChecked(cfg->readBoolEntry("ShowMiniature", true));
}

void SkoutPagerConfig::save()
{
    auto cfg = config();
    cfg->setGroup("Pager");

    // Appearance
    cfg->writeEntry("ShowInline", m_inline->isChecked());
    cfg->writeEntry("ShowMiniature", m_miniature->isChecked());

    cfg->sync();
}

// kate: replace-tabs true; tab-width 4; indent-width 4;