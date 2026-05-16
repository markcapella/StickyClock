
#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSettings>
#include <QVBoxLayout>

/**
 * Simple class to represent a ConfigDialog.
 */
class ConfigDialog : public QDialog {
    Q_OBJECT

public:
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
    QFormLayout* mFormLayout;
    QVBoxLayout* mMainLayout;
    QDialogButtonBox* mOkCancelButtons;

    /**
     * Setup main layout containing QLineEdit per Setting.
     */
    void setupMainLayout();
};
