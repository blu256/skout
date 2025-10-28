################################################################################
# Skout - a DeskBar-style panel for TDE                                        #
# Copyright (C) 2023-2025 Philippe Mavridis <philippe.mavridis@yandex.com>     #
#                                                                              #
# Improvements and feedback are welcome!                                       #
# This software is licensed under the terms of the GNU GPL v3 license.         #
################################################################################

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TDE_INCLUDE_DIR}
    ${TQT_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

link_directories(
    ${TQT_LIBRARY_DIRS}
)

### skoutapplet_systray (module) ###############################################
tde_add_kpart( skoutapplet_pager AUTOMOC
  SOURCES
    skout_pager.cpp
    skout_pager_desktop.cpp
    skout_pager_cfg.cpp

  LINK
    tdeui-shared
    skout-shared
    ${XDAMAGE_LIBRARIES}
    ${XCOMPOSITE_LIBRARIES}

  DESTINATION ${PLUGIN_INSTALL_DIR}
)

### desktop file ###############################################################
tde_create_translated_desktop(
  SOURCE skout_pager.desktop
  DESTINATION ${DATA_INSTALL_DIR}/skout/applets
  PO_DIR skout-applets-desktops
)

# kate: replace-tabs true; tab-width 2;