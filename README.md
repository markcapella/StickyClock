# StickyClock
    
!['StickyClockIcon'](https://github.com/markcapella/StickyClock/blob/main/StickyClock.png)
!['StickyClock'](https://github.com/markcapella/StickyClock/blob/main/screenshot.png)
    
## Description
    StickyClock is an X11 StickyWidgetIII based desktop view.
    
    A basic Digital Clock that is sizable and configurable.
    
    
    Desktop widgets with various views can be stuck in place to
    the desktop below other windows and display custom views with
    a transparent visual background such as Clocks, Reminders,
    Lightstrands, Meters & more!
    
    Widget remembers workstation, position, size, settings etc.
    
    Cursor hover reveals PinButton that toggles widget in
    & out of "Desktop Stuck" state.
    
    When stuck to the desktop, widget ignores clicks & passes
    all mouse actions to desktop (input transparency). (You can
    double click Desktop Icons underneath widget views.)
    
    StickyClock & StickyWidgets won't present an item in your
    desktop panel, nor in your system tray. Mouse hover the
    widget to reveal the Pin or Close button.
    
    
## Installation.
    You'll need an X11 based DisplayManager & a TrueColor32 capable
    Window Manager (Kwin, Openbox, Fluxbox, Xfwm4, Mutter/Gnome).
    
    Tiling managers such as Awesome may run but without pointer event
    transparency.
    
    
### Install Pre-reqs.

For Debian systems:

    sudo apt install git cmake build-essential pkg-config qt6-base-dev libx11-dev libxft-dev libxfixes-dev libxext-dev libpng-dev

For Fedora systems:

    sudo dnf install git cmake gcc gcc-c++ make pkg-config qt6-devel libX11-devel libXft-devel libXfixes-devel libXext-devel libpng-devel

### Clone StickyClock source folder.

    git clone https://github.com/markcapella/StickyClock

### CD into source repo.

    cd StickyClock


## Basic development.

    mkdir build
    cd build
    cmake ..

    make
    make run

    sudo make install
    sudo make uninstall
    
    make clean
    cd ..
    rm -rf build
    
    
## Usage after install.
    StickyClock
    
    
## markjamescapella@proton.me Rocks !
    Yeah I do.
    
    
