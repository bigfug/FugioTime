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

#------------------------------------------------------------------------------

find_package( Qt6 REQUIRED COMPONENTS Core Gui Network Widgets )

if(NOT Qt6_FOUND)
	find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Network Widgets)
endif()

#------------------------------------------------------------------------------

set( LIB_FILES
	../lib/timecast.cpp
	../lib/timecast.h
	../lib/timeserver.cpp
	../lib/timeserver.h
	../lib/timesync.cpp
	../lib/timesync.h
	../lib/timedatagram.h
)

set( GUI_FILES
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	${LIB_FILES}
)

set( CONSOLE_FILES
	console/main.cpp
	console/clientconsole.cpp
	console/clientconsole.h
	${LIB_FILES}
)

add_executable( FugioTime WIN32 MACOSX_BUNDLE ${GUI_FILES} )

if( APPLE )
	set_target_properties( FugioTime PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.bigfug.fugio
		MACOSX_FRAMEWORK_INFO_PLIST Info.plist
	)
endif()

install( TARGETS FugioTime
	RUNTIME DESTINATION .
	BUNDLE DESTINATION .
)

if( WIN32 )
	find_program( WINDEPLOYQT windeployqt )

	install( CODE "execute_process( COMMAND ${WINDEPLOYQT} ${CMAKE_INSTALL_PREFIX}/FugioTime.exe --no-angle )" )
endif( WIN32 )

if( APPLE )
	find_program( MACDEPLOYQT macdeployqt )

	install( CODE "execute_process( COMMAND ${MACDEPLOYQT} ${CMAKE_INSTALL_PREFIX}/FugioTime.app -always-overwrite )" )
endif( APPLE )

#------------------------------------------------------------------------------
# Qt

target_link_libraries( FugioTime Qt::Core Qt::Gui Qt::Network Qt::Widgets )

