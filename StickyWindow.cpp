
#include "Global.h"

/**
 * StickyWindow class wraps an x11 Window object.
 */
StickyWindow::StickyWindow() {
    mDeleteMessage = XInternAtom(mDisplay,
        "WM_DELETE_WINDOW", False);

    // Init visual transparency & TrueColor 32. Also,
    // Sets mVisualInfoStruct & mColorMap.
    mIsVisuallyTransparent = initVisualTransparency();
    if (!mIsVisuallyTransparent) {
        QMessageBox::information(NULL, APP_NAME, "Visual transparency "
            "is unavailable with this Desktop, FATAL.");
        cout << endl << XCOLOR_RED << "StickyClock: Visual "
            "transparency is unavailable with this Desktop, FATAL." <<
            XCOLOR_NORMAL << endl;
        return;
    }

    // Create & set base x11 window.
    setX11Window(createX11Window());

    // Instantiate widget canvas.
    mCanvas = new Canvas(mX11Window, mVisualInfoStruct.visual,
        mColorMap);

    // Create control buttons.
    createAllWindowButtons();

    // Create autohide timer for control buttons,
    // then set control buttons visibility.
    mControlsTimer = make_unique<QTimer>();
    mControlsTimer->setSingleShot(true);
    QObject::connect(mControlsTimer.get(), &QTimer::timeout,
        [this] () {
        setConfigModeOff();
    });

    setControlButtonsVisibility();
}

/**
 * StickyWindow destructor, cleanup Window object.
 */
StickyWindow::~StickyWindow() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    delete mCanvas;

    const int BUTTONS_SIZE = mButtons.size();
    for (int i = 0; i < BUTTONS_SIZE; i++) {
        delete mButtons[i];
    }
    mButtons.clear();

    // Uninit all.
    if (mIsVisuallyTransparent) {
        XFreeColormap(mDisplay, mColorMap);
    }

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
 * Overriden draw() method ensures we have a transparent
 * window with PinButton visible on mouse hover.
 */
void
StickyWindow::draw() {
    const XRenderPictFormat* RENDER_FORMAT =
        XRenderFindVisualFormat(mDisplay, mVisualInfoStruct.visual);
    const Picture RENDER_PICTURE = XRenderCreatePicture(
        mDisplay, mX11Window, RENDER_FORMAT, 0, nullptr);

    // If drawing on wrong desktop, erase instead.
    const int VISIBLE_DESKTOP = mXHelper->getVisibleDesktop();
    const int PREFERRED_DESKTOP = mSettingsHelper->
        getIntSetting(SettingsHelper::PREFERRED_DESKTOP);
    if (VISIBLE_DESKTOP != -1 && PREFERRED_DESKTOP != -1 &&
        (VISIBLE_DESKTOP != PREFERRED_DESKTOP)) {
        XRenderFillRectangle(mDisplay, PictOpSrc, RENDER_PICTURE,
            &TRANSPARENT_RCOLOR, 0, 0, mSettingsHelper->
            getWindowWidth(), mSettingsHelper->getWindowHeight());
        XRenderFreePicture(mDisplay, RENDER_PICTURE);
        XFlush(mDisplay);
        return;
    }

    // Erase entire window before draw while
    // resizing to avoid screen artifacts.
    if (mIsSizingWindow) {
        XRenderFillRectangle(mDisplay, PictOpSrc, RENDER_PICTURE,
            &TRANSPARENT_RCOLOR, 0, 0, mSettingsHelper->
            getWindowWidth(), mSettingsHelper->getWindowHeight());
    }

    // Draw Widget canvas.
    mCanvas->drawCanvas();

    // Draw Control buttons & pin button.
    drawAllWindowButtons(RENDER_PICTURE);

    // Cleanup.
    XRenderFreePicture(mDisplay, RENDER_PICTURE);
    XFlush(mDisplay);
}

/**
 * Overriden hide() method.
 */
