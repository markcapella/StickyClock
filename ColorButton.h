
#pragma once

#include <QMouseEvent>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>


/**
 * Simple class to represent a ColorButton
 */
class ColorButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor
        NOTIFY colorChanged)

public:
    explicit ColorButton(QWidget* parent = nullptr);
    
    QColor getColor() const;
    void setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QColor mColor;

    void updateButtonStyle();
};
