// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Dataset.h"
#include "DataType.h"
#include "PluginType.h"

#include "actions/PluginTriggerAction.h"

#include "util/ShortcutMap.h"

#include <QObject>
#include <QIcon>
#include <QVariant>

namespace mv {
    class DatasetImpl;

    namespace gui
    {
        class PluginTriggerAction;
        class PluginGlobalSettingsGroupAction;
        class PluginStatusBarAction;

        using PluginTriggerActions = QVector<QPointer<PluginTriggerAction>>;
    }
}

namespace mv::plugin
{

class Plugin;

class CORE_EXPORT PluginFactory : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param type The plugin type
     */
    PluginFactory(Type type);

    virtual ~PluginFactory() { }

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
    virtual gui::PluginGlobalSettingsGroupAction* getGlobalSettingsGroupAction() const;

    /**
     * Set plugin global settings group action to \p pluginGlobalSettingsGroupAction
     * @param pluginGlobalSettingsGroupAction Pointer to plugin global settings group action (maybe a nullptr)
     */
    void setGlobalSettingsGroupAction(gui::PluginGlobalSettingsGroupAction* pluginGlobalSettingsGroupAction);

public: // Status bar

    /**
     * Get plugin status bar action
     * @return Pointer to plugin status bar action (maybe nullptr)
     */
    gui::PluginStatusBarAction* getStatusBarAction() const;

    /**
     * Set plugin status bar action to \p statusBarAction
     * @param statusBarAction Pointer to plugin status bar action (maybe a nullptr)
     */
    void setStatusBarAction(gui::PluginStatusBarAction* statusBarAction);

public: // Help

    /**
     * Get whether the plugin has help information or not
     * @return Boolean determining whether the plugin has help information or not
     */
    virtual bool hasHelp() const;

    /**
     * Get the read me markdown file URL
     * @return URL of the read me markdown file
     */
    [[deprecated("This function is under development and has no effect (yet).")]]
    virtual QUrl getReadmeMarkdownUrl() const;

    /**
     * Get the URL of the GitHub repository
     * @return URL of the GitHub repository (or readme markdown URL if set)
     */
    virtual QUrl getRepositoryUrl() const;

    /**
     * Get the name of the default branch
     * @return Name of the default branch
     */
    virtual QString getDefaultBranch() const;

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
     * Get plugin category (loader/writer/transformation etc.) icon
     * @return Icon which belongs to the plugin factory category
     */
    virtual QIcon getCategoryIcon() const = 0;

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
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const DataTypes& dataTypes) const {
        return gui::PluginTriggerActions();
    }

    /**
     * Initializes a list of plugin trigger actions
     * @param pluginTriggerActions List of plugin trigger actions to be initialized
     */
    static void initializePluginTriggerActions(gui::PluginTriggerActions& pluginTriggerActions) {
        for (auto& pluginTriggerAction : pluginTriggerActions)
            pluginTriggerAction->initialize();
    }

public: // Shortcut map

    /**
     * Get shortcut map
     * @return Reference to the shortcut map
     */
    virtual util::ShortcutMap& getShortcutMap() final;

    /**
     * Get shortcut map
     * @return Const reference to the shortcut map
     */
    virtual const util::ShortcutMap& getShortcutMap() const final;

public: // Description

    /**
     * Get short description
     * @return String that shortly describes the plugin
     */
    QString getShortDescription() const;

    /**
     * Set short description to \p shortDescription
     * @param shortDescription String that shortly describes the plugin
     */
    void setShortDescription(const QString& shortDescription);

    /**
     * Get extended description in HTML format
     * @return Extended description in HTML format
     */
    [[deprecated("This function is under development and has no effect (yet).")]]
    QString getLongDescription() const;

    /**
     * Get extended description in Markdown format
     * @return Extended description in Markdown format
     */
    [[deprecated("This function is under development and has no effect (yet).")]]
    QString getLongDescriptionMarkdown() const;

    /**
     * Set long description to \p longDescription
     * @param longDescription Extended description in HTML format
     */
    [[deprecated("This function is under development and has no effect (yet).")]]
    void setLongDescription(const QString& longDescription);

    /**
     * Set long description Markdown to \p longDescription
     * @param longDescriptionMarkdown Extended description in Markdown format
     */
    [[deprecated("This function is under development and has no effect (yet).")]]
    void setLongDescriptionMarkdown(const QString& longDescriptionMarkdown);

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

