// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginMetadata.h"
#include "PluginFactory.h"

#include "Application.h"

#include "widgets/MarkdownDialog.h"

#include "widgets/PluginAboutDialog.h"
#include "widgets/PluginShortcutsDialog.h"

#include <QDesktopServices>

using namespace mv::gui;
using namespace mv::util;

namespace mv::plugin
{

PluginMetadata::PluginMetadata(const PluginFactory& pluginFactory) :
    _pluginFactory(pluginFactory),
    _triggerHelpAction(nullptr, "Trigger plugin help"),
    _triggerReadmeAction(nullptr, "Readme"),
    _visitRepositoryAction(nullptr, "Go to repository"),
    _viewAboutAction(nullptr, "View plugin about"),
    _viewShortcutsAction(nullptr, "View plugin shortcuts")
{
    _triggerReadmeAction.setIconByName("book");

    connect(&_triggerReadmeAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!_pluginFactory.getReadmeMarkdownUrl().isValid())
            return;

        MarkdownDialog markdownDialog(_pluginFactory.getReadmeMarkdownUrl());

        markdownDialog.setWindowTitle(QString("%1").arg(_pluginFactory.getKind()));
        markdownDialog.exec();
    });

    _visitRepositoryAction.setToolTip("Browse to the Github repository");
    _visitRepositoryAction.setIcon(Application::getIconFont("FontAwesomeBrands").getIcon("code-branch"));

    connect(&_visitRepositoryAction, &TriggerAction::triggered, this, [this]() -> void {
        if (_pluginFactory.getRepositoryUrl().isValid())
            QDesktopServices::openUrl(_pluginFactory.getRepositoryUrl());
    });

    _viewAboutAction.setIconByName("info");

    connect(&_viewAboutAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginAboutDialog pluginAboutDialog(_pluginFactory.getPluginMetadata());

        pluginAboutDialog.exec();
    });

    connect(&_viewShortcutsAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginShortcutsDialog pluginShortcutsDialog(_pluginFactory.getPluginMetadata());

        pluginShortcutsDialog.exec();
	});
}

QString PluginMetadata::getGuiName() const
{
    return _guiName.isEmpty() ? _pluginFactory.getKind() : _guiName;
}

void PluginMetadata::setGuiName(const QString& guiName)
{
    if (guiName == _guiName)
        return;

    const auto previousGuiName = _guiName;

    _description = guiName;

    emit guiNameChanged(previousGuiName, _guiName);
}

util::Version& PluginMetadata::getVersion()
{
    return _version;
}

const util::Version& PluginMetadata::getVersion() const
{
    return const_cast<PluginMetadata*>(this)->getVersion();
}

void PluginMetadata::setVersion(const util::Version& version)
{
    if (version == _version)
        return;

    const auto previousVersion = _version;

    _version = version;

    emit versionChanged(previousVersion, _version);
}

QString PluginMetadata::getDescription() const
{
    if (_summary.isEmpty())
        return "No description available.";

    return _description;
}

void PluginMetadata::setDescription(const QString& description)
{
    if (description == _description)
        return;

    const auto previousDescription = _description;

    _description = description;

    emit descriptionChanged(previousDescription, _description);
}

bool PluginMetadata::hasDescription() const
{
    return !_description.isEmpty();
}

QString PluginMetadata::getSummary() const
{
    if (_summary.isEmpty())
        return "No summary available.";
    
    return _summary;
}

void PluginMetadata::setSummary(const QString& summary)
{
    if (summary == _summary)
        return;

    const auto previousSummary = _summary;

    _summary = summary;

    emit summaryChanged(previousSummary, _summary);
}

bool PluginMetadata::hasSummary() const
{
    return !_summary.isEmpty();
}

PluginMetadata::Authors PluginMetadata::getAuthors() const
{
    return _authors;
}

void PluginMetadata::setAuthors(const Authors& authors)
{
    if (authors == _authors)
        return;

    const auto previousAuthors = _authors;

    _authors = authors;

    emit authorsChanged(previousAuthors, _authors);
}

