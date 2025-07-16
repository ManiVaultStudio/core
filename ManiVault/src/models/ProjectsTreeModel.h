// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractProjectsModel.h"
#include "models/ProjectsModelProject.h"

#include "actions/StringsAction.h"

namespace mv {

/**
 * Projects tree model class
 *
 * Contains grouped project content.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectsTreeModel final : public AbstractProjectsModel
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param mode Mode of the model (automatic/manual)
     * @param parent Pointer to parent object
     */
    ProjectsTreeModel(const PopulationMode& mode = PopulationMode::Automatic, QObject* parent = nullptr);

    /** Synchronize the model with the content of all Data Source Names (DSN) */
    void populateFromDsns();

    /** Synchronize the model with the content of all plugins Data Source Names (DSN) */
    void populateFromPluginDsns();

    /**
     * Populate the model from \p jsonByteArray
     * @param jsonByteArray JSON content as a byte array containing the projects data
     * @param dsnIndex Index of the Data Source Name (DSN) from which the JSON content was downloaded
     * @param jsonLocation Location of the JSON content, e.g., URL or file path
     */
    void populateFromJsonByteArray(const QByteArray& jsonByteArray, std::int32_t dsnIndex, const QString& jsonLocation);

    /**
     * Populate the model from a JSON file at \p filePath
     * @param filePath Path to the JSON file containing the projects data
     */
    void populateFromJsonFile(const QString& filePath);

public: // Action getters
    
    gui::StringsAction& getDsnsAction() { return _dsnsAction; }

    const gui::StringsAction& getDsnsAction() const { return _dsnsAction; }

private:
    gui::StringsAction      _dsnsAction;    /** Data source names action */
};

}
