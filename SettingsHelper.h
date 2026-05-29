
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
    PREFERRED_DESKTOP_VALUETYPE
};

class SettingsHelper {
    public:
        typedef QString ConfigSettingName;

        static inline const ConfigSettingName CFP_PREFERRED_DESKTOP =
            "preferred_Desktop";
        static inline const ConfigSettingName CFG_SHOW_WEED_CLOCK =
            "showWeed_Clock";
        static inline const ConfigSettingName CFG_WEED_CLOCK_COLOR =
            "weedClock_Color";


        struct SettingsProperty {
            QString group = "";
            QString name = "";
            SettingsPropertyType valueType = NONE_VALUETYPE;
            QString initialValue = "";
        };

        static inline const vector<SettingsProperty> PROPERTIES = {
            { .group = "Configurable", .name = CFP_PREFERRED_DESKTOP,
                .valueType = PREFERRED_DESKTOP_VALUETYPE, .initialValue = "-1" },

            { .group = "Configurable", .name = CFG_SHOW_WEED_CLOCK,
                .valueType = BOOL_VALUETYPE, .initialValue = "true" },
            { .group = "Configurable", .name = CFG_WEED_CLOCK_COLOR,
                .valueType = COLOR_VALUETYPE, .initialValue = "#00faff" }
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
         * Getters & setters of window config mode.
         */
        bool getConfigMode();
        void setConfigMode(const bool state);

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