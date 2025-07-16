// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/Logger.h"
#include "util/Version.h"

#include "actions/TriggerAction.h"
#include "actions/OptionsAction.h"

#include "ApplicationStartupTask.h"

#include <QApplication>
#include <QSettings>
#include <QTemporaryDir>
#include <QLockFile>

class QMainWindow;

namespace mv {

class CoreInterface;
class ProjectMetaAction;

/**
 * ManiVault (Studio) application class
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT Application final : public QApplication
{
    Q_OBJECT

public:

    /** Class for managing ManiVault application temporary directories */
    class CORE_EXPORT TemporaryDirs final : public QObject {
    public:

        /**
         * Construct with pointer to /p parent object
         * @param parent Pointer to parent object
         */
        TemporaryDirs(QObject* parent);
        
        /**
         * Get stale ManiVault temporary directories in the OS temporary directory
         * Stale in this context means that the <os_temp_dir>/ManiVault.<app_session_id>.<temp_dir_id>/app.lock is not locked or missing
         * and the directory can be safely removed because it is not in use by another (ManiVault) application.
         * @return List of stale ManiVault application temporary directories
         */
        QStringList getStale();

        /**
         * Removes stale directories in the OS temporary directory (ManiVault.<app_session_id>.<temp_dir_id>) and which are not locked by the OS
         * @stale Stale directories to remove, removes all stale when empty
         */
        void removeStale(const QStringList& stale = QStringList());

        /**
         * Get progress task
         * @return Reference to progress task
         */
        Task& getTask() {
            return _task;
        }

    private:
        Task  _task;     /** Task for reporting clean-up progress in the background */
    };

public: // Construction

    /**
     * Constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     */
    Application(int& argc, char** argv);

    /** Custom destructor for cleaning up nicely */
    ~Application() override;

public: // Miscellaneous

    /** Get the globally unique identifier of the application instance */
    QString getId() const;

    /** Returns a pointer to the current ManiVault application (if the current application derives from mv::Application) */
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
     * Get the application name string
     * @return The application name string
     */
    static QString getName();

    /**
     * Get the application about string
     * @return The application about string
     */
    static QString getAbout();

    /**
     * Get the startup project file URL
     * @return Startup project URL (can be empty, in which case no project is opened at startup)
     */
    QUrl getStartupProjectUrl() const;

    /**
     * Set startup project file URL to \p startupProjectUrl
     * @param startupProjectUrl Startup project file URL (can be empty, in which case no project is opened at startup)
     */
    void setStartupProjectUrl(const QUrl& startupProjectUrl);

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
    
public: // Settings API

    /**
     * Get whether a setting exists
     * @param path Path of the setting (e.g. General/Computation/NumberOfIterations)
     * @return Boolean determining whether a setting exists
     */
    bool hasSetting(const QString& path) const;

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
     * Get whether serialization was aborted
     * @return Boolean indicating whether serialization was aborted
     */
    static bool isSerializationAborted();

    /**
     * Set whether serialization was aborted
     * @param serializationAborted Boolean indicating whether serialization was aborted
     */
    static void setSerializationAborted(bool serializationAborted);

    /**
     * Get temporary dir
     * @return Directory where application temporary files reside
     */
    const QTemporaryDir& getTemporaryDir() const;

    /**
     * Get ManiVault application temporary directories
     * @return Reference to temporary directories
     */
    TemporaryDirs& getTemporaryDirs();

public: // Statics

    static QMainWindow* getMainWindow();

signals:

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

protected:
    QString                     _id;                                /** Globally unique identifier of the application instance */
    CoreInterface*              _core;                              /** Pointer to the ManiVault core */
    const util::Version         _version;                           /** Application version */
    QSettings                   _settings;                          /** Settings */
    QString                     _serializationTemporaryDirectory;   /** Temporary directory for serialization */
    bool                        _serializationAborted;              /** Whether serialization was aborted */
    util::Logger                _logger;                            /** Logger instance */
    gui::TriggerAction*         _exitAction;                        /** Action for exiting the application */
    QUrl                        _startupProjectUrl;                 /** URL of the project to automatically open upon startup (if set) */
    ProjectMetaAction*          _startupProjectMetaAction;          /** Pointer to project meta action (non-nullptr case ManiVault starts up with a project) */
    ApplicationStartupTask*     _startupTask;                       /** Application startup task */
    QTemporaryDir               _temporaryDir;                      /** Directory where application temporary files reside */
    TemporaryDirs               _temporaryDirs;                     /** ManiVault application temporary directories manager */
    QLockFile                   _lockFile;                          /** Lock file is used for fail-safe purging of the temporary directory */
};

}