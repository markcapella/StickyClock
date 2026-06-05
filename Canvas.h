
#pragma once

/**
 * Canvas is the main widget & draw.
 */
class Canvas {

    public:
        Canvas(const Window window, Visual* visual,
            const Colormap colormap);

        /**
         * StickyClock widget drawCanvas method.
         */
        void drawCanvas();

        /**
         * This method updates the windowRect once a second.
         */
        bool updateCanvas();

    private:
        // Members.
        Window mWindow = None;
        Visual* mVisual = nullptr;
        Colormap mColormap;

        string mPreviousClientUpdateSecond = "";

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
