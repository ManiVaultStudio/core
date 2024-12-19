// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QStandardItemModel>
#include <QString>

class PageAction;

/**
 * Page actions model class
 *
 * Contains actions for a page.
 *
 * @author Thomas Kroes
 */
class PageActionsModel final : public QStandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Icon,               /** Icon */
        Title,              /** Title */
        Description,        /** Description (maybe empty) */
        Comments,           /** Comments (maybe empty) */
        Tags,               /** Tags (maybe empty) */
        Subtitle,           /** Subtitle */
        MetaData,           /** Meta information */
        PreviewImage,       /** Action preview image */
        Tooltip,            /** Action tooltip */
        Contributors,       /** Action contributors */
        DownloadUrls,       /** Download URLs */
        ClickedCallback,    /** Callback which is called when the action is clicked */
        SummaryDelegate     /** Delegate item with title and subtitle */
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columns;

public:

    /**
     * Construct page actions model with \p parent object
     * @param parent Pointer to parent object
     */
    PageActionsModel(QObject* parent = nullptr);

    /**
     * Add start page action
     * @param pageAction Page action
     */
    void add(const PageAction& pageAction);

    /** Resets the rows and notifies others */
    void reset();
};
