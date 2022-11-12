#pragma once

#include "CoreInterface.h"
#include "util/IconFonts.h"

#include <QApplication>
#include <QSettings>

namespace hdps {

/**
 * HDPS application class
 * 
 * Its intended use is application-wide sharing of resources such as icons and fonts
 *
 * @author Thomas Kroes
 */
class Application : public QApplication
{
    Q_OBJECT

public: // Construction

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

public: // Project IO

    /**
     * Load project from disk
     * @param projectFilePath File path of the project (if empty, the user will  select a file location by hand)
     */
    virtual void loadProject(QString projectFilePath = "") = 0;

    /**
     * Save project to disk
     * @param projectFilePath File path of the project (if empty, the user will  select a file location by hand)
     */
    virtual void saveProject(QString projectFilePath = "") = 0;

    /**
     * Get current project file path
     * @return File path of the current project
     */
    virtual QString getCurrentProjectFilePath() const final;

    /**
     * Set current project file path
     * @param currentProjectFilePath Current project file path
     */
    virtual void setCurrentProjectFilePath(const QString& currentProjectFilePath) final;

    /**
     * Add recent project file path (adds the path to the settings so that users can select the project from a recent list)
     * @param recentProjectFilePath File path of the recent project
     */
    virtual void addRecentProjectFilePath(const QString& recentProjectFilePath) final;

public: // Serialization

    /**
     * Get serialization temporary directory
     * @return Serialization temporary directory
     */
    static QString getSerializationTemporaryDirectory();

    /**
     * Get whether (de)serialization was aborted
     * @return Boolean indicating whether (de)serialization was aborted
     */
    static bool isSerializationAborted();

signals:

    /**
     * Signals that the current project file changed
     */
    void currentProjectFilePathChanged(const QString& currentProjectFilePath);

    /** Signals that the core has been become available */
    void coreSet(CoreInterface* core);

protected:
    CoreInterface*      _core;                                  /** Shared pointer to HDPS core */
    IconFonts           _iconFonts;                             /** Icon fonts resource */
    QSettings           _settings;                              /** Settings */
    QString             _currentProjectFilePath;                /** File path of the current project */
    QString             _serializationTemporaryDirectory;       /** Temporary directory for serialization */
    bool                _serializationAborted;                  /** Whether (de)serialization was aborted */
};

}