
#pragma once


/**
 * Simple class to represent an X,Y pair.
 */
class SizeButton : public Button {
    public:
        SizeButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos) {
            setSizeable(true);
        }

        void draw(const Window window) override;
        void click(const Window window) const override;
};
