#ifndef HDPS_DATASET_H
#define HDPS_DATASET_H

#include "CoreInterface.h"
#include "RawData.h"
#include "Dataset.h"

#include "actions/WidgetAction.h"
#include "util/Miscellaneous.h"

#include <QString>
#include <QVector>
#include <QUuid>

#include <memory>

#define DATASET_IMPL_VERBOSE

namespace hdps
{

namespace plugin {
    class AnalysisPlugin;
}

class DataHierarchyItem;

/**
 * Dataset implementation class
 * Base dataset class from which concrete dataset classes derive
  */
class DatasetImpl : public gui::WidgetAction
{
public:

    /** Type of storage */
    enum class StorageType {
        Owner,      /** The set is the data owner (has raw data) */
        Proxy       /** The set does not have raw data, it relies on proxy datasets to obtain data */
    };

public:

    /**
     * Constructor
     * @param core Pointer to the core
     * @param rawDataName Name of the raw data
     */
    DatasetImpl(CoreInterface* core, const QString& rawDataName) :
        WidgetAction(nullptr),
        _core(core),
        _storageType(StorageType::Owner),
        _rawData(nullptr),
        _guid(QUuid::createUuid().toString(QUuid::WithoutBraces)),
        _guiName(),
        _rawDataName(rawDataName),
        _all(false),
        _derived(false),
        _sourceDataset(),
        _properties(),
        _groupIndex(-1),
        _analysis(nullptr)
    {
    }

    /** Destructor */
    virtual ~DatasetImpl()
    {
#ifdef DATASET_IMPL_VERBOSE
        qDebug() << _guiName << "destructed";
#endif
    }

    /** Performs startup initialization */
    virtual void init()
    {
    };

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

    /** Get the globally unique identifier of the dataset in string format */
    QString getGuid() const {
        return _guid;
    }

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
    QString getGuiName() const {
        return _guiName;
    }

    /**
     * Set the GUI name of the dataset
     * @param guiName Name of the dataset in the graphical user interface
     */
    void setGuiName(const QString& guiName) {
        // Cache the old GUI name
        const auto previousGuiName = _guiName;

        // Assign new GUI name
        _guiName = guiName;

        setText(_guiName);

        // Notify others that the data GUI name changed
        Application::core()->notifyDatasetGuiNameChanged(*this, previousGuiName);
    }

    /** Returns true if this set represents the full data and false if it's a subset */
    bool isFull() const {
        return _all;
    }

    /** Returns whether the dataset is derived */
    bool isDerivedData() const {
        return _derived;
    }

    /** Returns the data type of the raw data associated with this dataset */
    DataType getDataType() const {
        return _core->requestRawData(getRawDataName()).getDataType();
    }

    /** Get source dataset if the given set is derived */
    template<typename DatasetType>
    Dataset<DatasetType> getSourceDataset() const {
        if (!isDerivedData())
            return toSmartPointer<DatasetType>();

        if (!_sourceDataset.isValid())
            return toSmartPointer<DatasetType>();

        return Dataset<DatasetType>(_sourceDataset->getSourceDataset<DatasetType>());
    }

    /**
     * Marks this dataset as derived and sets the source dataset globally unique identifier
     * @param dataset Smart pointer to the source dataset
     */
    void setSourceDataSet(const Dataset<DatasetImpl>& dataset) {
        _sourceDataset  = dataset;
        _derived        = true;
    }

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
    Dataset<DatasetImpl> getSelection() const {
        return _core->requestSelection(getSourceDataset<DatasetImpl>()->getRawDataName());
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection of dataset type associated with this data set
     */
    template<typename DatasetType>
    Dataset<DatasetType> getSelection() const {
        return _core->requestSelection<DatasetType>(getSourceDataset<DatasetImpl>()->getRawDataName());
    }

    /**
     * Get smart pointer to set
     * @return Smart pointer to the set
     */
    Dataset<DatasetImpl> toSmartPointer() const {
        return Dataset<DatasetImpl>(const_cast<DatasetImpl*>(this));
    }

    /**
     * Get smart pointer to set
     * @return Smart pointer to the set
     */
    template<typename DatasetType>
    Dataset<DatasetType> toSmartPointer() const {
        auto nonConstThis = const_cast<DatasetImpl*>(this);
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(nonConstThis));
    }

    /** Get pointer to the core */
    CoreInterface* getCore() {
        return _core;
    }

    /**
     * Get set icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    virtual QIcon getIcon(const QColor& color = Qt::black) const = 0;

    /**
     * Get icon for \p storageType
     * @param storageType Data storage type
     * @param color Icon color for flat (font) icons
     * @return Icon for \p storageType
     */
    virtual QIcon getIcon(StorageType storageType, const QColor& color = Qt::black) const final;

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
    virtual std::uint64_t getRawDataSize() const {
        return 0;
    }