void
StickyWindow::hide() {
    // If hiding on wrong desktop, don't.
    const int VISIBLE_DESKTOP = mXHelper->getVisibleDesktop();
    const int PREFERRED_DESKTOP = mSettingsHelper->
        getIntSetting(SettingsHelper::PREFERRED_DESKTOP);

    if (VISIBLE_DESKTOP == -1 || PREFERRED_DESKTOP == -1 ||
        (VISIBLE_DESKTOP == PREFERRED_DESKTOP)) {
        return;
    }
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
 * Main X11 event cycle Handler.
 */
void
StickyWindow::run() {
    while (true) {
        // Process xEvents, close when requested.
        if (handleX11EventQueue()) {
            break;
        }

        // Support Move & Resize.
        cursorWatcherThread();

        // Draw App canvas.
        if (mCanvas->updateCanvas()) {
            draw();
        }

        // Support ConfigDialog loop.
        QCoreApplication::processEvents();
    }
}

/**
 * Ensure window opens on valid remembered desktop.
 */
void
StickyWindow::rangeCheckPreferredDesktopSetting(const Window window) {
    const int PREFERRED_DESKTOP = mSettingsHelper->
        getIntSetting(SettingsHelper::PREFERRED_DESKTOP);
    if (PREFERRED_DESKTOP == -1) {
        return;
    }

    const int BOUNDED_PREFERRED_DESKTOP = PREFERRED_DESKTOP + 1;
    const int MAX_OS_DESKTOPS = mXHelper->getMaximumDesktops();

    if (BOUNDED_PREFERRED_DESKTOP > MAX_OS_DESKTOPS) {
        mSettingsHelper->setIntSetting(
            SettingsHelper::PREFERRED_DESKTOP, -1);
    }
}

/**
 * Initialize Transparency & TrueColor 32.
 */
bool
StickyWindow::initVisualTransparency() {
    // Ensure we have a compositor running.
    if (!mXHelper->isACompositorRunning()) {
        return false;
    }

    const int VISUAL_COLOR_DEPTH = 32;
    if (XMatchVisualInfo(mDisplay, DefaultScreen(mDisplay),
        VISUAL_COLOR_DEPTH, TrueColor, &mVisualInfoStruct)) {
        mColorMap = XCreateColormap(mDisplay, RootWindow(mDisplay,
            DefaultScreen(mDisplay)), mVisualInfoStruct.visual,
                AllocNone);
        return true;
    }

    return false;
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
    // Determine if Widget first time run.
    const bool INITIAL_RUN = QPoint(mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos()) == INVALID_POINT;

    // Initial setup for Widget first time run.
    if (INITIAL_RUN) {
        defineWindowOnFirstRun();
        defineWindowCanvasPosition();
    }

    // Create window @ position & size.
    XSetWindowAttributes wAttrs;
    wAttrs.colormap = mColorMap;
    wAttrs.border_pixel = 0;

    mX11Window = XCreateWindow(mDisplay, RootWindow(mDisplay,
        mVisualInfoStruct.screen), mSettingsHelper->getWindowXPos(),
        mSettingsHelper->getWindowYPos(), mSettingsHelper->
        getWindowWidth(), mSettingsHelper->getWindowHeight(), 1,
        mVisualInfoStruct.depth, InputOutput, mVisualInfoStruct.visual,
        CWColormap | CWEventMask | CWBorderPixel, &wAttrs);

    if (mX11Window == None) {
        cout << endl << XCOLOR_RED << "StickyWindow: X11 failed "
            "to create a StickyClock window - Fatal." <<
            XCOLOR_NORMAL << endl << endl;
        return mX11Window;
    }

    // Set procID on our window, for RecentsHelper().
    mXHelper->setWindowPID(mX11Window);

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
    const QString FIRST_RECENTS_NAME =
        mRecentsHelper->RECENTS_NAMES[0];
    const QString APP_RECENT_NAME =
        mRecentsHelper->getAppRecentsName();

    QString TITLE = QString(APP_NAME);
    if (APP_RECENT_NAME != FIRST_RECENTS_NAME) {
        TITLE += " " + APP_RECENT_NAME;
    }
    mWindowTitle = strdup(TITLE.toUtf8().constData());
    XSetStandardProperties(mDisplay, mX11Window, mWindowTitle,
        mWindowTitle, None, nullptr, 0, nullptr);
    free(mWindowTitle);

    // Ensure we're placed on all desktops.
    mXHelper->setWindowDesktop(mX11Window, -1);

    // Ensure window opens on valid remembered desktop.
    rangeCheckPreferredDesktopSetting(mX11Window);

    // Set "StickyWindow" type, show window, set config state.
    setStickyWindowType();
    show();

    // On first time run, ensure window appears ontop
    // like a splash screen. Else, ensure stick state.
    if (INITIAL_RUN) {
        XRaiseWindow(mDisplay, mX11Window);
    } else {
        setWindowStickPosition();
    }

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
 * Set window type, which is normally "SplashScreen".
 *
 * TODO: ?? On KDE we use "Dock", as their "SplashScreen" window
 * doesn't support InputRectangles meaning we can't click buttons.
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
StickyWindow::setWindowStickPosition() {
    const bool PREFER_ONTOP = mSettingsHelper->getBoolSetting(
        SettingsHelper::ON_TOP_INSTEAD);

    mXHelper->makeWindowStayOnTop(mX11Window, PREFER_ONTOP);
    mXHelper->makeWindowStayOnBottom(mX11Window, !PREFER_ONTOP);
}

/**
 * Callback method for Autohide controls timer.
 */
void
StickyWindow::setConfigModeOff() {
    // If visibility already off, we're done.
    if (!mSettingsHelper->getBoolSetting(
        SettingsHelper::CONFIG_MODE)) {
        return;
    }

    // Set visibility off, & done.
    mSettingsHelper->setBoolSetting(
        SettingsHelper::CONFIG_MODE, false);
    setControlButtonsVisibility();
}

/**
 * This method defines the Control buttons.
 */
void
StickyWindow::createAllWindowButtons() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

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
StickyWindow::updateAllWindowButtons() {
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
 * Draw all visible buttons & set underlying input shapes.
 */
void
StickyWindow::drawAllWindowButtons(const Picture renderPicture) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    vector<XRectangle> rects;
    const int BUTTONS_END = mButtons.size();
    for (int i = 0; i < BUTTONS_END; i++) {
        if (mButtons[i]->isVisible()) {
            mButtons[i]->draw(mX11Window);

            XRectangle buttonInputRegion {
                .x = (short) mButtons[i]->getX(),
                .y = (short) mButtons[i]->getY(),
                .width = (unsigned short) mButtons[i]->getWidth(),
                .height = (unsigned short) mButtons[i]->getHeight()
            };
            rects.push_back(buttonInputRegion);
        } else {
            mButtons[i]->erase(mX11Window, renderPicture);
        }
    }
    XShapeCombineRectangles(mDisplay, mX11Window, ShapeInput,
        0, 0, rects.data(), rects.size(), ShapeSet, Unsorted);

    XFlush(mDisplay);
}

/**
 * Set visibility state of the four corner control buttons.
 */
void
StickyWindow::setControlButtonsVisibility() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const bool CONFIG_MODE = mSettingsHelper->getBoolSetting(
        SettingsHelper::CONFIG_MODE);

    // Pin button = 0, controls start @ 1;
    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 1; i < BUTTONS_COUNT; i++) {
        mButtons[i]->setVisible(CONFIG_MODE);
    }

    // If not Config mode, stop autohide timer & done.
    if (!CONFIG_MODE) {
        mControlsTimer->stop();
        return;
    }

    // If using autohide Config buttons, start timer.
    if (mSettingsHelper->getBoolSetting(SettingsHelper::
        AUTOHIDE_CONTROLS)) {
        mControlsTimer->start(mSettingsHelper->getIntSetting(
            SettingsHelper::AUTOHIDE_DELAY) * 1000);
    }
}

