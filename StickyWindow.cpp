
#include "Global.h"

/**
 * StickyWindow class wraps an x11 Window object.
 *
 */
StickyWindow::StickyWindow() {
    mDeleteProtocols = XInternAtom(mDisplay, "WM_PROTOCOLS", False);
    mDeleteMessage = XInternAtom(mDisplay, "WM_DELETE_WINDOW", False);

    setX11Window(createX11Window());

    createWindowButtons();
}

StickyWindow::~StickyWindow() {
    const int BUTTONS_SIZE = mButtons.size();
    for (int i = 0; i < BUTTONS_SIZE; i++) {
        delete mButtons[i];
    }
    mButtons.clear();

    if (mX11Window != None) {
        XUnmapWindow(mDisplay, mX11Window);
        XDestroyWindow(mDisplay, mX11Window);
        mX11Window = None;
    }
}

/**
 * Overriden show() method ensures we stay on bottom
 * when window is "stuck", window border state != visible.
 */
void
StickyWindow::show() {
    XMapWindow(mDisplay, mX11Window);
}

/**
 * Overriden hide() method.
 */
void
StickyWindow::hide() {
    XUnmapWindow(mDisplay, mX11Window);
}

/**
 * Overriden draw() method ensures we have a transparent
 * window with PinButton visible on mouse hover.
 */
void
StickyWindow::draw() {
    // Create a 1x1 pixmap to act as a solid color source.
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(mDisplay, mVisualInfoStruct.visual);
    if (!RENDER_FORMAT) {
        cout << endl << XCOLOR_YELLOW << "StickyWindow: Unable "
            "to draw() StickyClock lacking Visual Format." <<
            XCOLOR_NORMAL << endl;
        cout << "draw() Finishes - Error" << endl;
        return;
    }

    // Draw entire WINDOW transparent.
    Picture renderPicture = XRenderCreatePicture(
        mDisplay, mX11Window, RENDER_FORMAT, 0, nullptr);
    XRenderFillRectangle(mDisplay, PictOpSrc,
        renderPicture, &TRANSPARENT_RCOLOR, 0, 0,
        mSettingsHelper->getWindowWidth(),
        mSettingsHelper->getWindowHeight());

    // Draw canvas border gray, indented margin blue,
    // interior white or tourquiose for weedClocktime.
    const double X_POS = mSettingsHelper->getCanvasXPos();
    const double Y_POS = mSettingsHelper->getCanvasYPos();
    const double WIDTH = mSettingsHelper->getCanvasWidth();
    const double HEIGHT = mSettingsHelper->getCanvasHeight();
    XRenderFillRectangle(mDisplay, PictOpOver, renderPicture,
        &GRAY_RCOLOR, X_POS, Y_POS, WIDTH, HEIGHT);
    XRenderFillRectangle(mDisplay, PictOpOver, renderPicture,
        &BLUE_RCOLOR, X_POS + 2, Y_POS + 2, WIDTH - 4, HEIGHT - 4);

    Pixmap renderPixmap = XCreatePixmap(mDisplay, mX11Window, 1, 1, 32);
    const Picture BACKGROUND_R_PICTURE = XRenderCreatePicture(
        mDisplay, renderPixmap, RENDER_FORMAT, 0, nullptr);
    XRenderFillRectangle(mDisplay, PictOpSrc,
        BACKGROUND_R_PICTURE, &WHITE_RCOLOR, 0, 0, 1, 1);
    XRenderColor backgroundColor = (isItWeedTime() && mSettingsHelper->
        getBoolSetting(SettingsHelper::SHOW_WEED_CLOCK)) ?
            mSettingsHelper->getColorSetting(
                SettingsHelper::WEED_CLOCK_COLOR) : WHITE_RCOLOR;
    XRenderFillRectangle(mDisplay, PictOpOver, renderPicture,
        &backgroundColor, X_POS + 4, Y_POS + 4, WIDTH - 8, HEIGHT - 8);
    XRenderFreePicture(mDisplay, BACKGROUND_R_PICTURE);

    // Draw Widget Text.
    const QString TEXT = QString::fromStdString(getCurrentTime());
    //const QString TEXT = QString::number(getCursorPosition().x()) +
    //    QString(" - ") + QString::number(getCursorPosition().y());
    const int TEXT_WIDTH = mXHelper->getStringPixelWidth(TEXT);
    const int TEXT_HEIGHT = mXHelper->getStringPixelHeight(TEXT);
    const int TEXT_X = X_POS + (WIDTH - TEXT_WIDTH) / 2;
    const int TEXT_Y = Y_POS + (HEIGHT - TEXT_HEIGHT) / 2 +
        TEXT_HEIGHT;
    const string TEXT_STD_STRING = TEXT.toStdString();
    const char* TEXT_CHARS = TEXT_STD_STRING.c_str();

    XftDraw* xft_draw = XftDrawCreate(mDisplay, mX11Window,
        mVisualInfoStruct.visual, mColorMap);
    XftDrawStringUtf8(xft_draw, &BLACK_FCOLOR, mFont,
        TEXT_X, TEXT_Y, (FcChar8*) TEXT_CHARS,
        strlen(TEXT_CHARS));
    XftDrawDestroy(xft_draw);
    xft_draw = nullptr;

    // Define & draw all buttons.
    drawAllButtons();

    // Cleanup.
    XRenderFreePicture(mDisplay, renderPicture);
    XFreePixmap(mDisplay, renderPixmap);
    XFlush(mDisplay);
}

