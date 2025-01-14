// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/ShortcutMap.h"

#include "actions/TriggerAction.h"

#include <QObject>

namespace mv::plugin
{

class PluginFactory;

/**
 * Plugin metadata plugin class
 *
 * Contains plugin properties
 *
 * @author T. Kroes
 */
class CORE_EXPORT PluginMetadata : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct with reference to parent \p pluginFactory
     * @param pluginFactory Reference to parent plugin factory
     */
    PluginMetadata(const PluginFactory& pluginFactory);

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

public: // Description

    /**
     * Get description
     * @return String that shortly describes the plugin
     */
    QString getDescription() const;

    /**
     * Set description to \p description
     * @param description String that shortly describes the plugin
     */
    void setDescription(const QString& description);

    /**
     * Get whether the plugin has a description or not
     * @return Boolean determining whether the plugin has a description or not
     */
    bool hasDescription() const;

public: // Summary

    /**
     * Get summary
     * @return Elaborate description of the plugin
     */
    QString getSummary() const;

    /**
     * Set summary to \p summary
     * @param summary Elaborate description of the plugin
     */
    void setSummary(const QString& summary);

    /**
     * Get whether the plugin has a summary or not
     * @return Boolean determining whether the plugin has a summary or not
     */
    bool hasSummary() const;

public: // Authors

    /**
     * Get authors
     * @return Authors that created and maintain the plugin
     */
    QStringList getAuthors() const;

    /**
     * Set authors to \p authors
     * @param authors Authors that created and maintain the plugin
     */
    void setAuthors(const QStringList& authors);

    /**
     * Get whether the plugin has authors assigned or not
     * @return Boolean determining whether the plugin has authors assigned or not
     */
    bool hasAuthors() const;

public: // Copyright notice

    /**
     * Get copyright notice
     * @return Copyright notice
     */
    QString getCopyrightNotice() const;

    /**
     * Set copyrightNotice to \p copyrightNotice
     * @param copyrightNotice Copyright notice
     */
    void setCopyrightNotice(const QString& copyrightNotice);

    /**
     * Get whether the plugin has a copyright notice or not
     * @return Boolean determining whether the plugin has a copyright notice or not
     */
    bool hasCopyrightNotice() const;

public: // About markdown

    /**
     * Get about text in Markdown format
     * @return About text in Markdown format. When set, it overrides the standardized about Markdown generated from summary, authors and copyright notice.
     */
    QString getAboutMarkdown() const;

    /**
     * Set about text to \p aboutMarkdown
     * @param aboutMarkdown About text in Markdown format. When set, it overrides the standardized about Markdown generated from summary, authors and copyright notice.
     */
    void setAboutMarkdown(const QString& aboutMarkdown);

    /**
     * Get whether the plugin has about Markdown or not
     * @return Boolean determining whether the plugin has about Markdown or not
     */
    bool hasAboutMarkdown() const;

public: // Shortcut map

    /**
     * Get shortcut map
     * @return Reference to the shortcut map
     */
    util::ShortcutMap& getShortcutMap();

    /**
     * Get shortcut map
     * @return Const reference to the shortcut map
     */
    const util::ShortcutMap& getShortcutMap() const;

public: // Action getters

    gui::TriggerAction& getTriggerHelpAction() { return _triggerHelpAction; }
    gui::TriggerAction& getTriggerReadmeAction() { return _triggerReadmeAction; }
    gui::TriggerAction& getVisitRepositoryAction() { return _visitRepositoryAction; }
    gui::TriggerAction& getLaunchAboutAction() { return _launchAboutAction; }

signals:

    /**
     * Signals that the plugin GUI name changed from \p previousGuiName to \p currentGuiName
     * @param previousGuiName Previous GUI name
     * @param currentGuiName Current GUI name
     */
    void guiNameChanged(const QString& previousGuiName, const QString& currentGuiName);

    /**
     * Signals that the version changed from \p previousVersion to \p currentVersion
     * @param previousVersion Previous version
     * @param currentVersion Current version
     */
    void versionChanged(const QString& previousVersion, const QString& currentVersion);

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
    const PluginFactory&    _pluginFactory;             /** Reference to parent plugin factory */
    QString                 _guiName;                   /** Name of the plugin in the GUI */
    QString                 _version;                   /** Plugin version */
    gui::TriggerAction      _triggerHelpAction;         /** Trigger action that triggers help (icon and text are already set) */
    gui::TriggerAction      _triggerReadmeAction;       /** Trigger action that displays the read me markdown text in a modal dialog (if the read me markdown file URL is valid) */
    gui::TriggerAction      _visitRepositoryAction;     /** Trigger action that opens an external browser and visits the GitHub repository */
    gui::TriggerAction      _launchAboutAction;         /** Trigger action that displays the about Markdown text in the markdown dialog */
    util::ShortcutMap       _shortcutMap;               /** Shortcut cheatsheet map */
    QString                 _pluginTitle;               /** Human-readable plugin title in plain text format */
    QString                 _description;               /** Shortly describes the plugin */
    QString                 _summary;                   /** Elaborate description of the plugin (used in auto-generated about Markdown) */
    QStringList             _authors;                   /** Authors that created and maintain the plugin (used in auto-generated about Markdown) */
    QString                 _copyrightNotice;           /** Copyright notice (used in auto-generated about Markdown) */
    QString                 _aboutMarkdown;             /** About text in Markdown format. When set, it overrides the standardized about Markdown generated from summary, authors and copyright notice. */
};

}
