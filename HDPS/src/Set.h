#ifndef HDPS_DATASET_H
#define HDPS_DATASET_H

#include "CoreInterface.h"
#include "RawData.h"
#include "Dataset.h"

#include "actions/WidgetAction.h"

#include <QString>
#include <QVector>
#include <QUuid>

#include <memory>

namespace hdps
{

class DataHierarchyItem;

/**
 * Dataset implementation class
 * Base dataset class from which concrete dataset classes derive
  */
class DatasetImpl
{
public:

    /**
     * Constructor
     * @param core Pointer to the core
     * @param rawDataName Name of the raw data
     */
    DatasetImpl(CoreInterface* core, const QString& rawDataName) :
        _core(core),
        _rawData(nullptr),
        _guid(QUuid::createUuid().toString()),
        _guiName(),
        _rawDataName(rawDataName),
        _all(false),
        _derived(false),
        _sourceDataset(),
        _properties(),
        _groupIndex(-1)
    {
    }

    /** Destructor */
    virtual ~DatasetImpl()
    {
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
     * Create subset and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    virtual Dataset<DatasetImpl> createSubset(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const = 0;

    /** Get the globally unique identifier of the dataset in string format */
    QString getGuid() const
    {
        return _guid;
    }

    /** Get the GUI name of the dataset */
    QString getGuiName() const
    {
        return _guiName;
    }

    /**
     * Set the GUI name of the dataset
     * @param guiName Name of the dataset in the graphical user interface
     */
    void setGuiName(const QString& guiName)
    {
        // Cache the old GUI name
        const auto previousGuiName = _guiName;

        // Assign new GUI name
        _guiName = guiName;

        // Notify others that the data GUI name changed
        Application::core()->notifyDataGuiNameChanged(*this, previousGuiName);
    }

    /** Returns true if this set represents the full data and false if it's a subset */
    bool isFull() const
    {
        return _all;
    }

    /** Returns whether the dataset is derived */
    bool isDerivedData() const
    {
        return _derived;
    }

    /** Returns the data type of the raw data associated with this dataset */
    DataType getDataType() const
    {
        return _core->requestRawData(getRawDataName()).getDataType();
    }

    /** Get source dataset if the given set is derived */
    template<typename DatasetType>
    Dataset<DatasetType> getSourceDataset() const
    {
        if (!isDerivedData())
            return toSmartPointer<DatasetType>();

        return Dataset<DatasetType>(_sourceDataset->getSourceDataset<DatasetType>());
    }

    /**
     * Marks this dataset as derived and sets the source dataset globally unique identifier
     * @param dataset Smart pointer to the source dataset
     */
    void setSourceDataSet(const Dataset<DatasetImpl>& dataset)
    {
        _sourceDataset  = dataset;
        _derived        = true;
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection associated with this data set
     */
    Dataset<DatasetImpl> getSelection() const
    {
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
    Dataset<DatasetType> getSelection() const
    {
        return _core->requestSelection<DatasetType>(getSourceDataset<DatasetImpl>()->getRawDataName());
    }

    /**
     * Get smart pointer to set
     * @return Smart pointer to the set
     */
    Dataset<DatasetImpl> toSmartPointer() const
    {
        return Dataset<DatasetImpl>(const_cast<DatasetImpl*>(this));
    }

    /**
     * Get smart pointer to set
     * @return Smart pointer to the set
     */
    template<typename DatasetType>
    Dataset<DatasetType> toSmartPointer() const
    {
        auto nonConstThis = const_cast<DatasetImpl*>(this);
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(nonConstThis));
    }

    /** Get pointer to the core */
    CoreInterface* getCore() {
        return _core;
    }

    /** Get icon for the dataset */
    virtual QIcon getIcon() const = 0;


public: // Hierarchy

    /** Get reference to data hierarchy item */
    DataHierarchyItem& getDataHierarchyItem();

    /** Get reference to data hierarchy item */
    const DataHierarchyItem& getDataHierarchyItem() const;

    /** Get parent dataset (if any) */
    Dataset<DatasetImpl> getParent() const;

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
    QVector<Dataset<DatasetImpl>> getChildren(const DataType& filterDataType)
    {
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

    /** Get whether the dataset is locked */
    bool isLocked() const;

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
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const
    {
        if (!hasProperty(name))
            return defaultValue;

        return _properties[name];
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value)
    {
        _properties[name] = value;
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const
    {
        return _properties.contains(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const
    {
        return _properties.keys();
    }

public:

    /** Get group index */
    std::int32_t getGroupIndex() const;

    /**
     * Set group index
     * @param groupIndex group index
     */
    void setGroupIndex(const std::int32_t& groupIndex);

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
    QMenu* getContextMenu(QWidget* parent = nullptr);

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);;

protected:

    /** Get raw data */
    template <class DataType>
    DataType& getRawData() const
    {
        if (_rawData == nullptr)
            _rawData = &dynamic_cast<DataType&>(_core->requestRawData(getRawDataName()));

        return *static_cast<DataType*>(_rawData);
    }

    /** Get the name of the raw data */
    QString getRawDataName() const
    {
        return _rawDataName;
    }

    /**
     * Set whether this set represents all the data or only a subset
     * @param all Whether this is a full dataset
     */
    void setAll(bool all)
    {
        _all = all;
    }

public: // Operators

    /**
     * Assignment operator
     * @param other Reference to assign from
     */
    DatasetImpl& operator=(const DatasetImpl& other)
    {
        _core           = other._core;
        _guiName        = other._guiName;
        _rawDataName    = other._rawDataName;
        _all            = other._all;
        _derived        = other._derived;
        _sourceDataset  = other._sourceDataset;
        _properties     = other._properties;

        return *this;
    }

protected:
    CoreInterface*              _core;              /** Pointer to the core interface */

private:
    mutable plugin::RawData*    _rawData;           /** Pointer to the raw data referenced in this set */
    QString                     _guid;              /** Globally unique dataset name */
    QString                     _guiName;           /** Name of the dataset in the graphical user interface */
    QString                     _rawDataName;       /** Name of the raw data */
    bool                        _all;               /** Whether this is the full dataset */
    bool                        _derived;           /** Whether this dataset is derived from another dataset */
    Dataset<DatasetImpl>        _sourceDataset;     /** Smart pointer to the source dataset (if any) */
    QMap<QString, QVariant>     _properties;        /** Properties map */
    std::int32_t                _groupIndex;        /** Group index (sets with identical indices can for instance share selection) */

    friend class Core;
    friend class DataManager;
    friend class EventListener;

    template<typename DatasetType>
    friend class SmartDataset;
};

} // namespace hdps

#endif // HDPS_DATASET_H
