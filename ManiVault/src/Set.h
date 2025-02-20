// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "CoreInterface.h"
#include "Dataset.h"
#include "LinkedData.h"
#include "RawData.h"

#include "DatasetTask.h"
#include "ForegroundTask.h"
#include "ModalTask.h"

#include "actions/WidgetAction.h"
#include "util/Miscellaneous.h"

#include <QString>
#include <QUuid>
#include <QVector>

#include <memory>

#define DATASET_IMPL_VERBOSE

namespace mv
{

class CoreInterface;

namespace plugin {
    class AnalysisPlugin;
}

class DataHierarchyItem;

/**
 * Dataset implementation class
 * Base dataset class from which concrete dataset classes derive
  */
class CORE_EXPORT DatasetImpl : public gui::WidgetAction
{
public:

    /** Type of storage */
    enum class StorageType {
        Owner,      /** The set is the data owner (has raw data) */
        Proxy       /** The set does not have raw data, it relies on proxy datasets to obtain data */
    };

    /** Linked data propagation flags */
    enum LinkedDataFlag {
        Send        = 0x00001,          /** The set may send linked propagate linked data */
        Receive     = 0x00002,          /** The set may receive linked data */
        SendReceive = Send | Receive    /** The set may send and receive linked data */
    };

public:

    /**
     * Construct with \p rawDataName and possibly initialize with \p id
     * @param rawDataName Name of the raw data
     * @param mayUnderive Whether the dataset may be un-derived, if not it should always co-exist with its source
     * @param id Globally unique dataset identifier, if empty an new ID will be generated
     */
    explicit DatasetImpl(const QString& rawDataName, bool mayUnderive = true, const QString& id = "");

    /** Destructor */
    ~DatasetImpl() override;

    /** Performs startup initialization */
    virtual void init();

    /**
     * Get a copy of the dataset
     * @return Smart pointer to copy of dataset
     */
    virtual Dataset<DatasetImpl> copy() const = 0;

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    virtual Dataset<DatasetImpl> createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const = 0;

    /**
     * Get the data storage type
     * @return Data storage type
     */
    StorageType getStorageType() const;

    /**
     * Set the data storage type
     * @param type Data storage type
     */
    void setStorageType(const StorageType& storageType);

    /** Get the GUI name of the dataset */
    QString getGuiName() const;
    
    /**
     * Set gui name to \p guiName
     * @param guiName GUI name
     */
    void setGuiName(const QString& guiName);

    //void setText(const QString& text);

    /** Returns true if this set represents the full data and false if it's a subset */
    bool isFull() const;

    /** Returns whether the dataset is derived */
    bool isDerivedData() const;

    /** Returns whether the dataset may be un-derived, if not it should always co-exist with its source */
    bool mayUnderive() const;

    /** Returns the data type of the raw data associated with this dataset */
    DataType getDataType() const;

    /** Get source dataset if the given set is derived */
    template<typename DatasetType>
    Dataset<DatasetType> getSourceDataset() const {
        if (!isDerivedData())
            return toSmartPointer<DatasetType>();

        if (!_sourceDataset.isValid())
            return toSmartPointer<DatasetType>();

        return Dataset<DatasetType>(_sourceDataset->getSourceDataset<DatasetType>());
    }

    /** Get source dataset if the given set is derived */
    template<typename DatasetType>
    Dataset<DatasetType> getNextSourceDataset() const {
        if (!isDerivedData())
            return toSmartPointer<DatasetType>();

        if (!_sourceDataset.isValid())
            return toSmartPointer<DatasetType>();

        return _sourceDataset;
    }

    /**
     * Marks this dataset as derived and sets the source dataset to \p dataset
     * @param dataset Smart pointer to the source dataset
     */
    void setSourceDataset(Dataset<DatasetImpl> dataset);

    /**
     * Marks this dataset as derived and sets the source dataset globally unique identifier to \p datasetId
     * Note: use this when the source dataset itself is not physically available but can be lazily obtained later
     * @param datasetId Globally unique identifier of the source dataset
     */
    void setSourceDataset(const QString& datasetId);

