
#include "Global.h"

/**
 * Simple class to represent a ConfigDialog.
 */
ConfigDialog::ConfigDialog(const QString& filePath,
    QWidget* parent) : QDialog(parent) {

    // Set the window title.
    const QString FIRST_RECENTS_NAME = mRecentsHelper->RECENTS_NAMES[0];
    const QString APP_RECENT_NAME = mRecentsHelper->getAppRecentsName();

    QString TITLE = QString(APP_NAME);
    if (APP_RECENT_NAME != FIRST_RECENTS_NAME) {
        TITLE += " " + APP_RECENT_NAME;
    }
    TITLE += " Settings";
    setWindowTitle(QString(TITLE));

    // Set the window attributes.
    setWindowFlags(Qt::Dialog | Qt::Tool);
    setMinimumWidth(CONFIG_DIALOG_MIX_WIDTH);
    setMaximumHeight(CONFIG_DIALOG_MAX_HEIGHT);

    buildConfigForm();

    // Callback to Save UI form values to .Ini.
    connect(mConfigButtonBox, &QDialogButtonBox::accepted, this,
        &ConfigDialog::saveConfigFormSettings);
    connect(mConfigButtonBox, &QDialogButtonBox::rejected, this,
        &ConfigDialog::reject);

    connect(mAboutButton, &QPushButton::clicked, this,
        &ConfigDialog::about);
}

/**
 * Load UI form with values from .Ini.
 */
void
ConfigDialog::loadConfigFormSettings() {
    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
        const QWidget* LABEL_WIDGET = mFormLayout->itemAt(
            i, QFormLayout::LabelRole)->widget();
        if (!LABEL_WIDGET) {
            continue;
        }

        // Get key, valueType, & defaultValue.
        const QString THIS_KEY = LABEL_WIDGET->
            property("text").toString();
        const SettingsPropertyType THIS_VALUETYPE =
            mSettingsHelper->getSettingsValueType(THIS_KEY);
        const QString THIS_DEFAULT_VALUE =
            mSettingsHelper->getSettingsDefaultValue(THIS_KEY);

        // Get QLineEdit for Strings.
        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (stringEditWidget) {
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                mSettingsHelper->getQSettings()->endGroup();
                stringEditWidget->setText(VALUE);
            }
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (lineEditWidget) {
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                const int VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toInt();
                mSettingsHelper->getQSettings()->endGroup();
                lineEditWidget->setText(QString::number(VALUE));
            }
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = qobject_cast<QCheckBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (checkboxWidget) {
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                mSettingsHelper->getQSettings()->endGroup();
                checkboxWidget->setCheckState(VALUE == "true" ?
                    Qt::Checked : Qt::Unchecked );
            }
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = qobject_cast<ColorButton*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (colorButtonWidget) {
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                const QString VALUE = mSettingsHelper->getQSettings()->
                    value(THIS_KEY, THIS_DEFAULT_VALUE).toString();
                mSettingsHelper->getQSettings()->endGroup();
                colorButtonWidget->setButtonColor(QColor(VALUE));
            }
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = nullptr;
            sliderEditWidget = qobject_cast<QSlider*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (sliderEditWidget) {
                sliderEditWidget->setMinimum(mSettingsHelper->
                    getSettingsIntRangeMinimum(THIS_KEY));
                sliderEditWidget->setMaximum(mSettingsHelper->
                    getSettingsIntRangeMaximum(THIS_KEY));
                    const int VALUE = mSettingsHelper->getIntSetting(
                        THIS_KEY);
                    sliderEditWidget->setSliderPosition(VALUE);
            }
            continue;
        }
    }
}

/**
 * Update any runtime dialog controls, range settings, etc.
 */
