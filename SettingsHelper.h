
#pragma once


/**
 * SettingsHelper provides a permanant keyed values
 * store for user preferences in a file tied to appName.
 *
 */
enum SettingsPropertyType {
    NONE_VALUETYPE,
    BOOL_VALUETYPE,
    INT_VALUETYPE,
    COLOR_VALUETYPE,
    STRING_VALUETYPE
};

struct SettingsProperty {
    QString group = "";
    QString name = "";

    SettingsPropertyType valueType = NONE_VALUETYPE;
    QString initialValue = "";
};

const vector<SettingsProperty> SETTINGS_PROPERTIES = {
    { .group = "Configurable", .name = "showWeedClock",
        .valueType = BOOL_VALUETYPE, .initialValue = "true" },
    { .group = "Configurable", .name = "weedClockColor",
        .valueType = COLOR_VALUETYPE, .initialValue = "#00faff" }
};


class SettingsHelper {
    public:

        SettingsHelper(const QString appName);
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
         * Getters for window workspace, x & y, w & h.
         */
        long getWindowWorkspace();

        double getWindowXPos();
        double getWindowYPos();
        double getWindowWidth();
        double getWindowHeight();

        /**
         * Setters for window workspace, x & y, w & h.
         */
        void setWindowWorkspace(const long workspace);

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
         * Getters & setters for user configurable setting
         * ShowWeedClock.
         */
        bool getShowWeedClock();
        void setShowWeedClock(const bool value);

        /**
         * Getters & setters for user configurable setting
         * weedClockColor.
         */
        XRenderColor getWeedClockColor();
        void setWeedClockColor(const XRenderColor color);

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
         * Helper to explicitly write all default values
         * to our .Ini file.
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

    private:
        // Members.
        QString mSettingsApp = "";

        QSettings* mQSettings = nullptr;
};