
#include "Global.h"

/**
 * Simple class to represent a ConfigButton.
 *
 */

/**
 * Draws the ConfigButton.
 */
void
ConfigButton::draw(const Window window) {
    const XRenderPictureAttributes PIC_ATTR = {
        .poly_edge = PolyEdgeSmooth };
    const Picture CANVAS = XRenderCreatePicture(mDisplay,
        window, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

    // All button white with indented blue outline.
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getWidth());
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &BLUE_RCOLOR, getX() + 1, getY() + 1,
        getWidth() - 2, getHeight() - 2);

    // White or grayed button center.
    const XRenderColor BUTTON_COLOR =
        isPressed() ? GRAY_RCOLOR : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &BUTTON_COLOR, getX() + 3 , getY() + 3,
        getWidth() - 6, getHeight() - 6);

    // Draw '■' for Config glyph over center.
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &BLUE_RCOLOR, getX() + 10, getY() + 10, 4, 4);

    // Cleanup.
    XRenderFreePicture(mDisplay, CANVAS);
}

/**
 * Clicks the ConfigButton.
 */
void
ConfigButton::click(const Window window) {
    // Create Dialog jit.
    if (!mConfigDialog) {
        mConfigDialog = new ConfigDialog(
            mSettingsHelper->getQSettingsFile(), NULL);
    }

    // Open or close the dialog.
    if (!mConfigDialog->isVisible()) {
        mConfigDialog->loadSettings();
        mConfigDialog->show();
        mConfigDialog->raise();
        mConfigDialog->activateWindow();
    } else {
        mConfigDialog->close();
        //mConfigDialog = nullptr;
    }
}
