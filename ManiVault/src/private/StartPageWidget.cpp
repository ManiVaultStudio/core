// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageWidget.h"
#include "ManiVaultVersion.h"

#include <QDebug>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    PageWidget("StartPage", parent),
    _startPageContentWidget(this),
    _loadAction(this, "Load"),
    _saveAction(this, "Save"),
    _fileAction(this, "File"),
    _configurationAction(this, "Configuration")
{
    getContentLayout().addWidget(&_startPageContentWidget, Qt::AlignTop);

    getTitleAction().setString(QString("<b>ManiVault</b> v%1.%2.%3-%4").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::number(MV_VERSION_PATCH), QString(MV_VERSION_SUFFIX.data())));

    _fileAction.setShowLabels(false);

    _fileAction.addAction(&getLoadAction());
    _fileAction.addAction(&getSaveAction());

    _startPageContentWidget.getToolbarAction().addAction(&_configurationAction);

    _configurationAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _configurationAction.setIconByName("gear");
    _configurationAction.setPopupSizeHint(QSize(400, 0));
    _configurationAction.setVisible(false);

    _configurationAction.addAction(&_fileAction);
    _configurationAction.addAction(&getLogoAction());
    _configurationAction.addAction(&getTitleAction());

    _loadAction.setIconByName("folder-open");
    _loadAction.setToolTip("Open configuration");
    _loadAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::IconText);

    _saveAction.setIconByName("save");
    _saveAction.setToolTip("Save configuration");
    _saveAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::IconText);

    connect(&_loadAction, &TriggerAction::triggered, this, &StartPageWidget::loadConfiguration);
    connect(&_saveAction, &TriggerAction::triggered, this, &StartPageWidget::saveConfiguration);

    if (QFileInfo(getConfigurationFileName()).exists()) {
        loadConfiguration();
    }
}

void StartPageWidget::showEvent(QShowEvent* showEvent)
{
    _startPageContentWidget.updateActions();
}

bool StartPageWidget::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        if (auto keyEvent = dynamic_cast<QKeyEvent*>(event)) {
            if (keyEvent->key() == Qt::Key_F8) {
                _startPageContentWidget.getSettingsAction().setVisible(true);
                _configurationAction.setVisible(true);
            }
        }
    }
    return PageWidget::event(event);
}

void StartPageWidget::fromVariantMap(const QVariantMap& variantMap)
{
    _startPageContentWidget.fromParentVariantMap(variantMap);

    PageWidget::fromVariantMap(variantMap);
}

QVariantMap StartPageWidget::toVariantMap() const
{
    auto variantMap = PageWidget::toVariantMap();

	_startPageContentWidget.insertIntoVariantMap(variantMap);

	return variantMap;
}

void StartPageWidget::loadConfiguration()
{
    fromJsonFile(getConfigurationFileName());

    mv::help().addNotification(QString("%1 configuration").arg(getSerializationName()), QString("Configuration loaded from <b>%1.json</b>").arg(getSerializationName()), StyledIcon("folder-open"));
}

void StartPageWidget::saveConfiguration() const
{
    toJsonFile(getConfigurationFileName());

    mv::help().addNotification(QString("%1 configuration").arg(getSerializationName()), QString("Configuration saved to <b>%1.json</b>").arg(getSerializationName()), StyledIcon("save"));
}