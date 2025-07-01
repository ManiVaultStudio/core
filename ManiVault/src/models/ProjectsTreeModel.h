// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractProjectsModel.h"

#include "util/ProjectDatabaseProject.h"

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

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ProjectsTreeModel(QObject* parent = nullptr);

    /** Synchronize the model with the data source names */
    void synchronizeWithDsns();

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
    gui::StringsAction              _dsnsAction;    /** Data source names action */
    QFuture<QByteArray>             _future;        /** Future for downloading projects */
    QFutureWatcher<QByteArray>      _watcher;       /** Future watcher for downloading projects */
};

}
