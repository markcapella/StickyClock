
#pragma once

/**
 * StickyWindow class wraps an x11 Window object.
 *
 */
class StickyWindow {

    public:
        static inline constexpr QPoint INVALID_POINT {-1, -1};

        static inline constexpr chrono::milliseconds
            CURSOR_WATCHER_DELAY_MS{5};
        static inline constexpr chrono::milliseconds
            CLICK_LEN_MS{25};

        // Constructor.
        StickyWindow();
        ~StickyWindow();

        /**
         * Overriden show() method ensures we stay on bottom
         * when window is "stuck", window border state != visible.
         */
        void show();

        /**
         * Overriden draw() method ensures we have a transparent
         * window with PinButton visible on mouse hover.
         */
        void draw();

        /**
         * Overriden hide() method.
         */
        void hide();

        /**
         * Overriden resize() method ensures we remember window
         * position & size on restarts.
         *
         * Adjusts Control Button positions & their Input
         * Shape Regions to the new locations.
         */
        void resize(const int xPos, const int yPos,
            const int width, const int height);

        /**
         * Main X11 event cycle Handler.
         */
        void run();

        /**
         * This method returns StickyWindows internal x11 window.
         */
        Window getX11Window();

        /**
         * Ensure window opens on valid remembered desktop.
         */
        void rangeCheckPreferredDesktop(const Window window);

        XVisualInfo mVisualInfoStruct { };
    private:
        // Members.
        string mPreviousClientUpdateSecond = "";

        Window mX11Window = None;
        char* mWindowTitle = nullptr;

        Atom mDeleteMessage { };

        bool mIsVisuallyTransparent = false;
        Colormap mColorMap { };

        PinButton* mPinButton = nullptr;
        QuitButton* mQuitButton = nullptr;
        ConfigButton* mConfigButton = nullptr;
        MoveButton* mMoveButton = nullptr;
        SizeButton* mSizeButton = nullptr;

        vector<Button*> mButtons;
        mutable recursive_mutex mButtonsMutLock;

        bool mIsMouseClicked = false;
        QPoint mDragResizeButtonOffset { };
        QPoint mDragMoveButtonOffset { };
        bool mIsSizingWindow = false;
        unique_ptr<QTimer> mControlsTimer{nullptr};

        /**
         * Initialize Transparency & TrueColor 32.
         */
        bool initVisualTransparency();

        /**
         * This method sets StickyWindows internal x11 window.
         */
        void setX11Window(const Window window);

        /**
         * Create a new X11 window with appropriate decorations.
         */
        Window createX11Window();

        /**
         * Determine centered position on screen for first
         * time widget is run.
         */
        void defineWindowOnFirstRun();

        /**
         * Set window type, which is normally "SplashScreen".
         *
         * TODO: ?? On KDE we use "Dock", as their "SplashScreen" window
         * doesn't support InputRectangles meaning we can't click buttons.
         */
        void setStickyWindowType();

        /**
         * Set window to stay on bottom or float as normal
         * based on configState.
         */
        void setWindowStickPosition();

        /**
         * Callback method for Autohide controls timer.
         */
        void setConfigModeOff();

        /**
         * This method defines the Control buttons.
         */
        void createAllWindowButtons();

        /**
         * This method updates the defined Control buttons.
         */
        void updateAllWindowButtons();

        /**
         * Draw all visible buttons.
         */
        void drawAllWindowButtons(const Picture renderPicture);

        /**
         * Set visibility state of the four corner control buttons.
         */
        void setControlButtonsVisibility();

        /**
         * Setter for PinButton visibility state.
         */
        void setHoveredPinButtonVisibility(const bool visibility);

        /**
         * While not in configMode, hovering a (hidden)
         * Control button will make it visible & actionable.
         */
        void setHoveredControlButtonVisibility(
            const QPoint position);

        /**
         * Press hovered button, & return it's position.
         */
        QPoint pressHoveredButton(const QPoint position);

        /**
         * Return draggable status of pressed button.
         */
        bool isPressedButtonDraggable();

        /**
         * Return sizeable status of pressed button.
         */
        bool isPressedButtonSizable();

        /**
         * Clicking pressed hovered, doesn't trigger click if we
         * press button, drag off, then release.
         */
        void clickPressedHoveredButton(const QPoint position);

        /**
         * Toggle config mode.
         */
        void clickPressedPinButton();

        /**
         * Unpress all UI Buttons.
         */
        void unPressAllWindowButtons();

        /**
         * Define Canvas position inside Window.
         */
        void defineWindowCanvasPosition();

        /**
         * Define Canvas size inside Window.
         */
        void defineWindowCanvasSize();

        /**
         * This method updates the clocks time string for draw().
         */
        void drawWindowCanvas();

        /**
         * Main X11 Event handler.
         */
        bool handleX11EventQueue();

        /**
         * Cursor watcher detects user actions.
         */
        void cursorWatcherThread();

        /**
         * This method saves the window workspace when changed.
         */
        void onPropertyNotify(const XPropertyEvent& event);

        /**
         * Getter for current time.
         */
        string getCurrentTime();

        /**
         * Getter for current hour.
         */
        string getCurrentHour();

        /**
         * Getter for current minute.
         */
        string getCurrentMinute();

        /**
         * Getter for current second.
         */
        string getCurrentSecond();

        /**
         * Determine if it's WeedClock time.
         */
        bool isItWeedTime();
};