/**
 * Overriden resize() method ensures we remember window
 * position & size on restarts.
 *
 * Adjusts clickable button positions & their Input
 * Shape Regions to the new locations.
 */
void
StickyWindow::resize(const int xPos, const int yPos,
    const int width, const int height) {

    // Save the workspace, pos & size for later positioning.
    const int WORKSPACE = mXHelper->getWindowWorkspace(mX11Window);
    mSettingsHelper->setWindowWorkspace(WORKSPACE);

    mSettingsHelper->setWindowXPos(xPos);
    mSettingsHelper->setWindowYPos(yPos);
    mSettingsHelper->setWindowWidth(width);
    mSettingsHelper->setWindowHeight(height);

    // Reset position hints on resize.
    XSizeHints* hints = XAllocSizeHints();
    if (hints) {
        hints->flags = USPosition | PMinSize;
        hints->x = xPos;
        hints->y = yPos;
        hints->min_width = (int)
            mSettingsHelper->getWindowMinimumWidth();
        hints->min_height = (int)
            mSettingsHelper->getWindowMinimumHeight();
        XSetWMNormalHints(mDisplay, mX11Window, hints);
        XFree(hints);
    }

    draw();
}

/**
 * Getters & setters for cursor position.
 */
QPoint
StickyWindow::getCursorPosition() {
    return mCursorPosition;
}

void
StickyWindow::setCursorPosition(const QPoint position) {
    mCursorPosition = position;
}

/**
 * Main X11 event cycle Handler.
 */
void
StickyWindow::run() {
    // Set timer and start loop.
    Clock::time_point mCursorWatcherStart = Clock::now();
    while (true) {
        // Check if time to trigger cursorWatcher.
        Clock::time_point mCursorWatcherEnd = Clock::now();
        const chrono::milliseconds DURATION =
            chrono::duration_cast<chrono::milliseconds>
                (mCursorWatcherEnd - mCursorWatcherStart);
        if (DURATION > CURSOR_WATCHER_DELAY_MS) {
            cursorWatcherThread();
            mCursorWatcherStart = Clock::now();
        }

        // Time to process xEvents. Close requested?
        if (handleX11EventQueue()) {
            break;
        }

        // Support ConfigDialog loop.
        QCoreApplication::processEvents();
    }
}

