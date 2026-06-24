
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
int main(int argc, char** argv);

bool areWeUsingQtPlatformTheming();

bool initAppPngImages();

void sanitizeGlobals();


// App globals externs.
DisplayHelper* mDisplayHelper = nullptr;
Display* mDisplay = nullptr;
XHelper* mXHelper = nullptr;

XftFont* mFont = nullptr;
int mFontSize = -1;
RecentsHelper* mRecentsHelper = nullptr;
SettingsHelper* mSettingsHelper = nullptr;

StickyWindow* mStickyWindow = nullptr;

XImage* mPinInXImage = nullptr;
XImage* mPinOutXImage = nullptr;

// App globals;
QImage mPinInQImage{};
QImage mPinOutQImage{};


/**
 * Main.
 */
int
main(int argc, char** argv) {
    // Init Display global.
    XInitThreads();

    mDisplayHelper = new DisplayHelper();
    mDisplay = mDisplayHelper->getDisplay();

    // Set X Error handler (quiets non-errors).
    mXHelper = new XHelper();
    XSetErrorHandler(mXHelper->handleX11ErrorEvent);

    // QApplication app(argc, argv) issues warnings in stdout
    // if platform is using qt themeing - wrap before.
    if (areWeUsingQtPlatformTheming()) {
        cout << endl << XCOLOR_YELLOW << "Sees you using Qt Platform "
            "theming ... warnings start." << XCOLOR_NORMAL << endl;
    }

    // Qt6 Application setup.
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(ICON_PATH +
        QString(APP_NAME) + ".png"));

    // QApplication app(argc, argv) issues warnings in stdout
    // if platform is using qt themeing - wrap after.
    if (areWeUsingQtPlatformTheming()) {
        cout << XCOLOR_YELLOW << "Sees you using Qt Platform theming "
            "... warnings end." << XCOLOR_NORMAL << endl << endl;
    }

    // If no X Display, Qt can still display a gui error.
    if (!mDisplay) {
        QMessageBox::information(NULL, APP_NAME, "X11 Windows are "
            "unavailable with this Desktop, FATAL.");
        cout << endl << XCOLOR_RED << "X11 Windows are unavailable "
            "with this Desktop, FATAL." << XCOLOR_NORMAL << endl;
        return true;
    }

    // Init Recents helper.
    mRecentsHelper = new RecentsHelper();
    if (mRecentsHelper->getAppRecentsName().isEmpty()) {
        sanitizeGlobals();
        return true;
    }

    // Init Font global, Settings & App & Png Helpers.
    mSettingsHelper = new SettingsHelper();
    mSettingsHelper->ensureSettingsAreConfigurable();

    // Init Fonts.
    mFontSize = mSettingsHelper->getIntSetting(
        SettingsHelper::TEXT_SIZE);
    const string FONT_SIZE = to_string(mFontSize);
    const string TIME_DISPLAY_FONT = "Serif-" + FONT_SIZE + ":bold";
    mFont = XftFontOpenName(mDisplay, DefaultScreen(mDisplay),
        TIME_DISPLAY_FONT.c_str());

    // Init PinButton images.
    initAppPngImages();

    // Open the window & run.
    mStickyWindow = new StickyWindow();
    if (mStickyWindow->getX11Window() != None) {
        mStickyWindow->run();
    }

    // Cleanup & done.
    delete mStickyWindow;
    delete mSettingsHelper;
    delete mRecentsHelper;

    sanitizeGlobals();
    return false;
}

/**
 * Check for platforms use of Qt platformTheming.
 */
bool
areWeUsingQtPlatformTheming() {
    const char* PLATFORMTHEME = getenv("QT_QPA_PLATFORMTHEME");
    if (PLATFORMTHEME && (strcmp(PLATFORMTHEME, "qt5ct") == 0 ||
        strcmp(PLATFORMTHEME, "qt6ct") == 0)) {
        return true;
    }

    return false;
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
        cout << endl << XCOLOR_RED << "A required resource image "
            "can't be found - FATAL." << XCOLOR_NORMAL << endl;
        cout << endl << XCOLOR_YELLOW << "Missing: " <<
            PIN_IN_FILE.toStdString() << "." << XCOLOR_NORMAL << endl;
        return false;
    }

    mPinInQImage = mPinInQImage.convertToFormat(QImage::Format_RGB32);
    mPinInXImage = XCreateImage(mDisplay,
        DefaultVisual(mDisplay, DefaultScreen(mDisplay)),
        32, ZPixmap, 0, (char*) mPinInQImage.bits(),
        mPinInQImage.width(), mPinInQImage.height(), 32, 0);

    // Load the Pin-Out image.
    const QString PIN_OUT_FILE = ICON_PATH + QString(APP_NAME) +
        QString("-") + PIN_OUT_PNG_FILENAME;
    if (!mPinOutQImage.load(PIN_OUT_FILE)) {
        cout << endl << XCOLOR_RED << "A required resource image "
            "can't be found - FATAL." << XCOLOR_NORMAL << endl;
        cout << endl << XCOLOR_YELLOW << "Missing: " <<
            PIN_OUT_FILE.toStdString() << "." << XCOLOR_NORMAL << endl;
        return false;
    }

    mPinOutQImage = mPinOutQImage.convertToFormat(QImage::Format_RGB32);
    mPinOutXImage = XCreateImage(mDisplay,
        DefaultVisual(mDisplay, DefaultScreen(mDisplay)),
        32, ZPixmap, 0, (char*) mPinOutQImage.bits(),
        mPinOutQImage.width(), mPinOutQImage.height(), 32, 0);

    return true;
}

/**
 * This method sanitizes global members after use.
 */
void sanitizeGlobals() {
    XftFontClose(mDisplay, mFont);
    mFont = nullptr;
    FcFini();

    delete mXHelper;

    XCloseDisplay(mDisplay);
    delete mDisplayHelper;
}
