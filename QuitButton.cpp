
#include "Global.h"

/**
 * Simple class to represent a QuitButton.
 *
 */

/**
 * Draws the QuitButton.
 */
void
QuitButton::draw(const Window window) {
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

    // Draw 'X' for Close glyph over center.
    const int PIXEL_SIZE = 2;
    for (int i = 6; i <= getWidth() - 6 - PIXEL_SIZE; i++) {
        XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
            &BLUE_RCOLOR, getX() + i, getY() + i,
            PIXEL_SIZE, PIXEL_SIZE);
        XRenderFillRectangle(mDisplay, PictOpOver, CANVAS,
            &BLUE_RCOLOR,
            getX() + i,
            getY() + getHeight() - i - PIXEL_SIZE,
            PIXEL_SIZE, PIXEL_SIZE);
    }

    // Cleanup.
    XRenderFreePicture(mDisplay, CANVAS);
}

/**
 * Clicks the QuitButton.
 */
void
QuitButton::click(const Window window) {
    const Atom WM_PROTOCOLS = XInternAtom(mDisplay,
        "WM_PROTOCOLS", False);
    const Atom WM_DELETE_WINDOW = XInternAtom(mDisplay,
        "WM_DELETE_WINDOW", False);

    XClientMessageEvent CLOSE_EVENT = { 
        .type = ClientMessage, .window = window,
        .message_type = WM_PROTOCOLS, .format = 32,
        .data = { .l = { (long) WM_DELETE_WINDOW, CurrentTime } }
    };

    XSendEvent(mDisplay, window, False, NoEventMask,
        (XEvent*) &CLOSE_EVENT);
    XFlush(mDisplay);
}
