
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

        static inline constexpr XftColor BLACK_FCOLOR = {
            .pixel = 0x0, .color = { .red = 0xff, .green = 0xff,
                .blue = 0xff, .alpha = 0xffff } };

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
         * Adjusts clickable settings button position & its Input
         * Shape Region to the new top-right location.
         */
        void resize(const int xPos, const int yPos,
            const int width, const int height);

        /**
         * Setter for settings button visibility state.
         */
        void setSettingsButtonVisibility(const bool visibility);

        /**
         * Switch between window border state where titlebar &
         * border displayed or not.
         */
        void onSettingsButtonClicked();

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

    private:
        // Members.
        Window mX11Window = None;
        char* mWindowTitle = nullptr;

        PinButton* mSettingsButton = nullptr;
        QuitButton* mQuitButton = nullptr;
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

        Atom mDeleteProtocols { };
        Atom mDeleteMessage { };

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
         * Set window to stay on bottom or float as normal
         * based on configState.
         */
        void setWindowConfigState();

        /**
         * This method defines the Control buttons.
         */
        void setWindowButtons();

        /**
         * This method updates the defined Control buttons.
         */
        void updateWindowButtons();

        /**
         * Define Canvas position inside Window.
         */
        void defineWindowCanvasPosition();

        /**
         * Define Canvas size inside Window.
         */
        void defineWindowCanvasSize();

        /**
         * Unpress all UI Buttons.
         */
        void unPressAllButtons();

        /**
         * Check to see if any UI Buttons pressed.
         */
        Button* whichButtonIsPressed();

        /**
         * Check to see which UI Button hovered.
         */
        Button* whichButtonIsHovered(const QPoint pos);

        /**
         * Draw all visible buttons.
         */
        void drawAllButtons();

        /**
         * Main X11 Event handler.
         */
        bool handleX11EventQueue();

        /**
         * Timer callback to detect settings button clicks.
         */
        void cursorWatcherThread();

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
         * Helper to display leading 0's for Hour & minute.
         */
        string addLeadZeroToNN(const string NN);

        /**
         * This method returns pixel width of a text string.
         */
        int getStringPixelWidth(const QString textString);

        /**
         * This method returns pixel height of a text string.
         */
        int getStringPixelHeight(const QString textString);

        /**
         * This method saves the window workspace when changed.
         */
        void onPropertyNotify(const XPropertyEvent& event);
};