void
ConfigDialog::updateControls() {
    // Update preferred desktop slider, if user changes
    // maximum desktop value.
    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
        const QWidget* LABEL_WIDGET = mFormLayout->itemAt(
            i, QFormLayout::LabelRole)->widget();
        if (!LABEL_WIDGET) {
            continue;
        }

        // Ignore if wrong key.
        const QString THIS_KEY = LABEL_WIDGET->property(
            "text").toString();
        if (THIS_KEY != SettingsHelper::PREFERRED_DESKTOP) {
            continue;
        }

        // Ignore if control type mismatch :-/
        QSlider* sliderEditWidget = nullptr;
        sliderEditWidget = qobject_cast<QSlider*>(mFormLayout->
            itemAt(i, QFormLayout::FieldRole)->widget());
        if (!sliderEditWidget) {
            continue;
        }

        sliderEditWidget->setMaximum(
            mXHelper->getMaximumDesktops() - 1);
    }
}

/**
 * Override eventFilter for QSlider hover action & tooltip.
 */
bool
ConfigDialog::eventFilter(QObject* obj, QEvent* event) {
    // Intercept both static hover (ToolTip event)
    // and active movement (MouseMove).
    QSlider* slider = qobject_cast<QSlider*> (obj);
    if (slider) {
        if (event->type() == QEvent::ToolTip ||
            event->type() == QEvent::MouseMove) {
            if (!slider->isSliderDown()) { 
                QToolTip::showText(QCursor::pos(),
                    QString::number(slider->value()), slider);
            }
            return true;
        }
    }

    return ConfigDialog::eventFilter(obj, event);
}

/**
 * Build the UI form layout.
 */
