// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopeWidget.h"

#include <Application.h>

#include <util/StyledIcon.h>

#include "SampleScopePlugin.h"

#include <QDebug>
#include <QGraphicsProxyWidget>

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;
using namespace mv::util;

SampleScopeWidget::SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleScopePlugin(sampleScopePlugin),
    _proxyWidget(nullptr),
    _currentViewWidget(nullptr),
    _noSamplesOverlayWidget(&_textHtmlView, StyledIcon("eye-dropper"), "No samples view", "There is currently no samples view available...")
    _currentViewPlugin(nullptr)
{
}

void SampleScopeWidget::initialize()
{
    setAutoFillBackground(true);
    setLayout(&_layout);
    
    _widgetView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto& widgetFader = _noSamplesOverlayWidget.getWidgetFader();

    widgetFader.setOpacity(1.0f);
    widgetFader.setMaximumOpacity(0.5f);
    widgetFader.setFadeInDuration(100);
    widgetFader.setFadeOutDuration(300);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_htmlView);
    _layout.addWidget(&_widgetView);

	_widgetView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _widgetView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _noSamplesOverlayWidget.show();

    connect(&_sampleScopePlugin->getSourcePluginPickerAction(), &PluginPickerAction::pluginPicked, this, [this](plugin::Plugin* plugin) -> void {
        if (_currentViewPlugin) {
            disconnect(&_currentViewPlugin->getSamplerAction(), &ViewPluginSamplerAction::viewGeneratorTypeChanged, this, nullptr);
        }

        _currentViewPlugin = dynamic_cast<ViewPlugin*>(plugin);

        if (_currentViewPlugin) {
            connect(&_currentViewPlugin->getSamplerAction(), &ViewPluginSamplerAction::viewGeneratorTypeChanged, this, &SampleScopeWidget::updateVisibility);

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

        _proxyWidget = _widgetViewScene.addWidget(const_cast<QWidget*>(widget));

        _proxyWidget->setPos(0, 0);

        updateViewWidgetProxySize();
    }
}

InfoOverlayWidget& SampleScopeWidget::getNoSamplesOverlayWidget()
{
    return _noSamplesOverlayWidget;
}

void SampleScopeWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

    _widgetViewScene.setSceneRect(0, 0, width(), height());

    updateViewWidgetProxySize();
}

void SampleScopeWidget::updateVisibility()
{
    if (!_currentViewPlugin)
        return;

    auto& viewPluginSamplerAction = _currentViewPlugin->getSamplerAction();

	switch (viewPluginSamplerAction.getViewGeneratorType()) {
	    case ViewPluginSamplerAction::ViewGeneratorType::HTML:
        {
	        _htmlView.show();
	        _widgetView.hide();

	        break;
	    }

	    case ViewPluginSamplerAction::ViewGeneratorType::Widget:
	    {
	        _htmlView.hide();
	        _widgetView.show();

	        break;
	    }
    }
}

void SampleScopeWidget::updateViewWidgetProxySize() const
{
    if (_proxyWidget)
        _proxyWidget->resize(width(), height());
}
