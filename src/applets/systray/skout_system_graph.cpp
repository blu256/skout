/*******************************************************************************
  Skout - a Be-style panel for TDE
  Copyright (C) 2023 Mavridis Philippe <mavridisf@gmail.com>

  This file has borrowed code from tdebase/ksysguard/gui/SensorShellAgent.cpp
      KSysGuard, the KDE System Guard
      Copyright (c) 1999 - 2001 Chris Schlaeger <cs@kde.org>

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
#include <tqtimer.h>
#include <tqfontmetrics.h>
#include <tqpainter.h>
#include <tqtooltip.h>
#include <tqwhatsthis.h>

// TDE
#include <tdeapplication.h>
#include <tdeglobalsettings.h>
#include <kstandarddirs.h>
#include <tdeconfig.h>
#include <tdepopupmenu.h>
#include <kiconloader.h>
#include <kprocess.h>
#include <tdelocale.h>
#include <krun.h>
#include <kdebug.h>

// Skout
#include "skout_status_widget.h"
#include "skout_system_graph.h"
#include "skout_system_tray.h"

SkoutSystemGraph::SkoutSystemGraph(SkoutStatusWidget *status, const char *name,
                             TQStringList &endpoints, bool aggregateMaxValues)
  : TQWidget(status, name),
    m_isPolling(false),
    m_isReceiving(true),
    m_isOnline(false),
    m_isUpdating(false),
    m_pollingFrequency(1500),
    m_maxValue(0),
    m_value(0),
    m_aggregateMaxValues(aggregateMaxValues)
{
    m_endpoints = TQStringList(endpoints);

    setSizePolicy(TQSizePolicy::Expanding, TQSizePolicy::Minimum);

    TQToolTip::add(this, i18n(name));
    TQWhatsThis::add(this, i18n("<qt><p>This graph monitors the <b>%1</b> sensor "
                                "on this system."
                                "<ul><li>Click on it to launch KSysGuard.</li>"
                                "<li>Press the right mouse button to see a menu "
                                "with useful functions.</li></ul></qt>").arg(name));

    init();
}

SkoutSystemGraph::~SkoutSystemGraph() {
    deinit();
}

void SkoutSystemGraph::init() {
    resetRetryCount();

    m_ksgrd = new TDEProcess;
    *m_ksgrd << "ksysguardd";

    connect(m_ksgrd, SIGNAL(processExited(TDEProcess*)),
                     SLOT(slotDaemonExited(TDEProcess*)));

    connect(m_ksgrd, SIGNAL(receivedStdout(TDEProcess*, char*, int)),
                     SLOT(slotDaemonStdout(TDEProcess*, char*, int)));

    connect(m_ksgrd, SIGNAL(receivedStderr(TDEProcess*, char*, int)),
                     SLOT(slotDaemonStderr(TDEProcess*, char*, int)));

    if (!startDaemon()) {
        statusWidget()->sysTray()->popup("error",
            i18n("Could not initialize sensor \"%1\"!"),
            i18n("Cannot connect to KSysGuard service."));
    }
}

void SkoutSystemGraph::deinit() {
    if (m_ksgrd) {
        if (m_ksgrd->isRunning()) {
            write("quit\n");
        }
        delete m_ksgrd;
        m_ksgrd = nullptr;
    }
}

bool SkoutSystemGraph::startDaemon() {
    if (!m_ksgrd->start(TDEProcess::NotifyOnExit, TDEProcess::All)) {
        deinit();
        return false;
    }

    m_maxValue = 0;
    reset();
    return true;
}

void SkoutSystemGraph::resetRetryCount() {
    m_retryCount = 3;
}

void SkoutSystemGraph::reset() {
    m_endpoint = m_endpoints.begin();
    m_value = 0;
}

void SkoutSystemGraph::update() {
    if (m_isUpdating || !m_isOnline || m_isReceiving) return;

    m_isUpdating = true;
    // store current endpoint
    TQString endpoint(*m_endpoint);

    // check if we have reached the last item
    if (m_endpoint == m_endpoints.end()) { // update finished
        m_isUpdating = false;

        // update readings list
        int perc = m_value * 100 / m_maxValue;
        m_readings.push_back(perc);
        while (m_readings.count() > graphRect().width()) {
            m_readings.pop_front();
        }

        // update tooltip
        TDELocale *l = TDEGlobal::locale();
        TQToolTip::remove(this);
        if (m_maxValue != 100) {
            TQToolTip::add(this, TQString("%1: %2%3/%4%5 (%6%)")
                                 .arg(name())
                                 .arg(l->formatNumber(m_value, 0))
                                 .arg(" " + i18n(m_unit.local8Bit()))
                                 .arg(l->formatNumber(m_maxValue, 0))
                                 .arg(" " + i18n(m_unit.local8Bit()))
                                 .arg(perc));
        }
        else {
            TQToolTip::add(this, TQString("%1: %2%")
                                 .arg(name())
                                 .arg(perc));
        }
        repaint();

        reset();
        int pollingTimeout = m_pollingFrequency;
        if (!m_isPolling && m_maxValue > 0) {
            // we're probably ready to enter polling mode
            m_isPolling = true;
            pollingTimeout = 0; // update immediately
        }
        TQTimer::singleShot(pollingTimeout, this, SLOT(update()));
        return;
    }

    // set iterator to next endpoint
    ++m_endpoint;

    // poll endpoint
    TQString cmd = m_isPolling ? "%1\n" : "%1?\n";
    write(cmd.arg(endpoint));
    m_isUpdating = false;
    m_isReceiving = true;
}

bool SkoutSystemGraph::write(TQString msg) {
    if (!m_ksgrd) return false;
    TQCString data = msg.latin1();
    return m_ksgrd->writeStdin(data, data.length());
}

void SkoutSystemGraph::slotDaemonExited(TDEProcess *) {
    if (--m_retryCount <= 0 || !startDaemon()) {
        statusWidget()->sysTray()->popup("error",
            i18n("Sensor \"%1\" has crashed!"),
            i18n("The KSysGuard service has stopped abruptly and could "
                 "not be restarted."));
    }
}

#define CHECK_EMPTY_RESPONSE \
    if (!buffer || !buflen) { \
        kdWarning() << "Sensor " << name() << ": received empty response " \
                    << "from KSysGuard daemon." << endl; \
        return; \
    }

void SkoutSystemGraph::slotDaemonStdout(TDEProcess *, char *buffer, int buflen) {
    if (m_isOnline && !m_isReceiving) return;
    m_isReceiving = false;

    CHECK_EMPTY_RESPONSE
    resetRetryCount();

    TQString msg = TQString::fromLocal8Bit(buffer, buflen);

    if (msg.contains("ksysguardd>") && !m_isOnline) {
        kdDebug() << "Monitor " << name() << " goes online!" << endl;
        m_isOnline = true;
        update();
        return;
    }

    if (!m_isOnline) return;

    msg.remove("ksysguardd>");
    msg.remove("\n");

    bool ok;
    if (m_isPolling) {
        int value = msg.toInt(&ok);
        if (!ok) {
            kdError() << "Cannot cast value of endpoint " << (*m_endpoint)
                      << " to integer: " << value << endl;
        }
        else m_value += value;
    }
    else {
        TQStringList tok = TQStringList::split("\t", msg);
        m_unit = tok[3];
        TQString max(tok[2]);
        int maxValue = max.toInt(&ok);
        if (!ok) {
            kdError() << "Cannot cast max value of endpoint " << (*m_endpoint)
                      << " to integer: " << max << endl;
        }
        else {
            if (!m_aggregateMaxValues) {
                if (m_maxValue && m_maxValue != maxValue) {
                    kdWarning() << "Warning: max values are not to be aggregated"
                                << " for sensor " << name() << ", but the value"
                                << " just received (" << maxValue << ") differs"
                                << " from the previously received max value ("
                                << m_maxValue << "), ignoring the last value."
                                << endl;
                }
                else m_maxValue = maxValue;
            }
            else m_maxValue += maxValue;
        }
    }

    TQTimer::singleShot(500, this, SLOT(update()));
}

void SkoutSystemGraph::slotDaemonStderr(TDEProcess *, char *buffer, int buflen) {
    CHECK_EMPTY_RESPONSE
    kdWarning() << "Sensor " << name() << " received a message on stderr:"
                << endl << TQString::fromLocal8Bit(buffer, buflen) << endl;
}

#undef CHECK_EMPTY_RESPONSE

TQRect SkoutSystemGraph::labelRect() const {
    TQRect r = rect();
    r.setX(r.right() - fontMetrics().width("100%"));
    return r;
}

TQRect SkoutSystemGraph::graphRect() const {
    TQRect r = rect();
    r.setWidth(r.width() - labelRect().width());
    return r;
}

void SkoutSystemGraph::paintEvent(TQPaintEvent *e) {
    TQPainter p(this);
    p.setPen(TDEGlobalSettings::textColor());

    TQRect lr = labelRect();
    if (m_isOnline) {
        p.drawText(lr, AlignCenter, TQString("%1%").arg(m_readings.last()));
    }
    else {
        p.setPen(TQt::red);
        p.drawText(lr, AlignCenter, "X");
        TQToolTip::remove(this);
        TQToolTip::add(this, i18n("%1: monitor is offline").arg(name()));
    }

    TQRect gr = graphRect();
    p.drawLine(gr.left(), gr.bottom(), gr.right(), gr.bottom());

    p.setPen(TDEGlobalSettings::highlightColor());

    int x = TQMAX(gr.left(), gr.right() - m_readings.count());
    TQValueList<int>::iterator it;
    for (it = m_readings.begin(); it != m_readings.end(); ++it) {
        int y = gr.bottom() - 1;
        int h = (*it) * y / 100;
        p.drawLine(x, y, x, y - h);
        if (++x > gr.right()) return;
    }
}

void SkoutSystemGraph::mousePressEvent(TQMouseEvent *e) {
    if (e->button() == TQt::LeftButton) {
        launch(systemMonitor());
    }
    else if (e->button() == TQt::RightButton) {
        m_tools.clear();
        m_tools += systemMonitor();
        m_tools += processManager();
        m_tools += terminalEmulator();

        TDEPopupMenu popup;
        popup.insertTitle(SmallIcon("system-run"), i18n("Launch..."));

        KService::List::iterator it;
        int index = 100;
        for (it = m_tools.begin(); it != m_tools.end(); ++it) {
            KService::Ptr t = (*it);
            if (t && t->isValid()) {
                TQString label = t->genericName();
                if (label.isEmpty()) {
                    label = i18n(t->name().local8Bit());
                }
                popup.insertItem(SmallIcon(t->icon()), label, index);
            }
            ++index;
        }

        connect(&popup, SIGNAL(activated(int)), SLOT(launchMenuItem(int)));
        popup.exec(mapToGlobal(e->pos()));
    }
}

void SkoutSystemGraph::launch(KService::Ptr svc) {
    TDEProcess proc;
    proc << KRun::processDesktopExec(*svc, KURL::List(), false, false);
    if (!proc.start(TDEProcess::DontCare)) {
        statusWidget()->sysTray()->popup(
            "messagebox_warning", i18n("Launch error!"),
            i18n("Unable to launch <b>%1</b>!").arg(svc->name()));
    }
}

void SkoutSystemGraph::launchMenuItem(int id) {
    launch(m_tools[id - 100]);
}

KService::Ptr SkoutSystemGraph::systemMonitor() {
    return KService::serviceByDesktopName("ksysguard");
}

KService::Ptr SkoutSystemGraph::processManager() {
    return new KService("Process Manager", "kpm", "view_sidetree");
}

KService::Ptr SkoutSystemGraph::terminalEmulator() {
    TDEConfigGroup confGroup(TDEGlobal::config(), "General");
    TQString term = confGroup.readPathEntry("TerminalApplication", "konsole");
    return new KService("Terminal Emulator", term, "konsole");
}

#include "skout_system_graph.moc"