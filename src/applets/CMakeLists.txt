################################################################################
# Skout - a DeskBar-style panel for TDE                                        #
# Copyright (C) 2023-2025 Philippe Mavridis <philippe.mavridis@yandex.com>     #
#                                                                              #
# Improvements and feedback are welcome!                                       #
# This software is licensed under the terms of the GNU GPL v3 license.         #
################################################################################

add_subdirectory(systray)
add_subdirectory(taskman)
add_subdirectory(calendar)
tde_conditional_add_subdirectory(BUILD_PAGER pager)

# kate: replace-tabs true; tab-width 2;