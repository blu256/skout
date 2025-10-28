/*******************************************************************************
  Skout - a DeskBar-style panel for TDE
  Copyright (C) 2025 Mavridis Philippe <mavridisf@gmail.com>

  Partially based on KPager.
  Copyright (C) 2000  Antonio Larrosa Jimenez
                      Matthias Ettrich
                      Matthias Elter

  XComposite support from Komposé.
  Copyright (C) 2004 by Hans Oischinger <hans.oischinger@kde-mail.net>

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
#include <tdelocale.h>
#include <twinmodule.h>
#include <twin.h>
#include <kdebug.h>

// Skout
#include "skout_pager.h"
#include "skout_pager_cfg.h"
#include "skout_pager_desktop.h"
#include "skout_utils.h"
#include <config.h>

// X11
#ifdef COMPOSITE
# include <X11/Xatom.h>
# include <X11/extensions/Xcomposite.h>
# include <X11/extensions/Xdamage.h>
# include <X11/extensions/Xrender.h>
#endif

extern "C"
{
    TDE_EXPORT SkoutApplet *init(SkoutPanel *parent, TDEConfig *cfg)
    {
        return new SkoutPager(parent, cfg);
    }

    TDE_EXPORT TQWidget *config(TQWidget *parent, const TQString& cfg)
    {
        return new SkoutPagerConfig(parent, cfg);
    }
}

SkoutPager::SkoutPager(SkoutPanel *parent, TDEConfig *cfg)
  : SkoutApplet(parent, cfg, "SkoutPager"),
    m_twin(new KWinModule(this)),
    m_valid(false),
    m_layout(nullptr),
    m_useViewports(false),
    m_curDesktop(1),
    m_hasComposite(false)
{
    m_desktops.setAutoDelete(true);

    setFrameStyle(TQFrame::WinPanel | TQFrame::Sunken);
    setSizePolicy(TQSizePolicy::Minimum, TQSizePolicy::Fixed);
    setLineWidth(2);

    connect(m_twin, TQ_SIGNAL(currentDesktopChanged(int)),
                    TQ_SLOT(currentDesktopChanged(int)));
    connect(m_twin, TQ_SIGNAL(currentDesktopViewportChanged(int, const TQPoint&)),
                    TQ_SLOT(currentDesktopViewportChanged(int, const TQPoint&)));

    connect(m_twin, TQ_SIGNAL(numberOfDesktopsChanged(int)),
                    TQ_SLOT(numberOfDesktopsChanged(int)));
    connect(m_twin, TQ_SIGNAL(desktopGeometryChanged(int)),
                    TQ_SLOT(desktopGeometryChanged(int)));
    connect(m_twin, TQ_SIGNAL(desktopNamesChanged()),
                    TQ_SLOT(desktopNamesChanged()));

    connect(m_twin, TQ_SIGNAL(windowAdded(WId)),
                    TQ_SLOT(windowChanged(WId)));
    connect(m_twin, TQ_SIGNAL(windowRemoved(WId)),
                    TQ_SLOT(windowChanged(WId)));
    connect(m_twin, TQ_SIGNAL(windowChanged(WId)),
                    TQ_SLOT(windowChanged(WId)));

    connect(tdeApp, TQ_SIGNAL(backgroundChanged(int)),
                    TQ_SLOT(desktopUpdated(int)));

    reconfigure();
    m_valid = true;
}

SkoutPager::~SkoutPager()
{
    ZAP(m_twin);
    m_desktops.clear();
}

void SkoutPager::reconfigure()
{
    config()->setGroup("Pager");
    m_inline = config()->readBoolEntry("ShowInline", true);
    m_miniature = config()->readBoolEntry("ShowMiniature", false);

    if (m_miniature)
    {
        checkComposite();
    }

    updateCurrentDesktop();
    recreateDesktops();
    relayout();
}

void SkoutPager::recreateDesktops()
{
    m_desktops.clear();
    SkoutPagerDesktop *desktop;
    int desktops = m_twin->numberOfDesktops();
    int count = 1;
    int i = 1;
    do
    {
        TQSize viewports = m_twin->numberOfViewports(i);
        for (int j = 1; j <= viewports.width() * viewports.height(); ++j)
        {
            TQSize s = m_twin->numberOfViewports(m_twin->currentDesktop());
            TQPoint viewport((j - 1) % s.width(), (j - 1) % s.height());

            desktop = new SkoutPagerDesktop(this, count, viewport);
            m_desktops.append(desktop);

            ++count;
        }
    }
    while (++i <= desktops);
}

void SkoutPager::relayout()
{
    if (m_layout)
    {
        delete m_layout;
        m_layout = nullptr;
    }

    if (m_inline)
    {
        m_layout = new TQGridLayout(this, 0, 1);
    }
    else
    {
        m_layout = new TQGridLayout(this, 2, 0);
    }

    TQWidget *desktop;
    int i = 0, j = 0, count = 0, half = (m_desktops.count() + 1) / 2;
    for (desktop = m_desktops.first(); desktop; desktop = m_desktops.next())
    {
        desktop->hide();
        m_layout->addWidget(desktop, i, j);
        ++count;
        if (m_inline)
        {
            ++j;
        }
        else
        {
            i = count / half;
            j = count % half;
        }

        desktop->show();
    }

    float f = ((float)parentWidget()->width() / (float)tdeApp->desktop()->width()) / count;
    if (!m_inline) f *= 2;

    int rows = m_inline ? 1 : i;
    setFixedHeight(tdeApp->desktop()->height() * f * rows);

    m_layout->activate();
    emit updateGeometry();
}

void SkoutPager::update()
{
    TQWidget *desktop;
    for (desktop = m_desktops.first(); desktop; desktop = m_desktops.next())
    {
        desktop->update();
    }
}

void SkoutPager::currentDesktopChanged(int desktop)
{
    if (m_twin->numberOfDesktops() != m_desktops.count())
    {
        numberOfDesktopsChanged(m_twin->numberOfDesktops());
    }

    updateCurrentDesktop();

    update();
}

void SkoutPager::currentDesktopViewportChanged(int desktop, const TQPoint& viewport)
{
    // ### this is how minipager does it
    TQSize s = m_twin->numberOfViewports(m_twin->currentDesktop());
    currentDesktopChanged((viewport.y()-1) * s.width() + viewport.x());
}

void SkoutPager::numberOfDesktopsChanged(int desktops)
{
    TQSize s = m_twin->numberOfViewports(m_twin->currentDesktop());
    m_useViewports = s.width() * s.height() > 1;

    recreateDesktops();

    updateCurrentDesktop();
    relayout();
}

void SkoutPager::desktopGeometryChanged(int desktop)
{
}

void SkoutPager::desktopNamesChanged()
{
}

void SkoutPager::windowChanged(WId window)
{
}

void SkoutPager::desktopUpdated(int desktop)
{
}

void SkoutPager::updateCurrentDesktop()
{
    m_curDesktop = m_twin->currentDesktop();
    if (m_curDesktop == 0)
    {
        m_curDesktop = 1;
    }
}

void SkoutPager::gotoDesktop(int desktop)
{
    KWin::setCurrentDesktop(desktop);
}

void SkoutPager::checkComposite()
{
#ifdef COMPOSITE
    Display *dpy = tqt_xdisplay();
    int event_base, error_base;
    if (XCompositeQueryExtension(dpy, &event_base, &error_base))
    {
        m_hasComposite = true;

        int major = 1, minor = 1;
        XCompositeQueryVersion(dpy, &major, &minor);
        if (!(major > 0 || minor >= 2))
        {
            kdDebug() << "[SkoutPager] XComposite doesn't allow NamePixmap requests! - Disabling XComposite support" << endl;
            m_hasComposite = false;
        }

        int renderEvent, renderError;
        if (!XRenderQueryExtension(dpy, &renderEvent, &renderError))
        {
            kdDebug() << "[SkoutPager] XRender not available! - Disabling XComposite support" << endl;
            m_hasComposite = false;
        }

        int damageEvent, damageError;
        if (!XDamageQueryExtension(dpy, &damageEvent, &damageError))
        {
            kdDebug() << "[SkoutPager] XDamage not available! - Disabling XComposite support" << endl;
            m_hasComposite = false;
        }
    }

    if (m_hasComposite)
    {
        kdDebug() << "[SkoutPager] XComposite extension support enabled" << endl;
    }
#endif
}

#include "skout_pager.moc"

/* kate: replace-tabs true; tab-width 4; */