    /**
     * Returns the original full dataset this subset was created from
     */
    template<typename DatasetType>
    Dataset<DatasetType> getFullDataset() const
    {
        if (!_fullDataset.isValid())
            return toSmartPointer<DatasetType>();

        return _fullDataset;
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection associated with this data set
     */
    Dataset<DatasetImpl> getSelection() const;

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection of dataset type associated with this data set
     */
    template<typename DatasetType>
    Dataset<DatasetType> getSelection() const {
        return mv::data().getSelection<DatasetType>(getSourceDataset<DatasetImpl>()->getRawDataName());
    }

    /**
     * Get reference to smart pointer which is owned by the set
     * @return Reference to smart pointer which is owned by the set
     */
    Dataset<DatasetImpl>& getSmartPointer();

    /**
     * Get smart pointer to set
     * @return Smart pointer to the set
     */
    Dataset<DatasetImpl> toSmartPointer() const;

    /**
     * Get smart pointer to set
     * @return Smart pointer to the set
     */
    template<typename DatasetType>
    Dataset<DatasetType> toSmartPointer() const {
        auto nonConstThis = const_cast<DatasetImpl*>(this);
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(nonConstThis));
    }

    /**
     * Makes this set a subset of a full dataset
     * @param fullDataset Smart pointer to full dataset
     */
    void makeSubsetOf(Dataset<DatasetImpl> fullDataset);

    /**
     * Get plugin kind of the raw data
     * @return Plugin kind of the raw data
     */
    QString getRawDataKind() const;

    /**
     * Get amount of data occupied by the raw data
     * @return Size of the raw data in bytes
     */
    virtual std::uint64_t getRawDataSize() const;

    /**
     * Get amount of data occupied by the raw data in human readable string format
     * @return Size of the raw data in bytes in human readable string format
     */
    QString getRawDataSizeHumanReadable() const;

public: // Location

    /**
     * Get location
     * @return Path relative to the top-level action
     */
    QString getLocation(bool recompute = false) const override;

public: // Hierarchy

    /** Get reference to data hierarchy item */
    DataHierarchyItem& getDataHierarchyItem();

    /** Get reference to data hierarchy item */
    const DataHierarchyItem& getDataHierarchyItem() const;

    /** Get parent dataset (if any) */
    Dataset<DatasetImpl> getParent() const;

    /** Get parent dataset (if any) */
    template<typename DatasetType>
    Dataset<DatasetType> getParent() const {
        return Dataset<DatasetType>(getParent());
    }

    /**
     * Get child datasets (if any) of the specified type(s)
     * @param dataTypes Dataset type(s) to filter out (no filtering if \p dataTypes is empty)
     * @param recursively Whether to collect children recursively
     * @return Child datasets of the dataset type(s)
     */
    QVector<Dataset<DatasetImpl>> getChildren(const QVector<DataType>& dataTypes = QVector<DataType>(), bool recursively = true) const;

    /**
     * Get child datasets (if any) of the specified type
     * @param filterDataType Type of data to filter
     * @return Child datasets of the dataset type
     */
    QVector<Dataset<DatasetImpl>> getChildren(const DataType& filterDataType) const;

public: // Selection

    /**
     * Get selection indices
     * @return Selection indices
     */
    virtual std::vector<std::uint32_t>& getSelectionIndices() = 0;

    bool needsSelectionUpdate() const { return _dirtySelection; }

    /**
     * Select by indices
     * @param indices Selection indices
     */
    virtual void setSelectionIndices(const std::vector<std::uint32_t>& indices) = 0;

    /** Get size of the selection */
    std::int32_t getSelectionSize() const;

    /** Determines whether items can be selected */
    virtual bool canSelect() const = 0;

    /** Determines whether all items can be selected */
    virtual bool canSelectAll() const = 0;

    /** Determines whether there are any items which can be deselected */
    virtual bool canSelectNone() const = 0;

    /** Determines whether the item selection can be inverted (more than one) */
    virtual bool canSelectInvert() const = 0;

    /** Select all items */
    virtual void selectAll() = 0;

    /** De-select all items */
    virtual void selectNone() = 0;

    /** Invert item selection */
    virtual void selectInvert() = 0;

private:
    void markSelectionDirty(bool isDirty) const { _dirtySelection = isDirty; }

public: // Lock

