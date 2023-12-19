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
#include <tqfontmetrics.h>
#include <tqdatetime.h>
#include <tqtimer.h>
#include <tqlabel.h>
#include <tqclipboard.h>
#include <tqtooltip.h>
#include <tqwhatsthis.h>

// TDE
#include <tdeapplication.h>
#include <tdeglobalsettings.h>
#include <tdeglobal.h>
#include <kiconloader.h>
#include <kdatepicker.h>
#include <tdepopupmenu.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout_status_widget.h"
#include "skout_system_tray.h"
#include "skout_system_graph.h"

SkoutStatusWidget::SkoutStatusWidget(SkoutSysTray *tray)
  : TQHBox(tray)
{
    layout()->setSpacing(5);

    // Clock
    TQString clockPlaceholder = formatDateTime(TQDateTime::currentDateTime(),
                                               FormatTime);
    m_clock = new TQLabel(clockPlaceholder, this);
    m_clock->setFixedWidth(m_clock->fontMetrics().width(clockPlaceholder));
    m_clock->setAlignment(TQt::AlignCenter);
    updateClock();

    m_clockTimer = new TQTimer(this);
    connect(m_clockTimer, SIGNAL(timeout()), SLOT(updateClock()));

    TQWhatsThis::add(m_clock, i18n("<qt><p>The clock displays the current time."
                                   "<ul><li>Click on it to see a calendar.</li>"
                                   "<li>Press the middle mouse button to copy "
                                   "the time and/or date.</li>"
                                   "<li>Press the right mouse button to see "
                                   "a menu with more options.</li></ul></qt>"));

    // CPU
    TQStringList cpuEndpoints;
    cpuEndpoints << "cpu/nice" << "cpu/sys" << "cpu/user";
    m_cpuGraph = new SkoutSystemGraph(this, "CPU", cpuEndpoints, false);

    // Memory
    TQStringList memEndpoints;
    memEndpoints << "mem/physical/used" << "mem/swap/used";
    m_memGraph = new SkoutSystemGraph(this, "Memory", memEndpoints);
}

SkoutStatusWidget::~SkoutStatusWidget() {
    delete m_clock; m_clock = nullptr;
    delete m_clockTimer; m_clockTimer = nullptr;
}

void SkoutStatusWidget::updateClock() {
    TQDateTime now = TQDateTime::currentDateTime();
    m_clock->setText(formatDateTime(now, FormatTimeShort));
    TQToolTip::add(m_clock, formatDateTime(now, FormatDate));
}

const TQString SkoutStatusWidget::formatDateTime(TQDateTime dt, DateTimeFormat f) {
    TDELocale *l = TDEGlobal::locale();
    switch (f) {
        /* formatTime: includeSecs */
        case FormatTimeShort:         return l->formatTime(dt.time(), false);
        case FormatTime:              return l->formatTime(dt.time(), true);

        /* formatDate: shortFormat */
        case FormatDateShort:         return l->formatDate(dt.date(), true);
        case FormatDate:              return l->formatDate(dt.date(), false);

        /* formatDateTime: shortFormat, includeSecs */
        case FormatDateTimeShort:     return l->formatDateTime(dt, true, false);
        case FormatDateTimeSecs:      return l->formatDateTime(dt, false, true);
        case FormatDateTimeShortSecs: return l->formatDateTime(dt, true, true);

        default: // acts as fallback, just in case
        case FormatDateTime:          return l->formatDateTime(dt, false, false);
    }
}

void SkoutStatusWidget::configureDateTime() {
    sysTray()->launch("tdecmshell", "clock", "the clock settings module");
}

void SkoutStatusWidget::configureDateTimeFormat() {
    sysTray()->launch("tdecmshell", "language", "the locale settings module");
}

static KDatePicker *calendar;
static TQDateTime dtCopy;

void SkoutStatusWidget::mousePressEvent(TQMouseEvent *e) {
    // A mouse event handler for our clock
    if (m_clock->hasMouse()) {
        if (e->button() == TQt::LeftButton) {
            if (calendar) {
                delete calendar;
                calendar = nullptr;
            }
            else {
                calendar = new KDatePicker(0);
                calendar->setCaption(i18n("Calendar"));
                calendar->show();
            }
        }

        else if (e->button() == TQt::MidButton) {
            TDEPopupMenu popup;
            popup.insertTitle(SmallIcon("edit-copy"), i18n("Copy..."));
            dtCopy = TQDateTime::currentDateTime();
            DateTimeFormat f = (DateTimeFormat)0;
            while (f != DATETIMEFORMAT_END) {
                popup.insertItem(formatDateTime(dtCopy, f), f);
                f = (DateTimeFormat)(f + 1);
            }
            connect(&popup, SIGNAL(activated(int)), SLOT(copyDateTime(int)));
            popup.exec(mapToGlobal(e->pos()));
        }

        else if (e->button() == TQt::RightButton) {
            TDEPopupMenu popup;
            popup.insertItem(SmallIcon("date"), i18n("&Adjust Date && Time..."),
                             this, SLOT(configureDateTime()));
            popup.insertItem(SmallIcon("kcontrol"), i18n("Date && Time &Format..."),
                             this, SLOT(configureDateTimeFormat()));
            popup.exec(mapToGlobal(e->pos()));
        }
    }
}

void SkoutStatusWidget::copyDateTime(int choice) {
    if (choice < 0 || choice >= DATETIMEFORMAT_END) return;
    kapp->clipboard()->setText(formatDateTime(dtCopy, (DateTimeFormat)choice));
}

#include "skout_status_widget.moc"