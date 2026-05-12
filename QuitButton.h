
#pragma once


/**
 * Simple class to represent an X,Y pair.
 */
class QuitButton : public Button {
    public:
        QuitButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos) { }

        void draw(const Window window) override;
        void click(const Window window) const override;
};
