#pragma once

#include "util/IconFonts.h"

#include <QApplication>
#include <QSettings>

namespace hdps {

class CoreInterface;

/**
 * HDPS application class
 * 
 * Its intended use is application-wide sharing of resources such as icons and fonts
 *
 * @author Thomas Kroes
 */
class Application : public QApplication
{
public: // Construction/destruction

    /**
     * Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    Application(int& argc, char** argv);

public: // Miscellaneous

    /** Returns a pointer to the current HDPS application (if the current application derives from hdps::Application) */
    static Application* current();

    /** Get pointer to the core */
    CoreInterface* getCore();

    /**
     * Set pointer to the core
     * @param core Pointer to the core
     */
    void setCore(CoreInterface* core);

    /** Get pointer to the core */
    static CoreInterface* core();

public: // Static resource access functions

    /**
     * Returns an icon font by \p name \p majorVersion and \p minorVersion
     * @param name Name of the icon font (currently FontAwesome is supported)
     * @param majorVersion Major version number
     * @param minorVersion Minor version number
     */
    static const IconFont& getIconFont(const QString& name, const std::int32_t& majorVersion = -1, const std::int32_t& minorVersion = -1);

public: // Settings API

    /**
     * Load setting
     * @param path Path of the setting (e.g. General/Computation/NumberOfIterations)
     * @param defaultValue Value to return if the setting with \p path was not found
     * @return Plugin setting in variant form
     */
    QVariant getSetting(const QString& path, const QVariant& defaultValue = QVariant()) const;

    /**
     * Save setting
     * @param path Path of the setting (e.g. General/Computation/NumberOfIterations)
     * @param value Value in variant form
     */
    void setSetting(const QString& path, const QVariant& value);

public: // Serialization

    /** Load application from disk */
    void loadAnalysis();

    /** Save application to disk */
    void saveAnalysis();

public: // Serialization temporary directory

    /**
     * Get serialization temporary directory
     * @return Serialization temporary directory
     */
    static QString getSerializationTemporaryDirectory();

protected:
    CoreInterface*      _core;                                  /** Shared pointer to HDPS core */
    IconFonts           _iconFonts;                             /** Icon fonts resource */
    QSettings           _settings;                              /** Settings */
    QString             _serializationTemporaryDirectory;       /** Temporary directory for serialization */
};

}