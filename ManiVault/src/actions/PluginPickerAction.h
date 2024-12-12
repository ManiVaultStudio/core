// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"

#include "models/PluginsListModel.h"
#include "models/PluginsFilterModel.h"

#include <QAbstractListModel>

namespace mv::gui {

/**
 * Plugin picker action class
 *
 * Action class for picking a plugin from a list.
 *
 * Note: This class is not fully production yet, it needs some thorough testing
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PluginPickerAction : public OptionAction
{
Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE PluginPickerAction(QObject* parent, const QString& title);

    /**
     * Get plugins
     * @return Vector of plugins
     */
    plugin::Plugins getPlugins() const;

    /**
     * Set the \p plugins from which can be picked (mode is set to StorageMode::Manual)
     * @param plugins Plugins from which can be picked
     * @param silent Whether the signal pluginsChanged is emitted
     */
    void setPlugins(const plugin::Plugins& plugins, bool silent = false);

    /**
     * Set plugins filter function to \p filterFunction
     * @param filterFunction Filter lambda (triggered when plugins are added and/or removed)
     */
    void setFilterFunction(const PluginsFilterModel::FilterFunction& filterFunction);

    /**
     * Get selected plugin
     * @return Pointer to selected plugin (maybe nullptr)
     */
    plugin::Plugin* getCurrentPlugin() const;

    /**
     * Get selected plugin of \p PluginType
     * @return Pointer to selected plugin of \p PluginType (maybe nullptr)
     */
    template<typename PluginType>
    inline PluginType* getCurrentPlugin() const
    {
        return dynamic_cast<PluginType>(getCurrentPlugin());
    }

    /**
     * Set current plugin to \p currentPlugin
     * @param currentPlugin Pointer to current plugin (maybe nullptr)
     */
    void setCurrentPlugin(const plugin::Plugin* currentPlugin);

    /**
     * Set current plugin by \p pluginId
     * @param pluginId Current plugin globally unique identifier
     */
    void setCurrentPlugin(const QString& pluginId);

    /**
     * Get current plugin globally unique identifier
     * @return The globally unique identifier of the currently selected plugin (if any)
     */
    QString getCurrentPluginId() const;

public: // Population

    /**
     * Get current population mode
     * @return Population mode
     */
    AbstractPluginsModel::PopulationMode getPopulationMode() const;

    /**
     * Set population mode to \p populationMode
     * @param populationMode Population mode
     */
    void setPopulationMode(AbstractPluginsModel::PopulationMode populationMode);

public: // Plugin type filtering

    /**
     * Get plugin types to filter
     * @return Plugin types
     */
    plugin::Types getFilterPluginTypes() const;

    /**
     * Set plugin filter types to \p filterPluginTypes
     * @param filterPluginTypes Plugin filter types
     */
    void setFilterPluginTypes(const plugin::Types& filterPluginTypes);

private:

    /** Handle changes to the population mode */
    void populationModeChanged();

    /** Blocks the PluginPickerAction::pluginsChanged() signal from being emitted */
    void blockPluginsChangedSignal();

    /** Allows the PluginPickerAction::pluginsChanged() signal to be emitted */
    void unblockPluginsChangedSignal();

    /**
     * Get whether the PluginPickerAction::pluginsChanged() may be emitted
     * @return Boolean determining whether the PluginPickerAction::pluginsChanged() may be emitted
     */
    bool isPluginsChangedSignalBlocked() const;

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that \p pickedPlugin is picked
     * @param pickedPlugin Pointer to picked plugin
     */
    void pluginPicked(plugin::Plugin* pickedPlugin);

    /**
     * Signals that selectable plugins changed
     * @param plugins Selectable plugins
     */
    void pluginsChanged(const plugin::Plugins& plugins);

    /**
     * Signals that the population mode changed from \p previousPopulationMode to \p currentPopulationMode
     * @param previousPopulationMode Previous population mode
     * @param currentPopulationMode Current population mode
     */
    void populationModeChanged(AbstractPluginsModel::PopulationMode previousPopulationMode, AbstractPluginsModel::PopulationMode currentPopulationMode);

    /**
     * Signals that the filter plugin types changed from \p previousPopulationMode to \p currentPopulationMode
     * @param previousFilterPluginTypes Previous filter plugin types
     * @param currentFilterPluginTypes Current filter plugin types
     */
    void filterPluginTypesChanged(const plugin::Types& previousFilterPluginTypes, const plugin::Types& currentFilterPluginTypes);

private:
    AbstractPluginsModel::PopulationMode    _populationMode;                /** Population mode (e.g. manual or automatic) */
    PluginsListModel                        _listModel;                     /** Plugins list model */
    PluginsFilterModel                      _filterModel;                   /** Filter model for the plugins model above */
    bool                                    _blockPluginsChangedSignal;     /** Boolean determining whether the PluginPickerAction::pluginsChanged(...) signal may be engaged in response to change in the PluginPickerAction#_filterModel */
    QStringList                             _currentPluginsIds;             /** Keep a list of current plugins identifiers so that we can avoid unnecessary emits of the PluginPickerAction::pluginsChanged(...) signal */

    friend class AbstractPluginsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::PluginPickerAction)

inline const auto pluginPickerActionMetaTypeId = qRegisterMetaType<mv::gui::PluginPickerAction*>("mv::gui::PluginPickerAction");
