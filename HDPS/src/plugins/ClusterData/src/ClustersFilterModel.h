// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "clusterdata_export.h"

#include <QSortFilterProxyModel>

/**
 * Clusters filter model class
 *
 * A class for filtering clusters out of the clusters model
 *
 * @author Thomas Kroes
 */
class CLUSTERDATA_EXPORT ClustersFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    ClustersFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

    /** Returns the name filter */
    QString getNameFilter() const;

    /**
     * Sets the name filter
     * @param nameFilter Name filter
     */
    void setNameFilter(const QString& nameFilter);

private:
    QString     _nameFilter;    /** Cluster name filter */
};