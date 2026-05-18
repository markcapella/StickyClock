
#include "Global.h"

/**
 * Simple class to represent a ColorDialog.
 *
 */
class ColorDialog : public QPushButton {
    Q_OBJECT

public:
    explicit ColorDialog(const QColor& initialColor = Qt::white,
        QWidget* parent = nullptr) : QPushButton(parent),
        mColor(Qt::white) {

        updateButtonStyle();
        connect(this, &QPushButton::clicked, this,
            &ColorDialog::chooseColor);
    }

    QColor getColor() const {
        return mColor;
    }

signals:
    void colorChanged(const QColor& color);

private slots:
    void chooseColor() {
        QColor selectedColor = QColorDialog::getColor(
            mColor, this, tr("Select Color"));

        if (selectedColor.isValid()) {
            mColor = selectedColor;
            updateButtonStyle();
            emit colorChanged(mColor);
        }
    }

private:
    QColor mColor;

    void updateButtonStyle() {
        QString colorString = mColor.name(QColor::HexArgb);

        setStyleSheet(QString("background-color: %1; border: 1px "
            "solid gray; border-radius: 4px;").arg(colorString));
    }
};

#include "ColorDialog.moc"
