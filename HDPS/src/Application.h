// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/IconFonts.h"
#include "util/Logger.h"
#include "util/Version.h"

#include "actions/TriggerAction.h"

#include "ApplicationStartupTask.h"

#include <QApplication>
#include <QSettings>

class QMainWindow;

namespace hdps {

class CoreInterface;
class ProjectMetaAction;

/**
 * HDPS application class
 * 
 * @author Thomas Kroes
 */
class Application final : public QApplication
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

    /**
     * Get application version (major and minor version number)
     * @return Pair of integers representing major and minor version number respectively
     */
    util::Version getVersion() const;

    /**
     * Get the application name
     * @return The application name
     */
    static QString getName();

    /**
     * Get file path of the project to automatically open upon startup (if set)
     * @return Startup project file path
     */
    QString getStartupProjectFilePath() const;

    /**
     * Set file path of the project to automatically open upon startup (if set)
     * @param startupProjectFilePath Startup project file path
     */
    void setStartupProjectFilePath(const QString& startupProjectFilePath);

    /**
     * Get startup project meta action
     * @return Pointer to project meta action (non-nullptr when ManiVault starts up with a project)
     */
    ProjectMetaAction* getStartupProjectMetaAction();

    /**
     * Set startup project meta action to \p projectMetaAction (the application takes ownership of the pointer)
     * @param projectMetaAction Pointer to project meta action
     */
    void setStartupProjectMetaAction(ProjectMetaAction* projectMetaAction);

    /** Get application startup task */
    ApplicationStartupTask& getStartupTask();

    /**
     * Get whether a project should be opened after the application starts
     * @return Boolean determining whether a project should be opened after the application starts
     */
    bool shouldOpenProjectAtStartup() const;

    /** Perform one-time startup initialization */
    void initialize();
    
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

public: // Logging

    /**
     * Get application-wide logger instance
     * @return Reference to logger instance
     */
    static util::Logger& getLogger();

public: // Serialization

    /**
     * Get serialization temporary directory
     * @return Serialization temporary directory
     */
    static QString getSerializationTemporaryDirectory();

    /**
     * Set serialization temporary directory to \p serializationTemporaryDirectory
     * @param serializationTemporaryDirectory Serialization temporary directory
     */
    static void setSerializationTemporaryDirectory(const QString& serializationTemporaryDirectory);

    /**
     * Get whether serialization was aborted
     * @return Boolean indicating whether serialization was aborted
     */
    static bool isSerializationAborted();

    /**
     * Set whether serialization was aborted
     * @param serializationAborted Boolean indicating whether serialization was aborted
     */
    static void setSerializationAborted(bool serializationAborted);

public: // Statics

    static QMainWindow* getMainWindow();

signals:

    /**
     * Signals that the core has been assigned successfully to \p core
     * @param core Pointer to core instance
     */
    void coreAssigned(CoreInterface* core);

    /**
     * Invoked when the core is about to be initialized (re-broadcasts corresponding \p core signal)
     * @param core Pointer to core instance
     */
    void coreAboutToBeInitialized(CoreInterface* core);

    /**
     * Invoked when the core has been initialized (re-broadcasts corresponding \p core signal)
     * @param core Pointer to core instance
     */
    void coreInitialized(CoreInterface* core);

    /**
     * Invoked when the core managers have been created (re-broadcasts corresponding \p core signal)
     * @param core Pointer to core instance
     */
    void coreManagersCreated(CoreInterface* core);

    /** Signals that the main window has been fully initialized */
    void mainWindowInitialized();

protected:
    CoreInterface*          _core;                              /** Pointer to HDPS core */
    const util::Version     _version;                           /** Application version */
    IconFonts               _iconFonts;                         /** Icon fonts resource */
    QSettings               _settings;                          /** Settings */
    QString                 _serializationTemporaryDirectory;   /** Temporary directory for serialization */
    bool                    _serializationAborted;              /** Whether serialization was aborted */
    util::Logger            _logger;                            /** Logger instance */
    gui::TriggerAction*     _exitAction;                        /** Action for exiting the application */
    QString                 _startupProjectFilePath;            /** File path of the project to automatically open upon startup (if set) */
    ProjectMetaAction*      _startupProjectMetaAction;          /** Pointer to project meta action (non-nullptr case ManiVault starts up with a project) */
    ApplicationStartupTask* _startupTask;                       /** Application startup task */
};

}