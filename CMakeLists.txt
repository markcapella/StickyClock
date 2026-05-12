##
# Prototype desktop widget similar to kde plasmoid. Remembers
# it's position and size on screen. Hovered settings button
# allows click to show or hide the widgets movement & sizing
# frame.
#
# Widget views are transparent (see-through to desktop) &
# transparent to clicks. Desktop icons for example can be
# clicked "through" the Widget draw canvas.
#
# Widget draw canvas can be customized to new views easily.
#
set(APP_NAME "StickyClock")
set(ORG_NAME "skyrmion.inc")
set(APP_AUTHOR "Mark James Capella")

set(APP_VERSION "26.05.12")
set(APP_VERSION_SUFFIX "r3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

cmake_minimum_required(VERSION 3.10)
project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

find_package(PkgConfig QUIET)
if (NOT ${PkgConfig_FOUND})
    message(WARNING "Could not locate package PkgConfig -"
         " Please install |pkg-config|.")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(Qt6 QUIET COMPONENTS Core Widgets)
if (NOT ${Qt6_FOUND})
    message(WARNING "Could not locate package Qt6 -"
         " Please install |qt6-base-dev|.")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(X11 QUIET)
if (NOT ${X11_FOUND})
    message(WARNING "Could not locate package X11 -"
        " Please install |libx11-dev|.")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(X11 QUIET COMPONENTS Xft)
if (NOT ${X11_Xft_FOUND})
    message(WARNING "Could not locate package Xft -"
         " Please install |libxft-dev|.")
    message(FATAL_ERROR "FATAL.")
endif()

pkg_check_modules(XFIXES QUIET xfixes)
if (NOT XFIXES_FOUND)
    message(WARNING "Could not locate package xfixes -"
         " Please install |libxfixes-dev|.")
    message(FATAL_ERROR "FATAL.")
endif()

pkg_check_modules(XEXT QUIET xext)
if (NOT XEXT_FOUND)
    message(WARNING "Could not locate package xext -"
         " Please install |libxext-dev|.")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(PNG QUIET)
if (NOT PNG_FOUND)
    message(WARNING "Could not locate package Libpng -"
         " Please install |libpng-dev|.")
    message(FATAL_ERROR "FATAL.")
endif()

##
# Build widget project.
#
set(WIDGET_PROJECT ${APP_NAME})

# All input sources.
set(WIDGET_SOURCES
    PinButton.cpp QuitButton.cpp
    MoveButton.cpp SizeButton.cpp
    SettingsHelper.cpp DisplayHelper.cpp
    XHelper.cpp
    StickyClock.cpp StickyWindow.cpp)

# Final executable.
add_executable(${WIDGET_PROJECT} ${WIDGET_SOURCES})

# Create C++ Global #defines.
set(ICON_INSTALL_LOCATION "share/icons/hicolor/scalable/apps")
target_compile_definitions(${WIDGET_PROJECT} PRIVATE
    APP_NAME="${APP_NAME}"
    ORG_NAME="${ORG_NAME}"
    ICON_INSTALL_LOCATION="${CMAKE_INSTALL_PREFIX}/${ICON_INSTALL_LOCATION}/"
)

# All Include sources.
target_include_directories(${WIDGET_PROJECT} PRIVATE
    ${X11_INCLUDE_DIR}
    ${XFIXES_INCLUDE_DIRS}
    ${XEXT_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

# All linker libs.
target_link_libraries(${WIDGET_PROJECT} PRIVATE
    Qt6::Core Qt6::Widgets
    ${X11_LIBRARIES}
    X11::Xft
    ${XFIXES_LIBRARIES}
    ${XEXT_LIBRARIES}
    ${PNG_LIBRARIES}
)

##
# Display leak report after app runs.
#
#target_compile_options(${WIDGET_PROJECT} PUBLIC
#    -fsanitize=address -fno-omit-frame-pointer)
#target_link_options(${WIDGET_PROJECT} PUBLIC
#    -fsanitize=address)

##
# Halt any old active instances.
#
install(CODE "set(APP_NAME \"${APP_NAME}\")")
install(SCRIPT haltObsolete.cmake)

##
# Install executable.
#
install(TARGETS ${WIDGET_PROJECT} DESTINATION bin)

##
# Install App & Button icons.
#
# TODO: sudo touch /usr/share/icons/*
# TODO: sudo gtk-update-icon-cache -f -t /usr/share/icons/
# TODO: kill & restart lxqt-panel
#
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}.png"
    DESTINATION "${ICON_INSTALL_LOCATION}"
        RENAME ${APP_NAME}.png)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinIn.png"
    DESTINATION "${ICON_INSTALL_LOCATION}"
        RENAME ${APP_NAME}-pinIn.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinOut.png"
    DESTINATION "${ICON_INSTALL_LOCATION}"
        RENAME ${APP_NAME}-pinOut.png)

##
# Install .desktop.
#
# TODO: kill & restart lxqt-panel
# TODO: libglib2.0-bin: has gio launch FOO.desktop
# TODO: kde-cli-tools:  has kioclient exec FOO.desktop
#
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}.desktop"
    DESTINATION share/applications)

##
# Reset Lxqt panel.
#
# TODO:
# install(CODE [[message(STATUS "Resetting Icon cache for new App Icon ...")]])
# install(CODE "
#     execute_process(COMMAND gtk-update-icon-cache -f -t /usr/share/icons)
# ")

##
# Setup convenience target |make run|.
#
if(NOT TARGET run)
    add_custom_target(run
        COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}"
    )
endif()

##
# Setup convenience target |make uninstall|.
#
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND}
            -DAPP_NAME=${APP_NAME}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/haltObsolete.cmake
                
        COMMAND ${CMAKE_COMMAND}
            -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()
