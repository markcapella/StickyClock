
#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>


/**
 * Simple class to represent a ConfigDialog.
 *
 */
class ConfigDialog : public QDialog {
    Q_OBJECT

    public:
        static inline const long CONFIG_DIALOG_MIX_WIDTH = 300;
        static inline const long CONFIG_DIALOG_MAX_HEIGHT = 800;

        static inline const QString ABOUT_STRING = "About";

        // Constructor.
        explicit ConfigDialog(const QString& filePath,
            QWidget* parent = nullptr);

    public slots:
        /**
         * Load all Settings from .ini file.
         */
        void loadSettings();

        /**
         * Save all Settings back to .ini file.
         */
        void saveSettings();

    private:
        QFormLayout* mFormLayout = nullptr;
        QVBoxLayout* mMainLayout = nullptr;

        QDialogButtonBox* mConfigButtonBox = nullptr;
        QPushButton* mAboutButton = nullptr;
        QDialog* mAboutDialog = nullptr;

        /**
         * Setup main layout containing QLineEdit per Setting.
         */
        void setupMainLayout();

        /**
         * Show this apps "About" dialog.
         */
        void about();
};
