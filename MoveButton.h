
#pragma once


/**
 * Simple class to represent an X,Y pair.
 */
class MoveButton : public Button {
    public:
        MoveButton(const double mXPos, const double mYPos) :
            Button(mXPos, mYPos) {
            setDraggable(true);
        }

        void draw(const Window window) override;
        void click(const Window window) const override;
};
