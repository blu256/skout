################################################################################
# Skout - a Be-style panel for TDE                                             #
# Copyright (C) 2023 Mavridis Philippe <mavridisf@gmail.com>                   #
#                                                                              #
# Improvements and feedback are welcome!                                       #
# This software is licensed under the terms of the GNU GPL v3 license.         #
#                                                                              #
# Build file for dmctl (needed for Skout Menu)                                 #
# dmctl is a part of tdmlib                                                    #
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

### dmctl (library) ############################################################
tde_add_library(dmctl STATIC_PIC
  SOURCES dmctl.cpp

  LINK
    tdecore-shared Xau
)

# kate: replace-tabs true; tab-width 2;