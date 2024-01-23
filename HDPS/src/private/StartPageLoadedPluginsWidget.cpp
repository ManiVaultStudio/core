// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageLoadedPluginsWidget.h"
#include "StartPageContentWidget.h"

#include <Application.h>

#include <QDebug>

#ifdef _DEBUG
    #define START_PAGE_LOADED_PLUGINS_WIDGET_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

StartPageLoadedPluginsWidget::StartPageLoadedPluginsWidget(QWidget* parent, const QString& title, bool restyle /*= true*/) :
    QWidget(parent),
    _layout(),
    _loadedPluginsScrollArea(),
    _loadedPluginsWidget(),
    _loadedPluginsLayout(),
    _restyle(restyle)
{
    setObjectName("StartPageLoadedPluginsWidget");

    if (!title.isEmpty())
        _layout.addWidget(StartPageContentWidget::createHeaderLabel(title, title));

    _loadedPluginsLayout.setContentsMargins(0, 0, 0, 0);

    _loadedPluginsWidget.setLayout(&_loadedPluginsLayout);

    _loadedPluginsScrollArea.setObjectName("ScrollArea");
    _loadedPluginsScrollArea.setWidget(&_loadedPluginsWidget);
    _loadedPluginsScrollArea.setWidgetResizable(true);
    _loadedPluginsScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _layout.addWidget(&_loadedPluginsScrollArea);
    //_layout.addStretch(1);

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
        _loadedPluginsLayout.addWidget(new PluginWidget(pluginFactory));

    setLayout(&_layout);
    
    connect(qApp, &QApplication::paletteChanged, this, &StartPageLoadedPluginsWidget::updateCustomStyle);
}

QVBoxLayout& StartPageLoadedPluginsWidget::getLayout()
{
    return _layout;
}

void StartPageLoadedPluginsWidget::updateCustomStyle()
{
    auto color = QApplication::palette().color(QPalette::Normal, QPalette::Midlight).name();
    auto styleSheet = QString("QScrollArea#ScrollArea { background-color: %1;}").arg(color);
    
    _loadedPluginsScrollArea.setStyleSheet(styleSheet);
}

StartPageLoadedPluginsWidget::PluginWidget::PluginWidget(mv::plugin::PluginFactory* pluginfactory, QWidget* parent /*= nullptr*/) :
    QWidget(parent)
{
    Q_ASSERT(pluginfactory != nullptr);

    if (!pluginfactory)
        return;

    setObjectName("PluginWidget");
    setAutoFillBackground(true);

    auto layout = new QHBoxLayout();

    const auto margin = 1;

    layout->setContentsMargins(5, margin, 5, margin);

    auto iconLabel      = new QLabel();
    auto versionLabel   = new QLabel("v" + pluginfactory->getVersion());

    iconLabel->setPixmap(pluginfactory->getCategoryIcon().pixmap(QSize(12, 12)));

    versionLabel->setStyleSheet("font-weight: bold;");

    layout->addWidget(iconLabel);
    layout->addWidget(new QLabel(pluginfactory->getKind()));
    layout->addWidget(versionLabel);

    setLayout(layout);

    auto styleSheet = QString(" \
        QWidget#PluginWidget { \
            background-color: rgba(150, 150, 150, 255); \
            border-radius: 3px; \
        } \
    ");

    setStyleSheet(styleSheet);
}
