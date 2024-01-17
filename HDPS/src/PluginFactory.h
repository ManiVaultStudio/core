// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PluginType.h"
#include "DataType.h"
#include "Dataset.h"
#include "actions/PluginTriggerAction.h"

#include <QObject>
#include <QIcon>
#include <QVariant>

namespace mv
{
    class DatasetImpl;

    namespace gui
    {
        class PluginTriggerAction;

        using PluginTriggerActions = QVector<QPointer<PluginTriggerAction>>;
    }

    class PluginGlobalSettingsGroupAction;

namespace plugin
{

class Plugin;

class PluginFactory : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param type The plugin type
     */
    PluginFactory(Type type);

    /**
     * Get plugin kind
     * @return Plugin kind
     */
    QString getKind() const;

    /**
     * Set plugin kind
     * @param kind Plugin kind
     */
    void setKind(const QString& kind);

    /**
     * Get the plugin type
     * @return Plugin type
     */
    Type getType() const;

    /** Perform post-construction initialization */
    virtual void initialize();

public: // Global settings

    /**
     * Get settings prefix
     * @return Plugin factory global settings prefix
     */
    virtual QString getGlobalSettingsPrefix() const final;

    /**
     * Get global settings group action
     * @return Pointer to plugin global settings group action (maybe nullptr if plugin does not have global settings)
     */
    virtual PluginGlobalSettingsGroupAction* getGlobalSettingsGroupAction() const;

    /**
     * Set plugin global settings group action to \p pluginGlobalSettingsGroupAction
     * @param pluginGlobalSettingsGroupAction Pointer to plugin global settings group action (maybe a nullptr)
     */
    virtual void setGlobalSettingsGroupAction(PluginGlobalSettingsGroupAction* pluginGlobalSettingsGroupAction) final;

public: // Help

    /**
     * Get whether the plugin has help information or not
     * @return Boolean determining whether the plugin has help information or not
     */
    virtual bool hasHelp();

    /**
     * Get trigger action that shows help in some form (will be added to help menu, and if it is a view plugin also to the tab toolbar)
     * @return Reference to show help trigger action (maybe nullptr if the plugin does not provide any help)
     */
    virtual gui::TriggerAction& getTriggerHelpAction() final;

public: // GUI name

    /** Get the menu name of the plugin */
    QString getGuiName() const;

    /**
     * Set the GUI name of the plugin
     * @param guiName GUI name of the plugin
     */
    void setGuiName(const QString& guiName);

public: // Version

    /** Get the plugin version */
    QString getVersion() const;

    /**
     * Set the plugin version
     * @param version Plugin version
     */
    void setVersion(const QString& version);

public:

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    virtual QIcon getIcon(const QColor& color = Qt::black) const;

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    virtual Plugin* produce() = 0;

    /**
     * Get whether a plugin may be produced
     * @return Boolean determining whether a plugin may be produced
     */
    virtual bool mayProduce() const final;

    /**
     * Get the data types that the plugin supports
     * @return Supported data types
     */
    virtual mv::DataTypes supportedDataTypes() const {
        return mv::DataTypes();
    }

    /**
     * Get the trigger action that produces an instance of the plugin
     * @return Reference to a trigger action that produces an instance of the plugin
     */
    virtual gui::PluginTriggerAction& getPluginTriggerAction();

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const Datasets& datasets) const {
        return gui::PluginTriggerActions();
    }

    /**
     * Get plugin trigger actions given \p dataTypes
     * @param datasetTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const DataTypes& dataTypes) const {
        return gui::PluginTriggerActions();
    }

public: // Number of instances

    /** Get number of plugin instances currently loaded */
    std::uint32_t getNumberOfInstances() const;

    /**
     * Set number of plugin instances currently loaded
     * @param numberOfInstances Number of plugin instances currently loaded
     */
    void setNumberOfInstances(std::uint32_t numberOfInstances);

    /** Get maximum number of allowed plugin instances */
    std::uint32_t getMaximumNumberOfInstances() const;

    /**
     * Set maximum number of allowed plugin instances
     * @param maximumNumberOfInstances Maximum number of allowed plugin instances
     */
    void setMaximumNumberOfInstances(std::uint32_t maximumNumberOfInstances);

protected:

    /**
     * Get sequence of input datasets as a string list
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @return String list of input dataset types
     */
    static QStringList getDatasetTypesAsStringList(const Datasets& datasets);

    /**
     * Determine whether all datasets are of the same data type
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @param dataType Type of data
     * @return Whether each dataset is of the same data type
     */
    static bool areAllDatasetsOfTheSameType(const Datasets& datasets, const DataType& dataType);

    /**
     * Get number of datasets for \p dataType
     * @param dataType Data type
     * @return Number of datasets of \p datasetType
     */
    static std::uint16_t getNumberOfDatasetsForType(const Datasets& datasets, const DataType& dataType);

signals:

    /**
     * Signals that the number of instances changed to \p numberOfInstances
     * @param numberOfInstances Number of plugin instances
     */
    void numberOfInstancesChanged(std::uint32_t numberOfInstances);

    /**
     * Signals that the plugin global settings group action changed to \p pluginGlobalSettingsGroupAction
     * @param pluginGlobalSettingsGroupAction Pointer to plugin global settings group action (maybe a nullptr)
     */
    void pluginGlobalSettingsGroupActionChanged(PluginGlobalSettingsGroupAction* pluginGlobalSettingsGroupAction);

private:
    QString                             _kind;                                  /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type                                _type;                                  /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QString                             _guiName;                               /** Name of the plugin in the GUI */
    QString                             _version;                               /** Plugin version */
    gui::PluginTriggerAction            _pluginTriggerAction;                   /** Standard plugin trigger action that produces an instance of the plugin without any special behavior (respects the maximum number of allowed instances) */
    std::uint32_t                       _numberOfInstances;                     /** Number of plugin instances */
    std::uint32_t                       _maximumNumberOfInstances;              /** Maximum number of plugin instances (unlimited when -1) */
    gui::TriggerAction                  _triggerHelpAction;                     /** Trigger action that triggers help (icon and text are already set) */
    PluginGlobalSettingsGroupAction*    _pluginGlobalSettingsGroupAction;       /** Pointer to plugin global settings group action (maybe a nullptr) */
};

}

}

Q_DECLARE_INTERFACE(mv::plugin::PluginFactory, "hdps.PluginFactory")
