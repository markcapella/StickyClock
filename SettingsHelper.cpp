
#include "Global.h"

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 *
 */
SettingsHelper::SettingsHelper(const QString appName) {

    mSettingsApp = appName;
}

SettingsHelper::~SettingsHelper() {
    delete mQSettings;
}

/**
 * Getters for window minimum Width & Height.
 */
double
SettingsHelper::getWindowMinimumWidth() {
    return getQSettings()->value("minimumWindowWidth",
        Button::BUTTON_WIDTH * 9).toDouble();
}
double
SettingsHelper::getWindowMinimumHeight() {
    return getQSettings()->value("minimumWindowHeight",
        Button::BUTTON_WIDTH * 5).toDouble();
}

/**
 * Setters for window minimum Width & Height.
 */
void
SettingsHelper::setWindowMinimumWidth(const double width) {
    return getQSettings()->setValue("minimumWindowWidth", width);
}
void
SettingsHelper::setWindowMinimumHeight(const double height) {
    return getQSettings()->setValue("minimumWindowHeight", height);
}

/**
 * Getters for window workspace, x / y, width / height.
 */
long
SettingsHelper::getWindowWorkspace() {
    return getQSettings()->value("windowWorkspace", -1).toInt();
}
double
SettingsHelper::getWindowXPos() {
    return getQSettings()->value("windowXPos", -1).toDouble();
}
double
SettingsHelper::getWindowYPos() {
    return getQSettings()->value("windowYPos", -1).toDouble();
}
double
SettingsHelper::getWindowWidth() {
    return getQSettings()->value("windowWidth", -1).toDouble();
}
double
SettingsHelper::getWindowHeight() {
    return getQSettings()->value("windowHeight", -1).toDouble();
}

/**
 * Setters for window workspace, x / y, width / height.
 */
void
SettingsHelper::setWindowWorkspace(const long workspace) {
    getQSettings()->setValue("windowWorkspace", (int) workspace);
}
void
SettingsHelper::setWindowXPos(const double xPos) {
    if (xPos != getWindowXPos()) {
        getQSettings()->setValue("windowXPos", xPos);
    }
}
void
SettingsHelper::setWindowYPos(const double yPos) {
    if (yPos != getWindowYPos()) {
        getQSettings()->setValue("windowYPos", yPos);
    }
}
void
SettingsHelper::setWindowWidth(const double width) {
    if (width != getWindowWidth()) {
        getQSettings()->setValue("windowWidth", width);
    }
}
void
SettingsHelper::setWindowHeight(const double height) {
    if (height != getWindowHeight()) {
        getQSettings()->setValue("windowHeight", height);
    }
}

/**
 * Getters & setters of window config mode.
 */
bool
SettingsHelper::getConfigMode() {
    return getQSettings()->value("inConfigMode", true).toBool();
}
void
SettingsHelper::setConfigMode(const bool state) {
    getQSettings()->setValue("inConfigMode", state);

    getQSettings()->sync();
}

/**
 * Getters for canvas x & y, w & h.
 */
double
SettingsHelper::getCanvasXPos() {
    return getQSettings()->value("canvasXPos", -1).toDouble();
}
double
SettingsHelper::getCanvasYPos() {
    return getQSettings()->value("canvasYPos", -1).toDouble();
}
double
SettingsHelper::getCanvasWidth() {
    return getQSettings()->value("canvasWidth", 200).toDouble();
}
double
SettingsHelper::getCanvasHeight() {
    return getQSettings()->value("canvasHeight", 75).toDouble();
}

/**
 * Setters for canvas x & y, w & h.
 */
void
SettingsHelper::setCanvasXPos(const double xPos) {
    if (xPos != getCanvasXPos()) {
        getQSettings()->setValue("canvasXPos", xPos);
    }
}
void
SettingsHelper::setCanvasYPos(const double yPos) {
    if (yPos != getCanvasYPos()) {
        getQSettings()->setValue("canvasYPos", yPos);
    }
}
void
SettingsHelper::setCanvasWidth(const double width) {
    if (width != getCanvasWidth()) {
        getQSettings()->setValue("canvasWidth", width);
    }
}
void
SettingsHelper::setCanvasHeight(const double height) {
    if (height != getCanvasHeight()) {
        getQSettings()->setValue("canvasHeight", height);
    }
}

/**
 * Helper to return a new QSettings object for pref
 * access based on our appName.
 */
QSettings*
SettingsHelper::getQSettings() {
    if (!mQSettings) {
        mQSettings = new QSettings(getQSettingsFile(),
            QSettings::IniFormat);
    }
    return mQSettings;
}

/**
 * Helper to return a QSettings filename from appName.
 */
QString
SettingsHelper::getQSettingsFile() {
    return getenv("HOME") + QStringLiteral("/.local/") +
        mSettingsApp + ".ini";
}
