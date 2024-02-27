cmake_minimum_required( VERSION 3.1 )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

#------------------------------------------------------------------------------
# Qt5

# Instruct CMake to run moc automatically when needed.

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

set( CMAKE_INCLUDE_CURRENT_DIR ON )

SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

#------------------------------------------------------------------------------

project( FugioTime LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_subdirectory( gui )
add_subdirectory( console )
add_subdirectory( consoleclient )