/**
 * Public method returns StickyWindows internal x11 window.
 */
Window
StickyWindow::getX11Window() {
    return mX11Window;
}

/**
 * Private method sets StickyWindows internal x11 window.
 */
void 
StickyWindow::setX11Window(const Window window) {
    mX11Window = window;
}

/**
 * Create a new X11 window with appropriate decorations.
 */
Window
StickyWindow::createX11Window() {
    // If Widget first time run, center position.
    if (mSettingsHelper->getWindowXPos() == -1 &&
        mSettingsHelper->getWindowYPos() == -1) {
        defineWindowOnFirstRun();
        defineWindowCanvasPosition();
    }

    // Create window @ position & size.
    XSetWindowAttributes wAttrs;
    wAttrs.colormap = mColorMap;
    wAttrs.border_pixel = 0;
    wAttrs.event_mask = OBSERVABLE_EVENTS;

    mX11Window = XCreateWindow(mDisplay, RootWindow(mDisplay,
        mVisualInfoStruct.screen),
        mSettingsHelper->getWindowXPos(), mSettingsHelper->getWindowYPos(),
        mSettingsHelper->getWindowWidth(), mSettingsHelper->getWindowHeight(),
        1, mVisualInfoStruct.depth, InputOutput,
        mVisualInfoStruct.visual, CWColormap | CWEventMask |
        CWBorderPixel, &wAttrs);

    if (mX11Window == None) {
        cout << endl << XCOLOR_RED << "StickyWindow: X11 failed "
            "to create a StickyClock window - Fatal." <<
            XCOLOR_NORMAL << endl << endl;
        cout << "createX11Window() Finishes - error." << endl << endl;
        return mX11Window;
    }

    // Filter events we care to see in event loop.
    XSelectInput(mDisplay, mX11Window, OBSERVABLE_EVENTS);
    XSetWMProtocols(mDisplay, mX11Window, &mDeleteMessage, 1);

    // Set window Size Hints, position & minimum size.
    XSizeHints* hints = XAllocSizeHints();
    if (hints) {
        hints->flags = USPosition | PMinSize;
        hints->x = mSettingsHelper->getWindowXPos();
        hints->y = mSettingsHelper->getWindowYPos();
        hints->min_width = (int) mSettingsHelper->
            getWindowMinimumWidth();
        hints->min_height = (int) mSettingsHelper->
            getWindowMinimumHeight();
        XSetWMNormalHints(mDisplay, mX11Window, hints);
        XFree(hints);
    }

    // Set the window title.
    mWindowTitle = strdup(APP_NAME);
    XSetStandardProperties(mDisplay, mX11Window, mWindowTitle,
        mWindowTitle, None, nullptr, 0, nullptr);
    free(mWindowTitle);

    // Set window workspace & border state before showing.
    const int WORKSPACE = mSettingsHelper->getWindowWorkspace();
    mXHelper->setWindowWorkspace(mX11Window, WORKSPACE);

    // Set "StickyWindow" type, show window, set config state.
    setStickyWindowType();
    show();
    setWindowConfigState();

    // Apply strict configuration to the window. Awesome WM
    // specifically needs this.
    XWindowChanges changes;
    changes.x = mSettingsHelper->getWindowXPos();
    changes.y = mSettingsHelper->getWindowYPos();
    changes.width = mSettingsHelper->getWindowWidth();
    changes.height = mSettingsHelper->getWindowHeight();
    XConfigureWindow(mDisplay, mX11Window,
        CWX | CWY | CWWidth | CWHeight, &changes);
    XFlush(mDisplay);

    // Done!
    return mX11Window;
}

/**
 * Determine centered position & size for first run.
 */
