
#include "Global.h"

/**
 * Simple class to represent an AboutDialog.
 *
 */
AboutDialog::AboutDialog(QWidget* parent) :
    QMessageBox(parent) {

    init();
}

/**
 * Create a styled AboutDialog for this StickyWidgetIII.
 */
void
AboutDialog::init() {
    setWindowTitle(QString("About ") + QString(APP_NAME));
    const QString HTML_START = "<div align='center'>";
    const QString HTML_END = "</div>";

    const QString HTML_DIV_START = "<div>";
    const QString HTML_DIV_END = "</div>";

    const QString HTML_LINE_BR = "<br>";

    // App name.
    const QString HTML_LINE_1 = HTML_DIV_START +
        QString(APP_NAME) + HTML_DIV_END;

    // App version.
    const QString HTML_LINE_2 = HTML_DIV_START + QString(APP_VERSION) +
        QString(" : ") + QString(APP_VERSION_MINOR) + HTML_DIV_END;
    const QString HTML_LINE_3 = HTML_LINE_BR;

    // App description.
    const QString HTML_LINE_4 = HTML_DIV_START +
        QString(APP_DESC) + HTML_DIV_END;
    const QString HTML_LINE_5 = HTML_LINE_BR;

    // App author.
    const QString HTML_LINE_6 = HTML_DIV_START +
        QString(APP_AUTHOR) + HTML_DIV_END;

    // App author org.
    const QString HTML_LINE_7 = HTML_DIV_START +
        QString(ORG_NAME) + HTML_DIV_END;
    const QString HTML_LINE_8 = HTML_LINE_BR;

    const QString TEXT = HTML_START + HTML_LINE_BR +
        HTML_LINE_1 + HTML_LINE_2 + HTML_LINE_BR +
        HTML_LINE_4 + HTML_LINE_BR +
        HTML_LINE_6 + HTML_LINE_7 + HTML_LINE_BR +
        HTML_END;

    setStyleSheet(ABOUT_STYLESHEET);
    setText(TEXT);
}