/**
 * Setter for PinButton visibility state.
 */
void
StickyWindow::setHoveredPinButtonVisibility(
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
StickyWindow::setHoveredControlButtonVisibility(
    const QPoint position) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    // Pin button = 0, controls start @ 1;
    const bool CONFIG_MODE = mSettingsHelper->getBoolSetting(
        SettingsHelper::CONFIG_MODE);

    bool redrawRequired = false;

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 1; i < BUTTONS_COUNT; i++) {
        // Set visible hovered.
        if (mButtons[i]->getRect().contains(position)) {
            if (!mButtons[i]->isVisible()) {
                mButtons[i]->setVisible(true);
                redrawRequired = true;
            }
        } else {
            // Clear all others.
            if (mButtons[i]->isVisible() != CONFIG_MODE) {
                mButtons[i]->setVisible(CONFIG_MODE);
                redrawRequired = true;
            }
        }
    }

    if (redrawRequired) {
        draw();
    }
}

/**
 * Press hovered button, & return it's position.
 */
QPoint
StickyWindow::pressHoveredButton(const QPoint position) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->getRect().contains(position)) {
            mButtons[i]->setPressed(true);
            return QPoint(mButtons[i]->getX(),
                mButtons[i]->getY());
        }
    }
    return INVALID_POINT;
}

