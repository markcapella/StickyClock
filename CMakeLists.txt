##
# Prototype desktop widget similar to kde plasmoid. Remembers
# it's position and size on screen. Hovered Pin Button
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
set(APP_DESC "A simple desktop Clock app roughly in the style of")
set(APP_DESC "${APP_DESC} a KDE Plasmoid.")

set(APP_VERSION "26.05.19")
set(APP_VERSION_MINOR "01")

set(APP_AUTHOR "Mark James Capella")
set(ORG_NAME "skyrmion.inc")

# Enable cmake.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

cmake_minimum_required(VERSION 3.10)
project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

# Define escape codes for error messages.
string(ASCII 27 ESC)
set(COLOR_RED    "${ESC}[31m")
set(COLOR_YELLOW "${ESC}[33m")
set(COLOR_NORMAL "${ESC}[m")

# Create "Required Packages" list.
set(PACKAGE_REQS "pkg-config qt6-base-dev libx11-dev wmctrl")
set(PACKAGE_REQS "${PACKAGE_REQS} libxft-dev libxfixes-dev")
set(PACKAGE_REQS "${PACKAGE_REQS} libxext-dev libpng-dev")

# Create Pre-reqs install message 1.
set(COULDNT_LOCATE_MSG "${COLOR_RED}")
set(COULDNT_LOCATE_MSG "${COULDNT_LOCATE_MSG}Could not locate package.")
set(COULDNT_LOCATE_MSG "${COULDNT_LOCATE_MSG}${COLOR_NORMAL}")

# Create Pre-reqs install message 2.
set(INSTALL_MSG "${COLOR_YELLOW}")
set(INSTALL_MSG "${INSTALL_MSG}Please try |sudo apt install")
set(INSTALL_MSG "${INSTALL_MSG} ${PACKAGE_REQS}|${COLOR_NORMAL}")

# Chek all.
find_package(PkgConfig QUIET)
if (NOT ${PkgConfig_FOUND})
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(Qt6 QUIET COMPONENTS Core Widgets)
if (NOT ${Qt6_FOUND})
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(X11 QUIET)
if (NOT ${X11_FOUND})
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

find_program(WMCTRL_EXECUTABLE wmctrl)
if (NOT WMCTRL_EXECUTABLE)
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(X11 QUIET COMPONENTS Xft)
if (NOT ${X11_Xft_FOUND})
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

pkg_check_modules(XFIXES QUIET xfixes)
if (NOT XFIXES_FOUND)
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

pkg_check_modules(XEXT QUIET xext)
if (NOT XEXT_FOUND)
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

find_package(PNG QUIET)
if (NOT PNG_FOUND)
    message(STATUS "${COULDNT_LOCATE_MSG}")
    message(STATUS "${INSTALL_MSG}")
    message(STATUS "")
    message(FATAL_ERROR "FATAL.")
endif()

##
# Build widget project.
#
set(WIDGET_PROJECT ${APP_NAME})

# All input sources.
set(WIDGET_SOURCES
    StickyWidgetIII.cpp
    PinButton.cpp QuitButton.cpp
    MoveButton.cpp SizeButton.cpp
    ConfigButton.cpp
    ConfigDialog.cpp
    ColorButton.cpp
    AboutDialog.cpp
    SettingsHelper.cpp DisplayHelper.cpp
    XHelper.cpp RecentsHelper.cpp
    StickyWindow.cpp
)

# Final executable.
add_executable(${WIDGET_PROJECT} ${WIDGET_SOURCES})

# Define app pngs.
set(ICON_PATH "share/icons/hicolor/scalable/apps")

# Create C++ Global #defines.
target_compile_definitions(${WIDGET_PROJECT} PRIVATE
    APP_NAME="${APP_NAME}"
    APP_DESC="${APP_DESC}"
    APP_VERSION="${APP_VERSION}"
    APP_VERSION_MINOR="${APP_VERSION_MINOR}"
    APP_AUTHOR="${APP_AUTHOR}"
    ORG_NAME="${ORG_NAME}"
    ICON_PATH="${CMAKE_INSTALL_PREFIX}/${ICON_PATH}/")

# All Include sources.
target_include_directories(${WIDGET_PROJECT} PRIVATE
    ${X11_INCLUDE_DIR}
    ${XFIXES_INCLUDE_DIRS}
    ${XEXT_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS})

# All linker libs.
target_link_libraries(${WIDGET_PROJECT} PRIVATE
    Qt6::Core Qt6::Widgets
    ${X11_LIBRARIES}
    X11::Xft
    ${XFIXES_LIBRARIES}
    ${XEXT_LIBRARIES}
    ${PNG_LIBRARIES})

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
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}.png)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinIn.png"
    DESTINATION "${ICON_PATH}"
        RENAME ${APP_NAME}-pinIn.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}-pinOut.png"
    DESTINATION "${ICON_PATH}"
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
# TODO: gtk-update-icon-cache -f -t /usr/share/icons

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
