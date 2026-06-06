
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
    // All button white with indented blue outline.
    XRenderPictureAttributes PIC_ATTR{};
    PIC_ATTR.poly_edge = PolyEdgeSmooth;

    Picture canvasPic = XRenderCreatePicture(mDisplay,
        window, XRenderFindStandardFormat(mDisplay,
        PictStandardARGB32), CPPolyEdge, &PIC_ATTR);

    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &WHITE_RCOLOR, getX(), getY(), getWidth(), getHeight());
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BLUE_RCOLOR, getX() + 1, getY() + 1,
        getWidth() - 2, getHeight() - 2);

    // White or grayed button center.
    const XRenderColor BUTTON_COLOR = isPressed() ?
        GRAY_RCOLOR : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BUTTON_COLOR, getX() + 3 , getY() + 3,
        getWidth() - 6, getHeight() - 6);

    // Draw '=' for Move glyph over center.
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BLUE_RCOLOR, getX() + 5, getY() + 9, getWidth() - 10, 2);
    XRenderFillRectangle(mDisplay, PictOpOver, canvasPic,
        &BLUE_RCOLOR, getX() + 5, getY() + 13, getWidth() - 10, 2);

    // Cleanup.
    XRenderFreePicture(mDisplay, canvasPic);
}

/**
 * Erase the Button.
 */
void
MoveButton::erase(const Picture renderPicture) {
    XRenderFillRectangle(mDisplay, PictOpSrc, renderPicture,
        &TRANSPARENT_RCOLOR, getX(), getY(), getWidth(), getHeight());
}

/**
 * Clicks the MoveButton.
 */
// Suppress warnings based on Clang or GCC.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void
MoveButton::click(const Window window) {
    // Nothing.
}

#pragma GCC diagnostic pop

/**
 * Updates any Button Dialog.
 */
void
MoveButton::updateDialog() {
    // Nothing.
}
