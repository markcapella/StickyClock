
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
    const QString HTML_LINE_BR = "<br>";
    const QString HTML_DIV_END = "</div>";
    const QString HTML_END = "</div>";

    // Begin construction.
    QString TEXT = HTML_START + HTML_LINE_BR;

    // App name.
    const QString HTML_DIV_START_APP =
        "<div style='font-style: italic; font-weight: bold; "
        "font-size: 24px; color: #1ed760;'>";
    const QString HTML_LINE_1 = HTML_DIV_START_APP +
        QString(APP_NAME) + HTML_DIV_END;

    // App version.
    const QString HTML_DIV_START_VERSION = "<div>";
    const QString HTML_LINE_2 = HTML_DIV_START_VERSION +
        QString(APP_VERSION) + QString(" : ") +
        QString(APP_VERSION_MINOR) + HTML_DIV_END;
    TEXT += HTML_LINE_1 + HTML_LINE_2 + HTML_LINE_BR;

    // App description.
    const QString HTML_DIV_START_DESC =
        "<div style='font-style: italic; font-size: 20px;'>";
    const QString HTML_LINE_4 = HTML_DIV_START_DESC +
        QString(APP_DESC) + HTML_DIV_END;
    TEXT += HTML_LINE_4 + HTML_LINE_BR;

    // App author.
    const QString HTML_DIV_START_AUTHOR =
        "<div style='font-size: 14px;'>";
    const QString HTML_LINE_5 = HTML_DIV_START_AUTHOR +
        QString(APP_AUTHOR) + HTML_DIV_END;
    TEXT += HTML_LINE_5;

    // App author org.
    const QString HTML_LINE_6 = HTML_DIV_START_AUTHOR +
        QString(ORG_NAME) + HTML_DIV_END;
    TEXT += HTML_LINE_6 + HTML_LINE_BR;

    // Construct warning for Awesome Window Manager (?)
    const QString AWESOME_WM_NAME = "awesome";
    if (mXHelper->getWindowManagerName() == AWESOME_WM_NAME) {
        const QString HTML_DIV_START_AWESOME_WARNING =
            "<div style='font-size: 20px; color: blue;'>";
        const QString AWESOME_WM_WARNING = "Your Window Manager (WM) is "
            "'Awesome'. It won't allow you to click through StickyWidgetIII "
            " based widgets such as StickyClock.";
        TEXT += HTML_LINE_BR + HTML_DIV_START_AWESOME_WARNING + AWESOME_WM_WARNING;
    }

    // Finish the text & consume it.
    TEXT += HTML_END;
    setText(TEXT);

    // App source repo.
    const QString HTML_LINE_7 = HTML_DIV_START_AUTHOR +
        QString(SOURCE_REPO) + HTML_DIV_END;
    TEXT += HTML_LINE_7;

    // Set Dialog Ok and Repo buttons.
    setStandardButtons(QMessageBox::Ok);

    QPushButton* sourceRepo = addButton("Repo", QMessageBox::ActionRole);
    sourceRepo->disconnect();

    QObject::connect(sourceRepo, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl(SOURCE_REPO));
    });

    // Apply stylesheet & done.
    setStyleSheet(ABOUT_STYLESHEET);
}