void
StickyWindow::defineWindowOnFirstRun() {
    const int SCREEN_WIDTH = WidthOfScreen(
        DefaultScreenOfDisplay(mDisplay));
    const int SCREEN_HEIGHT = HeightOfScreen(
        DefaultScreenOfDisplay(mDisplay));

    mSettingsHelper->setWindowWidth(Button::BUTTON_WIDTH +
        mSettingsHelper->getCanvasWidth() +
        Button::BUTTON_WIDTH);
    mSettingsHelper->setWindowHeight(Button::BUTTON_HEIGHT +
        mSettingsHelper->getCanvasHeight() +
        Button::BUTTON_HEIGHT);

    mSettingsHelper->setWindowXPos((SCREEN_WIDTH -
        mSettingsHelper->getWindowWidth()) / 2);
    mSettingsHelper->setWindowYPos((SCREEN_HEIGHT -
        mSettingsHelper->getWindowHeight()) / 2);
}

/**
 * Set "StickyWindow" type, which is normally an x11
 * "SplashScreen". On KDE we use "Dock", as their
 * "SplashScreen" window doesn't support InputRectangles
 * meaning we can't click buttons.
 */
void
StickyWindow::setStickyWindowType() {
    const QString THIS_WM_NAME = mXHelper->
        getWindowManagerName().c_str();

    const QString AWESOME_WM_NAME = "awesome";
    if (THIS_WM_NAME == AWESOME_WM_NAME) {
        const Atom STICKY_WINDOW_TYPE = XInternAtom(mDisplay,
            "_NET_WM_WINDOW_TYPE_SPLASH", false);
        mXHelper->setWindowType(mX11Window, STICKY_WINDOW_TYPE);
        return;
    }

    const Atom STICKY_WINDOW_TYPE = XInternAtom(mDisplay,
        "_NET_WM_WINDOW_TYPE_DOCK", false);
    mXHelper->setWindowType(mX11Window, STICKY_WINDOW_TYPE);
}

/**
 * Set window to stay on bottom or float as normal
 * based on configState.
 */
void
StickyWindow::setWindowConfigState() {
    // Set window below all while "stuck" (borderless).
    if (mSettingsHelper->getConfigMode()) {
        mXHelper->makeWindowFloat(mX11Window);
    } else {
        mXHelper->makeWindowStayOnBottom(mX11Window);
    }
}

/**
 * This method defines the Control buttons.
 */
void
StickyWindow::createWindowButtons() {
    const int BUTTONS_SIZE = mButtons.size();
    for (int i = 0; i < BUTTONS_SIZE; i++) {
        delete mButtons[i];
    }
    mButtons.clear();

    mPinButton = new PinButton(mSettingsHelper->
        getWindowWidth() - 2 * Button::BUTTON_WIDTH,
            Button::BUTTON_HEIGHT);

    mQuitButton = new QuitButton(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH, 0);
    mConfigButton = new ConfigButton(0, mSettingsHelper->
        getWindowHeight() - Button::BUTTON_HEIGHT);
    mMoveButton = new MoveButton(0, 0);

    mSizeButton = new SizeButton(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH, mSettingsHelper->
        getWindowHeight() - Button::BUTTON_HEIGHT);

    // Pin Button First.
    mButtons.push_back(mPinButton);

    // All others.
    mButtons.push_back(mQuitButton);
    mButtons.push_back(mConfigButton);
    mButtons.push_back(mMoveButton);
    mButtons.push_back(mSizeButton);
}

/**
 * This method updates the defined Control buttons.
 */
void
StickyWindow::updateWindowButtons() {
    mPinButton->setX(mSettingsHelper->
        getWindowWidth() - 2 * Button::BUTTON_WIDTH);
    mPinButton->setY(Button::BUTTON_HEIGHT);

    mQuitButton->setX(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH);
    mQuitButton->setY(0);

    mConfigButton->setX(0);
    mConfigButton->setY(mSettingsHelper->getWindowHeight() -
        Button::BUTTON_HEIGHT);

    mMoveButton->setX(0);
    mMoveButton->setY(0);

    mSizeButton->setX(mSettingsHelper->
        getWindowWidth() - Button::BUTTON_WIDTH);
    mSizeButton->setY(mSettingsHelper->
        getWindowHeight() - Button::BUTTON_HEIGHT);
}

