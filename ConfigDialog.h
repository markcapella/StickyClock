
#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

#include <X11/Xutil.h>

/**
 * Simple class to represent a ConfigDialog.
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

        /**
         * Load UI form with values from .Ini.
         */
        void loadConfigFormSettings();

        /**
         * Gettters / Setters for StickyWindow.
         */
        Window getWindow() const {
            return mWindow;
        }
        void setWindow(const Window window) {
            mWindow = window;
        }

        /**
         * Update any runtime dialog controls, range settings, etc.
         */
        void updateControls();

    /**
     * Override eventFilter for QSlider hover action.
     */
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override;

    private:
        Window mWindow = None;
        QFormLayout* mFormLayout = nullptr;
        QVBoxLayout* mMainLayout = nullptr;

        QDialogButtonBox* mConfigButtonBox = nullptr;
        QPushButton* mAboutButton = nullptr;
        QDialog* mAboutDialog = nullptr;

        /**
         * Build the UI form layout.
         */
        void buildConfigForm();

        /**
         * Callback to Save UI form values to .Ini.
         */
        void saveConfigFormSettings();

        /**
         * Show this apps "About" dialog.
         */
        void about();
};
