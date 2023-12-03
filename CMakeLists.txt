################################################################################
# Skout - a Be-style panel for TDE                                             #
# Copyright (C) 2023 Mavridis Philippe <mavridisf@gmail.com>                   #
#                                                                              #
# Improvements and feedback are welcome!                                       #
# This software is licensed under the terms of the GNU GPL v3 license.         #
################################################################################

cmake_minimum_required( VERSION 3.1 )

project( skout )

### Required modules ###########################################################
include( FindPkgConfig          )
include( CheckFunctionExists    )
include( CheckSymbolExists      )
include( CheckIncludeFile       )
include( CheckLibraryExists     )
include( CheckCSourceCompiles   )
include( CheckCXXSourceCompiles )

### TDE macros #################################################################
include( TDEMacros )
tde_set_project_version( )

include( TDESetupPaths )
tde_setup_paths( )

### Optional support ###########################################################
# option( WITH_ALL_OPTIONS      "Enable all optional support" OFF )

### Modules to build ###########################################################
option( BUILD_ALL               "Build all"                   ON )
option( BUILD_DOC               "Build documentation"         ${BUILD_ALL} )
option( BUILD_TRANSLATIONS      "Build translations"          ${BUILD_ALL} )

### Configure checks ###########################################################
include( ConfigureChecks.cmake )

### Compiler settings ##########################################################
add_definitions( -DHAVE_CONFIG_H )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TQT_CXX_FLAGS}" )
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined" )

### Add subdirectories #########################################################
add_subdirectory( src )
add_subdirectory( data )
tde_conditional_add_project_docs( BUILD_DOC )
tde_conditional_add_project_translations( BUILD_TRANSLATIONS )

### Write configuration ########################################################
configure_file( config.h.cmake config.h @ONLY )