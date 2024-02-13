// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <Dataset.h>
#include <Set.h>

#include <actions/StringAction.h>

#include <QStandardItemModel>

/**
 * Raw data model class
 *
 * Standard item model for storing raw data
 *
 * @author Thomas Kroes
 */
class RawDataModel final : public QStandardItemModel
{
public:

    /** Dataset columns */
    enum Column {
        Name,       /** Name of the raw data */
        Size,       /** Size of the raw data */
        Type,       /** Data type of the raw data */

        Count
    };

private:

    /** Base standard model item class for raw data */
    class RawDataItem : public QStandardItem {
    public:

        /**
         * Construct with \p rawDataName
         * @param rawDataName Name of the raw data
         */
        RawDataItem(const QString& rawDataName);

        /**
         * Get raw data name
         * return Name of the raw data
         */
        QString getRawDataName();

    private:
        QString _rawDataName;   /** Name of the raw data */
    };

    /** Standard model item class for raw data */
    class RawDataNameItem : public RawDataItem {
    public:

        /** Use base item constructor */
        using RawDataItem::RawDataItem;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    return "Raw data Name";

                case Qt::ToolTipRole:
                    return "The name of the raw data";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the raw data type */
    class RawDataTypeItem final : public RawDataItem {
    public:

        /** Use base item constructor */
        using RawDataItem::RawDataItem;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    return "Raw data size";

                case Qt::ToolTipRole:
                    return "The size of the raw data";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the raw data size */
    class RawDataSizeItem final : public RawDataItem {
    public:

        /** Use base item constructor */
        using RawDataItem::RawDataItem;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    return "Raw data size";

                case Qt::ToolTipRole:
                    return "The size of the raw data";
            }

            return {};
        }
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit RawDataModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:

    /** Convenience class for combining dataset items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p rawDataName
         * @param rawDataName Name of the raw data
         */
        Row(const QString& rawDataName);
    };

protected: // Drag-and-drop behavior

    /** Get supported drag actions */
    Qt::DropActions supportedDragActions() const override {
        return Qt::IgnoreAction;
    };

    /** Get the supported drop actions */
    Qt::DropActions supportedDropActions() const override {
        return Qt::IgnoreAction;
    };

public:

    /** Populate the model with datasets from the data manager */
    void populateFromDataManager();

public: // Action getters

    mv::gui::StringAction& getCountAction() { return _countAction; }

private:
    mv::gui::StringAction   _countAction;    /** String action for displaying the number of datasets */
};
