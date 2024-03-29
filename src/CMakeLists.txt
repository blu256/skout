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
    ${CMAKE_CURRENT_SOURCE_DIR}/external/dmctl
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
    skout_applet_panel_extension.cpp
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
    skout_appletdb.cpp
    skout_menu_button.cpp
    skout_menu.cpp
    skout_root_menu.cpp
    skout_settings_menu.cpp
    skoutiface.skel

  LINK
    tdecore-shared
    tdeui-shared
    tdeio-shared
    skout-shared
    dmctl-static

  DESTINATION ${BIN_INSTALL_DIR}
)

### kcm_skout (module) #########################################################
tde_add_kpart(
  kcm_skout AUTOMOC

  SOURCES
    tdecm_skout.cpp
    skout_appletdb.cpp
    skout_applet_selector.cpp
    skoutsettings.kcfgc

  LINK
    tdecore-shared
    tdeui-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)

### applets ####################################################################
add_subdirectory(applets)
add_subdirectory(external/dmctl)

### headers ####################################################################
install(
  FILES
    skout_applet.h

  DESTINATION ${INCLUDE_INSTALL_DIR}
)

### translated desktop files ###################################################
tde_create_translated_desktop(skout.desktop
    PO_DIR skout-desktops)
tde_create_translated_desktop(skout_config.desktop
    PO_DIR skout-desktops)

# kate: replace-tabs true; tab-width 2;