/**
 * Return draggable status of pressed button.
 */
bool
StickyWindow::isPressedButtonDraggable() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->isPressed()) {
            return mButtons[i]->isDraggable();
        }
    }

    return false;
}

/**
 * Return sizeable status of pressed button.
 */
bool
StickyWindow::isPressedButtonSizable() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->isPressed()) {
            return mButtons[i]->isSizeable();
        }
    }

    return false;
}

/**
 * Click previously pressed, hovered button.
 */
void
StickyWindow::clickPressedHoveredButton(const QPoint position) {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->getRect().contains(position)) {
            if (mButtons[i]->isPressed()) {
                if (i == 0) {
                    clickPressedPinButton();
                    return;
                }
                mButtons[i]->click(mX11Window);
                return;
            }
        }
    }
}

/**
 * If Pin button clicked, toggle config mode.
 */
void
StickyWindow::clickPressedPinButton() {
    // Toggle current to new mode.
    const bool NEW_CONFIG_MODE = mSettingsHelper->getBoolSetting(
        SettingsHelper::CONFIG_MODE) ? false : true;

    mSettingsHelper->setBoolSetting(
        SettingsHelper::CONFIG_MODE, NEW_CONFIG_MODE);
    setControlButtonsVisibility();
}

/**
 * Unpress all UI Buttons.
 */
void
StickyWindow::unPressAllWindowButtons() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        mButtons[i]->setPressed(false);
    }
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
 * Main X11 Event handler.
 */
