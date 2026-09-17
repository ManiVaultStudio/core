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
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QTextBrowser>
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
    _aboutThirdPartiesAction(nullptr, "About third-parties"),
    _releaseNotesAction(nullptr, "Release notes")
{
    setTitle("Help");
    setToolTip("ManiVault help");

    _aboutThirdPartiesAction.setMenuRole(QAction::NoRole);
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
    dialog.setWindowIcon(StyledIcon("certificate"));
    dialog.resize(900, 500);

    auto layout = new QVBoxLayout(&dialog);
    auto filterLayout = new QHBoxLayout();

    auto filterLabel = new QLabel(tr("Filter:"), &dialog);
    auto filterEdit = new QLineEdit(&dialog);
    filterEdit->setPlaceholderText(tr("Search dependencies and licenses"));

    auto scopeComboBox = new QComboBox(&dialog);
    scopeComboBox->addItem(tr("All available plugins"), static_cast<int>(ThirdPartyLicensesFilterModel::PluginState::AllAvailable));
    scopeComboBox->addItem(tr("Loaded plugins only"), static_cast<int>(ThirdPartyLicensesFilterModel::PluginState::LoadedOnly));

    auto showCoreCheckBox = new QCheckBox(tr("Core"), &dialog);
    auto showPluginsCheckBox = new QCheckBox(tr("Plugins"), &dialog);
    showCoreCheckBox->setChecked(true);
    showPluginsCheckBox->setChecked(true);

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(filterEdit, 1);
    filterLayout->addWidget(scopeComboBox);
    filterLayout->addWidget(showCoreCheckBox);
    filterLayout->addWidget(showPluginsCheckBox);
    layout->addLayout(filterLayout);

    auto filterModel = new ThirdPartyLicensesFilterModel(&dialog);
    filterModel->setSourceModel(const_cast<ThirdPartyLicensesListModel*>(&mv::help().getThirdPartyLicensesModel()));

    auto textBrowser = new QTextBrowser(&dialog);
    textBrowser->setOpenExternalLinks(true);
    textBrowser->setOpenLinks(true);
    textBrowser->setStyleSheet("QTextBrowser { background-color: transparent; }");
    layout->addWidget(textBrowser, 1);

    const auto headingColor = dialog.palette().color(QPalette::WindowText).name();
    const auto headerColor = dialog.palette().color(QPalette::Mid).name();

    const auto updateText = [filterModel, textBrowser, headingColor, headerColor]() {
        QString coreText;
        QString pluginText;

        const auto formatLicense = [](const ThirdPartyLicenseUsage& usage) {
            const auto name = usage.license.name.toHtmlEscaped();
            const auto license = usage.license.license.toHtmlEscaped();
            const auto url = usage.license.url.toHtmlEscaped();
            const auto dependency = url.isEmpty() ? name : QString("<a href=\"%1\">%2</a>").arg(url, name);
            const auto usedBy = usage.isCore && usage.availablePlugins.isEmpty() ? QString("ManiVault Core") : usage.availablePlugins.join(", ").toHtmlEscaped();

            return QString("<tr><td style=\"padding: 2px 0; vertical-align: top\" width=\"38%\"><b>%1</b></td><td style=\"padding: 2px 0; vertical-align: top\" width=\"24%\">%2</td><td style=\"padding: 2px 0; vertical-align: top\">%3</td></tr>").arg(dependency, license, usedBy);
        };

        for (int row = 0; row < filterModel->rowCount(); ++row) {
            const auto usage = filterModel->data(filterModel->index(row, 0), Qt::UserRole + 1).value<ThirdPartyLicenseUsage>();

            if (usage.isCore)
                coreText += formatLicense(usage);

            if (!usage.availablePlugins.isEmpty())
                pluginText += formatLicense(usage);
        }

        const auto formatSection = [headingColor, headerColor](const QString& title, const QString& rows, bool firstSection, bool includeColumnHeaders) {
            const auto titlePadding = firstSection ? "2px" : "16px";
            const auto columnHeaders = includeColumnHeaders ? QString(
                "<tr style=\"color: %1\"><th align=\"left\" style=\"padding-bottom: 4px\" width=\"38%\">Dependency</th>"
                "<th align=\"left\" style=\"padding-bottom: 4px\" width=\"24%\">License</th>"
                "<th align=\"left\" style=\"padding-bottom: 4px\">Used by</th></tr>").arg(headerColor) : QString();

            return QString(
                "<tr><td colspan=\"3\" style=\"color: %1; font-size: 1.1em; font-weight: bold; padding-top: %2; padding-bottom: 8px\">%3</td></tr>"
                "%4%5")
                .arg(headingColor, titlePadding, title, columnHeaders, rows);
        };

        QString text;
        if (!coreText.isEmpty() || !pluginText.isEmpty()) {
            text = "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\">";

            if (!coreText.isEmpty())
                text += formatSection("Core", coreText, true, true);
            if (!pluginText.isEmpty())
                text += formatSection("Plugins", pluginText, coreText.isEmpty(), coreText.isEmpty());

            text += "</table>";
        }
        if (text.isEmpty())
            text = "<p>No third-party licenses match the current filters.</p>";

        textBrowser->setHtml(text);
    };

    updateText();

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    layout->addWidget(dialogButtonBox);

    connect(filterEdit, &QLineEdit::textChanged, &dialog, [filterModel, updateText](const QString& text) {
        filterModel->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(text), QRegularExpression::CaseInsensitiveOption));
        updateText();
    });
    connect(scopeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [filterModel, scopeComboBox, updateText](int) {
        filterModel->setPluginState(static_cast<ThirdPartyLicensesFilterModel::PluginState>(scopeComboBox->currentData().toInt()));
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
    connect(dialogButtonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.exec();
}
