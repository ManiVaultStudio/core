// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "PluginMetadata.h"
#include "Dataset.h"
#include "DataType.h"
#include "PluginType.h"

#include "actions/PluginTriggerAction.h"

#include <QObject>

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

class CORE_EXPORT PluginFactory : public gui::WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param type The plugin type
     * @param title The plugin factory title
     */
    PluginFactory(Type type, const QString& title);

    /** No need for custom destructor */
    ~PluginFactory() override = default;

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

    /**
     * Get plugin category icon
     * @return Category icon
     */
    const QIcon& getCategoryIcon() const;

protected:

    /**
     * Set category icon by name
     * @param category Category name
     */
    void setCategoryIconByName(const QString& category);

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

public: // Meta data

    /** Get the menu name of the plugin */
    QString getGuiName() const;

    /**
     * Set the GUI name of the plugin
     * @param guiName GUI name of the plugin
     */
    void setGuiName(const QString& guiName);

    /**
     * Get plugin version
     * @return Plugin semantic version
     */
    util::Version& getVersion();

    /**
     * Get plugin version
     * @return Plugin semantic version
     */
    const util::Version& getVersion() const;

    /**
     * Set the plugin version to \p version
     * @param version Plugin semantic version
     */
    void setVersion(const util::Version& version);

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    virtual Plugin* produce() = 0;

    /**
     * Get whether a plugin may be produced
     * @return Boolean determining whether a plugin may be produced
     */
    bool mayProduce() const;

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

    /**
     * Set whether the plugin may be produced from the standard GUI to \p allowPluginCreationFromStandardGui
     * @param allowPluginCreationFromStandardGui Boolean determining whether a plugin instance may be created from the standard GUI (e.g. main menu etc.)
     */
    void setAllowPluginCreationFromStandardGui(bool allowPluginCreationFromStandardGui);

    /**
     * Get whether the plugin may be produced from the standard GUI
     * @return Boolean determining whether a plugin instance may be created from the standard GUI (e.g. main menu etc.)
     */
    bool getAllowPluginCreationFromStandardGui() const;

public: // Number of instances

    /**
     * Get number of plugin instances currently loaded
     * @return Number of plugin instances currently loaded
     */
    std::uint32_t getNumberOfInstances() const;

    /**
     * Set number of plugin instances currently loaded
     * @param numberOfInstances Number of plugin instances currently loaded
     */
    void setNumberOfInstances(std::uint32_t numberOfInstances);

    /**
     * Get number of plugin instances produced in total
     * @return Number of plugin instances produced in total
     */
    std::uint32_t getNumberOfInstancesProduced() const;

    /**
     * Set number of plugin instances produced in total
     * @param numberOfInstancesProduced Number of plugin instances produced in total
     */
    void setNumberOfInstancesProduced(std::uint32_t numberOfInstancesProduced);

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

public: // Miscellaneous

    /** View the shortcut map */
    virtual void viewShortcutMap();

    /**
     * Get plugin metadata
     * @return Reference to plugin metadata
     */
    PluginMetadata& getPluginMetadata();

    /**
     * Get plugin metadata
     * @return Reference to plugin metadata
     */
    const PluginMetadata& getPluginMetadata() const;

public: // Action getters

    gui::StringsAction& getTutorialsDsnsAction()  { return _tutorialsDsnsAction; }
    gui::StringsAction& getProjectsDsnsAction()  { return _projectsDsnsAction; }

signals:

    /**
     * Signals that the number of instances changed to \p numberOfInstances
     * @param numberOfInstances Number of plugin instances
     */
    void numberOfInstancesChanged(std::uint32_t numberOfInstances);

    /**
     * Signals that the number of instances produced in total changed to \p numberOfInstancesProduced
     * @param numberOfInstancesProduced Number of plugin instances produced in total
     */
    void numberOfInstancesProducedChanged(std::uint32_t numberOfInstancesProduced);

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
     * Signals that the plugin title changed from \p previousPluginTitle to \p currentPluginTitle
     * @param previousPluginTitle Previous plugin title
     * @param currentPluginTitle Current plugin title
     */
    void pluginTitleChanged(const QString& previousPluginTitle, const QString& currentPluginTitle);

    /**
     * Signals that the description changed from \p previousDescription to \p currentDescription
     * @param previousDescription Previous description
     * @param currentDescription Current description
     */
    void descriptionChanged(const QString& previousDescription, const QString& currentDescription);

    /**
     * Signals that the summary changed from \p previousSummary to \p currentSummary
     * @param previousSummary Previous summary
     * @param currentSummary Current summary
     */
    void summaryChanged(const QString& previousSummary, const QString& currentSummary);

    /**
     * Signals that the authors changed from \p previousAuthors to \p currentAuthors
     * @param previousAuthors Previous authors
     * @param currentAuthors Current authors
     */
    void authorsChanged(const QStringList previousAuthors, const QStringList& currentAuthors);

    /**
     * Signals that the copyright notice changed from \p previousCopyrightNotice to \p currentCopyrightNotice
     * @param previousCopyrightNotice Previous copyright notice
     * @param currentCopyrightNotice Current copyright notice
     */
    void copyrightNoticeChanged(const QString& previousCopyrightNotice, const QString& currentCopyrightNotice);

    /**
     * Signals that the about text in Markdown format changed from \p previousAboutMarkdown to \p currentAboutMarkdown
     * @param previousAboutMarkdown Previous about text in Markdown format
     * @param currentAboutMarkdown Current about text in Markdown format
     */
    void aboutMarkdownChanged(const QString& previousAboutMarkdown, const QString& currentAboutMarkdown);

private:
    QString                                 _kind;                                  /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    Type                                    _type;                                  /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    gui::PluginTriggerAction                _pluginTriggerAction;                   /** Standard plugin trigger action that produces an instance of the plugin without any special behavior (respects the maximum number of allowed instances) */
    std::uint32_t                           _numberOfInstances;                     /** Number of plugin instances */
    std::uint32_t                           _numberOfInstancesProduced;             /** Number of plugin instances produced since the beginning of the factory */
    std::uint32_t                           _maximumNumberOfInstances;              /** Maximum number of plugin instances (unlimited when -1) */
    gui::PluginGlobalSettingsGroupAction*   _pluginGlobalSettingsGroupAction;       /** Pointer to plugin global settings group action (maybe a nullptr) */
    gui::PluginStatusBarAction*             _statusBarAction;                       /** Pointer to plugin status bar action (maybe a nullptr) */
    bool                                    _allowPluginCreationFromStandardGui;    /** Boolean determining whether a plugin instance may be created from the standard GUI (e.g. main menu etc.) */
    PluginMetadata                          _pluginMetadata;                        /** Plugin metadata */
	QIcon                                   _categoryIcon;                          /** Category icon */
    gui::StringsAction                      _tutorialsDsnsAction;                   /** Action for editing the tutorials Data Source Names */
    gui::StringsAction                      _projectsDsnsAction;                    /** Action for editing the project Data Source Names */
};

}

Q_DECLARE_INTERFACE(mv::plugin::PluginFactory, "ManiVault.PluginFactory")