bool
StickyWindow::handleX11EventQueue() {
    // ConfigureNotify.
    Window resizedWindow = None;
    int resizedPosX = -1;
    int resizedPosY = -1;

    // ButtonPress, ButtonRelease.
    unsigned int clickStatus = 0;

    Window root = None;
    int rootClickPositionX = -1;
    int rootClickPositionY = -1;

    Window window = None;
    int windowClickPositionX = -1;
    int windowClickPositionY = -1;

    // ButtonPress.
    QPoint clickedButtonPosition;

    // Filter events we care to see in event loop.
    const long OBSERVABLE_EVENTS = ConfigureNotify |
        StructureNotifyMask | PropertyChangeMask |
        ButtonPressMask | ButtonReleaseMask | ExposureMask;
    XSelectInput(mDisplay, mX11Window, OBSERVABLE_EVENTS);

    XSelectInput(mDisplay, DefaultRootWindow(mDisplay),
        PropertyChangeMask);
    XSetWMProtocols(mDisplay, mX11Window, &mDeleteMessage, 1);

    // Event loop, until window close.
    XEvent event;
    while (XPending(mDisplay)) {
        XNextEvent(mDisplay, &event);
        switch (event.type) {

            // ClientMsg says window close.
            case ClientMessage:
                if (event.xclient.data.l[0] == mDeleteMessage) {
                    return true;
                }
                break;

            // Detect root window or desktop property changes.
            case PropertyNotify:
                if (event.xproperty.window ==
                    DefaultRootWindow(mDisplay)) {
                    // MAXIMUM DESKTOPS parm change.
                    if (event.xproperty.atom == XInternAtom(mDisplay,
                        "_NET_NUMBER_OF_DESKTOPS", False)) {
                        rangeCheckPreferredDesktopSetting(getX11Window());
                        updateActiveConfigButtonDialog();
                    }
                    break;
                }
                // Else, our window property change.
                onPropertyNotify(event.xproperty);
                break;

            // Type = Expose.
            case Expose:
                draw();
                break;

            // Type = UnmapNotify.
            case UnmapNotify:
                hide();
                break;

            // Type = MapNotify.
            case MapNotify:
                show();
                break;

            // Type = 22; ConfigureNotify.
            case ConfigureNotify:
                XTranslateCoordinates(mDisplay, event.xconfigure.window,
                    RootWindow(mDisplay, DefaultScreen(mDisplay)),
                    0, 0, &resizedPosX, &resizedPosY, &resizedWindow);
                resize(resizedPosX, resizedPosY, event.xconfigure.width,
                    event.xconfigure.height);
                break;

            case ButtonPress:
                // Find the cursor down location.
                if (!XQueryPointer(mDisplay, mX11Window, &root,
                    &window, &rootClickPositionX, &rootClickPositionY,
                    &windowClickPositionX, &windowClickPositionY,
                    &clickStatus)) {
                    break;
                }

                // Do pressed highlighting for pinbutton & controls.
                clickedButtonPosition = pressHoveredButton(
                    QPoint(windowClickPositionX, windowClickPositionY));
                if (clickedButtonPosition == INVALID_POINT) {
                    break;
                }

                // Grab the cursor on entry.
                mIsMouseClicked = true;

                XGrabPointer(mDisplay, mX11Window, False,
                    ButtonPressMask | ButtonReleaseMask |
                    PointerMotionMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);

                // Set cursor offset position within button @ start
                // of move ... (Top left) corner distance.
                mDragMoveButtonOffset = QPoint(
                    windowClickPositionX - clickedButtonPosition.x(),
                    windowClickPositionY - clickedButtonPosition.y());

                // Set cursor offset position within button @ start
                // of resizing ... (Bottom right) corner distance.
                mDragResizeButtonOffset = QPoint(
                    windowClickPositionX - clickedButtonPosition.x() -
                        Button::BUTTON_WIDTH,
                    windowClickPositionY - clickedButtonPosition.y() -
                        Button::BUTTON_HEIGHT);
                break;

            case ButtonRelease:
                // Release the cursor on exit.
                XUngrabPointer(mDisplay, CurrentTime);

                // Find the cursor up location.
                if (!XQueryPointer(mDisplay, mX11Window, &root,
                    &window, &rootClickPositionX, &rootClickPositionY,
                    &windowClickPositionX, &windowClickPositionY,
                    &clickStatus)) {
                    break;
                }

                // If still hovering a control, release a click.
                clickPressedHoveredButton(QPoint(windowClickPositionX,
                    windowClickPositionY));

                if (mIsSizingWindow) {
                    defineWindowCanvasPosition();
                    defineWindowCanvasSize();
                    updateAllWindowButtons();
                    setHoveredPinButtonVisibility(true);
                    draw();
                    mIsSizingWindow = false;
                }

                unPressAllWindowButtons();
                mIsMouseClicked = false;
                break;
        }
    }
    return false;
}

