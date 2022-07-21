#pragma once

#include "PluginType.h"

#include <QStringList>

namespace hdps
{

namespace plugin
{

/**
 * Plugin producer meta data class
 */
class PluginProducerMetaData
{
public:

    /**
     * Constructor
     * * @param pluginType Type of plugin
     * * @param pluginKind Kind of plugin
     * * @param datasetTypes Sequence of dataset types for which this plugin producer is valid
     * * @param title Plugin producer title
     * * @param description Plugin producer description
     */
    PluginProducerMetaData(const Type& pluginType, const QString& pluginKind, const QStringList& datasetTypes, const QString& title, const QString& description);

    /** Get the plugin type */
    Type getPluginType() const {
        return _pluginType;
    }

    /**
     * Set the plugin type
     * @param pluginType Type of plugin
     */
    void setPluginType(const Type& pluginType) {
        _pluginType = pluginType;
    }

    /** Get the plugin kind */
    QString getPluginKind() const {
        return _pluginKind;
    }

    /**
     * Set the plugin kind
     * @param pluginKind Kind of plugin
     */
    void setPluginKind(const QString& pluginKind) {
        _pluginKind = pluginKind;
    }

    /** Get sequence of dataset types for which this plugin producer is valid */
    QStringList getDatasetTypes() const {
        return _datasetTypes;
    }

    /**
     * Set sequence of dataset types for which this plugin producer is valid
     * @param datasetTypes Sequence of dataset types for which this plugin producer is valid
     */
    void setDatasetTypes(const QStringList& datasetTypes) {
        _datasetTypes = datasetTypes;
    }

    /** Get the producer title (used in the user interface) */
    QString getTitle() const {
        return _title;
    }

    /**
     * Set the producer title (used in the user interface)
     * @param title Producer title
     */
    void setTitle(const QString& title) {
        _title = title;
    }

    /** Get the producer description (used in the user interface) */
    QString getDescription() const {
        return _description;
    }

    /**
     * Set the producer description (used in the user interface)
     * @param title Producer description
     */
    void setDescription(const QString& description) {
        _description = description;
    }

    /**
     * Assignment operator
     * @param other Plugin producer meta data to assign
     * @return Assigned plugin producer meta data
     */
    PluginProducerMetaData& operator=(const PluginProducerMetaData& other)
    {
        _pluginType     = other._pluginType;
        _pluginKind     = other._title;
        _datasetTypes   = other._datasetTypes;
        _title          = other._title;
        _description    = other._description;

        return *this;
    }

private:
    Type            _pluginType;        /** Type of plugin */
    QString         _pluginKind;        /** Kind of the plugin */
    QStringList     _datasetTypes;      /** Sequence of dataset types for which this plugin producer is valid */
    QString         _title;             /** Producer title (menu text) */
    QString         _description;       /** Producer description (menu tooltip) */
};

using PluginProducersMetaData = QList<PluginProducerMetaData>;

}
}