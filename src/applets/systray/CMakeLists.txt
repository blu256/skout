################################################################################
# Skout - a Be-style panel for TDE                                             #
# Copyright (C) 2023 Mavridis Philippe <mavridisf@gmail.com>                   #
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
tde_add_kpart( skoutapplet_systray AUTOMOC
  SOURCES
    skout_system_tray.cpp
    skout_status_widget.cpp
    skout_system_graph.cpp

  LINK
    tdecore-shared
    tdeui-shared
    skout-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)

### desktop file ###############################################################
tde_create_translated_desktop(
  SOURCE skout_system_tray.desktop
  DESTINATION ${DATA_INSTALL_DIR}/skout/applets
  PO_DIR skout-applets-desktops
)