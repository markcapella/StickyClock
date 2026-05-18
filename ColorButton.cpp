
#include "Global.h"

/**
 * Simple class to represent a ColorButton.
 *
 */
ColorButton::ColorButton(QWidget* parent) :
    QPushButton(parent), mColor(Qt::white) {

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    updateButtonStyle();
}

QColor
ColorButton::getColor() const {
    return mColor;
}

void
ColorButton::setColor(const QColor& color) {
    if (mColor != color) {
        mColor = color;
        updateButtonStyle();
        emit colorChanged(mColor);
    }
}

void 
ColorButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        const QColor selectedColor = QColorDialog::getColor(mColor,
            this, "Select Color", QColorDialog::ShowAlphaChannel);

        if (selectedColor.isValid()) {
            setColor(selectedColor);
        }
    }
    QPushButton::mousePressEvent(event);
}

void
ColorButton::updateButtonStyle() {
    setObjectName("ColorButton");

    const QString CSS = QString("ColorButton#%1 { "
        "margin-left: 1px; background-color: %2; "
        "border: 1px solid #555; border-radius: 4px; "
        "width: 15px; height: 15px;"
        "max-width: 15px; max-height: 15px; }").arg(objectName()).
        arg(mColor.name(QColor::HexArgb));

    setStyleSheet(CSS + " ColorButton { color: white; }");
}
