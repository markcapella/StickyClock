
#include "Global.h"

/**
 * Simple class to represent a MoveButton.
 *
 */

/**
 * Draws the MoveButton.
 */
void
MoveButton::draw(const Window window) {
    const XRenderPictureAttributes PIC_ATTR = {
        .poly_edge = PolyEdgeSmooth };
    const Picture CANVAS = XRenderCreatePicture(mDisplay,
        window, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

    // All button white with indented blue outline.
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getHeight());
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &BLUE_RCOLOR, getX() + 1, getY() + 1,
        getWidth() - 2, getHeight() - 2);

    // White or grayed button center.
    const XRenderColor BUTTON_COLOR =
        isPressed() ? GRAY_RCOLOR : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
        &BUTTON_COLOR, getX() + 3 , getY() + 3,
        getWidth() - 6, getHeight() - 6);

    // Draw '=' for Move glyph over center.
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS, &BLUE_RCOLOR,
        getX() + 5, getY() + 9, getWidth() - 10, 2);
    XRenderFillRectangle(mDisplay, PictOpOver, CANVAS, &BLUE_RCOLOR,
        getX() + 5, getY() + 13, getWidth() - 10, 2);

    // Cleanup.
    XRenderFreePicture(mDisplay, CANVAS);
}

/**
 * Erase the Button.
 */
void
MoveButton::erase(const Window window, const Picture renderPicture) {
    XRenderFillRectangle(mDisplay, PictOpSrc, renderPicture,
        &TRANSPARENT_RCOLOR, getX(), getY(), getWidth(), getHeight());
}

/**
 * Clicks the MoveButton.
 */
void
MoveButton::click(const Window window) {
    // Nothing.
}

/**
 * Updates any Button Dialog.
 */
void
MoveButton::updateDialog() {
    // Nothing.
}
