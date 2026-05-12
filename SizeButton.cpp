
#include "Global.h"

/**
 * Draws the SizeButton.
 *
 */
void
SizeButton::draw(const Window window) {
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

    // Draw 'down-right-arrow' for resize glyph over center.
    for (int i = 5; i <= getWidth() - 5; i++) {
        XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
            &BLUE_RCOLOR,
            getX() + i,
            getY() + getHeight() - i + 1,
            1, i - 5);
    }

    // Cleanup.
    XRenderFreePicture(mDisplay, CANVAS);
}

void
SizeButton::click(const Window window) const {
}
