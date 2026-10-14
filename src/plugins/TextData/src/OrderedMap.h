// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "textdata_export.h"

#include "util/Serializable.h"

#include <QString>
#include <QDebug>

#include <unordered_map>
#include <vector>

namespace mv
{
    class TEXTDATA_EXPORT OrderedMap : public mv::util::Serializable
    {
    public:
        OrderedMap();

        /**
         * Get the number of elements stored in each column.
         * @return Number of rows per column
         */
        size_t getNumRows() const;

        /**
         * Get the number of columns.
         * @return Number of columns
         */
        size_t getNumColumns() const;

        /**
         * Get an ordered list of names associated to each column. Names are listed in the order their columns were added.
         * @return Ordered list of column header names
         */
        const std::vector<QString>& getColumnNames() const;

        bool hasColumn(QString columnName) const;

        /**
         * Get a column by the name of its header
         * @return The column of text data associated with the given header name
         */
        const std::vector<QString>& getColumn(QString columnName) const;

        void addColumn(QString columnName, std::vector<QString>& columnData);

    public: // Serialization

        /**
         * Load widget action from variant
         * @param Variant representation of the widget action
         */
        void fromVariantMap(const QVariantMap& variantMap) override;

        /**
            * Save widget action to variant
            * @return Variant representation of the widget action
            */
        QVariantMap toVariantMap() const override;

    private:
        /** Number of elements stored in each column */
        size_t _numRows;

        /** List of column header names, ordered in the order columns were added */
        std::vector<QString> _columnHeaders;

        /** Unordered map from column header names to column data */
        std::unordered_map<QString, std::vector<QString>> _columns;
    };
}
