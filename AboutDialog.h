
#pragma once

#include <QMessageBox>

/**
 * Simple class to represent an AboutDialog.
 *
 */
class AboutDialog : public QMessageBox {
    Q_OBJECT

    public:
        static inline const QString ABOUT_STYLESHEET = "QLabel{ "
            "width: 500px; min-width: 500px; height: 100px; }";

        explicit AboutDialog(QWidget* parent = nullptr);

    private:
        /**
         * Create a styled AboutDialog for this StickyWidgetIII.
         */
        void init();
};
