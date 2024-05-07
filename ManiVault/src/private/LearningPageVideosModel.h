// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QStandardItemModel>

/**
 * Learning page videos model class
 *
 * Model class which contains learning page video content
 *
 * @author Thomas Kroes
 */
class LearningPageVideosModel final : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columns;

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningPageVideosModel(QObject* parent = nullptr);
};