/**
 * Check to see which UI Button pressed.
 */
Button*
StickyWindow::whichButtonIsPressed() {
    const int BUTTONS_COUNT = mButtons.size();

    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->isPressed()) {
            return mButtons[i];
        }
    }
    return nullptr;
}

/**
 * Unpress all UI Buttons.
 */
void
StickyWindow::unPressAllButtons() {
    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        mButtons[i]->setPressed(false);
    }
}

/**
 * Check to see which UI Button hovered.
 */
Button*
StickyWindow::whichButtonIsHovered(const QPoint pos) {
    const int BUTTONS_COUNT = mButtons.size();

    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->getRect().contains(pos)) {
            return mButtons[i];
        }
    }
    return nullptr;
}

/**
 * Setter for PinButton visibility state.
 */
void
StickyWindow::setPinButtonVisibility(
    const bool visibility) {
    if (mPinButton->isVisible() != visibility) {
        mPinButton->setVisible(visibility);
        draw();
    }
}

/**
 * While not in configMode, hovering a (hidden)
 * corner button will make it visible & actionable.
 */
void
StickyWindow::setHoveredButtonVisibility(const QPoint pos) {
    if (mSettingsHelper->getConfigMode()) {
        return;
    }

    // Corner buttons start @ 1.
    const Button* HOVERED_BUTTON = whichButtonIsHovered(pos);
    const int BUTTONS_COUNT = mButtons.size();

    bool redrawRequired = false;
    for (int i = 1; i < BUTTONS_COUNT; i++) {
        // Set visible hovered.
        if (mButtons[i]->getRect().contains(pos)) {
            if (!mButtons[i]->isVisible()) {
                mButtons[i]->setVisible(true);
                redrawRequired = true;
            }
        } else {
            // Clear all others.
            if (mButtons[i]->isVisible()) {
                mButtons[i]->setVisible(false);
                redrawRequired = true;
            }
        }
    }

    if (redrawRequired) {
        draw();
    }
}

/**
 * Switch between window border state where titlebar &
 * border displayed or not.
 */
void
StickyWindow::onPinButtonClicked() {
    const bool CONFIG_MODE = !mSettingsHelper->getConfigMode();
    mSettingsHelper->setConfigMode(CONFIG_MODE);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 1; i < BUTTONS_COUNT; i++) {
        mButtons[i]->setVisible(CONFIG_MODE);
    }

    setWindowConfigState();
    draw();
}

/**
 * Draw all visible buttons & sest underlying input shapes.
 */
void
StickyWindow::drawAllButtons() {
    // Erase PinButton during resize-draw.
    const int BUTTON_START = !mWindowResized ? 0 : 1;
    const int BUTTONS_END = mButtons.size();

    vector<XRectangle> rects;
    for (int i = BUTTON_START; i < BUTTONS_END; i++) {
        if (mButtons[i]->isVisible()) {
            mButtons[i]->draw(mX11Window);

            XRectangle buttonInputRegion {
                .x = (short) mButtons[i]->getX(),
                .y = (short) mButtons[i]->getY(),
                .width = (unsigned short) mButtons[i]->getWidth(),
                .height = (unsigned short) mButtons[i]->getHeight()
            };
            rects.push_back(buttonInputRegion);
        }
    }

    XShapeCombineRectangles(mDisplay, mX11Window, ShapeInput,
        0, 0, rects.data(), rects.size(), ShapeSet, Unsorted);
    XFlush(mDisplay);
}

/**
 * Define Canvas position inside Window.
 */
void
StickyWindow::defineWindowCanvasPosition() {
    mSettingsHelper->setCanvasXPos(Button::BUTTON_WIDTH);
    mSettingsHelper->setCanvasYPos(Button::BUTTON_HEIGHT);
}

