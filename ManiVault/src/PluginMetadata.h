// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/ShortcutMap.h"
#include "util/Version.h"

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

    /** Formatted item for use in auto-generated about Markdown */
    struct FormattedItem
    {
	    /**
	     * Construct with \p name, \p description and \p externalLink
	     * @param name Item name
	     * @param description Item description
	     * @param externalLink External link
	     */
	    FormattedItem(const QString& name, const QString& description, const QString& externalLink) :
	        _name(name),
	        _description(description),
			_externalLink(externalLink)
        {
        }

        /**
         * Convert to string
         * @return Markdown formatted string
         */
        QString toString() const {
            if (_externalLink.isEmpty())
                return QString("%1 (%2)").arg(_name, _description);

            return QString("[%1](%2) (%3)").arg(_name, _externalLink, _description);
        }

        /**
         * Equality operator
         * @param rhs Right-hand-side formatted item
         * @return Whether this formatted item and \p rhs formatted item are the same
         */
        bool operator==(const FormattedItem& rhs) const {
            if (_name != rhs._name)
                return false;

            if (_description != rhs._description)
                return false;

            if (_externalLink != rhs._externalLink)
                return false;

            return true;
        }

	    /**
         * Inequality operator
         * @param rhs Right-hand-side formatted item
         * @return Whether this formatted item and \p rhs formatted item are not the same
         */
        bool operator!=(const FormattedItem& rhs) const {
            return !(*this == rhs);
	    }

        QString     _name;              /** Item name*/
        QString     _description;       /** Item description */
        QString     _externalLink;      /** Item external link (maybe empty) */
    };

    /** Organization formatted item for use in auto-generated about Markdown */
    struct Organization : public FormattedItem
    {
        /** No need for custom constructor */
        using FormattedItem::FormattedItem;

        /**
         * Convert to string
         * @return Markdown formatted string
         */
        QString toString(std::int32_t index) const {
            const auto indexString = QString("<sup>%1</sup>").arg(QString::number(index));

            if (_externalLink.isEmpty())
                return QString("%1 (%2)%3").arg(_name, _description, indexString);

            return QString("[%1](%2) (%3)%4").arg(_name, _externalLink, _description, indexString);
        }
    };

    using Organizations = std::vector<Organization>;

    /** Author formatted item for use in auto-generated about Markdown */
    struct Author : protected FormattedItem
    {
        /**
         * Construct with \p name, \p description, \p externalLink and \p organizations
         * @param name Item name
         * @param roles Assumed roles
         * @param organizationNames Names of the affiliated organizations
         * @param externalLink External link (e.g. profile website or e-mail address)
         */
        Author(const QString& name, const QStringList& roles = QStringList(), const QStringList& organizationNames = QStringList(), const QString& externalLink = QString()) :
            FormattedItem(name, "", externalLink),
			_organizationNames(organizationNames),
            _roles(roles)
        {
        }
        
        /**
         * Convert to string
         * @param organizations Organizations list
         * @return Markdown formatted string
         */
        QString toString(const Organizations& organizations) const {
            QStringList indices;

            for (const auto& organizationName : _organizationNames) {
                const auto it = std::find_if(organizations.begin(), organizations.end(), [organizationName](const Organization& candidateOrganization) -> bool {
                    return organizationName == candidateOrganization._name;
                });

                if (it != organizations.end()) {
                    indices << QString::number(it - organizations.begin());
                }
            }

            const auto affiliations = indices.isEmpty() ? "" : QString("<sup>%1</sup>").arg(indices.join(","));

        	auto rolesString = _roles.isEmpty() ? "" : QString("(%1)").arg(_roles.join(", "));

            if (!_roles.isEmpty()) {
                int lastCommaIndex = rolesString.lastIndexOf(',');

                if (lastCommaIndex != -1)
                    rolesString.replace(lastCommaIndex, 1, " & ");
            }

            if (_externalLink.isEmpty())
                return QString("**%1** %2%3").arg(_name, rolesString, affiliations);

            return QString("[**%1**](%2) %3%4").arg(_name, _externalLink, rolesString, affiliations);
        }

        /**
         * Equality operator
         * @param rhs Right-hand-side author
         * @return Whether this author and \p rhs author are the same
         */
        bool operator==(const Author& rhs) const {
            if (FormattedItem::operator != (rhs))
                return false;

            if (_organizationNames != rhs._organizationNames)
                return false;

            if (_roles != rhs._roles)
                return false;

            return true;
        }

        /**
         * Inequality operator
         * @param rhs Right-hand-side author
         * @return Whether this author and \p rhs author are not the same
         */
        bool operator!=(const Author& rhs) const {
            return !(*this == rhs);
        }

        QStringList     _organizationNames;   /** Affiliated organizations */
        QStringList     _roles;               /** Assumed roles */
    };

    using Authors = std::vector<Author>;

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
    Authors getAuthors() const;

    /**
     * Set authors to \p authors
     * @param authors Authors that created and maintain the plugin
     */
    void setAuthors(const Authors& authors);

    /**
     * Get whether the plugin has authors assigned or not
     * @return Boolean determining whether the plugin has authors assigned or not
     */
    bool hasAuthors() const;

