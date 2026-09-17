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
#include <QTableView>
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

    auto tableView = new QTableView(&dialog);
    tableView->setModel(filterModel);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(static_cast<int>(AbstractThirdPartyLicensesModel::Column::Name), Qt::AscendingOrder);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setAlternatingRowColors(true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setSectionResizeMode(static_cast<int>(AbstractThirdPartyLicensesModel::Column::Name), QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(static_cast<int>(AbstractThirdPartyLicensesModel::Column::License), QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(static_cast<int>(AbstractThirdPartyLicensesModel::Column::Core), QHeaderView::ResizeToContents);
    layout->addWidget(tableView, 1);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    layout->addWidget(dialogButtonBox);

    connect(filterEdit, &QLineEdit::textChanged, &dialog, [filterModel](const QString& text) {
        filterModel->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(text), QRegularExpression::CaseInsensitiveOption));
    });
    connect(scopeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [filterModel, scopeComboBox](int) {
        filterModel->setPluginState(static_cast<ThirdPartyLicensesFilterModel::PluginState>(scopeComboBox->currentData().toInt()));
    });
    connect(showCoreCheckBox, &QCheckBox::toggled, filterModel, &ThirdPartyLicensesFilterModel::setShowCoreLicenses);
    connect(showPluginsCheckBox, &QCheckBox::toggled, filterModel, &ThirdPartyLicensesFilterModel::setShowPluginLicenses);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.exec();
}
