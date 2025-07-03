// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractProjectsModel.h"

#include "util/ProjectsModelProject.h"

#include "actions/StringsAction.h"

#include <QFuture>
#include <QFutureWatcher>

#include <QMap>

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

    enum class Mode
    {
		Automatic,  /** Automatically populate the model from the Data Source Names (DSNs) */
        Manual      /** Manually populate the model from a JSON file or byte array */
    };

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ProjectsTreeModel(const Mode& mode = Mode::Automatic, QObject* parent = nullptr);

    /** Synchronize the model with the content of all plugins Data Source Names */
    void populateFromPluginDsns();

    /**
     * Populate the model from \p jsonByteArray
     * @param jsonByteArray JSON content as a byte array containing the projects data
     * @param dsnIndex
     * @param jsonLocation
     */
    void populateFromJsonByteArray(const QByteArray& jsonByteArray, std::int32_t dsnIndex, const QString& jsonLocation);

    /**
     * Populate the model from a JSON file at \p filePath
     * @param filePath Path to the JSON file containing the projects data
     */
    void populateFromJsonFile(const QString& filePath);

private:

    /**
     * Download projects from \p dsn
     * @param dsn Projects Data Source Name (DSN)
     * @return Downloaded data
     */
    static QByteArray downloadProjectsFromDsn(const QString& dsn);

public: // Action getters
    
    gui::StringsAction& getDsnsAction() { return _dsnsAction; }

    const gui::StringsAction& getDsnsAction() const { return _dsnsAction; }

signals:

    /** Signals that the model was populated from one or more source DSNs */
    void populatedFromDsns();

private:
    Mode                            _mode;          /** Mode of the model (automatic/manual) */
    gui::StringsAction              _dsnsAction;    /** Data source names action */
    QFuture<QByteArray>             _future;        /** Future for downloading projects */
    QFutureWatcher<QByteArray>      _watcher;       /** Future watcher for downloading projects */
};

}
