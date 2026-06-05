
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

    const Picture RENDER_PICTURE = XRenderCreatePicture(mDisplay,
        mWindow, RENDER_FORMAT, 0, nullptr);

    // Draw canvas border gray, indented margin blue,
    // interior white or tourquiose for weedClocktime.
    const double X_POS = mSettingsHelper->getCanvasXPos();
    const double Y_POS = mSettingsHelper->getCanvasYPos();
    const double WIDTH = mSettingsHelper->getCanvasWidth();
    const double HEIGHT = mSettingsHelper->getCanvasHeight();
    XRenderFillRectangle(mDisplay, PictOpOver, RENDER_PICTURE,
        &GRAY_RCOLOR, X_POS, Y_POS, WIDTH, HEIGHT);
    XRenderFillRectangle(mDisplay, PictOpOver, RENDER_PICTURE,
        &BLUE_RCOLOR, X_POS + 2, Y_POS + 2, WIDTH - 4, HEIGHT - 4);

    Pixmap renderPixmap = XCreatePixmap(mDisplay,
        mWindow, 1, 1, 32);

    const Picture BACKGROUND_R_PICTURE = XRenderCreatePicture(
        mDisplay, renderPixmap, RENDER_FORMAT, 0, nullptr);
    XRenderFillRectangle(mDisplay, PictOpSrc,
        BACKGROUND_R_PICTURE, &WHITE_RCOLOR, 0, 0, 1, 1);

    XRenderColor backgroundColor = (isItWeedTime() && mSettingsHelper->
        getBoolSetting(SettingsHelper::SHOW_WEED_CLOCK)) ?
            mSettingsHelper->getColorSetting(
                SettingsHelper::WEED_CLOCK_COLOR) : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, RENDER_PICTURE,
        &backgroundColor, X_POS + 4, Y_POS + 4, WIDTH - 8, HEIGHT - 8);
    XRenderFreePicture(mDisplay, BACKGROUND_R_PICTURE);

    // Draw Widget Text.
    const QString TEXT = QString::fromStdString(getCurrentTime());
    const string TEXT_STD_STRING = TEXT.toStdString();

    const int TEXT_WIDTH = mXHelper->getStringPixelWidth(TEXT);
    const int TEXT_HEIGHT = mXHelper->getStringPixelHeight(TEXT);

    const int TEXT_X = X_POS + (WIDTH - TEXT_WIDTH) / 2;
    const int TEXT_Y = Y_POS + (HEIGHT - TEXT_HEIGHT) / 2 + TEXT_HEIGHT;

    XftDraw* xft_draw = XftDrawCreate(
        mDisplay, mWindow, mVisual, mColormap);

    const char* TEXT_CHARS = TEXT_STD_STRING.c_str();
    XftDrawStringUtf8(xft_draw, &BLACK_FCOLOR, mFont,
        TEXT_X, TEXT_Y, (FcChar8*) TEXT_CHARS, strlen(TEXT_CHARS));

    XftDrawDestroy(xft_draw);
    xft_draw = nullptr;
    XFreePixmap(mDisplay, renderPixmap);
    XRenderFreePicture(mDisplay, RENDER_PICTURE);

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
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

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
