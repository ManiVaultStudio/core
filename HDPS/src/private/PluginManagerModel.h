#pragma once

#include <Plugin.h>
#include <PluginFactory.h>

#include <QAbstractItemModel>

/**
 * Loaded plugins model class
 *
 * Model implementation for loaded plugins
 *
 * @author Thomas Kroes
 */
class LoadedPluginsModel : public QAbstractItemModel
{
public:

    /** Loaded plugin (instance) columns */
    enum class Column {
        Name,               /** Name of the loaded plugin (instance) */
        Type,               /** Type of plugin */
        Kind,               /** Kind of plugin */
        ID,                 /** Globally unique identifier of the plugin instance */
        NumberOfInstances,  /** Number of instances of a plugin kind */

        Count
    };

public:

    /**
     * Construct from \p parent
     * @param parent Pointer to parent object
     */
    LoadedPluginsModel(QObject* parent = nullptr);

    /** No need for copy constructor */
    LoadedPluginsModel(const LoadedPluginsModel& loadedPluginsModel) = delete;

    /** No need for destructor */
    ~LoadedPluginsModel() override = default;

    /** No need for assignment operator */
    LoadedPluginsModel& operator=(const LoadedPluginsModel& loadedPluginsModel) = delete;

    /**
     * Returns the number of rows for the \p parent model index
     * @param parent Parent model index
     * @return Number of rows in the model given the parent model index
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the number of columns for the \p parent model index
     * @param parent Parent model index
     * @return Number of columns in the model given the parent model index
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Returns the model index for the \p row, \p column and \p parent model index
     * @param row Row
     * @param column Column
     * @param parent Parent model index
     * @return Model index
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get parent model index of \p index
     * @param index Index to get the parent for
     * @return Parent model index
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * Returns the variant data for model \p index and data \p role
     * @param index Model index
     * @param role Data role
     * @return Data in variant form
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * Sets the data \p value for model \p index and data \p role
     * @param index Model index
     * @param value Value in variant form
     * @param role Data role
     * @return Whether the data was properly set or not
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    /**
     * Get header data for \p section, \p orientation and \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Get item flags for model \p index
     * @param index Model index
     * @return Item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:

    /** Synchronizes the internal data with the  */
    //void synchronizeWithPluginManager();

    //void addPlugin

    QMap<hdps::plugin::PluginFactory*, hdps::plugin::Plugin*>   _pluginFactories;
};
