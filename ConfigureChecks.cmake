################################################################################
# Skout - a Be-style panel for TDE                                             #
# Copyright (C) 2023 Mavridis Philippe <mavridisf@gmail.com>                   #
#                                                                              #
# Improvements and feedback are welcome!                                       #
# This software is licensed under the terms of the GNU GPL v3 license.         #
################################################################################

find_package( TQt )
find_package( TDE )

tde_setup_architecture_flags( )

include( TestBigEndian )
test_big_endian( WORDS_BIGENDIAN )

tde_setup_largefiles( )


if( WITH_GCC_VISIBILITY )
  tde_setup_gcc_visibility( )
endif( WITH_GCC_VISIBILITY )