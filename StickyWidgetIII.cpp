
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
bool initVisualTransparency();
bool initAppPngImages();

void unintPngImages();
void uninitVisualTransparency();
void uninitAppHelpers();

int getAppInstanceCount();
void signalIntToSTDOUT(int n);


// App globals.
DisplayHelper* mDisplayHelper = nullptr;
Display* mDisplay = nullptr;
XftFont* mFont = nullptr;

SettingsHelper* mSettingsHelper = nullptr;
XHelper* mXHelper = nullptr;

XVisualInfo mVisualInfoStruct { };
Colormap mColorMap { };

StickyWindow* mStickyWindow = nullptr;

QImage mPinInQImage { };
XImage* mPinInXImage = nullptr;

QImage mPinOutQImage { };
XImage* mPinOutXImage = nullptr;

ConfigDialog* mConfigDialog = nullptr;

/**
 * Main.
 */
int
main(int argc, char** argv) {
    // Init Display global.
    mDisplayHelper = new DisplayHelper();
    mDisplay = mDisplayHelper->getDisplay();
    if (mDisplay == nullptr) {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: "
            "X11 Windows are unavailable with this Desktop - FATAL." <<
            XCOLOR_NORMAL << endl;
        return true;
    }

    // Set X Error handler (quiets non-errors).
    mXHelper = new XHelper();
    XSetErrorHandler(mXHelper->handleX11ErrorEvent);

    // Enhance Qt6 bash splooge block starts.
    if (mXHelper->getCompositorName().isEmpty()) {
        cout << endl << XCOLOR_YELLOW << "StickyWidgetIII: "
            "Initializing QT6 with no compositor ... warnings start." <<
            XCOLOR_NORMAL << endl;
    }

    // Some Qt6 setup.
    QApplication app(argc, argv);

    // Enhance Qt6 bash splooge block finishes.
    if (mXHelper->getCompositorName().isEmpty()) {
        cout << XCOLOR_YELLOW << "StickyWidgetIII: "
            "Initializing QT6 with no compositor ... warnings end." <<
            XCOLOR_NORMAL << endl << endl;
    }

    // Set app icon.
    app.setWindowIcon(QIcon(ICON_PATH +
        QString(APP_NAME) + ".png"));
    if (getAppInstanceCount() > 1) {
        QMessageBox::information(NULL, APP_NAME, "StickyClock is "
            "already running & only allows for one to run @ a time.");
        cout << endl << XCOLOR_RED << "StickyClock is already "
            "running & only allows for one to run @ a time." << endl;
        return true;
    }

    // Init Font global, App & Png helpers.
    mFont = XftFontOpenName(mDisplay,
        DefaultScreen(mDisplay), TIME_DISPLAY_FONT);
    initAppHelpers();
    initAppPngImages();

    // Init visual transparency & TrueColor 32.
    const bool IS_VISUALLY_TRANSPARENT =
        initVisualTransparency();

    StickyWindow* mStickyWindow = new StickyWindow();
    if (mStickyWindow->getX11Window() != None) {
        mStickyWindow->run();
        delete mStickyWindow;
    }

    // Uninit all.
    if (IS_VISUALLY_TRANSPARENT) {
        uninitVisualTransparency();
    }

    // Uninit Font global, App & Png helpers.
    unintPngImages();
    delete mXHelper;
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
void
initAppHelpers() {
    // Init Settings helper global.
    mSettingsHelper = new SettingsHelper(APP_NAME);

    // Commit all QSettings defaults for ConfigDialog support.
    mSettingsHelper->ensureSettingsAreConfigurable();
}

/**
 * Initialize all Png Button Images.
 */
bool
initAppPngImages() {
    // Load the Pin-In image.
    const QString PIN_IN_FILE = ICON_PATH + QString(APP_NAME) +
        QString("-") + PIN_IN_PNG_FILENAME;
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
    const QString PIN_OUT_FILE = ICON_PATH + QString(APP_NAME) +
        QString("-") + PIN_OUT_PNG_FILENAME;
    if (!mPinOutQImage.load(PIN_OUT_FILE)) {
        cout << endl << XCOLOR_RED << "StickyWidgetIII: A "
            "required resource image can't be found - FATAL." <<
            XCOLOR_NORMAL << endl;
        cout << endl << XCOLOR_YELLOW << "StickyWidgetIII: Missing: " <<
            PIN_OUT_FILE.toStdString() << "." << XCOLOR_NORMAL << endl;
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
 * Initialize Transparency & TrueColor 32.
 */
bool
initVisualTransparency() {
    // Ensure we have a compositor running.
    if (!mXHelper->isACompositorRunning()) {
        return false;
    }

    const int VISUAL_COLOR_DEPTH = 32;
    if (XMatchVisualInfo(mDisplay, DefaultScreen(mDisplay),
        VISUAL_COLOR_DEPTH, TrueColor, &mVisualInfoStruct)) {
        mColorMap = XCreateColormap(mDisplay, RootWindow(mDisplay,
            DefaultScreen(mDisplay)), mVisualInfoStruct.visual,
                AllocNone);
        return true;
    }

    return false;
}

/**
 * Uninitialize Transparency & TrueColor 32.
 */
void
uninitVisualTransparency() {
    XFreeColormap(mDisplay, mColorMap);
}

/**
 * Uninitialize all Png Button Images.
 */
void
unintPngImages() {
    // Intentional blank.
}

/**
 * Uninitialize all, allow sanitizer perfection 💫.
 */
void
uninitAppHelpers() {
    delete mSettingsHelper;
}

/**
 * Helper method to determine the apps running instance
 * count. Currently limited to 1.
 *
 * TODO: Use this for Recents Context.
 */
int
getAppInstanceCount() {
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