void
ConfigDialog::buildConfigForm() {
    // Build form.
    mFormLayout = new QFormLayout();
    const int FORM_TOP_BOTTOM_SPACING = 30;
    const int FORM_LAYOUT_ROW_SPACING = 10;

    mFormLayout->setContentsMargins(0, FORM_TOP_BOTTOM_SPACING,
        0, FORM_TOP_BOTTOM_SPACING);
    mFormLayout->setVerticalSpacing(FORM_LAYOUT_ROW_SPACING);

    // Begin group, get all keys, close group.
    mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
    const QStringList ALL_KEYS = mSettingsHelper->
        getQSettings()->allKeys();
    mSettingsHelper->getQSettings()->endGroup();

    // Construct edit widget rows from keys & add to mFormLayout.
    for (const QString& THIS_KEY : ALL_KEYS) {
        // Get key valueType & key defaultValue.
        const SettingsPropertyType THIS_VALUETYPE =
            mSettingsHelper->getSettingsValueType(THIS_KEY);

        // Get QLineEdit for Strings.
        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = new QLineEdit(this);
            stringEditWidget->setObjectName(THIS_KEY);
            stringEditWidget->setFixedWidth(360);
            mFormLayout->addRow(THIS_KEY, stringEditWidget);
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = new QLineEdit(this);
            lineEditWidget->setObjectName(THIS_KEY);
            lineEditWidget->setFixedWidth(120);
            mFormLayout->addRow(THIS_KEY, lineEditWidget);
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = new QCheckBox(this);
            checkboxWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(THIS_KEY, checkboxWidget);
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = new ColorButton(this);
            colorButtonWidget->setObjectName(THIS_KEY);
            mFormLayout->addRow(THIS_KEY, colorButtonWidget);
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = nullptr;
            sliderEditWidget = new QSlider(Qt::Horizontal, this);
            sliderEditWidget->setObjectName(THIS_KEY);
            sliderEditWidget->setFixedWidth(120);
            mFormLayout->addRow(THIS_KEY, sliderEditWidget);

            // Nice tooltip on slow hover.
            if (THIS_KEY == SettingsHelper::AUTOHIDE_DELAY) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT = QString::number(value) +
                        " seconds";
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
            }
            if (THIS_KEY == SettingsHelper::PREFERRED_DESKTOP) {
                connect(sliderEditWidget, &QSlider::valueChanged,
                    sliderEditWidget, [sliderEditWidget] (int value) {
                    const QString TOOLTIP_TEXT = (value == -1) ?
                        "All" : "Desktop " + QString::number(value + 1);
                    QToolTip::showText(QCursor::pos(), TOOLTIP_TEXT,
                        sliderEditWidget);
                });
            }

            // Nice tooltip on immediate hover & change.
            if (THIS_KEY == SettingsHelper::AUTOHIDE_DELAY) {
                struct LambdaFilter : public QObject {
                    QSlider* s;
                    LambdaFilter(QSlider* slider) :
                        QObject(slider), s(slider) {}
                    bool eventFilter(QObject* obj, QEvent *event) override {
                        // Show the immediate millisecond the mouse
                        // crosses into the slider.
                        if (event->type() == QEvent::Enter) {
                            if (!s->isSliderDown()) {
                                const int VALUE = s->value();
                                const QString TOOLTIP_TEXT =
                                    QString::number(VALUE) + " seconds";
                                QToolTip::showText(QCursor::pos(),
                                    TOOLTIP_TEXT, s);
                            }
                            return false;
                        }
                        // Show also on interaction.
                        if (event->type() == QEvent::ToolTip ||
                            event->type() == QEvent::MouseMove) {
                            if (!s->isSliderDown()) {
                                const int VALUE = s->value();
                                const QString TOOLTIP_TEXT =
                                    QString::number(VALUE) + " seconds";
                                QToolTip::showText(QCursor::pos(),
                                    TOOLTIP_TEXT, s);
                            }
                        }
                        return false;
                    }
                };
                sliderEditWidget->installEventFilter(
                    new LambdaFilter(sliderEditWidget));
            }
            if (THIS_KEY == SettingsHelper::PREFERRED_DESKTOP) {
                struct LambdaFilter : public QObject {
                    QSlider* s;
                    LambdaFilter(QSlider* slider) :
                        QObject(slider), s(slider) {}
                    bool eventFilter(QObject* o, QEvent* e) override {
                        // Show the immediate millisecond the mouse
                        // crosses into the slider.
                        if (e->type() == QEvent::Enter) {
                            if (!s->isSliderDown()) {
                                const int VALUE = s->value();
                                const QString TOOLTIP_TEXT =
                                    (VALUE == -1) ? "All" : "Desktop " +
                                        QString::number(VALUE + 1);
                                QToolTip::showText(QCursor::pos(),
                                    TOOLTIP_TEXT, s);
                            }
                            return false;
                        }
                        // Show also on interaction.
                        if (e->type() == QEvent::ToolTip ||
                            e->type() == QEvent::MouseMove) {
                            if (!s->isSliderDown()) {
                                const int VALUE = s->value();
                                const QString TOOLTIP_TEXT =
                                    (VALUE == -1) ? "All" : "Desktop " +
                                        QString::number(VALUE + 1);
                                QToolTip::showText(QCursor::pos(),
                                    TOOLTIP_TEXT, s);
                            }
                        }
                        return false;
                    }
                };
                sliderEditWidget->installEventFilter(
                    new LambdaFilter(sliderEditWidget));
            }
        }
    }

    // Add the formlayout to a formcontainer.
    QWidget* formContainer = new QWidget();
    mFormLayout->setFormAlignment(Qt::AlignCenter);
    formContainer->setLayout(mFormLayout);

    // Add the formcontainer to a scrollarea.
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(formContainer);
    scrollArea->setWidgetResizable(true);

    // The whole thing wraps up into vbox layout.
    mMainLayout = new QVBoxLayout(this);
    mMainLayout->addWidget(scrollArea);

    // Create Ok / Cancel ButtonBoxBox with an About button.
    mConfigButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
        QDialogButtonBox::Cancel, this);

    // Create AboutDialog for About button dialog.
    mAboutDialog = new AboutDialog(this);
    mAboutButton = new QPushButton(ABOUT_STRING);
    mConfigButtonBox->addButton(mAboutButton,
        QDialogButtonBox::ActionRole);

    // Set mMainLayout as "the Layout" & done.
    mMainLayout->addWidget(mConfigButtonBox);
    setLayout(mMainLayout);
}

