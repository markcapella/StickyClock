
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
    SLIDER_VALUETYPE,
    DIVIDER_VALUETYPE
};

class SettingsHelper {
    #define IC_QString static inline const QString

    public:
        // Configurable.
        IC_QString AUTOHIDE_CONTROLS = "Auto hide Controls";
        IC_QString AUTOHIDE_DELAY = "Auto hide Delay";
        IC_QString PREFERRED_DESKTOP = "Preferred Desktop";
        IC_QString ALLOW_DESKTOP_DRAG = "Allow Desktop Drag";
        IC_QString ON_TOP_INSTEAD = "Stick to Top";
        IC_QString DIVIDER_1 = "divider01";

        IC_QString PANEL_COLOR = "Background Color";
        IC_QString PANEL_OPACITY = "Background Opacity";
        IC_QString PANEL_OUTLINE_COLOR = "Outline Color";
        IC_QString PANEL_OUTLINE_OPACITY = "Outline Opacity";
        IC_QString DIVIDER_2 = "divider02";

        IC_QString TEXT_SIZE = "Text Size";
        IC_QString TEXT_COLOR = "Text Color";
        IC_QString TEXT_OPACITY = "Text Opacity";
        IC_QString DIVIDER_3 = "divider03";

        IC_QString SHOW_WEED_CLOCK = "Observe Weedclock Time";
        IC_QString WEED_CLOCK_COLOR = "Weedtime Color";
        IC_QString WEED_CLOCK_OPACITY = "Weedtime Opacity";


        // Settings property struct.
        struct SettingsProperty {
            QString name = "";
            SettingsPropertyType valueType = NONE_VALUETYPE;
            QString initialValue = "";
            int rangeMinimum = numeric_limits<int>::min();
            int rangeMaximum = numeric_limits<int>::max();
        };

        static inline const vector<SettingsProperty> PROPERTIES = {
            // App configurables.
            { .name = AUTOHIDE_CONTROLS,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = AUTOHIDE_DELAY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "4",
              .rangeMinimum = 1, .rangeMaximum = 9
            },

            { .name = PREFERRED_DESKTOP,
              .valueType = SLIDER_VALUETYPE, .initialValue = "-1",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = ALLOW_DESKTOP_DRAG,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = ON_TOP_INSTEAD,
              .valueType = BOOL_VALUETYPE, .initialValue = "false",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = DIVIDER_1,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "10",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = PANEL_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "white",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = PANEL_OPACITY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "255",
              .rangeMinimum = 0, .rangeMaximum = 255
            },

            { .name = PANEL_OUTLINE_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "blue",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = PANEL_OUTLINE_OPACITY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "255",
              .rangeMinimum = 0, .rangeMaximum = 255
            },

            { .name = DIVIDER_2,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = TEXT_SIZE,
              .valueType = SLIDER_VALUETYPE, .initialValue = "42",
              .rangeMinimum = 10, .rangeMaximum = 80
            },

            { .name = TEXT_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "black",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = TEXT_OPACITY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "255",
              .rangeMinimum = 0, .rangeMaximum = 255
            },

            { .name = DIVIDER_3,
              .valueType = DIVIDER_VALUETYPE, .initialValue = "5",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = SHOW_WEED_CLOCK,
              .valueType = BOOL_VALUETYPE, .initialValue = "true",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = WEED_CLOCK_COLOR,
              .valueType = COLOR_VALUETYPE, .initialValue = "#00faff",
              .rangeMinimum = numeric_limits<int>::min(),
              .rangeMaximum = numeric_limits<int>::max()
            },

            { .name = WEED_CLOCK_OPACITY,
              .valueType = SLIDER_VALUETYPE, .initialValue = "255",
              .rangeMinimum = 0, .rangeMaximum = 255
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
         * Getters & setters of window config mode.
         */
        bool getConfigMode();

        void setConfigMode(const bool state);

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

        /**
         * Helper to return a new QSettings object for pref
         * access based on our appName.
         */
        QSettings* getQSettings();

    private:
        // Members.
        QString mSettingsApp = "";

        QSettings* mQSettings = nullptr;

        /**
         * Helper to return a QSettings filename from appName.
         */
        QString getQSettingsFile();
};