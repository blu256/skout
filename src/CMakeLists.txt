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

### libskout (library) #########################################################
tde_add_library(
  skout SHARED AUTOMOC

  SOURCES
    skout_applet.cpp
    skoutsettings.kcfgc

  LINK
    tdecore-shared

  DESTINATION ${LIB_INSTALL_DIR}
)

### skout (executable) #########################################################
tde_add_executable(
  skout AUTOMOC

  SOURCES
    main.cpp
    skout.cpp
    skout_panel.cpp
    skout_menu_button.cpp
    skout_menu.cpp
    skoutiface.skel

  LINK
    tdecore-shared
    tdeui-shared
    tdeio-shared
    skout-shared

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

### applets ####################################################################
add_subdirectory(applets)

### headers ####################################################################
install(
  FILES
    skout_applet.h

  DESTINATION ${INCLUDE_INSTALL_DIR}
)

### translated desktop files ###################################################
tde_create_translated_desktop( skout.desktop )
tde_create_translated_desktop( skout_config.desktop )