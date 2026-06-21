
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
    // All button white with indented blue outline.
    XRenderPictureAttributes PIC_ATTR{};
    PIC_ATTR.poly_edge = PolyEdgeSmooth;

    Picture canvasPic = XRenderCreatePicture(mDisplay,
        window, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getWidth());
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BLUE_RCOLOR, getX() + 1, getY() + 1,
        getWidth() - 2, getHeight() - 2);

    // White or grayed button center.
    const XRenderColor BUTTON_COLOR = isPressed() ?
        GRAY_RCOLOR : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BUTTON_COLOR, getX() + 3 , getY() + 3,
        getWidth() - 6, getHeight() - 6);

    // Draw '■' for Config glyph over center.
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BLUE_RCOLOR, getX() + 9, getY() + 9, 6, 6);

    // Cleanup.
    XRenderFreePicture(mDisplay, canvasPic);
}

/**
 * Erase the Button.
 */
void
ConfigButton::erase(const Picture renderPicture) {
    XRenderFillRectangle(mDisplay, PictOpSrc, renderPicture,
        &TRANSPARENT_RCOLOR, getX(), getY(), getWidth(), getHeight());
}

/**
 * Clicks the ConfigButton.
 */
// Suppress warnings based on Clang or GCC.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void
ConfigButton::click(const Window window) {
    // Create Dialog jit.
    if (!mConfigDialog) {
        mConfigDialog = new ConfigDialog(NULL);
    }

    // Maybe close the dialog.
    if (mConfigDialog->isVisible()) {
        mConfigDialog->close();
        return;
    }

    // Build the UI form layout & open it.
    mConfigDialog->setWindow(window);
    mConfigDialog->loadConfigDialogControls();

    mConfigDialog->show();
    mConfigDialog->raise();
    mConfigDialog->activateWindow();
}

#pragma GCC diagnostic pop

/**
 * Updates any Button Dialog.
 */
void
ConfigButton::updateDialog() {
    // Create Dialog jit.
    if (!mConfigDialog || !mConfigDialog->isVisible()) {
        return;
    }

    mConfigDialog->updateConfigDialogControls();
}
