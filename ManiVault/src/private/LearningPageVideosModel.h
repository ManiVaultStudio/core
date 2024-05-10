// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QStandardItemModel>
#include <QJsonObject>

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
        Title,
        Tags,
        Data,
        Summary,
        YouTubeId,
        YouTubeUrl
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Base standard model item class for video */
    class Item : public QStandardItem {
    public:

        /**
         * Construct with \p jsonObject of video
         * @param jsonObject JSON object describing the video
         */
        Item(QJsonObject jsonObject, bool editable = false);

        /**
         * Get JSON object
         * return Const reference to the JSON object
         */
        const QJsonObject& getJsonObject() const;

    private:
        QJsonObject _jsonObject;      /** The video JSON object */
    };

    /** Standard model item class for displaying the video title */
    class TitleItem final : public Item {
    public:

        /**
         * Construct with \p jsonObject of video
         * @param jsonObject JSON object describing the video
         */
        TitleItem(QJsonObject jsonObject);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the video tags */
    class TagsItem final : public Item {
    public:

        /**
         * Construct with \p jsonObject of video
         * @param jsonObject JSON object describing the video
         */
        TagsItem(QJsonObject jsonObject);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p jsonObject of video
         * @param jsonObject JSON object describing the video
         */
        Row(QJsonObject jsonObject) :
            QList<QStandardItem*>()
        {
            append(new TitleItem(jsonObject));
            append(new TagsItem(jsonObject));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningPageVideosModel(QObject* parent = nullptr);
};
