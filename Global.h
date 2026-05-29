
#pragma once

/**
 * Global headers, order important.
 *
 */
// Standard C libraries.
#include <chrono>
#include <cstdint>
#include <dirent.h>
#include <fcntl.h>
#include <format>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
typedef chrono::steady_clock Clock;

// Qt libraries.
#include <QApplication>
#include <QSettings>
#include <QImage>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QCheckBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QMouseEvent>
#include <QFileInfo>
#include <QDir>
#include <QToolTip>
#include <QStyleOptionFrame>
#include <QPointer>

// x11 libraries.
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

// x11 xft library.
#include <X11/Xft/Xft.h>

// x11 xfixes libraries.
#include <X11/extensions/Xfixes.h>

// x11 xext Shape libraries.
#include <X11/extensions/shape.h>

// x11 xext Xcomposite libraries.
#include <X11/extensions/Xcomposite.h>

// libpng library.
#include <png.h>

// libasan library.
#include <sanitizer/lsan_interface.h>
#include <sys/resource.h>

// Application libraries.
#include "Button.h"
#include "ColorButton.h"

#include "PinButton.h"
#include "QuitButton.h"
#include "MoveButton.h"
#include "SizeButton.h"

#include "ConfigDialog.h"
#include "ConfigButton.h"

#include "AboutDialog.h"
#include "DisplayHelper.h"
#include "RecentsHelper.h"
#include "SettingsHelper.h"
#include "WinInfo.h"
#include "XHelper.h"

#include "StickyWindow.h"
#include "StickyWidgetIII.h"
