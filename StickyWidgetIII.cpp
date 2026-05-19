
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
void setAppSignalCatcher();
void appSignalCatcher(int signal);

void initAppHelpers();
bool initWindowCompositor();
bool initAppPngImages();

void unintPngImages();
void uninitWindowCompositor();
void uninitAppHelpers();

int getAppInstanceCount();
void appSignalCatcher(int signal);
void signalIntToSTDOUT(int n);


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

ConfigDialog* mConfigDialog = nullptr;

/**
 * Main.
 */
int main(int argc, char** argv) {
    // Some Qt6 setup.
    QApplication app(argc, argv);
    // setAppSignalCatcher();

    app.setWindowIcon(QIcon(ICON_PATH +
        QString(APP_NAME) + ".png"));
    if (getAppInstanceCount() > 1) {
        cout << endl << XCOLOR_RED << "StickyClock() is already running "
            "& only allows for one to run @ a time." << endl;
        return true;
    }

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
    unintPngImages();
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
 * Initialize App signal catchers.
 */
void setAppSignalCatcher() {
    for (int eachSignal = 0; eachSignal <= SIGSYS/*31*/; eachSignal++) {
        // Can't catch these two.
        if (eachSignal == SIGKILL || eachSignal == SIGSTOP) {
            continue;
        }
        signal(eachSignal, appSignalCatcher);
    }
}

/**
 * Initialize App global helpers.
 */
void initAppHelpers() {
    // Init Settings helper global.
    mSettingsHelper = new SettingsHelper(APP_NAME);

    // Commit all QSettings defaults for ConfigDialog support.
    mSettingsHelper->ensureSettingsAreConfigurable();

    // Set X Error handler (quiets non-errors).
    mXHelper = new XHelper();
    XSetErrorHandler(mXHelper->handleX11ErrorEvent);
}

/**
 * Initialize all Png Button Images.
 */
bool initAppPngImages() {
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
 * Initialize window compositor.
 */
void uninitWindowCompositor() {
    XFreeColormap(mDisplay, mColormap);
}

/**
 * Uninitialize all Png Button Images.
 */
void unintPngImages() {
    // Intentional blank.
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

/**
 * This method runs in async-signal-safe mode & logs
 * signal event shutdowns called from the OS.
 */
void appSignalCatcher(int signal) {
    // Print message Start, stack-allocated string buffer.
    char part1[] = "\nStickyWidgetIII: Signal Handler Shutdown by : [ ";
    { [[maybe_unused]] ssize_t writeResult =
        write(STDERR_FILENO, part1, sizeof(part1)); }

    // Print message signalNumber.
    signalIntToSTDOUT(signal);

    // Print message End, stack-allocated string buffer.
    char part2[] = " ].\n";
    { [[maybe_unused]] ssize_t writeResult =
        write(STDERR_FILENO, part2, sizeof(part2)); }
}

/**
 * Async-signal-safe conversion of integer to string.
 */
void signalIntToSTDOUT(int n) {
    // Early out if easy answer.
    if (n == 0) {
        { [[maybe_unused]] ssize_t writeResult =
            write(STDERR_FILENO, "0", sizeof("0")); }
        return;
    }

    // Note & remove sign from numbers.
    bool isNegative = false;
    if (n < 0) {
        isNegative = true;
        n = -n;
    }

    // Init enough for -2147483648, & copy numbers.
    char temp[12]; int i = 0;
    while (n > 0) {
        char c[1];
        c[0] = (n % 10) + '0';
        { [[maybe_unused]] ssize_t writeResult =
            write(STDERR_FILENO, c, sizeof(c)); }
        n /= 10;
    }

    // Add minus & endl;
    if (isNegative) {
        { [[maybe_unused]] ssize_t writeResult =
            write(STDERR_FILENO, "-", sizeof("-")); }
    }
}
