
#pragma once


/**
 * Simple class to represent an X,Y pair.
 */
class PinButton : public Button {
    public:
        PinButton(const double mXPos, const double mYPos);
        ~PinButton() override;

        void draw(const Window window) override;
        void click(const Window window) const override;
};
