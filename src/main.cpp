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

#include <stdlib.h>

// TDE
#include <tdeaboutdata.h>
#include <tdecmdlineargs.h>
#include <tdelocale.h>
#include <kdebug.h>

// Skout
#include "skout.h"
#include "skoutsettings.h"
#include "skout_panel.h" // PanelPosition
#include "version.h"

static const char description[] = I18N_NOOP("a DeskBar-style panel for TDE");

static TDECmdLineOptions options[] = {
    {"topright", I18N_NOOP("Place panel in the top right corner (default)"), 0},
    {"topleft",  I18N_NOOP("Place panel in the top left corner"), 0},
    TDECmdLineLastOption
};

int main(int argc, char **argv) {
    TDEAboutData about("skout", I18N_NOOP("Skout"), skout::version, description,
                       TDEAboutData::License_GPL_V3, skout::copyright,
                       I18N_NOOP("Skout is a DeskBar-style panel for TDE"));

    TDECmdLineArgs::init(argc, argv, &about);
    TDECmdLineArgs::addCmdLineOptions(options);

    TDEUniqueApplication::addCmdLineOptions();
    if (!TDEUniqueApplication::start()) {
        fprintf(stderr, "Skout is already running!\n");
        exit(0);
    }

    TDECmdLineArgs *args = TDECmdLineArgs::parsedArgs();

    PanelPosition pos = PanelPosition::Saved;
    if (args->isSet("topleft"))
        pos = PanelPosition::TopLeft;
    else if (args->isSet("topright"))
        pos = PanelPosition::TopRight;
    args->clear();

    Skout *app = new Skout(pos);
    int ret = app->exec();
    delete app;
    return ret;
}

/* kate: replace-tabs true; tab-width 4; */
