// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpMenu.h"
#include "PluginManager.h"
#include "CoreInterface.h"
#include "UserFeedbackDialog.h"

#include <util/Miscellaneous.h>
#include <actions/TriggerAction.h>
#include <models/AbstractThirdPartyLicensesModel.h>
#include <models/ThirdPartyLicensesFilterModel.h>

#include <QMessageBox> 
#include <QCheckBox>
#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringLiteral>
#include <QOperatingSystemVersion>
#include <QDesktopServices>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;
using namespace mv::plugin;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _devDocAction(nullptr, "Developer Documentation"),
    _sendFeedbackAction(nullptr, "Send feedback..."),
    _aboutAction(nullptr, QString("About %1").arg(Application::getBaseName())),
    _aboutQtAction(nullptr, "About Qt"),
    _aboutThirdPartiesAction(nullptr, "About third-parties..."),
    _releaseNotesAction(nullptr, "Release notes")
{
    setTitle("Help");
    setToolTip("ManiVault help");

    _aboutThirdPartiesAction.setMenuRole(QAction::NoRole);
    _aboutThirdPartiesAction.setIconByName("file-contract");
    _aboutQtAction.setMenuRole(QAction::NoRole);

    _releaseNotesAction.setIconByName("scroll");
    _sendFeedbackAction.setIconByName("comment-dots");

    populate();
}

void HelpMenu::showEvent(QShowEvent* event)
{
    QMenu::showEvent(event);

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)
        populate();
}

void HelpMenu::populate()
{
    clear();
    
    addAction(&mv::help().getShowLearningCenterPageAction());
    addAction(&_devDocAction);
#ifdef MV_USE_ERROR_LOGGING
    addAction(&_sendFeedbackAction);
#endif
    addSeparator();
    
    QVector<QPointer<TriggerAction>> actions;

    for (auto& pluginFactory : plugins().getPluginFactoriesByTypes({ Type::ANALYSIS, Type::DATA, Type::LOADER, Type::WRITER, Type::TRANSFORMATION, Type::VIEW }))
        if (pluginFactory->hasHelp())
            actions << &pluginFactory->getPluginMetadata().getTriggerHelpAction();

    sortActions(actions);

    if (!actions.isEmpty()) {
        auto pluginHelpMenu = new QMenu("Plugins");

        pluginHelpMenu->setToolTip("ManiVault plugin documentation");
        pluginHelpMenu->setIcon(StyledIcon("plug"));

        for (auto action : actions)
            pluginHelpMenu->addAction(action);

        addMenu(pluginHelpMenu);
    }

    addSeparator();

    addMenu(mv::help().getVideosMenu());
    addMenu(mv::help().getTutorialsMenu());

    if(!isEmpty())
        addSeparator();

    addAction(&_aboutAction);
    addAction(&_aboutThirdPartiesAction);
    addAction(&_aboutQtAction);

    addSeparator();

    addAction(&_releaseNotesAction);

    connect(&_devDocAction, &QAction::triggered, this, [this](bool) {
        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/PublicWiki", QUrl::TolerantMode));
    });

#ifdef MV_USE_ERROR_LOGGING
    connect(&_sendFeedbackAction, &TriggerAction::triggered, this, &HelpMenu::sendFeedback, Qt::UniqueConnection);
#endif
    connect(&_aboutAction, &TriggerAction::triggered, this, &HelpMenu::about);
    connect(&_aboutThirdPartiesAction, &TriggerAction::triggered, this, &HelpMenu::aboutThirdParties);

    connect(&_aboutQtAction, &TriggerAction::triggered, this, [this](bool) {
        QMessageBox::aboutQt(this->parentWidget(), "About Qt");
    });

    connect(&_releaseNotesAction, &QAction::triggered, this, [this](bool) {
        const auto applicationVersion = Application::current()->getVersion();

        QDesktopServices::openUrl(QUrl("https://github.com/ManiVaultStudio/core/releases/", QUrl::TolerantMode));
    });
}