public: // Organizations

    /**
     * Get organizations
     * @return Organizations that created and maintain the plugin
     */
    Organizations getOrganizations() const;

    /**
     * Set organizations to \p organizations
     * @param organizations Organizations that created and maintain the plugin
     */
    void setOrganizations(const Organizations& organizations);

    /**
     * Get whether the plugin has organizations assigned or not
     * @return Boolean determining whether the plugin has organizations assigned or not
     */
    bool hasOrganizations() const;

public: // Copyright holder

    /**
     * Get copyright holder
     * @return Copyright holder
     */
    QString getCopyrightHolder() const;

    /**
     * Set copyright holder to \p copyrightHolder
     * @param copyrightHolder Copyright holder
     */
    void setCopyrightHolder(const QString& copyrightHolder);

    /**
     * Get whether the plugin has a copyright holder or not
     * @return Boolean determining whether the plugin has a copyright holder or not
     */
    bool hasCopyrightHolder() const;

public: // License text

    /**
     * Get license text
     * @return License text
     */
    QString getLicenseText() const;

    /**
     * Set license text to \p licenseText
     * @param licenseText License text
     */
    void setLicenseText(const QString& licenseText);

    /**
     * Get whether the plugin has a license text or not
     * @return Boolean determining whether the plugin has a license text or not
     */
    bool hasLicenseText() const;

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
    gui::TriggerAction& getViewAboutAction() { return _viewAboutAction; }
    gui::TriggerAction& getViewShortcutsAction() { return _viewShortcutsAction; }

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
    void versionChanged(const util::Version& previousVersion, const util::Version& currentVersion);

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
    void authorsChanged(const Authors& previousAuthors, const Authors& currentAuthors);

    /**
     * Signals that the organizations changed from \p previousOrganizations to \p currentOrganizations
     * @param previousOrganizations Previous organizations
     * @param currentOrganizations Current organizations
     */
    void organizationsChanged(const Organizations& previousOrganizations, const Organizations& currentOrganizations);

    /**
     * Signals that the copyright holder changed from \p previousCopyrightHolder to \p currentCopyrightHolder
     * @param previousCopyrightHolder Previous copyright holder
     * @param currentCopyrightHolder Current copyright holder
     */
    void copyrightHolderChanged(const QString& previousCopyrightHolder, const QString& currentCopyrightHolder);

    /**
     * Signals that the license text changed from \p previousLicenseText to \p currentLicenseText
     * @param previousLicenseText Previous license text
     * @param currentLicenseText Current license text
     */
    void licenseTextChanged(const QString& previousLicenseText, const QString& currentLicenseText);

    /**
     * Signals that the about text in Markdown format changed from \p previousAboutMarkdown to \p currentAboutMarkdown
     * @param previousAboutMarkdown Previous about text in Markdown format
     * @param currentAboutMarkdown Current about text in Markdown format
     */
    void aboutMarkdownChanged(const QString& previousAboutMarkdown, const QString& currentAboutMarkdown);

private:
    const PluginFactory&    _pluginFactory;             /** Reference to parent plugin factory */
    QString                 _guiName;                   /** Name of the plugin in the GUI */
    util::Version           _version;                   /** Plugin semantic version */
    gui::TriggerAction      _triggerHelpAction;         /** Trigger action that triggers help (icon and text are already set) */
    gui::TriggerAction      _triggerReadmeAction;       /** Trigger action that displays the read me markdown text in a modal dialog (if the read me markdown file URL is valid) */
    gui::TriggerAction      _visitRepositoryAction;     /** Trigger action that opens an external browser and visits the GitHub repository */
    gui::TriggerAction      _viewAboutAction;           /** Trigger action that displays the plugin about dialog */
    gui::TriggerAction      _viewShortcutsAction;       /** Trigger action that displays the plugin shortcuts dialog */
    util::ShortcutMap       _shortcutMap;               /** Shortcut cheatsheet map */
    QString                 _description;               /** Shortly describes the plugin */
    QString                 _summary;                   /** Elaborate description of the plugin (used in auto-generated about Markdown) */
    Organizations           _organizations;             /** Which organizations create and maintain the plugin */
    Authors                 _authors;                   /** Authors that created and maintain the plugin (used in auto-generated about Markdown) */
    QString                 _copyrightHolder;           /** Copyright holder (used in auto-generated about Markdown) */
    QString                 _licenseText;               /** License text (used in auto-generated about Markdown) */
    QString                 _aboutMarkdown;             /** About text in Markdown format. When set, it overrides the standardized about Markdown generated from summary, authors and copyright notice. */
};

}