    /**
     * Get amount of data occupied by the raw data in human readable string format
     * @return Size of the raw data in bytes in human readable string format
     */
    QString getRawDataSizeHumanReadable() const {
        return util::getNoBytesHumanReadable(getRawDataSize());
    }

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
     * @param dataTypes Dataset type(s) to filter out
     * @return Child datasets of the dataset type(s)
     */
    QVector<Dataset<DatasetImpl>> getChildren(const QVector<DataType>& dataTypes = QVector<DataType>()) const;

    /**
     * Get child datasets (if any) of the specified type
     * @param filterDataType Type of data to filter
     * @return Child datasets of the dataset type
     */
    QVector<Dataset<DatasetImpl>> getChildren(const DataType& filterDataType) {
        return getChildren(QVector<DataType>({ filterDataType }));
    }

public: // Selection

    /**
     * Get selection indices
     * @return Selection indices
     */
    virtual std::vector<std::uint32_t>& getSelectionIndices() = 0;

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

    /** Deselect all items */
    virtual void selectNone() = 0;

    /** Invert item selection */
    virtual void selectInvert() = 0;

public: // Lock

    /** Lock the dataset */
    void lock();

    /** Unlock the dataset */
    void unlock();

    /**
     * Get whether the dataset is locked
     * @return Boolean indicating whether the dataset is locked
     */
    bool isLocked() const;

    /**
     * Set whether the dataset is locked
     * @parem locked Boolean indicating whether the dataset is locked
     */
    void setLocked(bool locked);

public: // Operators

    /**
     * Equality operator
     * @param rhs Right-hand-side operator
     */
    const bool operator == (const DatasetImpl& rhs) const {
        return rhs.getGuid() == _guid;
    }

    /**
     * Inequality operator
     * @param rhs Right-hand-side operator
     */
    const bool operator != (const DatasetImpl& rhs) const {
        return rhs.getGuid() != _guid;
    }

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const {
        if (!hasProperty(name))
            return defaultValue;

        return _properties[name];
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value) {
        _properties[name] = value;
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const {
        return _properties.contains(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const {
        return _properties.keys();
    }

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
     * Get proxy datasets
     * @return Proxy datasets
     */
    Datasets getProxyDatasets() const;

    /**
     * Set the proxy datasets (automatically sets the dataset type to Type::Proxy)
     * @param proxyDatasets Proxy datasets
     */
    void setProxyDatasets(const Datasets& proxyDatasets);

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

    /** Returns list of shared action widgets*/
    void addAction(hdps::gui::WidgetAction& widgetAction);

    /** Returns list of shared action widgets*/
    hdps::gui::WidgetActions getActions() const;

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

protected:

    /** Get raw data */
    template <class DataType>
    DataType& getRawData() const {
        if (_rawData == nullptr)
            _rawData = &dynamic_cast<DataType&>(_core->requestRawData(getRawDataName()));

        return *static_cast<DataType*>(_rawData);
    }

    /** Get the name of the raw data */
    QString getRawDataName() const {
        return _rawDataName;
    }

    /**
     * Set whether this set represents all the data or only a subset
     * @param all Whether this is a full dataset
     */
    void setAll(bool all) {
        _all = all;
    }

public: // Operators

    /**
     * Assignment operator
     * @param other Reference to assign from
     */
    DatasetImpl& operator=(const DatasetImpl& other) {
        _core           = other._core;
        _storageType    = other._storageType;
        _guiName        = other._guiName;
        _rawData        = other._rawData;
        _rawDataName    = other._rawDataName;
        _all            = other._all;
        _derived        = other._derived;
        _sourceDataset  = other._sourceDataset;
        _properties     = other._properties;
        _groupIndex     = other._groupIndex;
        _analysis       = other._analysis;
        _proxyDatasets  = other._proxyDatasets;

        return *this;
    }

protected:
    CoreInterface*              _core;              /** Pointer to the core interface */

private:
    StorageType                 _storageType;       /** Type of storage (own raw data or act as proxy for other datasets) */
    mutable plugin::RawData*    _rawData;           /** Pointer to the raw data referenced in this set */
    QString                     _guid;              /** Globally unique dataset name */
    QString                     _guiName;           /** Name of the dataset in the graphical user interface */
    QString                     _rawDataName;       /** Name of the raw data */
    bool                        _all;               /** Whether this is the full dataset */
    bool                        _derived;           /** Whether this dataset is derived from another dataset */
    Dataset<DatasetImpl>        _sourceDataset;     /** Smart pointer to the source dataset (if any) */
    Dataset<DatasetImpl>        _fullDataset;       /** Smart pointer to the original full dataset (if this is a subset) */
    QMap<QString, QVariant>     _properties;        /** Properties map */
    std::int32_t                _groupIndex;        /** Group index (sets with identical indices can for instance share selection) */
    plugin::AnalysisPlugin*     _analysis;          /** Pointer to analysis plugin that created the set (if any) */
    Datasets                    _proxyDatasets;     /** Datasets which together form a group */

    friend class Core;
    friend class DataManager;
    friend class EventListener;

    template<typename DatasetType>
    friend class SmartDataset;
};

} // namespace hdps

#endif // HDPS_DATASET_H
