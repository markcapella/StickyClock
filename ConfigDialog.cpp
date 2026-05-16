
#include "Global.h"

/**
 * Simple class to represent a ConfigDialog.
 *
 */
ConfigDialog::ConfigDialog(const QString& filePath,
    QWidget* parent) : QDialog(parent) {

    setWindowTitle("Settings");
    setWindowFlags(windowFlags() | Qt::Tool);
    resize(400, 250);

    setupMainLayout();

    loadSettings();

    // Connect OK and Cancel buttons using modern Qt signals
    connect(mOkCancelButtons, &QDialogButtonBox::accepted, this,
        &ConfigDialog::saveSettings);
    connect(mOkCancelButtons, &QDialogButtonBox::rejected, this,
        &ConfigDialog::reject);
}

/**
 * Load all Settings from .ini file.
 */
void ConfigDialog::loadSettings() {
    mSettingsHelper->getQSettings()->beginGroup("Configurable");

    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
        const QWidget* LABEL_WIDGET = mFormLayout->itemAt(
            i, QFormLayout::LabelRole)->widget();
        QLineEdit* fieldWidget = qobject_cast<QLineEdit*>
            (mFormLayout->itemAt(i, QFormLayout::FieldRole)->widget());

        if (LABEL_WIDGET && fieldWidget) {
            const QString THIS_KEY = LABEL_WIDGET->
                property("text").toString();

            QString defaultValue = "";
            SettingsHelper::SettingsPropertyType valueType;

            const int SETTINGS_SIZE = SettingsHelper::
                SETTINGS_PROPERTIES.size();
            for (int ii = 0; ii < SETTINGS_SIZE; ii++) {
                const SettingsHelper::SettingsProperty THIS_SETTING =
                    SettingsHelper::SETTINGS_PROPERTIES[ii];
                if (THIS_KEY == THIS_SETTING.name) {
                    valueType = THIS_SETTING.valueType;
                    defaultValue = THIS_SETTING.initialValue;
                    //cout << "loadSettings reads  STRING: [" <<
                    //     THIS_KEY.toStdString() <<
                    //    "] = [" << defaultValue.toStdString() <<
                    // "]." << endl;
                    break;
                }
            }

            QString value = mSettingsHelper->getQSettings()->
                value(THIS_KEY, defaultValue).toString();
            if (valueType == SettingsHelper::BOOL_VALUETYPE) {
                value = (value == "true") ? "true" : "false";
            }
            fieldWidget->setText(value);
        }
    }

    mSettingsHelper->getQSettings()->endGroup();
}

/**
 * Save all Settings back to .ini file.
 */
void ConfigDialog::saveSettings() {
    mSettingsHelper->getQSettings()->beginGroup("Configurable");

    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
        const QWidget* LABEL_WIDGET = mFormLayout->itemAt(i,
            QFormLayout::LabelRole)->widget();
        QLineEdit* fieldWidget = qobject_cast<QLineEdit*>
            (mFormLayout->itemAt(i, QFormLayout::FieldRole)->widget());

        if (LABEL_WIDGET && fieldWidget) {
            const QString key = LABEL_WIDGET->property("text").toString();
            const QString value = fieldWidget->text();
            mSettingsHelper->getQSettings()->setValue(key, value);
        }
    }

    mSettingsHelper->getQSettings()->endGroup();
    accept(); 
}

/**
 * Setup main layout containing QLineEdit per Setting.
 */
void ConfigDialog::setupMainLayout() {
    mMainLayout = new QVBoxLayout(this);
    mFormLayout = new QFormLayout();

    // Get all keys.
    mSettingsHelper->getQSettings()->beginGroup("Configurable");
    const QStringList ALL_KEYS = mSettingsHelper->
        getQSettings()->allKeys();

    // Construct QLineEdit mFormLayout from keys
    // & add to mMainLayout.
    for (const QString& THIS_KEY : ALL_KEYS) {
        QLineEdit* lineEdit = new QLineEdit(this);
        lineEdit->setObjectName(THIS_KEY);
        mFormLayout->addRow(THIS_KEY, lineEdit);
    }
    mMainLayout->addLayout(mFormLayout);
    mSettingsHelper->getQSettings()->endGroup();

    // Create Ok / Cancel ButtonBox and add to mMainLayout.
    mOkCancelButtons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mMainLayout->addWidget(mOkCancelButtons);

    // Set mMainLayout as "the Layout" & done.
    setLayout(mMainLayout);
}