    /**
     * Lock the dataset and possibly \p cache the current lock state
     * @param cache Whether to cache the current lock state
     */
    void lock(bool cache = false);

    /**
     * Unlock the dataset and possibly \p cache the current lock state
     * @param cache Whether to cache the current lock state
     */
    void unlock(bool cache = false);

    /**
     * Get whether the dataset is locked
     * @return Boolean indicating whether the dataset is locked
* 
     */
    bool isLocked() const;

    /**
     * Set whether the dataset is \p locked and possibly \p cache the current lock state
     * @param locked Boolean indicating whether the dataset is locked
     * @param cache Whether to cache the current lock state
     */
    void setLocked(bool locked, bool cache = false);

    /** Restore the lock status to the cached value */
    void restoreLockedFromCache();

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const;

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value);

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const;

    /** Returns a list of available property names */
    QStringList propertyNames() const;

public: // Analysis

    /**
     * Set analysis
     * @param analysis Pointer to analysis plugin
     */
    void setAnalysis(plugin::AnalysisPlugin* analysis);

    /**
     * Get analysis
     * @return Pointer to analysis plugin
     */
    plugin::AnalysisPlugin* getAnalysis();

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

public: // Grouping

    /** Get group index */
    std::int32_t getGroupIndex() const;

    /**
     * Set group index
     * @param groupIndex group index
     */
    void setGroupIndex(const std::int32_t& groupIndex);

    /**
     * Get member datasets
     * @return Proxy member datasets
     */
    Datasets getProxyMembers() const;

    /**
     * Set the proxy member datasets (automatically sets the dataset type to Type::Proxy)
     * @param proxyMembers Proxy member datasets
     */
    virtual void setProxyMembers(const Datasets& proxyMembers);

    /**
     * Establish whether a proxy dataset may be created with candidate \p proxyDatasets
     * @param proxyDatasets Candidate proxy datasets
     * @return Boolean indicating whether a proxy dataset may be created with candidate \p proxyDatasets
     */
    virtual bool mayProxy(const Datasets& proxyDatasets) const;

    /**
     * Establish whether the set is a proxy dataset (comprised of foreign datasets)
     * @return Boolean indicating whether the set is a proxy dataset
     */
    bool isProxy() const;

public: // Actions

    /**
     * Add \p widgetAction to the corresponding data hierarchy item
     * @param widgetAction Widget action to add
     */
    void addAction(mv::gui::WidgetAction& widgetAction);

    /**
     * Get attached actions
     * @return List of widget actions
     */
    mv::gui::WidgetActions getActions() const;

    /**
     * Get the context menu
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);

public: // Linked data

    void addLinkedData(const mv::Dataset<DatasetImpl>& targetDataSet, mv::SelectionMap& mapping);

    void addLinkedData(const mv::Dataset<DatasetImpl>& targetDataSet, mv::SelectionMap&& mapping);

    /**
     * Removes all mappings of global selection indices from this dataset
     */
    void removeAllLinkedData();

    /**
     * Removes mappings of global selection indices from this dataset to a target dataset
     * @param targetDataSet The target dataset
     */
    void removeLinkedDataset(const mv::Dataset<DatasetImpl>& targetDataSet);

    /**
     * Removes a mapping from this dataset
     * @param mappingID unique identifier (as obtained with getId()) of to-be-removed mapping
     */
    void removeLinkedDataMapping(const QString& mappingID);

    const std::vector<mv::LinkedData>& getLinkedData() const;

    std::vector<mv::LinkedData>& getLinkedData();

    /**
     * Get flags for linked data
     * @return Flags for linked data
     */
    std::int32_t getLinkedDataFlags() const;

    /**
     * Set flags for linked data
     * @param linkedDataFlags Flags for linked data
     */
    void setLinkedDataFlags(std::int32_t linkedDataFlags);

    /**
     * Set linked data flag
     * @param linkedDataFlag Linked data
     */
    void setLinkedDataFlag(std::int32_t linkedDataFlag, bool set = true);

