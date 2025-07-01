// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractProjectDatabaseModel.h"

#include "util/ProjectDatabaseProject.h"

#include "actions/StringsAction.h"

#include <QFuture>
#include <QFutureWatcher>

#include <QMap>

namespace mv {

/**
 * Project database tree model class
 *
 * Contains grouped project content.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectDatabaseTreeModel final : public AbstractProjectDatabaseModel
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ProjectDatabaseTreeModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Get tags
     * @return All tags
     */
    QSet<QString> getTagsSet() const;

    /** Builds a set of all project tags and emits ProjectDatabaseModel::tagsChanged(...) */
    void updateTags();

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
