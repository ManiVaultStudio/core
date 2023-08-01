// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include "actions/StringAction.h"
#include "actions/StringsAction.h"
#include "actions/IntegralAction.h"
#include "actions/ProjectSplashScreenAction.h"
#include "actions/VersionAction.h"

#include "ProjectCompressionAction.h"

namespace hdps {

class Project;

/**
 * Project meta class
 *
 * Project class for preview purposes (contains project meta data).
 *
 * @author Thomas Kroes
 */
class ProjectMeta final : public QObject, public hdps::util::Serializable
{
    Q_OBJECT

public:

    /**
    * Construct project with source \p project and \p parent object
    * @param project Pointer to source project to get the meta data from
    * @param parent Pointer to parent object
    */
    ProjectMeta(Project* project, QObject* parent = nullptr);

    /**
     * Construct project from \p filePath and \p parent object
     * @param filePath Path of the project meta file
     * @param parent Pointer to parent object
     */
    ProjectMeta(const QString& filePath, QObject* parent = nullptr);

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

public: // Action getters

    gui::VersionAction& getApplicationVersionAction() { return _applicationVersionAction; }
    gui::VersionAction& getProjectVersionAction() { return _projectVersionAction; }
    gui::ToggleAction& getReadOnlyAction() { return _readOnlyAction; }
    gui::StringAction& getTitleAction() { return _titleAction; }
    gui::StringAction& getDescriptionAction() { return _descriptionAction; }
    gui::StringsAction& getTagsAction() { return _tagsAction; }
    gui::StringAction& getCommentsAction() { return _commentsAction; }
    gui::StringsAction& getContributorsAction() { return _contributorsAction; }
    gui::ProjectSplashScreenAction& getSplashScreenAction() { return _splashScreenAction; }
    ProjectCompressionAction& getCompressionAction() { return _compressionAction; }

private:
    Project*                        _project;                   /** Pointer to source project to get the meta data from */
    gui::VersionAction              _applicationVersionAction;  /** Action for storing the application version */
    gui::VersionAction              _projectVersionAction;      /** Action for storing the project version */
    gui::ToggleAction               _readOnlyAction;            /** Read-only action */
    gui::StringAction               _titleAction;               /** Title action */
    gui::StringAction               _descriptionAction;         /** Description action */
    gui::StringsAction              _tagsAction;                /** Tags action */
    gui::StringAction               _commentsAction;            /** Comments action */
    gui::StringsAction              _contributorsAction;        /** Contributors action */
    gui::ProjectSplashScreenAction  _splashScreenAction;        /** Action for configuring the project splash screen */
    ProjectCompressionAction        _compressionAction;         /** Project compression action */
};

}