/**
 * Define Canvas size inside Window.
 */
void
StickyWindow::defineWindowCanvasSize() {
    mSettingsHelper->setCanvasWidth(mSettingsHelper->
        getWindowWidth() - 2 * Button::BUTTON_WIDTH);
    mSettingsHelper->setCanvasHeight(mSettingsHelper->
        getWindowHeight() - 2 * Button::BUTTON_WIDTH);
}

/**
 * This method updates the windowRect once a second.
 */
void
StickyWindow::drawCanvas() {
    const string CURRENT_SECOND = getCurrentSecond();

    if (mPreviousClientUpdateSecond == "" ||
        mPreviousClientUpdateSecond != CURRENT_SECOND) {
        mPreviousClientUpdateSecond = CURRENT_SECOND;
        draw();
    }
}

/**
 * Getter for current time.
 */
string
StickyWindow::getCurrentTime() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    return mXHelper->addLeadZeroToNN(getCurrentHour()) + ":" +
        mXHelper->addLeadZeroToNN(getCurrentMinute());
}

/**
 * Getter for current hour.
 */
string
StickyWindow::getCurrentHour() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    const string HOUR = to_string(now->tm_hour);
    return mXHelper->addLeadZeroToNN(HOUR);
}

/**
 * Getter for current minute.
 */
string
StickyWindow::getCurrentMinute() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    const string MINUTE = to_string(now->tm_min);
    return mXHelper->addLeadZeroToNN(MINUTE);
}

/**
 * Getter for current second.
 */
string
StickyWindow::getCurrentSecond() {
    const time_t time = std::time(0);
    const tm* now = localtime(&time);

    const string SECOND = to_string(now->tm_sec);
    return mXHelper->addLeadZeroToNN(SECOND);
}

/**
 * Determine if it's WeedClock time.
 */
bool
StickyWindow::isItWeedTime() {
    return getCurrentMinute() == "55" || getCurrentMinute() == "08" ||
        getCurrentMinute() == "38" || getCurrentMinute() == "39" ||
        getCurrentMinute() == "40" || getCurrentMinute() == "41" ||
        getCurrentMinute() == "42" || getCurrentMinute() == "43" ||
        getCurrentMinute() == "44" || getCurrentMinute() == "45" ||
        getCurrentMinute() == "46" || getCurrentMinute() == "20";
}

/**
 * Main X11 Event handler.
 */
bool
StickyWindow::handleX11EventQueue() {
    XEvent event;

    while (XPending(mDisplay)) {
        XNextEvent(mDisplay, &event);

        //const XAnyEvent* EVENT = (XAnyEvent*) &event;
        //mXHelper->debugXAnyEvent(EVENT);

        // Type = 33; ClientMsg Close event.
        if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == mDeleteMessage) {
                return true;
            }
            continue;
        }

        // Type = 28; Workspace property change.
        if (event.type == PropertyNotify) {
            onPropertyNotify(event.xproperty);
            continue;
        }

        // Type = 12; Expose.
        if (event.type == Expose) {
            draw();
            continue;
        }

        // Type = 22; ConfigureNotify.
        if (event.type == ConfigureNotify) {
            int posX; int posY; Window unused;
            XTranslateCoordinates(mDisplay, event.xconfigure.window,
                RootWindow(mDisplay, DefaultScreen(mDisplay)),
                0, 0, &posX, &posY, &unused);
            resize(posX, posY, event.xconfigure.width,
                event.xconfigure.height);
            continue;
        }
    }
    return false;
}

/**
 * Cursor watcher detects user actions.
 */