/**
 * Callback to Save UI form values to .Ini.
 */
void
ConfigDialog::saveConfigFormSettings() {
    for (int i = 0; i < mFormLayout->rowCount(); ++i) {
        const QWidget* LABEL_WIDGET = mFormLayout->itemAt(i,
            QFormLayout::LabelRole)->widget();
        if (!LABEL_WIDGET) {
            continue;
        }

        // Get key, valueType, & defaultValue.
        const QString THIS_KEY = LABEL_WIDGET->
            property("text").toString();
        const SettingsPropertyType THIS_VALUETYPE =
            mSettingsHelper->getSettingsValueType(THIS_KEY);
        const QString THIS_DEFAULT_VALUE =
            mSettingsHelper->getSettingsDefaultValue(THIS_KEY);

        // Get QLineEdit for Strings.
        if (THIS_VALUETYPE == STRING_VALUETYPE) {
            QLineEdit* stringEditWidget = nullptr;
            stringEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (stringEditWidget) {
                const QString VALUE = stringEditWidget->text();
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
                mSettingsHelper->getQSettings()->endGroup();
            }
            continue;
        }

        // Get QlineEdit for Ints.
        if (THIS_VALUETYPE == INT_VALUETYPE) {
            QLineEdit* lineEditWidget = nullptr;
            lineEditWidget = qobject_cast<QLineEdit*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (lineEditWidget) {
                const int VALUE = lineEditWidget->text().toInt();
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
                mSettingsHelper->getQSettings()->endGroup();
            }
            continue;
        }

        // Get QCheckBox for Booleans.
        if (THIS_VALUETYPE == BOOL_VALUETYPE) {
            QCheckBox* checkboxWidget = nullptr;
            checkboxWidget = qobject_cast<QCheckBox*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (checkboxWidget) {
                const bool VALUE = checkboxWidget->checkState();
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
                mSettingsHelper->getQSettings()->endGroup();

                if (THIS_KEY == SettingsHelper::ON_TOP_INSTEAD) {
                    mXHelper->makeWindowStayOnTop(getWindow(), VALUE);
                    mXHelper->makeWindowStayOnBottom(getWindow(), !VALUE);
                    continue;
                }
            }
            continue;
        }

        // Get QColorButton for Colors.
        if (THIS_VALUETYPE == COLOR_VALUETYPE) {
            ColorButton* colorButtonWidget = nullptr;
            colorButtonWidget = qobject_cast<ColorButton*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (colorButtonWidget) {
                const QString VALUE = colorButtonWidget->
                    getButtonColor().name();
                mSettingsHelper->getQSettings()->beginGroup(APP_NAME);
                mSettingsHelper->getQSettings()->
                    setValue(THIS_KEY, VALUE);
                mSettingsHelper->getQSettings()->endGroup();
            }
            continue;
        }

        // Get QSlider for preferredDesktop.
        if (THIS_VALUETYPE == SLIDER_VALUETYPE) {
            QSlider* sliderEditWidget = nullptr;
            sliderEditWidget = qobject_cast<QSlider*>(mFormLayout->
                itemAt(i, QFormLayout::FieldRole)->widget());
            if (sliderEditWidget) {
                const int VALUE = sliderEditWidget->value();
                mSettingsHelper->setIntSetting(THIS_KEY, VALUE);
                // Ensure setting against invalidation by OS.
                if (THIS_KEY == SettingsHelper::PREFERRED_DESKTOP) {
                    mStickyWindow->rangeCheckPreferredDesktopSetting(getWindow());
                }
            }
            continue;
        }
    }

    // And done.
    accept();
}

/**
 * Show this apps "About" dialog.
 */
void
ConfigDialog::about() {
    mAboutDialog->show();
}
