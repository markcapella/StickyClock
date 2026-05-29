
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

    // public slots:
        /**
         * Load UI form with values from .Ini.
         */
        void loadConfigFormSettings();

        Window getWindow() const { return mWindow; }
        void setWindow(const Window window) {
            mWindow = window; }

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
         * Callback to Save UI form values to .Ini.
         */
        void saveConfigFormSettings();

        /**
         * Build the UI form layout.
         */
        void buildConfigForm();

        /**
         * Show this apps "About" dialog.
         */
        void about();
};