void
StickyWindow::cursorWatcherThread() {
    // Update clock time.
    drawCanvas();

    // If can't find cursor location, exit.
    if (!mXHelper->isWindowInStackedList(mX11Window)) {
        unPressAllButtons();
        if (mIsPointerGrabbed) {
            XUngrabPointer(mDisplay, CurrentTime);
            mIsPointerGrabbed = false;
        }
        return;
    }

    // Find the cursor location.
    Window root, child;
    int rootX, rootY, winX, winY;
    unsigned int mask;
    if (!XQueryPointer(mDisplay, mX11Window, &root,
        &child, &rootX, &rootY, &winX, &winY, &mask)) {
        unPressAllButtons();
        if (mIsPointerGrabbed) {
            XUngrabPointer(mDisplay, CurrentTime);
            mIsPointerGrabbed = false;
        }
        return;
    }

    // Useful later.
    setCursorPosition(QPoint(rootX, rootY));

    // Set widget PinButton visibility state.
    // Window is Entire window, or Canvas when pinned.
    const bool IS_WINDOW_HOVERED = mXHelper->isWindowHovered(
        mX11Window, QPoint(rootX, rootY), false);
    setPinButtonVisibility(IS_WINDOW_HOVERED);

    // Switch cursor to ours when hovering PinButton.
    if (IS_WINDOW_HOVERED) {
        if (!mIsPointerGrabbed) {
            XGrabPointer(mDisplay, mX11Window, False,
                ButtonPressMask | ButtonReleaseMask |
                PointerMotionMask, GrabModeAsync, GrabModeAsync,
                None, None, CurrentTime);
            mIsPointerGrabbed = true;
        }
    }

    // Entire Window is always entire window.
    // If not in config mode, hovering a non-PinButton
    // will make it visible & active.
    const bool IS_ENTIRE_WINDOW_HOVERED = mXHelper->
        isWindowHovered(mX11Window, QPoint(rootX, rootY), true);
    setHoveredButtonVisibility(QPoint(winX, winY));

    // If not mouse clicked.
    if (!(mask & Button1Mask)) {
        // When mouse unclicked, if hovered over widget settings
        // button, trigger it's click() handler.
        if (IS_ENTIRE_WINDOW_HOVERED && whichButtonIsPressed()) {
            Button* HOVERED_BUTTON =
                whichButtonIsHovered(QPoint(winX, winY));
            if (HOVERED_BUTTON) {
                HOVERED_BUTTON->click(mX11Window);
            }
        }
        if (mIsPointerGrabbed) {
            XUngrabPointer(mDisplay, CurrentTime);
            mIsPointerGrabbed = false;
        }
        if (mWindowResized) {
            defineWindowCanvasPosition();
            defineWindowCanvasSize();
            createWindowButtons();
            setPinButtonVisibility(true);
            draw();
            mWindowResized = false;
        }
        unPressAllButtons();
        return;
    }

    // If we're waiting to be unclicked.
    const Button* PRESSED_BUTTON = whichButtonIsPressed();
    if (PRESSED_BUTTON) {
        if (PRESSED_BUTTON->isDraggable()) {
            const QPoint CURRENT_POSITION = QPoint(rootX, rootY);

            mSettingsHelper->setWindowXPos(CURRENT_POSITION.x() -
                mDragMoveButtonOffset.x());
            mSettingsHelper->setWindowYPos(CURRENT_POSITION.y() -
                mDragMoveButtonOffset.y());

            XMoveWindow(mDisplay, mX11Window, mSettingsHelper->
                getWindowXPos(), mSettingsHelper->getWindowYPos());
            return;
        }

        if (PRESSED_BUTTON->isSizeable()) {
            const int SCREEN_WIDTH = WidthOfScreen(
                DefaultScreenOfDisplay(mDisplay));
            const int SCREEN_HEIGHT = HeightOfScreen(
                DefaultScreenOfDisplay(mDisplay));

            const QRect ORIGINAL_WINDOW = QRect(
                QPoint(mSettingsHelper->getWindowXPos(),
                    mSettingsHelper->getWindowYPos()),
                QSize(mSettingsHelper->getWindowWidth(),
                    mSettingsHelper->getWindowHeight())
            );

            const QPoint ORIGINAL_POSITION = ORIGINAL_WINDOW.topLeft();
            const QPoint CURRENT_POSITION = QPoint(rootX, rootY);
            const QPoint DIFF_POSITION = CURRENT_POSITION -
                ORIGINAL_POSITION;

            // Ensure window minimum size.
            QSize NEW_WINDOW_SIZE(
                DIFF_POSITION.x() - mDragResizeButtonOffset.x(),
                DIFF_POSITION.y() - mDragResizeButtonOffset.y()
            );
            if (NEW_WINDOW_SIZE.width() <
                    mSettingsHelper-> getWindowMinimumWidth()) {
                NEW_WINDOW_SIZE.setWidth(
                    mSettingsHelper-> getWindowMinimumWidth());
            }
            if (NEW_WINDOW_SIZE.height() <
                    mSettingsHelper->getWindowMinimumHeight()) {
                NEW_WINDOW_SIZE.setHeight(
                    mSettingsHelper->getWindowMinimumHeight());
            };

            // Disallow drag off right or bottom of screen.
            const int NEW_WINDOW_RIGHT_POS =
                mSettingsHelper->getWindowXPos() +
                    NEW_WINDOW_SIZE.width();
            if (NEW_WINDOW_RIGHT_POS > SCREEN_WIDTH) {
                NEW_WINDOW_SIZE.setWidth(
                    SCREEN_WIDTH - mSettingsHelper->getWindowXPos());
            }
            const int NEW_WINDOW_BOTTOM_POS =
                mSettingsHelper->getWindowYPos() +
                    NEW_WINDOW_SIZE.height();
            if (NEW_WINDOW_BOTTOM_POS > SCREEN_HEIGHT) {
                NEW_WINDOW_SIZE.setHeight(
                    SCREEN_HEIGHT - mSettingsHelper->getWindowYPos());
            }

            // Perform the move.
            mSettingsHelper->setWindowWidth(NEW_WINDOW_SIZE.width());
            mSettingsHelper->setWindowHeight(NEW_WINDOW_SIZE.height());

            XResizeWindow(mDisplay, mX11Window,
                mSettingsHelper->getWindowWidth(),
                mSettingsHelper->getWindowHeight());

            // mDragResizeButtonOffset = CURRENT_POSITION;
            defineWindowCanvasPosition();
            defineWindowCanvasSize();
            updateWindowButtons();

            mWindowResized = true;
            draw();
            return;
        }

        // Else, done.
        return;
    }

    if (IS_ENTIRE_WINDOW_HOVERED && !whichButtonIsPressed()) {
        Button* HOVERED_BUTTON =
            whichButtonIsHovered(QPoint(winX, winY));
        if (HOVERED_BUTTON) {
            mDragMoveButtonOffset = QPoint(
                winX - HOVERED_BUTTON->getX(),
                winY - HOVERED_BUTTON->getY());
            mDragResizeButtonOffset = QPoint(
                winX - HOVERED_BUTTON->getX() - Button::BUTTON_WIDTH,
                winY - HOVERED_BUTTON->getY() - Button::BUTTON_HEIGHT);

            HOVERED_BUTTON->setPressed(true);
            mClickStart = Clock::now();
            mClickStartValueSet = true;

            if (PinButton* T = dynamic_cast<PinButton*>
                (HOVERED_BUTTON)) {
                onPinButtonClicked();
            }
            return;
        }
    }

    mClickStartValueSet = false;
}

/**
 * This method saves the window workspace when changed.
 */
void
StickyWindow::onPropertyNotify(const XPropertyEvent& event) {
    char* atomName = XGetAtomName(event.display, event.atom);
    if (string(atomName) != "_NET_WM_DESKTOP") {
        XFree(atomName);
        return;
    }
    XFree(atomName);

    // If user changes workspace of window via gui panel.
    mSettingsHelper->setWindowWorkspace(mXHelper->
        getWindowWorkspace(mX11Window));
}