bool PluginMetadata::hasAuthors() const
{
    return !_authors.empty();
}

PluginMetadata::Organizations PluginMetadata::getOrganizations() const
{
    return _organizations;
}

void PluginMetadata::setOrganizations(const Organizations& organizations)
{
    if (organizations == _organizations)
        return;

    const auto previousOrganizations = _organizations;

    _organizations = organizations;

    emit organizationsChanged(previousOrganizations, _organizations);
}

bool PluginMetadata::hasOrganizations() const
{
    return !_organizations.empty();
}

QString PluginMetadata::getCopyrightHolder() const
{
    if (_copyrightHolder.isEmpty())
        return "No copyright holder available.";

    return _copyrightHolder;
}

void PluginMetadata::setCopyrightHolder(const QString& copyrightHolder)
{
    if (copyrightHolder == _copyrightHolder)
        return;

    const auto previousCopyrightHolder = _copyrightHolder;

    _copyrightHolder = copyrightHolder;

    emit copyrightHolderChanged(previousCopyrightHolder, _copyrightHolder);
}

bool PluginMetadata::hasCopyrightHolder() const
{
    return !_copyrightHolder.isEmpty();
}

QString PluginMetadata::getLicenseText() const
{
    if (!_licenseText.isEmpty())
        return _licenseText;

    return "No license text available.";
}

void PluginMetadata::setLicenseText(const QString& licenseText)
{
    if (licenseText == _licenseText)
        return;

    const auto previousLicenseText = _licenseText;

    _licenseText = licenseText;

    emit licenseTextChanged(previousLicenseText, _licenseText);
}

bool PluginMetadata::hasLicenseText() const
{
    return !_licenseText.isEmpty();
}

QString PluginMetadata::getAboutMarkdown() const
{
    if (!_aboutMarkdown.isEmpty())
        return _aboutMarkdown;

    QStringList authors, organizations;
    
    for (const auto& author : getAuthors())
        authors << author.toString() + "\n";

    if (!hasAuthors())
        authors << "No authors available.\n";

    for (const auto& organization : getOrganizations())
        organizations << organization.toString() + "\n";

    if (!hasOrganizations())
        organizations << "No organizations available.\n";

    const auto repositoryUrl = _pluginFactory.getRepositoryUrl().isValid() ? QString("Click [here](%1) for the GitHub repository.").arg(_pluginFactory.getRepositoryUrl().toString()) : "";

    const auto copyright = hasCopyrightHolder() ? QString("Copyright &copy; %4 %5").arg(QString::number(QDate::currentDate().year()), getCopyrightHolder()) : getCopyrightHolder();

    return QString(
        "# %1 v**%2**\n"
        "%3\n\n"
        "<details>\n\n"
        "<summary>View authors</summary>\n\n"
        "%6\n\n"
        "</details>\n\n"
        "<details>\n\n"
        "<summary>View organizations</summary>\n\n"
        "%7\n\n"
        "</details>\n\n"
        "---\n\n"
        "%4\n\n"
        "%5\n\n"
        "%8\n\n"
    ).arg(getGuiName(), QString::fromStdString(getVersion().getVersionString()), getSummary(), copyright, getLicenseText(), authors.join("\n"), organizations.join("\n"), repositoryUrl);
}

void PluginMetadata::setAboutMarkdown(const QString& aboutMarkdown)
{
    if (aboutMarkdown == _aboutMarkdown)
        return;

    const auto previousAboutMarkdown = _aboutMarkdown;

    _aboutMarkdown = aboutMarkdown;

    emit aboutMarkdownChanged(previousAboutMarkdown, _aboutMarkdown);
}

bool PluginMetadata::hasAboutMarkdown() const
{
    return !_aboutMarkdown.isEmpty();
}

util::ShortcutMap& PluginMetadata::getShortcutMap()
{
    return _shortcutMap;
}

const util::ShortcutMap& PluginMetadata::getShortcutMap() const
{
    return _shortcutMap;
}

}
