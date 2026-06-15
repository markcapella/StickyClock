
#include "Global.h"

/**
 * Canvas is the main widget & draw.
 */
Canvas::Canvas(const Window window, Visual* visual,
    const Colormap colormap) {

    mWindow = window;
    mVisual = visual;
    mColormap = colormap;
}

/**
 * StickyClock widget drawCanvas method.
 */
void
Canvas::drawCanvas() {
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(mDisplay, mVisual);

    // Draw canvas border gray, indented margin blue,
    // interior white or tourquiose for weedClocktime.
    const double X_POS = mSettingsHelper->getCanvasXPos();
    const double Y_POS = mSettingsHelper->getCanvasYPos();
    const double WIDTH = mSettingsHelper->getCanvasWidth();
    const double HEIGHT = mSettingsHelper->getCanvasHeight();

    Picture renderPic = XRenderCreatePicture(mDisplay,
        mWindow, RENDER_FORMAT, 0, nullptr);

    // Get blended Canvas panel color.
    const XRenderColor PANEL_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::PANEL_COLOR);
    const int PANEL_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::PANEL_OPACITY);
    const XRenderColor BLENDED_PANEL_COLOR = newRenderColor(
        PANEL_COLOR.red, PANEL_COLOR.green,
        PANEL_COLOR.blue, PANEL_OPACITY);

    // Get blended Canvas panel color for weedclock.
    const XRenderColor WEED_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::WEED_CLOCK_COLOR);
    const int WEED_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::WEED_CLOCK_OPACITY);
    const XRenderColor BLENDED_WEED_COLOR = newRenderColor(
        WEED_COLOR.red, WEED_COLOR.green,
        WEED_COLOR.blue, WEED_OPACITY);

    // Get blended Canvas background color we're using.
    const bool SHOW_WEED_CLOCK = mSettingsHelper->getBoolSetting(
        SettingsHelper::SHOW_WEED_CLOCK);
    const XRenderColor BLENDED_BACKGROUND_COLOR =
        SHOW_WEED_CLOCK && isItWeedTime() ?
            BLENDED_WEED_COLOR : BLENDED_PANEL_COLOR;

    // Get blended Canvas panel outline color.
    const XRenderColor PANEL_OUTLINE_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::PANEL_OUTLINE_COLOR);
    const int PANEL_OUTLINE_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::PANEL_OUTLINE_OPACITY);
    const XRenderColor BLENDED_PANEL_OUTLINE_COLOR = newRenderColor(
        PANEL_OUTLINE_COLOR.red, PANEL_OUTLINE_COLOR.green,
        PANEL_OUTLINE_COLOR.blue, PANEL_OUTLINE_OPACITY);

    // Draw the Canvas and the colored outline.
    XRenderFillRectangle(mDisplay, PictOpSrc, renderPic,
        &BLENDED_BACKGROUND_COLOR, X_POS, Y_POS, WIDTH, HEIGHT);
    XRenderFillRectangle(mDisplay, PictOpSrc, renderPic,
        &BLENDED_PANEL_OUTLINE_COLOR,
            X_POS + 2, Y_POS + 2, WIDTH - 4, HEIGHT - 4);
    XRenderFillRectangle(mDisplay, PictOpSrc, renderPic,
        &BLENDED_BACKGROUND_COLOR,
            X_POS + 4, Y_POS + 4, WIDTH - 8, HEIGHT - 8);

    // Get blended Canvas text color.
    XftColor xftTextColor;
    const XRenderColor TEXT_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::TEXT_COLOR);
    const int TEXT_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::TEXT_OPACITY);
    const XRenderColor BLENDED_TEXT_COLOR = newRenderColor(
        TEXT_COLOR.red, TEXT_COLOR.green,
        TEXT_COLOR.blue, TEXT_OPACITY);

    // Get text string & geometry.
    const QString TEXT = QString::fromStdString(getCurrentTime());
    const string TEXT_STD_STRING = TEXT.toStdString();
    const char* TEXT_CHARS = TEXT_STD_STRING.c_str();

    const int TEXT_WIDTH = mXHelper->getStringPixelWidth(TEXT);
    const int TEXT_HEIGHT = mXHelper->getStringPixelHeight(TEXT);

    const int TEXT_X = X_POS + (WIDTH - TEXT_WIDTH) / 2;
    const int TEXT_Y = Y_POS + (HEIGHT - TEXT_HEIGHT) / 2 + TEXT_HEIGHT;

    // Draw text.
    XftColorAllocValue(mDisplay, mVisual, mColormap,
        &BLENDED_TEXT_COLOR, &xftTextColor);
    XftDraw* xftDraw = XftDrawCreate(mDisplay, mWindow,
        mVisual, mColormap);
    XftDrawStringUtf8(xftDraw, &xftTextColor, mFont,
        TEXT_X, TEXT_Y, (FcChar8*) TEXT_CHARS,
        strlen(TEXT_CHARS));
    XftDrawDestroy(xftDraw);

    // End transparent rendering.
    XRenderFreePicture(mDisplay, renderPic);

    XFlush(mDisplay);
}

/**
 * This method updates the windowRect once a second.
 */
bool
Canvas::updateCanvas() {
    const string CURRENT_SECOND = getCurrentSecond();

    if (mPreviousClientUpdateSecond == "" ||
        mPreviousClientUpdateSecond != CURRENT_SECOND) {
        mPreviousClientUpdateSecond = CURRENT_SECOND;
        return true;
    }

    return false;
}

/**
 * Getter for current time.
 */
string
Canvas::getCurrentTime() {
    return mXHelper->addLeadZeroToNN(getCurrentHour()) + ":" +
        mXHelper->addLeadZeroToNN(getCurrentMinute());
}

/**
 * Getter for current hour.
 */
string
Canvas::getCurrentHour() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    const string HOUR = to_string(now->tm_hour);
    return mXHelper->addLeadZeroToNN(HOUR);
}

/**
 * Getter for current minute.
 */
string
Canvas::getCurrentMinute() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    const string MINUTE = to_string(now->tm_min);
    return mXHelper->addLeadZeroToNN(MINUTE);
}

/**
 * Getter for current second.
 */
string
Canvas::getCurrentSecond() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    const string SECOND = to_string(now->tm_sec);
    return mXHelper->addLeadZeroToNN(SECOND);
}

/**
 * Determine if it's WeedClock time.
 */
bool
Canvas::isItWeedTime() {
    return getCurrentMinute() == "55" || getCurrentMinute() == "08" ||
        getCurrentMinute() == "38" || getCurrentMinute() == "39" ||
        getCurrentMinute() == "40" || getCurrentMinute() == "41" ||
        getCurrentMinute() == "42" || getCurrentMinute() == "43" ||
        getCurrentMinute() == "44" || getCurrentMinute() == "45" ||
        getCurrentMinute() == "46" || getCurrentMinute() == "20";
}
