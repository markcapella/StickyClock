
#include "Global.h"

/**
 * StickyClock is the first app in the StickyWidgetIII
 * prototype series, an X-based prototype similar
 * to KDE Plasmoids.
 *
 * Desktop widgets with various views can be stuck in
 * place to the desktop below other windows.
 *
 * Widget remembers position, size, settings etc.
 *
 * Cursor hover reveals PinButton that toggles widget
 * in & out of "Stuck" state.
 *
 * When stuck to the desktop, widget ignores clicks &
 * passes all mouse actions to desktop (input transparency),
 * and has a transparent visual background.
 *
 * While "Stuck" to the desktop, the widgets window
 * button in the panels Task manager is removed.
 *
 */

// App forward decls.
void initAppHelpers();
bool initWindowCompositor();

bool initAppPngImages(); // No uninit.

void uninitWindowCompositor();
void uninitAppHelpers();

int getAppInstanceCount();


// App globals.
DisplayHelper* mDisplayHelper = nullptr;
Display* mDisplay = nullptr;
XftFont* mFont = nullptr;

SettingsHelper* mSettingsHelper = nullptr;
XHelper* mXHelper = nullptr;

XVisualInfo mVisualInfoStruct { };
Colormap mColormap { };

StickyWindow* mStickyWindow = nullptr;

QImage mPinInQImage { };
XImage* mPinInXImage = nullptr;

QImage mPinOutQImage { };
XImage* mPinOutXImage = nullptr;


/**
 * Main.
 */
int main(int argc, char** argv) {
    if (getAppInstanceCount() > 1) {
        cout << endl << XCOLOR_RED << "StickyClock() is already running "
            "& only allows for one to run @ a time." << endl;
        return true;
    }

    // Some Qt6 setup.
    QCoreApplication::setOrganizationName(ORG_NAME);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication app(argc, argv);

    // Init Display global.
    mDisplayHelper = new DisplayHelper();
    mDisplay = mDisplayHelper->getDisplay();
    if (mDisplay == nullptr) {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: X11 "
            "Windows are unavailable with this Desktop - "
            "FATAL." << XCOLOR_NORMAL << endl;
        return true;
    }

    // Init Font global & App helpers.
    mFont = XftFontOpenName(mDisplay, DefaultScreen(mDisplay),
        TIME_DISPLAY_FONT);
    initAppHelpers();
    initAppPngImages();

    // Init Compositor, run app, & uninit Compositor.
    if (initWindowCompositor()) {
        StickyWindow* mStickyWindow = new StickyWindow();
        if (mStickyWindow->getX11Window() != None) {
            mStickyWindow->run();
            delete mStickyWindow;
            uninitWindowCompositor();
        }
    }

    // Uninit App helpers & Font global.
    uninitAppHelpers();
    XftFontClose(mDisplay, mFont);
    mFont = nullptr;
    FcFini();

    // Uninit Display global.
    delete mDisplayHelper;
    XCloseDisplay(mDisplay);
    return false;
}

/**
 * Initialize App global helpers.
 */
void initAppHelpers() {
    // Init Settings helper global.
    mSettingsHelper = new SettingsHelper(APP_NAME);

    // Set X Error handler (quiets non-errors).
    mXHelper = new XHelper();
    XSetErrorHandler(mXHelper->handleX11ErrorEvent);
}

/**
 * Initialize window compositor.
 */
bool initWindowCompositor() {
    // Ensure we have a compositor running.
    if (!mXHelper->isACompositorRunning()) {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: A "
            "Compositor isn't running, widgets lose "
            "transparency - FATAL." << XCOLOR_NORMAL << endl;
        return false;
    }

    // Ensure the compositor has true, per-pixel, alpha
    // blended transparency.
    const int VISUAL_COLOR_DEPTH = 32;
    if (XMatchVisualInfo(mDisplay, DefaultScreen(mDisplay),
        VISUAL_COLOR_DEPTH, TrueColor, &mVisualInfoStruct)) {
        mColormap = XCreateColormap(mDisplay,
            RootWindow(mDisplay, DefaultScreen(mDisplay)),
            mVisualInfoStruct.visual, AllocNone);
    } else {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: A "
            "Compositor is running, but is incapable of "
            "transparency - FATAL." << XCOLOR_NORMAL << endl;
        return false;
    }

    return true;
}

/**
 * Initialize all Png Button Images.
 */
bool initAppPngImages() {
    // Load the Pin-In image.
    const QString PIN_IN_FILE = ICON_INSTALL_LOCATION + PIN_IN_PNG_FILENAME;
    if (!mPinInQImage.load(PIN_IN_FILE)) {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: A "
            "required resource image can't be found - FATAL." <<
            XCOLOR_NORMAL << endl;
        cout << endl << XCOLOR_YELLOW << "StickyWidgetIII: Missing: " <<
            PIN_IN_FILE.toStdString() << "." << XCOLOR_NORMAL << endl;
        return false;
    }
    mPinInQImage = mPinInQImage.convertToFormat(QImage::Format_RGB32);
    mPinInXImage = XCreateImage(mDisplay,
        DefaultVisual(mDisplay, DefaultScreen(mDisplay)),
        32,
        ZPixmap, 0,
        (char*) mPinInQImage.bits(), mPinInQImage.width(), mPinInQImage.height(),
        32, 0);

    // Load the Pin-Out image.
    if (!mPinOutQImage.load(ICON_INSTALL_LOCATION + PIN_OUT_PNG_FILENAME)) {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: A "
            "required resource image can't be found - FATAL." <<
            XCOLOR_NORMAL << endl;
        cout << endl << XCOLOR_YELLOW << "StickyWidgetIII: Missing: " <<
            PIN_OUT_PNG_FILENAME.toStdString() << "." << XCOLOR_NORMAL << endl;
        return false;
    }
    mPinOutQImage = mPinOutQImage.convertToFormat(QImage::Format_RGB32);
    mPinOutXImage = XCreateImage(mDisplay,
        DefaultVisual(mDisplay, DefaultScreen(mDisplay)),
        32,
        ZPixmap, 0,
        (char*) mPinOutQImage.bits(), mPinOutQImage.width(), mPinOutQImage.height(),
        32, 0);
    XImage* fff = mPinOutXImage;

    return true;
}

/**
 * Initialize window compositor.
 */
void uninitWindowCompositor() {
    XFreeColormap(mDisplay, mColormap);
}

/**
 * Uninitialize all, allow sanitizer perfection 💫.
 */
void uninitAppHelpers() {
    delete mXHelper;
    delete mSettingsHelper;
}

/**
 * Helper method to determine the Display Manager (DM).
 */
int getAppInstanceCount() {
    int instanceCount = 0;

    // Open procs file for read.
    const char* CMD = "pgrep StickyClock";
    FILE* procsFile = popen(CMD, "r");
    if (!procsFile) {
        return instanceCount;
    }

    // Read each record, counting app instances.
    #define BUFFER_LENGTH 1024
    char inFileBuffer[BUFFER_LENGTH];
    try {
        while (fgets(inFileBuffer, BUFFER_LENGTH,
            procsFile) != nullptr) {
            instanceCount++;
        }
    } catch (...) { }

    // Close file & return result.
    pclose(procsFile);
    return instanceCount;
}
