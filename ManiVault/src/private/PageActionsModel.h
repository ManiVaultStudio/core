// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QPair>
#include <QStandardItemModel>
#include <QString>

class StartPageAction;

/**
 * Start page actions model class
 *
 * Model class which contains actions for the start page.
 *
 * @author Thomas Kroes
 */
class StartPageActionsModel final : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Icon,               /** Icon */
        Title,              /** Title */
        Description,        /** Description (may be empty) */
        Comments,           /** Comments (may be empty) */
        Tags,               /** Tags (may be empty) */
        Subtitle,           /** Sub title */
        MetaData,           /** Meta information */
        PreviewImage,       /** Action preview image */
        Tooltip,            /** Action tooltip */
        Contributors,       /** Action contributors */
        ClickedCallback,    /** Callback which is called when the action is clicked */
        SummaryDelegate     /** Delegate item with title and subtitle */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columns;

public:

    /**
     * Construct start page actions model with \p parent object
     * @param parent Pointer to parent object
     */
    StartPageActionsModel(QObject* parent = nullptr);

    /**
     * Add start page action
     * @param startPageAction Start page action
     */
    void add(const StartPageAction& startPageAction);

    /** Resets the rows and notifies others */
    void reset();
};
