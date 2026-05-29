
#pragma once

/**
 * StickyWindow class wraps an x11 Window object.
 *
 */
class StickyWindow {

    public:
        static inline const long OBSERVABLE_EVENTS =
            ConfigureNotify | StructureNotifyMask |
            ExposureMask | PropertyChangeMask |
            PointerMotionMask | ButtonPressMask;

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
         * Overriden hide() method.
         */
        void hide();

        /**
         * Overriden draw() method ensures we have a transparent
         * window with PinButton visible on mouse hover.
         */
        void draw();

        /**
         * Overriden resize() method ensures we remember window
         * position & size on restarts.
         *
         * Adjusts clickable button positions & their Input
         * Shape Regions to the new locations.
         */
        void resize(const int xPos, const int yPos,
            const int width, const int height);

        /**
         * Getters & setters for cursor position.
         */
        QPoint getCursorPosition();
        void setCursorPosition(const QPoint position);

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

    private:
        // Members.
        Window mX11Window = None;
        char* mWindowTitle = nullptr;

        PinButton* mPinButton = nullptr;
        QuitButton* mQuitButton = nullptr;
        ConfigButton* mConfigButton = nullptr;
        MoveButton* mMoveButton = nullptr;
        SizeButton* mSizeButton = nullptr;
        vector<Button*> mButtons;

        string mPreviousClientUpdateSecond = "";
        QPoint mCursorPosition { };

        Clock::time_point mClickStart { };
        bool mClickStartValueSet = false;

        Clock::time_point mClickEnd { };

        bool mIsPointerGrabbed = false;
        QPoint mDragResizeButtonOffset { };
        QPoint mDragMoveButtonOffset { };
        bool mWindowResized = false;

        Atom mDeleteMessage { };

        bool mIsVisuallyTransparent = false;
        XVisualInfo mVisualInfoStruct { };
        Colormap mColorMap { };

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
         * Set "StickyWindow" type, which is normally an x11
         * "SplashScreen". On KDE we use "Dock", as their
         * "SplashScreen" window doesn't support InputRectangles
         * meaning we can't click buttons.
         */
        void setStickyWindowType();

        /**
         * Set window to stay on bottom or float as normal
         * based on configState.
         */
        void setWindowConfigState();

        /**
         * This method defines the Control buttons.
         */
        void createWindowButtons();

        /**
         * This method updates the defined Control buttons.
         */
        void updateWindowButtons();

        /**
         * Check to see if any UI Buttons pressed.
         */
        Button* whichButtonIsPressed();

        /**
         * Unpress all UI Buttons.
         */
        void unPressAllButtons();

        /**
         * Check to see which UI Button hovered.
         */
        Button* whichButtonIsHovered(const QPoint pos);

        /**
         * Setter for PinButton visibility state.
         */
        void setPinButtonVisibility(const bool visibility);

        /**
         * While not in configMode, hovering a (hidden)
         * corner button will make it visible & actionable.
         */
        void setHoveredButtonVisibility(const QPoint pos);

        /**
         * Switch between window border state where titlebar &
         * border displayed or not.
         */
        void onPinButtonClicked();

        /**
         * Draw all visible buttons.
         */
        void drawAllButtons();

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
        void drawCanvas();

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
};