/**
 * Update the Config Dialog if it's active &
 * the UI needs updating.
 */
void
StickyWindow::updateActiveConfigButtonDialog() {
    lock_guard<recursive_mutex> lock(mButtonsMutLock);

    const int BUTTONS_COUNT = mButtons.size();
    for (int i = 0; i < BUTTONS_COUNT; i++) {
        if (mButtons[i]->hasDialog()) {
            mButtons[i]->updateDialog();
            return;
        }
    }
}

/**
 * This method observes property changes to window.
 * Useful for debug.
 */
void
StickyWindow::onPropertyNotify(const XPropertyEvent& event) {
/*
    if (event.display && event.atom) {
        char* atomName = XGetAtomName(event.display, event.atom);
        if (atomName && string(atomName) == "_NET_WM_DESKTOP") {
            // Foo;
            XFree(atomName);
            return;
        }
        XFree(atomName);
    }
*/
}

/**
 * Cursor watcher detects user actions.
 */
void
StickyWindow::cursorWatcherThread() {
    // Find the cursor location.
    Window root, window;
    int rootX, rootY, winX, winY;
    unsigned int clickStatus;

    if (!XQueryPointer(mDisplay, mX11Window, &root, &window,
        &rootX, &rootY, &winX, &winY, &clickStatus)) {
        cout << XCOLOR_YELLOW << "StickyClock: Failed to query "
            "Cursor location." << XCOLOR_NORMAL << endl;
        return;
    }

    // Set PinButton visibility if hovered.
    const QPoint CURSOR_ROOT_POSITION = QPoint(rootX, rootY);
    const bool IS_WINDOW_HOVERED = mXHelper->isWindowHovered(
        mX11Window, CURSOR_ROOT_POSITION, false);
    setHoveredPinButtonVisibility(IS_WINDOW_HOVERED);

    // Set all other controls visibility if hovered.
    const QPoint CURSOR_WINDOW_POSITION = QPoint(winX, winY);
    setHoveredControlButtonVisibility(CURSOR_WINDOW_POSITION);

    // Are we moving or sizing the window?
    if (mIsMouseClicked) {
        if (isPressedButtonDraggable()) {
            dragWindowToPoint(CURSOR_ROOT_POSITION);
            return;
        }
        if (isPressedButtonSizable()) {
            resizeWindowToPoint(CURSOR_ROOT_POSITION);
        }
    }
}

/**
 * Perform window drag.
 */
void
StickyWindow::dragWindowToPoint(const QPoint position) {
    QPoint dragPosition = position;

    const bool ON_ALL_DESKTOPS = mSettingsHelper->getIntSetting(
        SettingsHelper::PREFERRED_DESKTOP) == -1;
    const bool ALLOW_DRAG_THRU_DESKTOPS = mSettingsHelper->
        getBoolSetting(SettingsHelper::DRAG_THRU_DESKTOPS);

    const int VISIBLE_DESKTOP = mXHelper->getVisibleDesktop();
    const int SCREEN_WIDTH = WidthOfScreen(
        DefaultScreenOfDisplay(mDisplay));

    if ((dragPosition.x() <= 0) &&
        ON_ALL_DESKTOPS && ALLOW_DRAG_THRU_DESKTOPS) {
        int desktop = VISIBLE_DESKTOP - 1;
        if (desktop < 0) {
            desktop = mXHelper->getMaximumDesktops() - 1;
        }
        mXHelper->setVisibleDesktop(desktop);

        const int KICK_DISTANCE = 2;
        dragPosition.setX(SCREEN_WIDTH - KICK_DISTANCE);
        XWarpPointer(mDisplay, None, DefaultRootWindow(mDisplay),
            0, 0, 0, 0, dragPosition.x(), dragPosition.y());

    } else {
       if ((dragPosition.x() >= SCREEN_WIDTH - 1) &&
           ON_ALL_DESKTOPS && ALLOW_DRAG_THRU_DESKTOPS) {
            int desktop = VISIBLE_DESKTOP + 1;
            if (desktop >= mXHelper->getMaximumDesktops()) {
                desktop = 0;
            }
            mXHelper->setVisibleDesktop(desktop);

            const int KICK_DISTANCE = 2;
            dragPosition.setX(KICK_DISTANCE);
            XWarpPointer(mDisplay, None, DefaultRootWindow(mDisplay),
                0, 0, 0, 0, dragPosition.x(), dragPosition.y());
        }
    }

    mSettingsHelper->setWindowXPos(dragPosition.x() -
        mDragMoveButtonOffset.x());
    mSettingsHelper->setWindowYPos(dragPosition.y() -
        mDragMoveButtonOffset.y());

    XMoveWindow(mDisplay, mX11Window, mSettingsHelper->
        getWindowXPos(), mSettingsHelper->getWindowYPos());
}

