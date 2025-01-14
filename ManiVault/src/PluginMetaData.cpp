// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginMetaData.h"
#include "PluginFactory.h"

#include "Application.h"

#include "widgets/MarkdownDialog.h"
#include "widgets/PluginAboutDialog.h"

#include <QDesktopServices>

using namespace mv::gui;
using namespace mv::util;

namespace mv::plugin
{

PluginMetaData::PluginMetaData(const PluginFactory& pluginFactory) :
    _pluginFactory(pluginFactory),
    _triggerHelpAction(nullptr, "Trigger plugin help"),
    _triggerReadmeAction(nullptr, "Readme"),
    _visitRepositoryAction(nullptr, "Go to repository"),
    _launchAboutAction(nullptr, "Read plugin about information")
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

    _launchAboutAction.setIconByName("info");

    connect(&_launchAboutAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginAboutDialog pluginAboutDialog(_pluginFactory.getPluginMetaData());

        pluginAboutDialog.exec();
    });
}

QString PluginMetaData::getGuiName() const
{
    return _guiName;
}

void PluginMetaData::setGuiName(const QString& guiName)
{
    if (guiName == _guiName)
        return;

    const auto previousGuiName = _guiName;

    _description = guiName;

    emit guiNameChanged(previousGuiName, _guiName);
}

QString PluginMetaData::getVersion() const
{
    return _version;
}

void PluginMetaData::setVersion(const QString& version)
{
    if (version == _version)
        return;

    const auto previousVersion = _version;

    _version = version;

    emit versionChanged(previousVersion, _version);
}

QString PluginMetaData::getDescription() const
{
    return _description;
}

void PluginMetaData::setDescription(const QString& description)
{
    if (description == _description)
        return;

    const auto previousDescription = _description;

    _description = description;

    emit descriptionChanged(previousDescription, _description);
}

bool PluginMetaData::hasDescription() const
{
    return !_description.isEmpty();
}

QString PluginMetaData::getSummary() const
{
    return _summary;
}

void PluginMetaData::setSummary(const QString& summary)
{
    if (summary == _summary)
        return;

    const auto previousSummary = _summary;

    _summary = summary;

    emit summaryChanged(previousSummary, _summary);
}

bool PluginMetaData::hasSummary() const
{
    return !_summary.isEmpty();
}

QStringList PluginMetaData::getAuthors() const
{
    return _authors;
}

void PluginMetaData::setAuthors(const QStringList& authors)
{
    if (authors == _authors)
        return;

    const auto previousAuthors = _authors;

    _authors = authors;

    emit authorsChanged(previousAuthors, _authors);
}

bool PluginMetaData::hasAuthors() const
{
    return !_authors.isEmpty();
}

QString PluginMetaData::getCopyrightNotice() const
{
    return _copyrightNotice;
}

void PluginMetaData::setCopyrightNotice(const QString& copyrightNotice)
{
    if (copyrightNotice == _copyrightNotice)
        return;

    const auto previousCopyrightNotice = _copyrightNotice;

    _copyrightNotice = copyrightNotice;

    emit copyrightNoticeChanged(previousCopyrightNotice, _copyrightNotice);
}

bool PluginMetaData::hasCopyrightNotice() const
{
    return !_copyrightNotice.isEmpty();
}

QString PluginMetaData::getAboutMarkdown() const
{
    if (!_aboutMarkdown.isEmpty())
        return _aboutMarkdown;

    return QString(
        "%1"
        "%2"
        "%3"
    ).arg(getSummary(), getAuthors().join(", "), getCopyrightNotice());
}

void PluginMetaData::setAboutMarkdown(const QString& aboutMarkdown)
{
    if (aboutMarkdown == _aboutMarkdown)
        return;

    const auto previousAboutMarkdown = _aboutMarkdown;

    _aboutMarkdown = aboutMarkdown;

    emit aboutMarkdownChanged(previousAboutMarkdown, _aboutMarkdown);
}

bool PluginMetaData::hasAboutMarkdown() const
{
    return !_aboutMarkdown.isEmpty();
}

util::ShortcutMap& PluginMetaData::getShortcutMap()
{
    return _shortcutMap;
}

const util::ShortcutMap& PluginMetaData::getShortcutMap() const
{
    return _shortcutMap;
}

}
