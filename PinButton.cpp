
#include "Global.h"

/**
 * Simple class to represent a PinButton.
 *
 */
PinButton::PinButton(double mXPos, double mYPos) :
    Button(mXPos, mYPos) { }

// Provides dynamic_cast<PinButton*> type cast ability.
PinButton::~PinButton() { }

/**
 * Draws the PinButton.
 */
void
PinButton::draw(const Window window) {
    const XRenderPictureAttributes PIC_ATTR = {
        .poly_edge = PolyEdgeSmooth };
    const Picture CANVAS = XRenderCreatePicture(mDisplay, window,
        XRenderFindStandardFormat(mDisplay, PictStandardARGB32),
        CPPolyEdge, &PIC_ATTR);

    // All button white.
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getHeight());

    // Display pinIn or pinOut png.
    GC gc = XCreateGC(mDisplay, window, 0, nullptr);
    XPutImage(mDisplay, window, gc, (mSettingsHelper->getBoolSetting(
        SettingsHelper::CONFIG_MODE) ?
        mPinOutXImage : mPinInXImage), 0, 0, getX() + 2, getY() + 2,
        getWidth() - 4, getHeight() - 4);
    XFreeGC(mDisplay, gc);

    // Cleanup.
    XRenderFreePicture(mDisplay, CANVAS);
}

/**
 * Erase the Button.
 */
void
PinButton::erase(const Window window, const Picture renderPicture) {
    // Nothing.
}

/**
 * Clicks the PinButton.
 */
void
PinButton::click(const Window window) {
    // Nothing.
}

/**
 * Updates any Button Dialog.
 */
void
PinButton::updateDialog() {
    // Nothing.
}
