
#pragma once

/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 */
enum SettingsPropertyType {
    NONE_VALUETYPE,
    STRING_VALUETYPE,
    INT_VALUETYPE,
    BOOL_VALUETYPE,
    COLOR_VALUETYPE,
    SLIDER_VALUETYPE
};

class SettingsHelper {

    public:
        static inline const QString GROUP_CONFIGURABLE = APP_NAME;
        static inline const QString GROUP_VOLATILE = "Volatile";

        // Configurable.
        static inline const QString AUTOHIDE_CONTROLS = "Auto hide Buttons";
        static inline const QString AUTOHIDE_DELAY = "Auto hide Delay";
        static inline const QString DRAG_THRU_DESKTOPS = "Drag thru Desktops";
        static inline const QString PREFERRED_DESKTOP = "Preferred Desktop";
        static inline const QString ON_TOP_INSTEAD = "On Top Instead";
        static inline const QString SHOW_WEED_CLOCK = "Show Weedtime";
        static inline const QString WEED_CLOCK_COLOR = "Weedtime color";

        // Volatile.
        static inline const QString CONFIG_MODE = "In Config mode";


        struct SettingsProperty {
            QString group = "";
            QString name = "";
            SettingsPropertyType valueType = NONE_VALUETYPE;
            QString initialValue = "";
            int rangeMinimum = 0;
            int rangeMaximum = 0;
        };

        static inline const vector<SettingsProperty> PROPERTIES = {
            // App configurables.
            { .group = GROUP_CONFIGURABLE, .name = AUTOHIDE_CONTROLS,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = AUTOHIDE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "4",
              .rangeMinimum = 1, .rangeMaximum = 9
            },

            { .group = GROUP_CONFIGURABLE, .name = DRAG_THRU_DESKTOPS,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = PREFERRED_DESKTOP,
              .valueType = SLIDER_VALUETYPE, .initialValue = "-1",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = ON_TOP_INSTEAD,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = SHOW_WEED_CLOCK,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .group = GROUP_CONFIGURABLE, .name = WEED_CLOCK_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "#00faff",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            // Volatile internals, hidden from ConfigDialog.
            { .group = GROUP_VOLATILE, .name = CONFIG_MODE,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = 0, .rangeMaximum = 1
            }
        };

        SettingsHelper();
        ~SettingsHelper();

        /**
         * Getters for window minimum Width & Height.
         */
        double getWindowMinimumWidth();
        double getWindowMinimumHeight();

        /**
         * Setters for window minimum Width & Height.
         */
        void setWindowMinimumWidth(const double width);
        void setWindowMinimumHeight(const double height);

        /**
         * Getters for window x & y, w & h.
         */
        double getWindowXPos();
        double getWindowYPos();
        double getWindowWidth();
        double getWindowHeight();

        /**
         * Setters for window x & y, w & h.
         */
        void setWindowXPos(const double xPos);
        void setWindowYPos(const double yPos);
        void setWindowWidth(const double width);
        void setWindowHeight(const double height);

        /**
         * Getters for canvas x & y, w & h.
         */
        double getCanvasXPos();
        double getCanvasYPos();
        double getCanvasWidth();
        double getCanvasHeight();

        /**
         * Setters for canvas x & y, w & h.
         */
        void setCanvasXPos(const double xPos);
        void setCanvasYPos(const double yPos);
        void setCanvasWidth(const double width);
        void setCanvasHeight(const double height);

        /**
         * Helper to return a QSettings filename from appName.
         */
        QString getQSettingsFile();

        /**
         * Helper to return a new QSettings object for pref
         * access based on our appName.
         */
        QSettings* getQSettings();

        /**
         * Getters & setters for user configurable bool settings.
         */
        bool getBoolSetting(const QString setting);
        void setBoolSetting(const QString setting, const bool value);

        /**
         * Getter for user configurable int settings.
         */
        int getIntSetting(const QString setting);

        /**
         * Setter for user configurable int settings.
         */
        void setIntSetting(const QString setting, int value);

        /**
         * Getters & setters for user configurable XRenderColor settings.
         */
        XRenderColor getColorSetting(const QString setting);

        /**
         * Each runtime start we ensure Settings keys & default values
         * are flushed to .ini file for ConfigDialog to load & modify.
         */
        void ensureSettingsAreConfigurable();

        /**
         * Return the group of a Setting by key.
         */
        QString getSettingsGroup(const QString key);

        /**
         * Return the value type of a Setting by key.
         */
        SettingsPropertyType getSettingsValueType(const QString key);

        /**
         * Return the default value of a Setting by key.
         */
        QString getSettingsDefaultValue(const QString key);

        /**
         * Get a Minimum int value to load a UI widget.
         */
        int getSettingsIntRangeMinimum(const QString key);

        /**
         * Get a Maximum int value to load a UI widget.
         */
        int getSettingsIntRangeMaximum(const QString key);

    private:
        // Members.
        QString mSettingsApp = "";

        QSettings* mQSettings = nullptr;
};