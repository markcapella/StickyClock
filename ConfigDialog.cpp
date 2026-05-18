
#include "Global.h"

/**
 * Simple class to represent a ConfigDialog.
 *
 */
ConfigDialog::ConfigDialog(const QString& filePath,
    QWidget* parent) : QDialog(parent) {

    setWindowTitle(QString(APP_NAME) + QString(" Settings"));
    setWindowFlags(windowFlags() | Qt::Tool);

    setMinimumWidth(CONFIG_DIALOG_MIX_WIDTH);
    setMaximumHeight(CONFIG_DIALOG_MAX_HEIGHT);
    setupMainLayout();

    loadSettings();

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
        if (!LABEL_WIDGET) {
            continue;
        }

        // Get key.
        const QString THIS_KEY = LABEL_WIDGET->
            property("text").toString();

        // Get key valueType.
        const SettingsPropertyType THIS_VALUETYPE =
            mSettingsHelper->getSettingsValueType(THIS_KEY);

        // Get key defaultValue.
        const QString THIS_DEFAULT_VALUE =
            mSettingsHelper->getSettingsDefaultValue(THIS_KEY);

        // Get EditWidget for keyType.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = qobject_cast<QCheckBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (checkboxWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                checkboxWidget->setCheckState(VALUE == "true" ?
                    Qt::Checked : Qt::Unchecked );
            }
            continue;
        }

        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (lineEditWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                lineEditWidget->setText(VALUE);
            }
            continue;
        }

        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = qobject_cast<ColorButton*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (colorButtonWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                colorButtonWidget->setColor(QColor(VALUE));
            }
            continue;
        }

        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (stringEditWidget) {
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                stringEditWidget->setText(VALUE);
            }
            continue;
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
        if (!LABEL_WIDGET) {
            continue;
        }

        // Get key.
        const QString THIS_KEY = LABEL_WIDGET->
            property("text").toString();

        // Get key valueType.
        const SettingsPropertyType THIS_VALUETYPE =
            mSettingsHelper->getSettingsValueType(THIS_KEY);

        // Get key defaultValue.
        const QString THIS_DEFAULT_VALUE =
            mSettingsHelper->getSettingsDefaultValue(THIS_KEY);

        // Get EditWidget for keyType.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = qobject_cast<QCheckBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (checkboxWidget) {
                const bool VALUE = checkboxWidget->checkState();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (lineEditWidget) {
                const QString VALUE = lineEditWidget->text();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = qobject_cast<ColorButton*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (colorButtonWidget) {
                const QString VALUE = colorButtonWidget->
                    getColor().name();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
        }

        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (stringEditWidget) {
                const QString VALUE = stringEditWidget->text();
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
            }
            continue;
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
    mMainLayout->setSpacing(5);

    mFormLayout = new QFormLayout();
    mFormLayout->setFormAlignment(
        Qt::AlignHCenter | Qt::AlignVCenter);

    // Get all keys.
    mSettingsHelper->getQSettings()->beginGroup("Configurable");
    const QStringList ALL_KEYS = mSettingsHelper->
        getQSettings()->allKeys();

    // Construct QLineEdit mFormLayout from keys
    // & add to mMainLayout.
    for (const QString& THIS_KEY : ALL_KEYS) {

        // Get key valueType.
        const SettingsPropertyType THIS_VALUETYPE =
            mSettingsHelper->getSettingsValueType(THIS_KEY);

        // Get key defaultValue.
        const QString THIS_DEFAULT_VALUE =
            mSettingsHelper->getSettingsDefaultValue(THIS_KEY);

        // Get EditWidget for keyType.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = new QCheckBox(this);
            checkboxWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(THIS_KEY, checkboxWidget);
            continue;
        }

        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = new QLineEdit(this);
            lineEditWidget->setObjectName(THIS_KEY);
            lineEditWidget->setFixedWidth(120);
            mFormLayout->addRow(THIS_KEY, lineEditWidget);
            continue;
        }

        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = new ColorButton(this);
            colorButtonWidget->setObjectName(THIS_KEY);
            connect(colorButtonWidget, &ColorButton::colorChanged,
                [](const QColor& color) {
                // qDebug() << "New color chosen:" << color;
            } );
            mFormLayout->addRow(THIS_KEY, colorButtonWidget);
            continue;
        }

        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = new QLineEdit(this);
            stringEditWidget->setFixedWidth(360);
            stringEditWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(THIS_KEY, stringEditWidget);
            continue;
        }
    }

    QWidget* formContainer = new QWidget();
    formContainer->setLayout(mFormLayout);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(formContainer);
    scrollArea->setWidgetResizable(true);

    mMainLayout->addWidget(scrollArea);
    mSettingsHelper->getQSettings()->endGroup();

    // Create Ok / Cancel ButtonBox and add to mMainLayout.
    mOkCancelButtons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mMainLayout->addWidget(mOkCancelButtons);

    // Set mMainLayout as "the Layout" & done.
    setLayout(mMainLayout);
}