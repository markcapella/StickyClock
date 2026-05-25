
#pragma once


/**
 * Simple class to represent a Button.
 *
 */
class ConfigButton : public Button {
    public:
        ConfigButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos) { }

        void draw(const Window window) override;
        void click(const Window window) override;

    private:
        bool mConfigShown = false;

        ConfigDialog* mConfigDialog = nullptr;
};
