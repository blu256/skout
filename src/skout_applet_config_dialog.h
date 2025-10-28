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

#ifndef _SKOUT_APPLET_CONFIG_DIALOG_H
#define _SKOUT_APPLET_CONFIG_DIALOG_H

// TDE
#include <kdialogbase.h>

// Skout
#include "skout_appletdb.h"

class SkoutAppletConfigDialog : public KDialogBase
{
    TQ_OBJECT

    public:
        SkoutAppletConfigDialog(TQWidget *parent, AppletData applet);
        ~SkoutAppletConfigDialog();

        bool changed();

    protected slots:
        virtual void slotOk();
        virtual void slotApply();
        virtual void slotDefault();
        virtual void slotChanged();

    private slots:
        void reconfigure();

    private:
        AppletData m_applet;
        SkoutAppletConfig *m_config;
        bool m_changed;
};

#endif // _SKOUT_APPLET_CONFIG_DIALOG_H