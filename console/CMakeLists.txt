cmake_minimum_required( VERSION 3.1 )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

#------------------------------------------------------------------------------

project( fugiotimeserver LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

#------------------------------------------------------------------------------

find_package( Qt6 REQUIRED COMPONENTS Core Network )

if(NOT Qt6_FOUND)
	find_package(Qt5 5.15 REQUIRED COMPONENTS Core Network)
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

set( APP_FILES
	main.cpp
	clientconsole.cpp
	clientconsole.h
	${LIB_FILES}
)

add_executable( ${PROJECT_NAME} ${APP_FILES} )

install( TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION .
	BUNDLE DESTINATION .
)

if( WIN32 )
	find_program( WINDEPLOYQT windeployqt )

	install( CODE "execute_process( COMMAND ${WINDEPLOYQT} ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.exe --no-angle )" )
endif( WIN32 )

if( APPLE )
	find_program( MACDEPLOYQT macdeployqt )

	install( CODE "execute_process( COMMAND ${MACDEPLOYQT} ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME} -always-overwrite )" )
endif( APPLE )

#------------------------------------------------------------------------------
# Qt5

target_link_libraries( ${PROJECT_NAME} Qt::Core Qt::Network )