void HelpMenu::sendFeedback()
{
    auto feedbackDialog = new UserFeedbackDialog(this->parentWidget());

    feedbackDialog->setAttribute(Qt::WA_DeleteOnClose);
    feedbackDialog->open();
}

void HelpMenu::about() const
{
    QMessageBox::about(this->parentWidget(), tr("About ManiVault"), Application::getAbout());
}

void HelpMenu::aboutThirdParties() const
{
    QDialog dialog(this->parentWidget());

    dialog.setWindowTitle(tr("Third-party licenses"));
    dialog.setWindowIcon(StyledIcon("file-contract"));
    dialog.resize(750, 500);

    auto layout         = new QVBoxLayout(&dialog);
    auto filterLayout   = new QHBoxLayout();
    auto filterLabel    = new QLabel(tr("Filter:"), &dialog);
    auto filterEdit     = new QLineEdit(&dialog);

    filterEdit->setPlaceholderText(tr("Search dependencies and licenses"));

    auto showCoreCheckBox       = new QCheckBox(tr("Core"), &dialog);
    auto showPluginsCheckBox    = new QCheckBox(tr("Plugins"), &dialog);

    showCoreCheckBox->setChecked(true);
    showPluginsCheckBox->setChecked(true);

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterEdit, 1);
    filterLayout->addWidget(showCoreCheckBox);
    filterLayout->addWidget(showPluginsCheckBox);

    layout->addLayout(filterLayout);

    auto filterModel = new ThirdPartyLicensesFilterModel(&dialog);

    filterModel->setSourceModel(const_cast<ThirdPartyLicensesListModel*>(&mv::help().getThirdPartyLicensesModel()));
    filterModel->setPluginState(ThirdPartyLicensesFilterModel::PluginState::AllAvailable);

    auto textBrowser = new QTextBrowser(&dialog);

    textBrowser->setOpenExternalLinks(true);
    textBrowser->setOpenLinks(true);
    textBrowser->setStyleSheet("QTextBrowser { background-color: transparent; }");
    layout->addWidget(textBrowser, 1);

    const auto headingColor = dialog.palette().color(QPalette::WindowText).name();
    const auto headerColor  = dialog.palette().color(QPalette::Mid).name();

    const auto getVisibleUsages = [filterModel]() {
        ThirdPartyLicenseUsages usages;

        for (int row = 0; row < filterModel->rowCount(); ++row)
            usages.push_back(filterModel->data(filterModel->index(row, 0), Qt::UserRole + 1).value<ThirdPartyLicenseUsage>());

        return usages;
    };

    const auto updateText = [filterModel, textBrowser, headingColor, headerColor, getVisibleUsages]() {
        QString coreText;
        QString pluginText;

        const auto formatLicense = [](const ThirdPartyLicenseUsage& usage) {
            const auto name         = usage.license.name.toHtmlEscaped();
            const auto license      = usage.license.license.toHtmlEscaped();
            const auto url          = usage.license.url.toHtmlEscaped();
            const auto dependency   = url.isEmpty() ? name : QString("<a href=\"%1\">%2</a>").arg(url, name);
            const auto usedBy       = usage.isCore ? QString() : usage.availablePlugins.join(", ").toHtmlEscaped();

            return QString("<tr><td style=\"padding: 2px 0; vertical-align: top\" width=\"28%\"><b>%1</b></td><td style=\"padding: 2px 0; vertical-align: top\" width=\"24%\">%2</td><td style=\"padding: 2px 0; vertical-align: top\">%3</td></tr>").arg(dependency, license, usedBy);
        };

        for (const auto& usage : getVisibleUsages()) {

            if (usage.isCore)
                coreText += formatLicense(usage);

            if (!usage.availablePlugins.isEmpty())
                pluginText += formatLicense(usage);
        }

        const auto formatSection = [headingColor, headerColor](const QString& title, const QString& rows, bool firstSection, bool includeColumnHeaders) {
            const auto titlePadding     = firstSection ? "10px" : "16px";
            const auto columnHeaders    = includeColumnHeaders ? QString(
                "<tr style=\"color: %1\"><th align=\"left\" style=\"padding-bottom: 4px\" width=\"28%\">Dependency</th>"
                "<th align=\"left\" style=\"padding-bottom: 4px\" width=\"24%\">License</th>"
                "<th align=\"left\" style=\"padding-bottom: 4px\">Used by</th></tr>").arg(headerColor) : QString();

            return QString(
                "<tr><td colspan=\"3\" style=\"color: %1; font-size: 1.3em; font-weight: bold; padding-top: %2; padding-bottom: 8px\">%3</td></tr>"
                "%4%5")
                .arg(headingColor, titlePadding, title, columnHeaders, rows);
        };

        QString text;
        if (!coreText.isEmpty() || !pluginText.isEmpty()) {
            text = "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">";

            if (!coreText.isEmpty())
                text += formatSection("Core", coreText, true, false);

            if (!pluginText.isEmpty()) {
                text += formatSection("Plugins (all available)", pluginText, coreText.isEmpty(), coreText.isEmpty());
            }

            text += "</table>";
        }
        if (text.isEmpty())
            text = "<p>No third-party licenses match the current filters.</p>";

        textBrowser->setHtml(text);
    };

    updateText();

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    auto copyJsonButton = dialogButtonBox->addButton(QString(), QDialogButtonBox::ActionRole);
    copyJsonButton->setIcon(StyledIcon("copy"));
    copyJsonButton->setToolTip(tr("Copy visible license data as JSON"));

    layout->addWidget(dialogButtonBox);

    connect(filterEdit, &QLineEdit::textChanged, &dialog, [filterModel, updateText](const QString& text) {
        filterModel->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(text), QRegularExpression::CaseInsensitiveOption));
        updateText();
    });

    connect(showCoreCheckBox, &QCheckBox::toggled, &dialog, [filterModel, updateText](bool show) {
        filterModel->setShowCoreLicenses(show);
        updateText();
    });

    connect(showPluginsCheckBox, &QCheckBox::toggled, &dialog, [filterModel, updateText](bool show) {
        filterModel->setShowPluginLicenses(show);
        updateText();
    });

    connect(copyJsonButton, &QPushButton::clicked, &dialog, [copyJsonButton, getVisibleUsages]() {
        QJsonArray coreLicenses;
        QJsonArray availablePluginLicenses;
        QJsonArray loadedPluginLicenses;

        const auto toJson = [](const ThirdPartyLicenseUsage& usage, const QStringList& usedByPlugins = {}) {
            auto license = QJsonObject {
                {"name", usage.license.name},
                {"license", usage.license.license},
                {"url", usage.license.url}
            };

            if (!usedByPlugins.isEmpty())
                license.insert("usedByPlugins", QJsonArray::fromStringList(usedByPlugins));

            return license;
        };

        for (const auto& usage : getVisibleUsages()) {
            if (usage.isCore)
                coreLicenses.append(toJson(usage));

            if (!usage.availablePlugins.isEmpty())
                availablePluginLicenses.append(toJson(usage, usage.availablePlugins));

            if (!usage.loadedPlugins.isEmpty())
                loadedPluginLicenses.append(toJson(usage, usage.loadedPlugins));
        }

        QJsonArray sections {
            QJsonObject {{"core", coreLicenses}},
            QJsonObject {{"availablePlugins", availablePluginLicenses}},
            QJsonObject {{"loadedPlugins", loadedPluginLicenses}}
        };

        QGuiApplication::clipboard()->setText(QJsonDocument(sections).toJson(QJsonDocument::Indented));

        copyJsonButton->setIcon(StyledIcon("check"));
        mv::help().addNotification("Third-party licenses", "License data copied as JSON.", StyledIcon("check"), util::Notification::DurationType::Fixed);

        QTimer::singleShot(4000, copyJsonButton, [copyJsonButton]() {
            copyJsonButton->setIcon(StyledIcon("copy"));
        });
    });

    connect(dialogButtonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.exec();
}
