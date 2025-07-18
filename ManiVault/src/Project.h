// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include "util/Version.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"
#include "actions/VersionAction.h"
#include "actions/OptionsAction.h"
#include "actions/HorizontalGroupAction.h"

#include "ProjectMetaAction.h"

#include <QSharedPointer>

namespace mv {

/**
 * Project class
 *
 * Serializable class which encapsulates the entire ManiVault project.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Project final : public QObject, public mv::util::Serializable
{
    Q_OBJECT

public:

    /**
    * Construct project with \p parent
    * @param parent Pointer to parent object
    */
    Project(QObject* parent = nullptr);

    /**
     * Construct project from project JSON \p filePath and \p parent 
     * @param filePath Path of the project file
     * @param parent Pointer to parent object
     */
    Project(const QString& filePath, QObject* parent = nullptr);

    /**
     * Get project file path
     * @return Location on disk where the project resides
     */
    QString getFilePath() const;

    /**
     * Set project file path
     * @param filePath Location on disk where the project resides
     */
    void setFilePath(const QString& filePath);

    /**
     * Get whether this is a startup project
     * @return Boolean determining whether this project is loaded at startup of ManiVault
     */
    bool isStartupProject() const;

public: // Miscellaneous

    /**
     * Get version of the application (major and minor version number) with which the project is created
     * @return Pair of integers representing major and minor version number respectively
     */
    util::Version getVersion() const;

    /** Update the contributors (adds current user if not already added) */
    void updateContributors();

    /**
     * Set studio mode to \p studioMode
     * @param studioMode Studio mode
     */
    void setStudioMode(bool studioMode);

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public:

    /**
     * Get project meta action
     * @return Reference to project meta action
     */
    ProjectMetaAction& getProjectMetaAction();

    /**
     * Get shared pointer to project meta action from the compressed \p projectFilePath
     * @return Shared pointer to project meta action (nullptr if not found)
     */
    static QSharedPointer<ProjectMetaAction> getProjectMetaActionFromProjectFilePath(const QString& projectFilePath);

private:

    /** IsStartup initialization */
    void initialize();

public: // Project meta action getters facade

    const gui::VersionAction& getApplicationVersionAction() const { return _projectMetaAction.getApplicationVersionAction(); }
    const gui::VersionAction& getProjectVersionAction() const { return _projectMetaAction.getProjectVersionAction(); }
    const gui::ToggleAction& getReadOnlyAction() const { return _projectMetaAction.getReadOnlyAction(); }
    const gui::StringAction& getTitleAction() const { return _projectMetaAction.getTitleAction(); }
    const gui::StringAction& getDescriptionAction() const { return _projectMetaAction.getDescriptionAction(); }
    const gui::StringsAction& getTagsAction() const { return _projectMetaAction.getTagsAction(); }
    const gui::StringAction& getCommentsAction() const { return _projectMetaAction.getCommentsAction(); }
    const gui::StringsAction& getContributorsAction() const { return _projectMetaAction.getContributorsAction(); }
    const ProjectCompressionAction& getCompressionAction() const { return _projectMetaAction.getCompressionAction(); }
    const gui::ToggleAction& getAllowProjectSwitchingAction() const { return _projectMetaAction.getAllowProjectSwitchingAction(); }
    const gui::ToggleAction& getAllowedPluginsOnlyAction() const { return _projectMetaAction.getAllowedPluginsOnlyAction(); }
    const gui::StringsAction& getAllowedPluginsAction() const { return _projectMetaAction.getAllowedPluginsAction(); }
    const gui::SplashScreenAction& getSplashScreenAction() const { return _projectMetaAction.getSplashScreenAction(); }
    const gui::ToggleAction& getStudioModeAction() const { return _projectMetaAction.getStudioModeAction(); }
    const gui::ApplicationIconAction& getApplicationIconAction() const { return _projectMetaAction.getApplicationIconAction(); }
    const gui::ToggleAction& getSelectionGroupingAction() const { return _selectionGroupingAction; }
    const gui::ToggleAction& getOverrideApplicationStatusBarAction() const { return _overrideApplicationStatusBarAction; }
    const gui::ToggleAction& getStatusBarVisibleAction() const { return _statusBarVisibleAction; }
    const gui::OptionsAction& getStatusBarOptionsAction() const { return _statusBarOptionsAction; }

    gui::VersionAction& getApplicationVersionAction() { return _projectMetaAction.getApplicationVersionAction(); }
    gui::VersionAction& getProjectVersionAction() { return _projectMetaAction.getProjectVersionAction(); }
    gui::ToggleAction& getReadOnlyAction() { return _projectMetaAction.getReadOnlyAction(); }
    gui::StringAction& getTitleAction() { return _projectMetaAction.getTitleAction(); }
    gui::StringAction& getDescriptionAction() { return _projectMetaAction.getDescriptionAction(); }
    gui::StringsAction& getTagsAction() { return _projectMetaAction.getTagsAction(); }
    gui::StringAction& getCommentsAction() { return _projectMetaAction.getCommentsAction(); }
    gui::StringsAction& getContributorsAction() { return _projectMetaAction.getContributorsAction(); }
    ProjectCompressionAction& getCompressionAction() { return _projectMetaAction.getCompressionAction(); }
    gui::ToggleAction& getAllowProjectSwitchingAction() { return _projectMetaAction.getAllowProjectSwitchingAction(); }
    gui::ToggleAction& getAllowedPluginsOnlyAction() { return _projectMetaAction.getAllowedPluginsOnlyAction(); }
    gui::StringsAction& getAllowedPluginsAction() { return _projectMetaAction.getAllowedPluginsAction(); }
    gui::SplashScreenAction& getSplashScreenAction() { return _projectMetaAction.getSplashScreenAction(); }
    gui::ToggleAction& getStudioModeAction() { return _projectMetaAction.getStudioModeAction(); }
    gui::ApplicationIconAction& getApplicationIconAction() { return _projectMetaAction.getApplicationIconAction(); }
    gui::ToggleAction& getSelectionGroupingAction() { return _selectionGroupingAction; }
    gui::ToggleAction& getOverrideApplicationStatusBarAction() { return _overrideApplicationStatusBarAction; }
    gui::ToggleAction& getStatusBarVisibleAction() { return _statusBarVisibleAction; }
    gui::OptionsAction& getStatusBarOptionsAction() { return _statusBarOptionsAction; }

signals:

    /**
     * Signals that the project file path changed
     * @param filePath Location on disk where the project resides
     */
    void filePathChanged(const QString& filePath);

private:
    QString                 _filePath;                              /** Location on disk where the project resides */
    bool                    _startupProject;                        /** Boolean determining whether this project is loaded at startup of ManiVault */
    util::Version           _applicationVersion;                    /** Version of the application with which the project is created */
    ProjectMetaAction       _projectMetaAction;                     /** Project meta info action (i.e. title and version) */
    gui::ToggleAction       _selectionGroupingAction;               /** Action for toggling whether dataset selection grouping is enabled or not */
    gui::ToggleAction       _overrideApplicationStatusBarAction;    /** Action for toggling whether the project inherits the status bar settings from the application or not */
    gui::ToggleAction       _statusBarVisibleAction;                /** Action for toggling the status bar visibility */
    gui::OptionsAction      _statusBarOptionsAction;                /** Options action for toggling status bar items on/off */

protected:
    static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
    static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
};

}
