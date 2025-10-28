/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2023-2025 Philippe Mavridis <philippe.mavridis@yandex.com>

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
#include <klibloader.h>

// Skout
#include "skout_panel.h"
#include "skoutsettings.h"
#include "skout_kicker_applet.h"

// HACK
int SkoutKickerAppletInterface::heightFor(int width)
{
    kdDebug() <<"hforw" << endl;
    setPosition(KPanelApplet::pTop);
    orientationChange(TQt::Horizontal);
    int h = heightForWidth(width);
    kdDebug() << "w = " << width << ", h = " << h << endl;
    return h;
}

SkoutKickerApplet::SkoutKickerApplet(SkoutPanel *parent, AppletData data)
  : SkoutApplet(parent, nullptr, data.id),
    m_data(data),
    m_lib(nullptr),
    m_applet(nullptr),
    m_valid(false)
{
    setFrameStyle(TQFrame::WinPanel | TQFrame::Sunken);
    setLineWidth(2);

    new TQHBoxLayout(this);
    loadApplet();
    if (!m_applet)
    {
        return;
    }

    setSizePolicy(TQSizePolicy::Fixed, TQSizePolicy::Fixed);
    slotUpdateGeometry();

    m_valid = true;
    m_applet->show();

    SkoutAppletPanelExtension::instance();
    connect(parent, TQ_SIGNAL(appletsReconfigure()), TQ_SLOT(reconfigure()));
}

SkoutKickerApplet::~SkoutKickerApplet()
{
    unloadApplet();
}

bool SkoutKickerApplet::valid()
{
    return m_valid;
}

TQString SkoutKickerApplet::lastErrorMessage()
{
    return TQString::null;
}

void SkoutKickerApplet::resizeEvent(TQResizeEvent *e)
{
    slotUpdateGeometry();
}

void SkoutKickerApplet::slotUpdateGeometry()
{
    TQSize newSize(
        SkoutSettings::panelWidth(),
        m_applet->heightFor(SkoutSettings::panelWidth())
    );
    kdDebug() << m_data.name << " updateGeometry: " << size() << " => " << newSize << endl;
    setFixedSize(newSize);
    emit updateGeometry();
}

void SkoutKickerApplet::loadApplet()
{
    m_lib = KLibLoader::self()->library(m_data.libPath());
    void *init = m_lib->symbol("init");
    SkoutKickerAppletInterface *(*c)(TQWidget *, const TQString&, KPanelApplet::Type) =
        (SkoutKickerAppletInterface *(*)(TQWidget *, const TQString&, KPanelApplet::Type))init;

    m_applet = c(this, "skout_" + m_data.id + "_rc", KPanelApplet::Stretch);
    connect(m_applet, TQ_SIGNAL(updateLayout()), TQ_SLOT(slotUpdateGeometry()));

    layout()->add(m_applet);
}

void SkoutKickerApplet::unloadApplet()
{
    delete m_applet;
    m_applet = nullptr;

    KLibLoader::self()->unloadLibrary(m_data.libPath());
}

/// slot
void SkoutKickerApplet::reconfigure()
{
//     m_applet->loadSettings();
}

void SkoutKickerApplet::about()
{
//     m_applet->about();
}

void SkoutKickerApplet::preferences()
{
//     m_applet->preferences();
}

#include "skout_kicker_applet.moc"

/* kate: replace-tabs true; tab-width 4; */