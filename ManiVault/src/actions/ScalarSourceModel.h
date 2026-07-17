#pragma once

#include "Dataset.h"

#include <QStandardItemModel>

namespace mv::gui {

/**
 * Scalar source model class
 *
 * Model which defines the source(s) to size scalars (by constant or by dataset)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ScalarSourceModel : public QStandardItemModel
{
protected:

    /** (Default) constructor */
    ScalarSourceModel(QObject* parent = nullptr);

public:

    /** Default scalar options */
    enum DefaultRow {
        Constant,           /** Scale by constant */
        Selection,          /** Scale by selected */
        DatasetStart        /** Start row of the dataset(s) */
    };

    /** Model columns */
    enum class Column {
        Name,       /** Scalar dataset name */
        Id,         /** Globally unique scalar dataset identifier */

        Count
    };

protected:

    /** Base standard model item class for a dataset */
    class Item : public QStandardItem {
    public:

        /**
         * Construct with reference to \p scalarSourceModel and pointer to \p scalarDataset
         * @param scalarSourceModel Reference to the scalar source model
         * @param scalarDataset Pointer to scalar dataset (maybe nullptr)
         */
        Item(const ScalarSourceModel& scalarSourceModel, const mv::Dataset<DatasetImpl>& scalarDataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get reference to the scalar source model
         * @return Reference to the scalar source model
         */
        const ScalarSourceModel& getScalarSourceModel() const;

        /**
         * Get the scalar dataset associated with this item (if any)
         * @return Pointer to scalar dataset (maybe nullptr)
         */ 
        const mv::Dataset<>& getScalarDataset() const;

    private:
        const ScalarSourceModel&    _scalarSourceModel;     /** Reference to the scalar source model */
        mv::Dataset<>               _scalarDataset;         /** Pointer to scalar dataset (maybe nullptr) */
    };

    /** Standard model item class for displaying the dataset GUI name */
    class NameItem final : public Item, public QObject {
    public:

        /**
         * Construct with reference to \p scalarSourceModel and pointer to \p scalarDataset
         * @param scalarSourceModel Reference to the scalar source model
         * @param scalarDataset Pointer to scalar dataset (maybe nullptr)
         */
        NameItem(const ScalarSourceModel& scalarSourceModel, const mv::Dataset<>& scalarDataset);

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
	                return "Name";

	            case Qt::ToolTipRole:
	                return "Dataset name";

                default:
                    break;
            }

            return {};
        }
    };

    /** Standard model item class for displaying the dataset GUI ID */
    class IdItem final : public Item {
    public:

        /** No need for specialized constructor */
        using Item::Item;

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
	                return "ID";

	            case Qt::ToolTipRole:
	                return "Dataset unique identifier";

	            default:
	                break;
            }

            return {};
        }
    };

public:

    /**
     * Add a dataset
     * @param dataset Smart pointer to dataset
     */
    void addDataset(const Dataset<DatasetImpl>& dataset);

    /**
     * Determines whether a given dataset is already loaded
     * @param dataset Smart pointer to dataset
     * @return whether the dataset is already loaded
     */
    bool hasDataset(const Dataset<DatasetImpl>& dataset) const;

    /**
     * Remove a dataset
     * @param dataset Smart pointer to dataset
     */
    void removeDataset(const Dataset<DatasetImpl>& dataset);

    /** Remove all datasets from the model */
    void removeAllDatasets();

    /**
     * Get datasets
     * @return Vector of smart pointers to datasets
     */
    Datasets getDatasets() const;

    /**
     * Get dataset at the specified row index
     * @param rowIndex Index of the row
     * @return Smart pointer to dataset
     */
    Dataset<DatasetImpl> getDataset(const std::int32_t& rowIndex) const;

    /**
     * Set datasets (resets the model)
     * @param datasets Vector of smart pointers to datasets
     */
    void setDatasets(const Datasets& datasets);

    /**
     * Get row index of the specified \p dataset
     * @param dataset Smart pointer to dataset
     * @return Row index of the dataset, or -1 if the dataset is not in the model
     */
    std::int32_t getRowIndex(const Dataset<DatasetImpl>& dataset) const;

    /** Get whether to show the full path name in the GUI */
    bool getShowFullPathName() const;

    /**
     * Set whether to show the full path name in the GUI
     * @param showFullPathName Whether to show the full path name in the GUI
     */
    void setShowFullPathName(const bool& showFullPathName);

protected:

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with pointer to \p scalarDataset
         * @param scalarDataset Pointer to scalar dataset (maybe nullptr)
         */
        Row(const ScalarSourceModel& scalarSourceModel, const mv::Dataset<>& scalarDataset) :
    		QList<QStandardItem*>()
        {
            append(new NameItem(scalarSourceModel, scalarDataset));
            append(new IdItem(scalarSourceModel, scalarDataset));
        }
    };

protected:
    bool        _showFullPathName;      /** Whether to show the full path name in the GUI */

    friend class ScalarAction;
    friend class ScalarSourceAction;
};

}