/**
 * Perform window resizing.
 */
void
StickyWindow::resizeWindowToPoint(const QPoint position) {
    mIsSizingWindow = true;

    // Don't allow sizing below window minimum.
    const QRect ORIGINAL_WINDOW = QRect(QPoint(mSettingsHelper->
        getWindowXPos(), mSettingsHelper->getWindowYPos()),
        QSize(mSettingsHelper->getWindowWidth(), mSettingsHelper->
        getWindowHeight())
    );
    const QPoint ORIGINAL_POSITION = ORIGINAL_WINDOW.topLeft();
    const QPoint DIFF_POSITION = position - ORIGINAL_POSITION;

    // Get new window size, enforcing minimum and screen edge.
    QSize newWindowSize = QSize(DIFF_POSITION.x() -
        mDragResizeButtonOffset.x(), DIFF_POSITION.y() -
        mDragResizeButtonOffset.y());

    // Enforce window minimum size.
    if (newWindowSize.width() <
        mSettingsHelper-> getWindowMinimumWidth()) {
        newWindowSize.setWidth(mSettingsHelper->
            getWindowMinimumWidth());
    }
    if (newWindowSize.height() <
        mSettingsHelper->getWindowMinimumHeight()) {
        newWindowSize.setHeight(mSettingsHelper->
            getWindowMinimumHeight());
    };

    // Enforce window edges.
    const int SCREEN_WIDTH = WidthOfScreen(DefaultScreenOfDisplay(
        mDisplay));
    const int NEW_WINDOW_RIGHT_POS = mSettingsHelper->getWindowXPos() +
        newWindowSize.width();
    if (NEW_WINDOW_RIGHT_POS > SCREEN_WIDTH) {
        newWindowSize.setWidth(SCREEN_WIDTH - mSettingsHelper->
            getWindowXPos());
    }
    const int SCREEN_HEIGHT = HeightOfScreen(DefaultScreenOfDisplay(
        mDisplay));
    const int NEW_WINDOW_BOTTOM_POS = mSettingsHelper->getWindowYPos() +
        newWindowSize.height();
    if (NEW_WINDOW_BOTTOM_POS > SCREEN_HEIGHT) {
        newWindowSize.setHeight(SCREEN_HEIGHT - mSettingsHelper->
            getWindowYPos());
    }

    // All is good, resize the window with new size.
    mSettingsHelper->setWindowWidth(newWindowSize.width());
    mSettingsHelper->setWindowHeight(newWindowSize.height());

    XResizeWindow(mDisplay, mX11Window, mSettingsHelper->
        getWindowWidth(), mSettingsHelper->getWindowHeight());

    // Resize canvas.
    defineWindowCanvasPosition();
    defineWindowCanvasSize();
    updateAllWindowButtons();

    // Re-draw all & done.
    draw();
}