public: // Views

    /** View the shortcut map */
    virtual void viewShortcutMap();

public: // Action getters

    gui::TriggerAction& getTriggerHelpAction() { return _triggerHelpAction; };
    gui::TriggerAction& getTriggerReadmeAction() { return _triggerReadmeAction; };
    gui::TriggerAction& getVisitRepositoryAction() { return _visitRepositoryAction; };

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
    void pluginGlobalSettingsGroupActionChanged(gui::PluginGlobalSettingsGroupAction* pluginGlobalSettingsGroupAction);

    /**
     * Signals that the plugin status bar action changed to \p statusBarAction
     * @param statusBarAction Pointer to plugin status bar action (maybe a nullptr)
     */
    void statusBarActionChanged(gui::PluginStatusBarAction* statusBarAction);

    /**
     * Signals that the short description changed from \p previousShortDescription to \p currentShortDescription
     * @param previousShortDescription Previous short description
     * @param currentShortDescription Current short description
     */
    void shortDescriptionChanged(const QString& previousShortDescription, const QString& currentShortDescription);
    
    /**
     * Signals that the HTML-formatted long description changed from \p previousLongDescription to \p currentLongDescription
     * @param previousLongDescription Previous long description in HTML format
     * @param currentLongDescription Current long description in HTML format
     */
    void longDescriptionChanged(const QString& previousLongDescription, const QString& currentLongDescription);

    /**
     * Signals that the Markdown-formatted long description format changed from \p previousLongDescriptionMarkdown to \p currentLongDescriptionMarkdown
     * @param previousLongDescriptionMarkdown Previous long description in Markdown format
     * @param currentLongDescriptionMarkdown Current long description in Markdown format
     */
    void longDescriptionMarkdownChanged(const QString& previousLongDescriptionMarkdown, const QString& currentLongDescriptionMarkdown);

private:
    QString                                 _kind;                                  /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type                                    _type;                                  /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QString                                 _guiName;                               /** Name of the plugin in the GUI */
    QString                                 _version;                               /** Plugin version */
    gui::PluginTriggerAction                _pluginTriggerAction;                   /** Standard plugin trigger action that produces an instance of the plugin without any special behavior (respects the maximum number of allowed instances) */
    std::uint32_t                           _numberOfInstances;                     /** Number of plugin instances */
    std::uint32_t                           _maximumNumberOfInstances;              /** Maximum number of plugin instances (unlimited when -1) */
    gui::TriggerAction                      _triggerHelpAction;                     /** Trigger action that triggers help (icon and text are already set) */
    gui::TriggerAction                      _triggerReadmeAction;                   /** Trigger action that displays the read me markdown text in a modal dialog (if the read me markdown file URL is valid) */
    gui::TriggerAction                      _visitRepositoryAction;                 /** Trigger action that opens an external browser and visits the GitHub repository */
    gui::PluginGlobalSettingsGroupAction*   _pluginGlobalSettingsGroupAction;       /** Pointer to plugin global settings group action (maybe a nullptr) */
    gui::PluginStatusBarAction*             _statusBarAction;                       /** Pointer to plugin status bar action (maybe a nullptr) */
    util::ShortcutMap                       _shortcutMap;                           /** Shortcut cheatsheet map */
    QString                                 _shortDescription;                      /** Shortly describes the plugin */
    QString                                 _longDescription;                       /** Extended description in HTML format */
    QString                                 _longDescriptionMarkdown;               /** Extended description in Markdown format */
};

}

Q_DECLARE_INTERFACE(mv::plugin::PluginFactory, "ManiVault.PluginFactory")
