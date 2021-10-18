#ifndef HDPS_DATASET_H
#define HDPS_DATASET_H

#include "CoreInterface.h"
#include "RawData.h"

#include <QString>
#include <QVector>
#include <memory>

namespace hdps
{

class DataHierarchyItem;

using SharedDataHierarchyItem = QSharedPointer<DataHierarchyItem>;

class DataSet
{
public:
    DataSet(CoreInterface* core, QString dataName) :
        _core(core),
        _dataName(dataName),
        _all(false),
        _rawData(nullptr)
    {
    }

    virtual ~DataSet() {}

    virtual void init() {};

    virtual DataSet* copy() const = 0;

    /**
     * Create subset
     * @param subsetName Name of the subset
     * @param parentSetName Name of the parent dataset
     * @param visible Whether the subset will be visible in the UI
     */
    virtual QString createSubset(const QString subsetName = "subset", const QString parentSetName = "", const bool& visible = true) const = 0;

    QString getName() const
    {
        return _name;
    }

    void setName(QString name)
    {
        _name = name;
    }

    /**
     * Returns true if this set represents the full data and false if it's a subset.
     */
    bool isFull() const
    {
        return _all;
    }

    bool isDerivedData() const
    {
        return _derived;
    }

    /**
     * Returns the data type of the raw data associated with this dataset.
     */
    DataType getDataType() const
    {
        return _core->requestRawData(getDataName()).getDataType();
    }

    /**
     * If the given set is derived from another set, then calls this function on the other set.
     * If the given set is not derived from another set, then returns the given set.
     */
    template <class T>
    static T& getSourceData(T& set)
    {
        return set.isDerivedData() ? getSourceData(static_cast<T&>(set._core->requestData(set._sourceSetName))) : set;
    }

    /**
     * Marks this dataset as derived and sets the dataset it's derived from to sourceDataName.
     */
    void setSourceData(QString sourceDataName)
    {
        _sourceSetName = sourceDataName;
        _derived = true;
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection associated with this data set
     */
    DataSet& getSelection() const
    {
        return _core->requestSelection(getSourceData(*this).getDataName());
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection of dataset type associated with this data set
     */
    template<typename DatasetType>
    DatasetType& getSelection() const
    {
        return dynamic_cast<DatasetType&>(_core->requestSelection(getSourceData(*this).getDataName()));
    }

    void setSelection(std::vector<unsigned int> indices)
    {

    }

    /** Get icon for the dataset */
    virtual QIcon getIcon() const = 0;

    /** Get reference to data hierarchy item */
    DataHierarchyItem& getHierarchyItem();

    /** Get reference to data hierarchy item */
    const DataHierarchyItem& getHierarchyItem() const;

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
    QMenu* getContextMenu(QWidget* parent = nullptr);;

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);;

protected:
    template <class DataType>
    DataType& getRawData() const
    {
        if (_rawData == nullptr)
            _rawData = &dynamic_cast<DataType&>(_core->requestRawData(getDataName()));
        return *static_cast<DataType*>(_rawData);
    }

    QString getDataName() const
    {
        return _dataName;
    }

    QString getSourceName() const
    {
        return _sourceSetName;
    }

    /**
     * Set whether this set represents all the data or only a subset
     */
    void setAll(bool all)
    {
        _all = all;
    }

    CoreInterface* _core;
private:
    mutable plugin::RawData*    _rawData;

    QString                     _name;                  /** Name of the dataset */
    QString                     _dataName;              /** Name of the raw data */
    bool                        _all;                   /** Whether this is the full dataset */
    bool                        _derived = false;       /** Whether this dataset is derived from another dataset */
    QString                     _sourceSetName;         /** Name of the source dataset (if any) */
    QMap<QString, QVariant>     _properties;            /** Properties map */

    friend class Core;
    friend class DataManager;
    friend class EventListener;
};

} // namespace hdps

#endif // HDPS_DATASET_H
