// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopeWidget.h"

#include <Application.h>

#include "SampleScopePlugin.h"

#include <QDebug>
#include <QGraphicsProxyWidget>

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

SampleScopeWidget::SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleScopePlugin(sampleScopePlugin),
    _currentViewWidget(nullptr),
    _noSamplesOverlayWidget(&_htmlView, Application::getIconFont("FontAwesome").getIcon("eye-dropper"), "No samples view", "There is currently no samples view available..."),
    _currentViewPlugin(nullptr)
{
}

void SampleScopeWidget::initialize()
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    auto& widgetFader = _noSamplesOverlayWidget.getWidgetFader();

    widgetFader.setOpacity(0.0f);
    widgetFader.setMaximumOpacity(0.5f);
    widgetFader.setFadeInDuration(100);
    widgetFader.setFadeOutDuration(300);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_htmlView);
    _layout.addWidget(&_widgetView);
    _layout.addStretch(1);

    _noSamplesOverlayWidget.show();

    connect(&_sampleScopePlugin->getSourcePluginPickerAction(), &PluginPickerAction::pluginPicked, this, [this](plugin::Plugin* plugin) -> void {
        if (_currentViewPlugin) {
            disconnect(&_currentViewPlugin->getSamplerAction(), &ViewPluginSamplerAction::generatedViewTypeChanged, this, nullptr);
        }

        _currentViewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (_currentViewPlugin) {
            connect(&_currentViewPlugin->getSamplerAction(), &ViewPluginSamplerAction::generatedViewTypeChanged, this, &SampleScopeWidget::updateVisibility);

            updateVisibility();
        }
	});

    _widgetView.setScene(&_widgetViewScene);
    _widgetView.setRenderHint(QPainter::Antialiasing);

    updateVisibility();
}

void SampleScopeWidget::setViewHtml(const QString& html)
{
    _htmlView.setHtml(html);
}

void SampleScopeWidget::setViewWidget(const QWidget* widget)
{
    Q_ASSERT(widget);

    if (!widget)
        return;

    if (widget != _currentViewWidget) {
        _currentViewWidget = const_cast<QWidget*>(widget);
        _widgetViewScene.clear();

        auto proxy = _widgetViewScene.addWidget(const_cast<QWidget*>(widget));

        proxy->setPos(0, 0);
    }
    
}

InfoOverlayWidget& SampleScopeWidget::getNoSamplesOverlayWidget()
{
    return _noSamplesOverlayWidget;
}

void SampleScopeWidget::updateVisibility()
{
    if (!_currentViewPlugin)
        return;

    auto& viewPluginSamplerAction = _currentViewPlugin->getSamplerAction();

	switch (viewPluginSamplerAction.getGeneratedViewType()) {
	    case ViewPluginSamplerAction::GeneratedViewType::HTML:
        {
            qDebug() << "HTML view";
	        _htmlView.show();
	        _widgetView.hide();

	        break;
	    }

	    case ViewPluginSamplerAction::GeneratedViewType::Widget:
	    {
            qDebug() << "Widget view";
	        _htmlView.hide();
	        _widgetView.show();

	        break;
	    }
    }
}