    /**
     * Determine whether \p linkedDataFlag is set or not
     * @param linkedDataFlag Linked data
     * @return Boolean determining  linkedDataFlag Linked data
     */
    bool hasLinkedDataFlag(std::int32_t linkedDataFlag) const;

    /**
     * Resolves linked data for the dataset
     * @param force Force update of all linked data (ignores linked data flags)
     */
    virtual void resolveLinkedData(bool force = false) {};

public:
        
    /** Get the name of the raw data */
    QString getRawDataName() const;

public: // Task access

    /**
     * Get task
     * @return Task for display in the data hierarchy and foreground
     */
    DatasetTask& getTask();

public: // Removal

    /**
     * Set about to be removed to \p aboutToBeRemoved
     * @param aboutToBeRemoved Boolean determining whether the set is in the process of being removed
     */
    void setAboutToBeRemoved(bool aboutToBeRemoved = true);

    /**
     * Get whether the set is in the process of being removed
     * @return Boolean determining whether the set is in the process of being removed
     */
    bool isAboutToBeRemoved() const;

protected:

    /**
     * Get raw data for \p DataType 
     * @return Pointer to raw data of \p DataType 
     */
    template <class DataType>
    DataType* getRawData() const {
        if (_rawData == nullptr)
            _rawData = dynamic_cast<DataType*>(mv::data().getRawData(getRawDataName()));

        return static_cast<DataType*>(_rawData);
    }

    /**
     * Set whether this set represents all the data or only a subset
     * @param all Whether this is a full dataset
     */
    void setAll(bool all);

public: // Operators

    /**
     * Assignment operator
     * @param other Reference to assign from
     */
    DatasetImpl& operator=(const DatasetImpl& other) {
        _rawData            = other._rawData;
        _rawDataName        = other._rawDataName;
        _storageType        = other._storageType;
        _all                = other._all;
        _derived            = other._derived;
        _sourceDataset      = other._sourceDataset;
        _properties         = other._properties;
        _groupIndex         = other._groupIndex;
        _analysis           = other._analysis;
        _proxyMembers       = other._proxyMembers;
        _linkedData         = other._linkedData;
        _linkedDataFlags    = other._linkedDataFlags;
        _aboutToBeRemoved   = other._aboutToBeRemoved;

        return *this;
    }

private:
    mutable plugin::RawData*    _rawData;               /** Pointer to the raw data referenced in this set */
    QString                     _rawDataName;           /** Name of the raw data */
    StorageType                 _storageType;           /** Type of storage (own raw data or act as proxy for other datasets) */
    bool                        _all;                   /** Whether this is the full dataset */
    bool                        _derived;               /** Whether this dataset is derived from another dataset */
    Dataset<DatasetImpl>        _sourceDataset;         /** Smart pointer to the source dataset (if any) */
    Dataset<DatasetImpl>        _fullDataset;           /** Smart pointer to the original full dataset (if this is a subset) */
    bool                        _mayUnderive;           /** Whether a dataset may be un-derived, if not it should always co-exist with its source */
    QMap<QString, QVariant>     _properties;            /** Properties map */
    std::int32_t                _groupIndex;            /** Group index (sets with identical indices can for instance share selection) */
    plugin::AnalysisPlugin*     _analysis;              /** Pointer to analysis plugin that created the set (if any) */
    Datasets                    _proxyMembers;          /** Member datasets in case of a proxy dataset */
    std::vector<LinkedData>     _linkedData;            /** List of linked datasets */
    std::int32_t                _linkedDataFlags;       /** Flags for linked data */
    bool                        _locked;                /** Whether the dataset is locked */
    bool                        _lockedCache;           /** Cached locked state */
    Dataset<DatasetImpl>        _smartPointer;          /** Smart pointer to own dataset */
    DatasetTask                 _task;                  /** Task for display in the data hierarchy and foreground */
    bool                        _aboutToBeRemoved;      /** Boolean determining whether the set is in the process of being removed */
    mutable bool                _dirtySelection;        /** Whether the dataset should be notified about a changed selection */

    friend class CoreInterface;
    friend class Core;
    friend class DataManager;
    friend class EventListener;
    friend class EventManager;
    friend class KeyBasedSelectionGroup;
};

}
