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
)

link_directories(
    ${TQT_LIBRARY_DIRS}
)

### skout (executable) #########################################################
tde_add_executable(
  skout AUTOMOC

  SOURCES
    main.cpp
    skout.cpp
    skout_panel.cpp
    skout_widget.cpp
    skout_menu_button.cpp
    skout_menu.cpp
    skout_system_tray.cpp
    skout_task_manager.cpp
    skout_task_container.cpp
    skout_task_button.cpp
    skout_task_grouper.cpp
    skout_task.cpp
    skoutsettings.kcfgc

  LINK
    tdecore-shared
    tdeui-shared
    tdeio-shared

  DESTINATION ${BIN_INSTALL_DIR}
)

### kcm_skout (module) #########################################################
tde_add_kpart(
  kcm_skout AUTOMOC

  SOURCES
    tdecm_skout.cpp
    skout_config.ui
    skoutsettings.kcfgc

  LINK
    tdecore-shared
    tdeui-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)

### translated desktop files ###################################################
tde_create_translated_desktop( skout.desktop )
tde_create_translated_desktop( skout_config.desktop )