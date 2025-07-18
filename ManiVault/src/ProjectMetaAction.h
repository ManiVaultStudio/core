// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/SplashScreenAction.h"
#include "actions/StringAction.h"
#include "actions/StringsAction.h"
#include "actions/VersionAction.h"
#include "actions/VerticalGroupAction.h"
#include "actions/ApplicationIconAction.h"

#include "ProjectCompressionAction.h"

#include <QCompleter>
#include <QStringListModel>

namespace mv {

class Project;

/**
 * Project meta class
 *
 * Project class for preview purposes (contains project meta data).
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectMetaAction final : public gui::VerticalGroupAction
{
    Q_OBJECT

public:

    /**
    * Construct project with source \p project and \p parent object
    * @param project Pointer to source project to get the meta data from
    * @param parent Pointer to parent object
    */
    ProjectMetaAction(Project* project, QObject* parent = nullptr);

    /**
     * Construct project from \p filePath and \p parent object
     * @param filePath Path of the project meta file
     * @param parent Pointer to parent object
     */
    ProjectMetaAction(const QString& filePath, QObject* parent = nullptr);

    /**
     * Get owning project
     * @return Pointer to owning project (might be nullptr if project meta action is used in isolation)
     */
    Project* getProject() const;

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

private:

    /** IsStartup initialization */
    void initialize();

public: // Action getters

    const gui::VersionAction& getApplicationVersionAction() const { return _applicationVersionAction; }
    const gui::VersionAction& getProjectVersionAction() const { return _projectVersionAction; }
    const gui::ToggleAction& getReadOnlyAction() const { return _readOnlyAction; }
    const gui::StringAction& getTitleAction() const { return _titleAction; }
    const gui::StringAction& getDescriptionAction() const { return _descriptionAction; }
    const gui::StringsAction& getTagsAction() const { return _tagsAction; }
    const gui::StringAction& getCommentsAction() const { return _commentsAction; }
    const gui::StringsAction& getContributorsAction() const { return _contributorsAction; }
    const gui::SplashScreenAction& getSplashScreenAction() const { return _splashScreenAction; }
    const gui::ToggleAction& getStudioModeAction() const { return _studioModeAction; }
    const gui::ApplicationIconAction& getApplicationIconAction() const { return _applicationIconAction; }
    const ProjectCompressionAction& getCompressionAction() const { return _compressionAction; }
    const gui::ToggleAction& getAllowedPluginsOnlyAction() const { return _allowedPluginsOnlyAction; }
    const gui::StringsAction& getAllowedPluginsAction() const { return _allowedPluginsAction; }

    gui::VersionAction& getApplicationVersionAction() { return _applicationVersionAction; }
    gui::VersionAction& getProjectVersionAction() { return _projectVersionAction; }
    gui::ToggleAction& getReadOnlyAction() { return _readOnlyAction; }
    gui::StringAction& getTitleAction() { return _titleAction; }
    gui::StringAction& getDescriptionAction() { return _descriptionAction; }
    gui::StringsAction& getTagsAction() { return _tagsAction; }
    gui::StringAction& getCommentsAction() { return _commentsAction; }
    gui::StringsAction& getContributorsAction() { return _contributorsAction; }
    gui::SplashScreenAction& getSplashScreenAction() { return _splashScreenAction; }
    gui::ToggleAction& getStudioModeAction() { return _studioModeAction; }
    gui::ApplicationIconAction& getApplicationIconAction() { return _applicationIconAction; }
    ProjectCompressionAction& getCompressionAction() { return _compressionAction; }
    gui::ToggleAction& getAllowedPluginsOnlyAction() { return _allowedPluginsOnlyAction; }
    gui::StringsAction& getAllowedPluginsAction() { return _allowedPluginsAction; }

private:
    Project*                        _project;                           /** Pointer to source project to get the meta data from */
    gui::VersionAction              _applicationVersionAction;          /** Action for storing the application version */
    gui::VersionAction              _projectVersionAction;              /** Action for storing the project version */
    gui::ToggleAction               _readOnlyAction;                    /** Read-only action */
    gui::StringAction               _titleAction;                       /** Title action */
    gui::StringAction               _descriptionAction;                 /** Description action */
    gui::StringsAction              _tagsAction;                        /** Tags action */
    gui::StringAction               _commentsAction;                    /** Comments action */
    gui::StringsAction              _contributorsAction;                /** Contributors action */
    gui::SplashScreenAction         _splashScreenAction;                /** Action for configuring the project splash screen */
    gui::ToggleAction               _studioModeAction;                  /** Toggle between view- and studio mode action */
    gui::ApplicationIconAction      _applicationIconAction;             /** Application icon action (only used in application mode) */
    ProjectCompressionAction        _compressionAction;                 /** Project compression action */
    gui::ToggleAction               _allowedPluginsOnlyAction;          /** Restrict to allowed plugins only action (only used in application mode) */
    gui::StringsAction              _allowedPluginsAction;              /** Allowed plugins action (only used in application mode) */
    QCompleter                      _allowedPluginsCompleter;           /** Completer for allowed plugins action */
    QStringListModel                _allowedPluginsModel;               /** Model for allowed plugins action */
};

}
